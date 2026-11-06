#pragma once
namespace ollie::menu {
    void init();
    void shutdown();
    void toggle();
    bool visible();
    void render();
}
bool g_menu_visible_bool();
