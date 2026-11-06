// speedometer.cpp -- bottom-right pill, mph + kmh
#include "imgui.h"
#include "../invoker/natives.h"
#include <cmath>
#include <cstdio>

namespace ollie::state {
    extern bool speedo_on;
    extern bool speedo_kmh;
    extern int  accent_r, accent_g, accent_b;
}

namespace ollie::features {
    void tick_speedo() {
        if (!ollie::state::speedo_on) return;

        float speed = 0.f;
        int ped = N::PLAYER_PED_ID();
        if (ped) {
            int ent = ped;
            if (N::IS_PED_IN_ANY_VEHICLE(ped, false)) {
                ent = N::GET_VEHICLE_PED_IS_IN(ped, false);
                if (!ent) ent = ped;
            }
            float vx = 0, vy = 0, vz = 0;
            N::GET_ENTITY_VELOCITY(ent, &vx, &vy, &vz);
            speed = std::sqrt(vx*vx + vy*vy + vz*vz); // m/s
        }
        float display = ollie::state::speedo_kmh ? speed * 3.6f : speed * 2.23694f;

        auto& io = ImGui::GetIO();
        auto dl = ImGui::GetBackgroundDrawList();
        float w = 180.f, h = 54.f;
        float x = io.DisplaySize.x - w - 20.f;
        float y = io.DisplaySize.y - h - 20.f;

        dl->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), IM_COL32(16, 16, 22, 220), 12.f);
        dl->AddRect(ImVec2(x, y), ImVec2(x + w, y + h),
                    IM_COL32(ollie::state::accent_r, ollie::state::accent_g, ollie::state::accent_b, 230), 12.f, 0, 1.4f);

        char val[32];
        std::snprintf(val, sizeof(val), "%.0f", display);
        char unit[8];
        std::snprintf(unit, sizeof(unit), "%s", ollie::state::speedo_kmh ? "km/h" : "mph");

        dl->AddText(ImVec2(x + 14, y + 10), IM_COL32(255, 255, 255, 255), val);
        dl->AddText(ImVec2(x + 14, y + 32), IM_COL32(220, 160, 200, 255), unit);
    }
}
