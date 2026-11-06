#include "pointers.h"
#include "offsets.h"
#include "logger.h"
#include <Windows.h>
#include <Psapi.h>
#include <vector>
#include <string>
#include <cstring>

#pragma comment(lib, "psapi.lib")

namespace ollie::pointers {
    std::uintptr_t native_registration_table = 0;
    std::uintptr_t world_ptr = 0;
    std::uintptr_t globals_ptr = 0;
    std::uintptr_t game_base = 0;
    std::size_t    game_size = 0;

    static std::vector<int> parse(const char* pat) {
        std::vector<int> bytes;
        const char* p = pat;
        while (*p) {
            if (*p == ' ') { ++p; continue; }
            if (*p == '?') { bytes.push_back(-1); ++p; if (*p == '?') ++p; }
            else {
                char buf[3] = { p[0], p[1], 0 };
                bytes.push_back((int)std::strtoul(buf, nullptr, 16));
                p += 2;
            }
        }
        return bytes;
    }

    void* sig_scan(const char* pattern) {
        if (!pattern || std::strcmp(pattern, "FILL_SIG") == 0) return nullptr;
        auto bytes = parse(pattern);
        if (bytes.empty() || !game_base || !game_size) return nullptr;
        const auto* mem = reinterpret_cast<const unsigned char*>(game_base);
        const std::size_t n = bytes.size();
        for (std::size_t i = 0; i + n < game_size; ++i) {
            bool hit = true;
            for (std::size_t j = 0; j < n; ++j) {
                if (bytes[j] == -1) continue;
                if (mem[i + j] != (unsigned char)bytes[j]) { hit = false; break; }
            }
            if (hit) return (void*)(game_base + i);
        }
        return nullptr;
    }

    // resolve rip-relative lea/mov/call at `at` where the 4-byte disp sits at `disp_off`
    static std::uintptr_t rip_rel(void* at, int disp_off, int instr_len) {
        auto p = (std::uint8_t*)at;
        auto rel = *reinterpret_cast<std::int32_t*>(p + disp_off);
        return reinterpret_cast<std::uintptr_t>(p) + instr_len + rel;
    }

    struct Candidate {
        const char* name;
        const char* pattern;
        int         disp_off;   // byte offset of the 4-byte displacement inside the match
        int         instr_len;  // total instruction length (disp_off + 4 + tail)
    };

    // Candidate patterns for GTA5 / FiveM native registration table.
    // Multiple sigs tried so we catch build drift. First hit wins.
    // Patterns drawn from public RE references (ScriptHookV era onward).
    static const Candidate g_ntab_sigs[] = {
        // CLASSIC: lea rcx, [rip+disp]  (inside native call wrapper)
        //   48 8D 0D ?? ?? ?? ??  E8 ?? ?? ?? ??
        { "ntab-A: lea rcx,[rip+d]; call", "48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B C8 48 85 C0 74", 3, 7 },

        // mov rax, [rip+disp] near native dispatcher
        //   48 8B 05 ?? ?? ?? ??  48 85 C0 74 ?? 48 8B 00
        { "ntab-B: mov rax,[rip+d]", "48 8B 05 ? ? ? ? 48 85 C0 74 ? 48 8B 00 48 85 C0", 3, 7 },

        // lea rax, [rip+disp] variant
        //   48 8D 05 ?? ?? ?? ??  48 89 ?? ?? 4C 8D 05
        { "ntab-C: lea rax,[rip+d]", "48 8D 05 ? ? ? ? 48 89 ? ? 4C 8D 05", 3, 7 },

        // JB 0x32 hint sequence, scan BACKWARD for the lea
        { "ntab-D: 76 32 48 8B 47 40", "76 32 48 8B 47 40 48 85 C0 74 09 48 8B 00 48 85 C0 74 02 FF E0 48 8B 05 ? ? ? ?", 24, 28 },
    };

    static const Candidate g_world_sigs[] = {
        { "world-A: mov rax,[rip+d] + mov cx",      "48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74", 3, 7 },
        { "world-B: lea rax,[rip+d]",               "48 8D 15 ? ? ? ? 48 8B 0C C2",             3, 7 },
        { "world-C: mov rbx,[rip+d]",               "48 8B 1D ? ? ? ? 48 85 DB 74 ? 48 8B 7B",  3, 7 },
    };

    static const Candidate g_globals_sigs[] = {
        { "globals-A: lea r8,[rip+d]",              "4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74",    3, 7 },
        { "globals-B: mov rax,[rip+d]",             "48 8B 05 ? ? ? ? 8B 14 ? 48 03",           3, 7 },
    };

    static std::uintptr_t resolve_first(const Candidate* list, int count, const char* label) {
        for (int i = 0; i < count; ++i) {
            void* at = sig_scan(list[i].pattern);
            if (!at) continue;
            std::uintptr_t resolved = rip_rel(at, list[i].disp_off, list[i].instr_len);
            // sanity: resolved must point inside the module
            if (resolved >= game_base && resolved < game_base + game_size) {
                ollie::log::line("%s HIT (%s) @ game+0x%llx -> 0x%llx",
                                 label, list[i].name,
                                 (unsigned long long)((std::uintptr_t)at - game_base),
                                 (unsigned long long)(resolved - game_base));
                return resolved;
            } else {
                ollie::log::line("%s match but resolved OOB (%s)", label, list[i].name);
            }
        }
        ollie::log::line("%s NO HIT across %d candidates", label, count);
        return 0;
    }

    bool scan() {
        HMODULE m = GetModuleHandleA(offsets::game_module);
        if (!m) m = GetModuleHandleA(nullptr);
        if (!m) { log::line("no game module"); return false; }
        MODULEINFO mi{};
        GetModuleInformation(GetCurrentProcess(), m, &mi, sizeof(mi));
        game_base = (std::uintptr_t)mi.lpBaseOfDll;
        game_size = mi.SizeOfImage;
        log::line("game base %p size %zx", (void*)game_base, game_size);

        // Sig scanning on GTA5.exe is a dead path: the module is Arxan-packed,
        // bytes on disk != bytes in memory, and raw byte walking can hit protected
        // pages that trip Arxan's VEH -> subprocess crash. We take the scripthookv
        // export route instead (see invoker.cpp), so these pointers are no longer
        // required. Keep them zeroed.
        native_registration_table = 0;
        world_ptr                 = 0;
        globals_ptr               = 0;
        log::line("sig scan skipped (scripthookv route owns native invocation)");
        return true;
    }
}
