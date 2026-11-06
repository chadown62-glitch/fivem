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

    static std::uintptr_t rip_rel(void* at, int instr_len) {
        auto p = (std::uint8_t*)at;
        auto rel = *reinterpret_cast<std::int32_t*>(p + instr_len - 4);
        return reinterpret_cast<std::uintptr_t>(p) + instr_len + rel;
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

        if (void* p = sig_scan(offsets::sig_native_registration_table)) {
            native_registration_table = rip_rel(p, 7);
        }
        if (void* p = sig_scan(offsets::sig_world))   world_ptr   = rip_rel(p, 7);
        if (void* p = sig_scan(offsets::sig_globals)) globals_ptr = rip_rel(p, 7);

        log::line("ntab=%p world=%p glob=%p",
                  (void*)native_registration_table, (void*)world_ptr, (void*)globals_ptr);
        return true;
    }
}
