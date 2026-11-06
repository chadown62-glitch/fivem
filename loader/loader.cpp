// loader/loader.cpp
// ollie.exe :: built by olivia :: https://discord.gg/ojailbreaks
// Single-file launcher. Payload DLL rides inside the exe as RT_RCDATA 101.
// At runtime: resolve FiveM_GTAProcess.exe pid, extract payload to camo temp path,
// remote-thread LoadLibraryW, mark the drop for delete-on-close.
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <filesystem>
#include <thread>
#include <chrono>
#include <cstdio>
#include <random>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shlwapi.lib")

namespace fs = std::filesystem;

static void banner() {
    SetConsoleTitleA("ollie :: fivem internal :: by olivia :: .gg/ojailbreaks");
    std::printf("[ollie] fridge hum. bench lit.\n");
    std::printf("[ollie] built by olivia -- https://discord.gg/ojailbreaks\n");
}

// Match any FiveM game-child process. Newer builds carry the build number:
//   FiveM_GTAProcess.exe          (legacy)
//   FiveM_b3095_GTAProcess.exe    (current retail)
//   FiveM_<branch>_GTAProcess.exe
// Pick the process containing "GTAProcess" that is not the host "FiveM.exe".
// Prefer the biggest working set (the game is heavier than the launcher).
static DWORD pid_for_game() {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W pe{ sizeof(pe) };
    DWORD best_pid = 0;
    SIZE_T best_mem = 0;
    if (Process32FirstW(snap, &pe)) {
        do {
            std::wstring name = pe.szExeFile;
            for (auto& c : name) c = towlower(c);
            bool is_candidate =
                   name.find(L"gtaprocess") != std::wstring::npos
                || name == L"fivem_gtaprocess.exe";
            if (!is_candidate) continue;

            HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,
                                   FALSE, pe.th32ProcessID);
            SIZE_T mem = 0;
            if (h) {
                PROCESS_MEMORY_COUNTERS pmc{};
                if (GetProcessMemoryInfo(h, &pmc, sizeof(pmc))) mem = pmc.WorkingSetSize;
                CloseHandle(h);
            }
            if (mem > best_mem) { best_mem = mem; best_pid = pe.th32ProcessID; }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return best_pid;
}

// legacy exact-name path, kept for the "waiting" message
static DWORD pid_for(const wchar_t*) { return pid_for_game(); }

static DWORD wait_for_game(const wchar_t* exe, int seconds) {
    for (int i = 0; i < seconds * 10; ++i) {
        DWORD p = pid_for(exe);
        if (p) return p;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}

// pulls the embedded payload out of our own .rsrc
static bool extract_payload(const fs::path& out) {
    HRSRC r = FindResourceA(nullptr, MAKEINTRESOURCEA(101), "PAYLOAD");
    if (!r) { std::printf("[ollie] no embedded payload\n"); return false; }
    HGLOBAL g = LoadResource(nullptr, r);
    if (!g) return false;
    DWORD sz = SizeofResource(nullptr, r);
    void* p = LockResource(g);
    if (!p || !sz) return false;

    HANDLE h = CreateFileW(out.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_TEMPORARY, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    DWORD w{};
    BOOL ok = WriteFile(h, p, sz, &w, nullptr);
    CloseHandle(h);
    return ok && w == sz;
}

static std::wstring random_name() {
    // camo prefix so a path dump reads like a cached shader
    static const wchar_t* prefix[] = { L"d3dcompiler_47_cache_", L"shader_cache_", L"dxgi_v_" };
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<int> pick(0, 2);
    std::uniform_int_distribution<int> hex(0, 15);
    std::wstring n = prefix[pick(eng)];
    for (int i = 0; i < 8; ++i) {
        wchar_t c = L"0123456789abcdef"[hex(eng)];
        n.push_back(c);
    }
    n += L".dll";
    return n;
}

static bool inject(DWORD pid, const fs::path& dll) {
    HANDLE proc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
                              PROCESS_VM_WRITE     | PROCESS_VM_READ      |
                              PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!proc) { std::printf("[ollie] open failed %lu\n", GetLastError()); return false; }

    std::wstring path = dll.wstring();
    SIZE_T bytes = (path.size() + 1) * sizeof(wchar_t);
    LPVOID remote = VirtualAllocEx(proc, nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote) { CloseHandle(proc); return false; }

    SIZE_T wr{};
    if (!WriteProcessMemory(proc, remote, path.c_str(), bytes, &wr)) {
        VirtualFreeEx(proc, remote, 0, MEM_RELEASE);
        CloseHandle(proc);
        return false;
    }

    auto load = (LPTHREAD_START_ROUTINE)GetProcAddress(
                    GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");
    HANDLE th = CreateRemoteThread(proc, nullptr, 0, load, remote, 0, nullptr);
    if (!th) {
        VirtualFreeEx(proc, remote, 0, MEM_RELEASE);
        CloseHandle(proc);
        return false;
    }

    WaitForSingleObject(th, 15000);
    DWORD rc = 0;
    GetExitCodeThread(th, &rc);
    CloseHandle(th);
    VirtualFreeEx(proc, remote, 0, MEM_RELEASE);
    CloseHandle(proc);
    return rc != 0;
}

int wmain() {
    banner();

    std::printf("[ollie] scanning for FiveM game process ...\n");
    DWORD pid = 0;
    for (int i = 0; i < 300 * 10; ++i) {
        pid = pid_for_game();
        if (pid) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (!pid) {
        std::printf("[ollie] game not seen -- fire fivem first\n");
        std::getchar();
        return 1;
    }
    std::printf("[ollie] pid %lu\n", pid);

    wchar_t temp[MAX_PATH]{};
    GetTempPathW(MAX_PATH, temp);
    fs::path drop = fs::path(temp) / random_name();

    if (!extract_payload(drop)) {
        std::printf("[ollie] extract failed\n");
        std::getchar();
        return 2;
    }
    std::printf("[ollie] payload staged.\n");

    bool ok = inject(pid, drop);

    // mark the drop for delete after the game process closes its handle;
    // LoadLibraryW already opened and mapped it, so this fires on handle close.
    MoveFileExW(drop.c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
    // best-effort immediate unlink too (fails while loaded, that's fine)
    DeleteFileW(drop.c_str());

    if (!ok) {
        std::printf("[ollie] inject failed\n");
        std::getchar();
        return 3;
    }
    std::printf("[ollie] cheeks in. INSERT = menu, END = unload.\n");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
