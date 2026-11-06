// overlay.cpp -- top-left status pill + top-center watermark. Streamer mode hides branding.
#include "imgui.h"
#include <cstdio>
#include <cmath>

namespace ollie::state {
    extern bool overlay_on, watermark_on, streamer_mode;
    extern int  aimbot_target_id;
    extern bool aimbot_enabled;
    extern int  accent_r, accent_g, accent_b;
}

namespace ollie::features {
    void tick_overlay() {
        auto& io = ImGui::GetIO();
        auto dl = ImGui::GetBackgroundDrawList();

        if (ollie::state::overlay_on) {
            float x = 20.f, y = 20.f;
            float w = 240.f, h = 54.f;
            dl->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), IM_COL32(16, 16, 22, 220), 12.f);
            dl->AddRect(ImVec2(x, y), ImVec2(x + w, y + h),
                        IM_COL32(ollie::state::accent_r, ollie::state::accent_g, ollie::state::accent_b, 230), 12.f, 0, 1.4f);
            dl->AddText(ImVec2(x + 14, y + 8),  IM_COL32(255, 255, 255, 255),
                        ollie::state::streamer_mode ? "menu" : "ollie");
            if (!ollie::state::streamer_mode) {
                dl->AddText(ImVec2(x + 60, y + 8),  IM_COL32(220, 160, 200, 255), "built by olivia");
            }
            char line[64];
            std::snprintf(line, sizeof(line), "%.0f fps   target %d", io.Framerate,
                          ollie::state::aimbot_enabled ? ollie::state::aimbot_target_id : -1);
            dl->AddText(ImVec2(x + 14, y + 30), IM_COL32(230, 230, 240, 230), line);
        }

        if (ollie::state::watermark_on && !ollie::state::streamer_mode) {
            const char* txt = "ollie  //  .gg/ojailbreaks";
            ImVec2 ts = ImGui::CalcTextSize(txt);
            float t = (float)ImGui::GetTime();
            float breath = 0.65f + 0.35f * (0.5f + 0.5f * std::sin(t * 1.4f));
            int a = (int)(255 * breath);
            dl->AddText(ImVec2((io.DisplaySize.x - ts.x) * 0.5f, 14.f),
                        IM_COL32(ollie::state::accent_r, ollie::state::accent_g, ollie::state::accent_b, a), txt);
        }
    }
}
