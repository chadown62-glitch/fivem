#include "invoker.h"
#include "../core/pointers.h"
#include "../core/logger.h"
#include <Windows.h>
#include <unordered_map>

namespace ollie::invoker {
    using NativeMap = std::unordered_map<std::uint64_t, NativeHandler>;
    static NativeMap g_map;

    struct Reg {
        Reg* next;
        std::uint32_t count;
        std::uint64_t hashes[7];
        NativeHandler handlers[7];
    };

    bool init() {
        if (!pointers::native_registration_table) {
            log::line("invoker: ntab is 0 -- fill offsets, running stub");
            return true;
        }
        Reg** table = reinterpret_cast<Reg**>(pointers::native_registration_table);
        for (int i = 0; i < 0xFF; ++i) {
            for (Reg* r = table[i]; r; r = r->next) {
                for (std::uint32_t k = 0; k < r->count && k < 7; ++k) {
                    g_map[r->hashes[k]] = r->handlers[k];
                }
            }
        }
        log::line("invoker: %zu natives mapped", g_map.size());
        return true;
    }

    void shutdown() { g_map.clear(); }

    NativeHandler get(std::uint64_t hash) {
        if (auto it = g_map.find(hash); it != g_map.end()) return it->second;
        return nullptr;
    }
}
