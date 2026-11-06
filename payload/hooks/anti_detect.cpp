// Client-side AC surface mitigations. Conservative set — only things that don't
// interfere with FiveM's own tick.
//   1) Unlink the payload module from PEB LDR lists
//   2) Wipe the PE header in memory
//   3) Hook GetAsyncKeyState -- hide menu toggle key presses
//   4) Hide our boot thread from debugger
#include "../core/logger.h"
#include "hooking.h"
#include "MinHook.h"
#include <Windows.h>
#include <winternl.h>
#include <intrin.h>

extern "C" NTSTATUS NTAPI NtSetInformationThread(HANDLE, THREADINFOCLASS, PVOID, ULONG);
#pragma comment(lib, "ntdll.lib")

struct LDR_DATA_TABLE_ENTRY_FULL {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase; PVOID EntryPoint; ULONG SizeOfImage;
    UNICODE_STRING FullDllName; UNICODE_STRING BaseDllName;
};

static void unlink_self(HMODULE self) {
#ifdef _WIN64
    auto peb = (PPEB)__readgsqword(0x60);
#else
    auto peb = (PPEB)__readfsdword(0x30);
#endif
    if (!peb || !peb->Ldr) return;
    auto head = &peb->Ldr->InMemoryOrderModuleList;
    for (auto cur = head->Flink; cur != head; cur = cur->Flink) {
        auto e = CONTAINING_RECORD(cur, LDR_DATA_TABLE_ENTRY_FULL, InMemoryOrderLinks);
        if (e->DllBase == self) {
            e->InLoadOrderLinks.Blink->Flink           = e->InLoadOrderLinks.Flink;
            e->InLoadOrderLinks.Flink->Blink           = e->InLoadOrderLinks.Blink;
            e->InMemoryOrderLinks.Blink->Flink         = e->InMemoryOrderLinks.Flink;
            e->InMemoryOrderLinks.Flink->Blink         = e->InMemoryOrderLinks.Blink;
            e->InInitializationOrderLinks.Blink->Flink = e->InInitializationOrderLinks.Flink;
            e->InInitializationOrderLinks.Flink->Blink = e->InInitializationOrderLinks.Blink;
            ollie::log::line("unlinked from PEB");
            break;
        }
    }
}

static void wipe_header(HMODULE self) {
    auto base = (PBYTE)self;
    DWORD old;
    if (VirtualProtect(base, 0x1000, PAGE_READWRITE, &old)) {
        SecureZeroMemory(base, 0x1000);
        VirtualProtect(base, 0x1000, old, &old);
        ollie::log::line("PE header wiped");
    }
}

using fnGAKS = SHORT(WINAPI*)(int);
static fnGAKS oGAKS = nullptr;
static SHORT WINAPI hkGAKS(int vk) {
    if (vk == VK_INSERT || vk == VK_END) return 0;
    return oGAKS ? oGAKS(vk) : 0;
}

bool install_anti_detect() {
    HMODULE me = nullptr;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCSTR)&install_anti_detect, &me);

    NtSetInformationThread(GetCurrentThread(), (THREADINFOCLASS)0x11, nullptr, 0);

    if (auto u = GetModuleHandleW(L"user32")) {
        auto p = GetProcAddress(u, "GetAsyncKeyState");
        if (p && MH_CreateHook(p, &hkGAKS, (void**)&oGAKS) == MH_OK) MH_EnableHook(p);
    }

    unlink_self(me);
    wipe_header(me);
    ollie::log::line("anti-detect armed (conservative)");
    return true;
}
