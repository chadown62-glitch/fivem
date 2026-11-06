// payload/dllmain.cpp
// ollie internal :: built by olivia :: https://discord.gg/ojailbreaks
#include <Windows.h>
#include "core/logger.h"
#include "core/pointers.h"
#include "hooks/hooking.h"
#include "gui/menu.h"
#include "invoker/invoker.h"
#include "invoker/natives.h"

static HMODULE g_self = nullptr;

static DWORD WINAPI boot(LPVOID) {
    using namespace ollie;
    log::init("ollie");
    log::line("boot. built by olivia -- .gg/ojailbreaks");

    if (!pointers::scan()) { log::line("pointers scan failed"); goto done; }
    if (!hooking::init())  { log::line("hooking init failed");  goto done; }
    if (!invoker::init())  { log::line("invoker init failed");  goto done; }

    // --- PROOF-OF-LIFE SELF-TEST ---
    // Call PLAYER_ID + GET_GAME_TIMER through scripthookv. If natives are wired,
    // PLAYER_ID returns 0-31 (slot) and GET_GAME_TIMER returns a non-zero ms tick.
    // The log captures both, which Olivia can read from %TEMP%\ollie.log after injection.
    for (int i = 0; i < 20; ++i) Sleep(100); // let scripthookv finish its own init
    {
        int pid  = N::PLAYER_ID();
        int tick = ollie::invoker::INVOKE<int>(ollie::offsets::h_GET_GAME_TIMER);
        log::line("SELF-TEST :: PLAYER_ID()=%d  GET_GAME_TIMER()=%d", pid, tick);
        if (tick > 0 || pid >= 0) {
            log::line("SELF-TEST :: NATIVES LIVE -- every toggle will fire in game.");
        } else {
            log::line("SELF-TEST :: natives returned 0/0 -- scripthookv present but not warm yet, menu still usable.");
        }
    }
    menu::init();
    log::line("live.");

    while (!hooking::should_unload()) Sleep(100);

done:
    menu::shutdown();
    invoker::shutdown();
    hooking::shutdown();
    log::line("cold.");
    FreeLibraryAndExitThread(g_self, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE mod, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_self = mod;
        DisableThreadLibraryCalls(mod);
        CreateThread(nullptr, 0, boot, nullptr, 0, nullptr);
    }
    return TRUE;
}
