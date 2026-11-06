#include "hooking.h"
#include "../core/logger.h"
#include "MinHook.h"
#include <atomic>

namespace ollie::hooking {
    static std::atomic<bool> g_unload = false;

    bool init() {
        if (MH_Initialize() != MH_OK) return false;
        if (!install_d3d11())    { log::line("d3d11 hook failed"); return false; }
        if (!install_wndproc())  { log::line("wndproc hook failed"); return false; }
        install_anti_detect();
        return true;
    }
    void shutdown() {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }
    bool should_unload()  { return g_unload.load(); }
    void request_unload() { g_unload.store(true); }
}
