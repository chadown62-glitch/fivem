// keybinds.cpp -- runtime rebinding.
#include "imgui.h"
#include "../invoker/natives.h"
#include <Windows.h>
#include <atomic>
#include <string>
#include <array>
#include <cmath>

namespace ollie::state {
    extern bool  godmode, invisible, never_wanted, super_jump, aimbot_enabled, triggerbot_enabled;
    extern bool  esp_box, veh_fly, freecam, rain_explosions_on_me, panic_active;
    extern int   accent_r, accent_g, accent_b;
}

namespace ollie::keys {

    struct Bind {
        const char* label;
        int         vk;
        bool*       target;
        void      (*action)();
    };

    void panic_fire();

    static void throw_grenade_at_crosshair() {
        ollie::invoker::run_on_script([] {
            float px = 0, py = 0, pz = 0;
            N::GET_ENTITY_COORDS(N::PLAYER_PED_ID(), true, &px, &py, &pz);
            float cx = 0, cy = 0, cz = 0;
            N::GET_GAMEPLAY_CAM_COORD(&cx, &cy, &cz);
            float rx = 0, ry = 0, rz = 0;
            N::GET_GAMEPLAY_CAM_ROT(2, &rx, &ry, &rz);
            float yaw   = rz * 3.14159265f / 180.f;
            float pitch = rx * 3.14159265f / 180.f;
            float dx = -std::sin(yaw) * std::cos(pitch);
            float dy =  std::cos(yaw) * std::cos(pitch);
            float dz =  std::sin(pitch);
            float D  = 60.f;
            float ex = cx + dx * D, ey = cy + dy * D, ez = cz + dz * D;
            N::ADD_EXPLOSION(ex, ey, ez, 7, 80.f, true, false, 1.f);
        });
    }

    static std::array<Bind, 11> g_binds = { {
        { "godmode",       VK_F3,  &ollie::state::godmode,            nullptr },
        { "invisible",     VK_F4,  &ollie::state::invisible,          nullptr },
        { "never wanted",  VK_F5,  &ollie::state::never_wanted,       nullptr },
        { "super jump",    VK_F6,  &ollie::state::super_jump,         nullptr },
        { "aimbot",        VK_F7,  &ollie::state::aimbot_enabled,     nullptr },
        { "triggerbot",    VK_F8,  &ollie::state::triggerbot_enabled, nullptr },
        { "esp box",       VK_F9,  &ollie::state::esp_box,            nullptr },
        { "vehicle fly",   VK_F10, &ollie::state::veh_fly,            nullptr },
        { "freecam",       VK_F11, &ollie::state::freecam,            nullptr },
        { "throw grenade", 'G',    nullptr,                           &throw_grenade_at_crosshair },
        { "PANIC!",        VK_F12, nullptr,                           &panic_fire },
    } };

    static std::atomic<int> g_capture = -1;
    static std::array<bool, 11> g_was_down = { false };

    int   slot_count() { return (int)g_binds.size(); }
    Bind& slot(int i)  { return g_binds[i]; }

    void request_capture(int slot) { g_capture.store(slot); }
    int  capturing()               { return g_capture.load(); }

    static const char* vk_name(int vk) {
        if (vk >= 'A' && vk <= 'Z') { static char b[2]; b[0] = (char)vk; b[1] = 0; return b; }
        if (vk >= '0' && vk <= '9') { static char b[2]; b[0] = (char)vk; b[1] = 0; return b; }
        if (vk >= VK_F1 && vk <= VK_F24) {
            static char b[8]; _snprintf_s(b, sizeof(b), "F%d", vk - VK_F1 + 1); return b;
        }
        switch (vk) {
            case VK_SHIFT: return "SHIFT";
            case VK_CONTROL: return "CTRL";
            case VK_MENU: return "ALT";
            case VK_SPACE: return "SPACE";
            case VK_LBUTTON: return "LMB";
            case VK_RBUTTON: return "RMB";
            case VK_MBUTTON: return "MMB";
            case VK_XBUTTON1: return "MOUSE4";
            case VK_XBUTTON2: return "MOUSE5";
            case VK_INSERT: return "INS";
            case VK_END: return "END";
            case VK_HOME: return "HOME";
            case VK_DELETE: return "DEL";
            case VK_PRIOR: return "PGUP";
            case VK_NEXT: return "PGDN";
            case VK_TAB: return "TAB";
            case VK_RETURN: return "ENTER";
            case VK_ESCAPE: return "ESC";
        }
        static char b[16]; _snprintf_s(b, sizeof(b), "VK%d", vk); return b;
    }
    const char* name(int vk) { return vk_name(vk); }

    void tick() {
        int cap = g_capture.load();
        if (cap >= 0) {
            for (int vk = 1; vk < 256; ++vk) {
                if (vk == VK_LBUTTON) continue;
                if (GetAsyncKeyState(vk) & 0x8000) {
                    if (vk == VK_ESCAPE) { g_capture.store(-1); return; }
                    g_binds[cap].vk = vk;
                    g_capture.store(-1);
                    return;
                }
            }
            return;
        }
        for (int i = 0; i < (int)g_binds.size(); ++i) {
            bool down = (GetAsyncKeyState(g_binds[i].vk) & 0x8000) != 0;
            if (down && !g_was_down[i]) {
                if (g_binds[i].target) *g_binds[i].target = !*g_binds[i].target;
                if (g_binds[i].action) g_binds[i].action();
            }
            g_was_down[i] = down;
        }
    }
}

namespace ollie::features { void tick_keybinds() { ollie::keys::tick(); } }
