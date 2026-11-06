// payload/dllmain.cpp
// ollie internal :: built by olivia :: https://discord.gg/ojailbreaks
#include <Windows.h>
#include "core/logger.h"
#include "core/pointers.h"
#include "hooks/hooking.h"
#include "gui/menu.h"
#include "invoker/invoker.h"

static HMODULE g_self = nullptr;

static DWORD WINAPI boot(LPVOID) {
    using namespace ollie;
    log::init("ollie");
    log::line("boot. built by olivia -- .gg/ojailbreaks");

    if (!pointers::scan()) { log::line("pointers scan failed"); goto done; }
    if (!hooking::init())  { log::line("hooking init failed");  goto done; }
    if (!invoker::init())  { log::line("invoker init failed");  goto done; }
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
