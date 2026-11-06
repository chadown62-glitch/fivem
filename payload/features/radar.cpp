// radar.cpp -- top-right 2D minimap. Local at center, dots per nearby player.
#include "imgui.h"
#include "../invoker/natives.h"
#include <cmath>
#include <algorithm>

namespace ollie::state {
    extern bool  radar_on;
    extern float radar_range;
    extern int   radar_size;
    extern bool  radar_rotate;
    extern int   aimbot_target_id;
}

namespace ollie::features {
    void tick_radar() {
        using namespace ollie::state;
        if (!radar_on) return;

        auto& io = ImGui::GetIO();
        float r = (float)radar_size;
        ImVec2 center(io.DisplaySize.x - r - 30.f, 110.f + r);

        auto dl = ImGui::GetBackgroundDrawList();
        dl->AddRectFilled(ImVec2(center.x - r, center.y - r), ImVec2(center.x + r, center.y + r),
                          IM_COL32(10, 10, 14, 220), 10.f);
        dl->AddRect(ImVec2(center.x - r, center.y - r), ImVec2(center.x + r, center.y + r),
                    IM_COL32(255, 60, 170, 240), 10.f, 0, 1.3f);
        dl->AddLine(ImVec2(center.x - r, center.y), ImVec2(center.x + r, center.y), IM_COL32(255, 255, 255, 60), 1.f);
        dl->AddLine(ImVec2(center.x, center.y - r), ImVec2(center.x, center.y + r), IM_COL32(255, 255, 255, 60), 1.f);

        int localPed = N::PLAYER_PED_ID();
        int localPid = N::PLAYER_ID();
        float lx = 0, ly = 0, lz = 0;
        N::GET_ENTITY_COORDS(localPed, true, &lx, &ly, &lz);

        float heading = N::GET_ENTITY_HEADING(localPed) * 3.14159265f / 180.f;
        float ch = std::cos(heading), sh = std::sin(heading);

        for (int i = 0; i < 32; ++i) {
            if (i == localPid) continue;
            if (!N::IS_PLAYER_ACTIVE(i)) continue;
            int ped = N::GET_PLAYER_PED(i);
            if (!ped) continue;
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            float dx = x - lx, dy = y - ly;
            float d = std::sqrt(dx*dx + dy*dy);
            if (d > radar_range) continue;

            float px = dx, py = dy;
            if (radar_rotate) {
                float nx =  ch * px + sh * py;
                float ny = -sh * px + ch * py;
                px = nx; py = ny;
            }
            float u = px / radar_range * r;
            float v = -py / radar_range * r;
            ImVec2 p(center.x + u, center.y + v);

            ImU32 c = (i == aimbot_target_id) ? IM_COL32(255, 230, 60, 255)
                                              : IM_COL32(255, 60, 170, 255);
            dl->AddCircleFilled(p, 3.5f, c);
        }

        // local marker (triangle)
        dl->AddTriangleFilled(ImVec2(center.x, center.y - 6),
                              ImVec2(center.x - 4, center.y + 4),
                              ImVec2(center.x + 4, center.y + 4),
                              IM_COL32(255, 255, 255, 255));
    }
}
