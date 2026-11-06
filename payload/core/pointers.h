#pragma once
#include <cstdint>
#include <cstddef>

namespace ollie::pointers {
    extern std::uintptr_t native_registration_table;
    extern std::uintptr_t world_ptr;
    extern std::uintptr_t globals_ptr;
    extern std::uintptr_t game_base;
    extern std::size_t    game_size;

    bool scan();
    void* sig_scan(const char* pattern);
}
