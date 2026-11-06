// aimbot + triggerbot : priority modes, lead, sticky, rapid fire, draw line, custom bones.
#include "imgui.h"
#include "../invoker/natives.h"
#include <Windows.h>
#include <cmath>
#include <algorithm>
#include <unordered_set>

namespace ollie::state {
    extern bool  aimbot_enabled, aimbot_hold_rmb, aimbot_visible_check, aimbot_ignore_dead;
    extern int   aimbot_bone;
    extern float aimbot_fov, aimbot_smooth;
    extern int   aimbot_target_id;
    extern bool  aimbot_lead, aimbot_sticky, aimbot_draw_line;
    extern float aimbot_bullet_speed;
    extern int   aimbot_sticky_ms;
    extern int   aimbot_priority;   // 0 cross, 1 distance, 2 health
    extern bool  triggerbot_enabled, triggerbot_rmb_only, triggerbot_rapid;
    extern int   triggerbot_delay_ms;
    extern std::unordered_set<int> friends_set;
    extern bool  friends_skip_aim;
    extern int   accent_r, accent_g, accent_b;
}

static int bone_for(int sel) {
    using namespace ollie::offsets;
    switch (sel) {
        case 0: return BONE_HEAD;
        case 1: return BONE_NECK;
        case 2: return BONE_SPINE3;
        case 3: return BONE_PELVIS;
        case 4: return BONE_L_HAND;
        case 5: return BONE_R_HAND;
        case 6: return BONE_L_FOOT;
        case 7: return BONE_R_FOOT;
    }
    return BONE_HEAD;
}

namespace ollie::features {
    static int g_last_fire = 0;
    static int g_sticky_last_seen = 0;
    static float g_sticky_wx = 0, g_sticky_wy = 0, g_sticky_wz = 0;
    static bool rmb() { return (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0; }

    void tick_aimbot() {
        ollie::invoker::run_on_script([] {
            using namespace ollie::state;
            if (!aimbot_enabled && !triggerbot_enabled) return;

            int localPid = N::PLAYER_ID();
            int localPed = N::PLAYER_PED_ID();
            if (localPed == 0) return;

            bool gated = aimbot_enabled && aimbot_hold_rmb && !rmb() && !triggerbot_enabled;
            if (gated) { aimbot_target_id = -1; return; }

            float lx = 0, ly = 0, lz = 0;
            N::GET_ENTITY_COORDS(localPed, true, &lx, &ly, &lz);

            auto& io = ImGui::GetIO();
            float cx = io.DisplaySize.x * 0.5f;
            float cy = io.DisplaySize.y * 0.5f;
            float fov_px = std::tan(aimbot_fov * 0.5f * 3.14159265f / 180.f) * io.DisplaySize.y;

            int   best = -1;
            float best_score = 1e18f;
            float best_wx = 0, best_wy = 0, best_wz = 0;

            for (int i = 0; i < 32; ++i) {
                if (i == localPid) continue;
                if (!N::IS_PLAYER_ACTIVE(i)) continue;
                if (aimbot_ignore_dead && N::IS_PLAYER_DEAD(i)) continue;
                if (friends_skip_aim && friends_set.count(i)) continue;
                int ped = N::GET_PLAYER_PED(i);
                if (!ped) continue;
                if (aimbot_visible_check && !N::IS_ENTITY_ON_SCREEN(ped)) continue;

                float bx = 0, by = 0, bz = 0;
                N::GET_PED_BONE_COORDS(ped, bone_for(aimbot_bone), 0, 0, 0, &bx, &by, &bz);

                if (aimbot_lead && aimbot_bullet_speed > 1.f) {
                    float dx = bx - lx, dy = by - ly, dz = bz - lz;
                    float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
                    float t = dist / aimbot_bullet_speed;
                    float vx = 0, vy = 0, vz = 0;
                    N::GET_ENTITY_VELOCITY(ped, &vx, &vy, &vz);
                    bx += vx * t; by += vy * t; bz += vz * t;
                }

                float sx = 0, sy = 0;
                if (!N::GET_SCREEN_COORD_FROM_WORLD(bx, by, bz, &sx, &sy)) continue;
                sx *= io.DisplaySize.x; sy *= io.DisplaySize.y;

                float sdx = sx - cx, sdy = sy - cy;
                float cross_d2 = sdx * sdx + sdy * sdy;
                if (cross_d2 > fov_px * fov_px) continue;

                float score = cross_d2;
                if (aimbot_priority == 1) {
                    // closest to local
                    float dx = bx - lx, dy = by - ly, dz = bz - lz;
                    score = dx*dx + dy*dy + dz*dz;
                } else if (aimbot_priority == 2) {
                    // lowest health
                    score = (float)N::GET_ENTITY_HEALTH(ped);
                }

                if (score < best_score) {
                    best_score = score;
                    best    = i;
                    best_wx = bx; best_wy = by; best_wz = bz;
                }
            }

            int now = N::GET_GAME_TIMER();
            if (best < 0 && aimbot_sticky && aimbot_target_id >= 0 && now - g_sticky_last_seen < aimbot_sticky_ms) {
                best = aimbot_target_id;
                best_wx = g_sticky_wx; best_wy = g_sticky_wy; best_wz = g_sticky_wz;
            } else if (best >= 0) {
                g_sticky_last_seen = now;
                g_sticky_wx = best_wx; g_sticky_wy = best_wy; g_sticky_wz = best_wz;
            }
            aimbot_target_id = best;
            if (best < 0) return;

            if (aimbot_draw_line) {
                float sx = 0, sy = 0;
                if (N::GET_SCREEN_COORD_FROM_WORLD(best_wx, best_wy, best_wz, &sx, &sy)) {
                    auto dl = ImGui::GetBackgroundDrawList();
                    dl->AddLine(ImVec2(cx, cy),
                                ImVec2(sx * io.DisplaySize.x, sy * io.DisplaySize.y),
                                IM_COL32(accent_r, accent_g, accent_b, 220), 1.2f);
                }
            }

            if (aimbot_enabled && (!aimbot_hold_rmb || rmb())) {
                float dx = best_wx - lx, dy = best_wy - ly;
                float heading = std::atan2(-dx, dy) * 180.f / 3.14159265f;
                float cur = N::GET_ENTITY_HEADING(localPed);
                float smoothed = cur + (heading - cur) * std::clamp(1.f - aimbot_smooth, 0.05f, 1.f);
                N::SET_ENTITY_HEADING(localPed, smoothed);
            }

            if (triggerbot_enabled) {
                if (triggerbot_rmb_only && !rmb()) return;
                int delay = triggerbot_rapid ? std::max(30, triggerbot_delay_ms / 2) : triggerbot_delay_ms;
                if (now - g_last_fire < delay) return;
                if (triggerbot_rapid) N::CLEAR_PED_TASKS_IMMEDIATELY(N::PLAYER_PED_ID());
                N::SET_PED_SHOOTS_AT_COORD(N::PLAYER_PED_ID(), best_wx, best_wy, best_wz, true);
                g_last_fire = now;
            }
        });
    }
}
