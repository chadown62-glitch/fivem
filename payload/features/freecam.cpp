#include "../invoker/natives.h"
#include <Windows.h>
#include <cmath>

namespace ollie::state { extern bool freecam; extern float noclip_speed; }

namespace ollie::features {
    static int   g_cam = 0;
    static float g_x = 0, g_y = 0, g_z = 0;
    static float g_pitch = 0, g_yaw = 0;
    static bool  g_was_on = false;
    static POINT g_last = {};

    static float axis(int vk) { return (GetAsyncKeyState(vk) & 0x8000) ? 1.f : 0.f; }

    void tick_freecam() {
        ollie::invoker::run_on_script([] {
            bool on = ollie::state::freecam;
            if (on && !g_was_on) {
                int ped = N::PLAYER_PED_ID();
                N::GET_ENTITY_COORDS(ped, true, &g_x, &g_y, &g_z);
                g_cam = N::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", true);
                N::SET_CAM_COORD(g_cam, g_x, g_y, g_z + 1.5f);
                N::SET_CAM_ACTIVE(g_cam, true);
                N::RENDER_SCRIPT_CAMS(true, false, 0, true, false);
                N::FREEZE_ENTITY_POSITION(ped, true);
                N::SET_ENTITY_VISIBLE(ped, false, false);
                GetCursorPos(&g_last);
            } else if (!on && g_was_on) {
                N::RENDER_SCRIPT_CAMS(false, false, 0, true, false);
                N::DESTROY_CAM(g_cam, false);
                int ped = N::PLAYER_PED_ID();
                N::FREEZE_ENTITY_POSITION(ped, false);
                N::SET_ENTITY_VISIBLE(ped, true, false);
            }
            g_was_on = on;
            if (!on) return;

            float speed = ollie::state::noclip_speed * 0.4f;
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) speed *= 6.0f;

            float fwd  = axis('W') - axis('S');
            float strf = axis('D') - axis('A');
            float up   = (GetAsyncKeyState(VK_SPACE) & 0x8000 ? 1.f : 0.f)
                       - (GetAsyncKeyState(VK_LCONTROL) & 0x8000 ? 1.f : 0.f);

            float yaw_rad = g_yaw * 3.14159265f / 180.f;
            float cx = std::cos(yaw_rad), sx = std::sin(yaw_rad);
            g_x += (-sx * fwd + cx * strf) * speed;
            g_y += ( cx * fwd + sx * strf) * speed;
            g_z += up * speed;

            POINT p; GetCursorPos(&p);
            g_yaw   -= (p.x - g_last.x) * 0.15f;
            g_pitch -= (p.y - g_last.y) * 0.15f;
            if (g_pitch >  89) g_pitch =  89;
            if (g_pitch < -89) g_pitch = -89;

            HWND fg = GetForegroundWindow();
            RECT rc{};
            if (fg) GetClientRect(fg, &rc);
            POINT mid{ (rc.right - rc.left) / 2, (rc.bottom - rc.top) / 2 };
            if (fg) ClientToScreen(fg, &mid);
            SetCursorPos(mid.x, mid.y);
            g_last = mid;

            N::SET_CAM_COORD(g_cam, g_x, g_y, g_z);
            N::SET_CAM_ROT(g_cam, g_pitch, 0.f, g_yaw, 2);
        });
    }
}
