#pragma once
namespace ollie::hooking {
    bool init();
    void shutdown();
    bool should_unload();
    void request_unload();
}

bool install_d3d11();
bool install_wndproc();
bool install_anti_detect();
