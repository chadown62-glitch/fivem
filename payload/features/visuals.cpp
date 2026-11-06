// ESP overlay with chams tint, 3D box, skeleton, bone dots, head dot, crosshair styles,
// FOV ring, line-to-target, distance-to-crosshair text.
#include "imgui.h"
#include "../invoker/natives.h"
#include "../core/offsets.h"
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <unordered_set>

namespace ollie::state {
    extern bool  esp_box, esp_name, esp_dist, esp_health, esp_tracers;
    extern bool  esp_skeleton, esp_bones, esp_crosshair, esp_chams;
    extern bool  esp_aim_fov_ring, esp_head_dot, esp_dist_to_cross;
    extern bool  esp_box_3d;
    extern int   esp_chams_alpha;
    extern int   crosshair_style; // 0 cross, 1 dot, 2 circle, 3 plus-dot, 4 T
    extern float esp_max_distance;
    extern int   aimbot_target_id;
    extern bool  aimbot_enabled;
    extern float aimbot_fov;
    extern std::unordered_set<int> friends_set;
    extern bool  friends_skip_esp;
    extern int   accent_r, accent_g, accent_b;
}

static bool w2s(float wx, float wy, float wz, ImVec2& out) {
    float sx = 0.f, sy = 0.f;
    if (!N::GET_SCREEN_COORD_FROM_WORLD(wx, wy, wz, &sx, &sy)) return false;
    auto& io = ImGui::GetIO();
    out = ImVec2(sx * io.DisplaySize.x, sy * io.DisplaySize.y);
    return true;
}

static void dot(ImDrawList* dl, int ped, int bone, ImU32 col, float r = 2.f) {
    float bx = 0, by = 0, bz = 0;
    N::GET_PED_BONE_COORDS(ped, bone, 0, 0, 0, &bx, &by, &bz);
    ImVec2 p; if (w2s(bx, by, bz, p)) dl->AddCircleFilled(p, r, col);
}

static void seg(ImDrawList* dl, int ped, int a, int b, ImU32 col) {
    float ax = 0, ay = 0, az = 0, bx = 0, by = 0, bz = 0;
    N::GET_PED_BONE_COORDS(ped, a, 0, 0, 0, &ax, &ay, &az);
    N::GET_PED_BONE_COORDS(ped, b, 0, 0, 0, &bx, &by, &bz);
    ImVec2 pa, pb;
    if (w2s(ax, ay, az, pa) && w2s(bx, by, bz, pb)) dl->AddLine(pa, pb, col, 1.2f);
}

static void draw_crosshair(ImDrawList* dl, float cx, float cy, int style, ImU32 c) {
    switch (style) {
        case 0: // classic cross
            dl->AddLine(ImVec2(cx - 5, cy), ImVec2(cx + 5, cy), c, 1.2f);
            dl->AddLine(ImVec2(cx, cy - 5), ImVec2(cx, cy + 5), c, 1.2f);
            break;
        case 1: // dot
            dl->AddCircleFilled(ImVec2(cx, cy), 2.0f, c);
            break;
        case 2: // circle
            dl->AddCircle(ImVec2(cx, cy), 6.0f, c, 24, 1.2f);
            break;
        case 3: // plus + dot
            dl->AddLine(ImVec2(cx - 6, cy), ImVec2(cx + 6, cy), c, 1.2f);
            dl->AddLine(ImVec2(cx, cy - 6), ImVec2(cx, cy + 6), c, 1.2f);
            dl->AddCircleFilled(ImVec2(cx, cy), 1.5f, c);
            break;
        case 4: // T
            dl->AddLine(ImVec2(cx - 6, cy - 3), ImVec2(cx + 6, cy - 3), c, 1.2f);
            dl->AddLine(ImVec2(cx, cy - 3), ImVec2(cx, cy + 6), c, 1.2f);
            break;
    }
}

// Pedestrian-sized default bbox when model dims aren't handy
static void default_ped_bbox(float* mn, float* mx) {
    mn[0] = -0.45f; mn[1] = -0.45f; mn[2] = -1.0f;
    mx[0] =  0.45f; mx[1] =  0.45f; mx[2] =  1.0f;
}

static void draw_box_3d(ImDrawList* dl, int ped, ImU32 col) {
    float px = 0, py = 0, pz = 0;
    N::GET_ENTITY_COORDS(ped, true, &px, &py, &pz);
    float h = N::GET_ENTITY_HEADING(ped) * 3.14159265f / 180.f;
    float ch = std::cos(h), sh = std::sin(h);

    float mn[3], mx[3];
    default_ped_bbox(mn, mx);

    // 8 corners of a box, rotated by heading around z
    ImVec2 pts[8];
    bool all_ok = true;
    for (int i = 0; i < 8; ++i) {
        float lx = (i & 1) ? mx[0] : mn[0];
        float ly = (i & 2) ? mx[1] : mn[1];
        float lz = (i & 4) ? mx[2] : mn[2];
        float wx = lx * ch - ly * sh;
        float wy = lx * sh + ly * ch;
        if (!w2s(px + wx, py + wy, pz + lz, pts[i])) { all_ok = false; break; }
    }
    if (!all_ok) return;

    // bottom square: 0-1,1-3,3-2,2-0
    dl->AddLine(pts[0], pts[1], col, 1.2f);
    dl->AddLine(pts[1], pts[3], col, 1.2f);
    dl->AddLine(pts[3], pts[2], col, 1.2f);
    dl->AddLine(pts[2], pts[0], col, 1.2f);
    // top square
    dl->AddLine(pts[4], pts[5], col, 1.2f);
    dl->AddLine(pts[5], pts[7], col, 1.2f);
    dl->AddLine(pts[7], pts[6], col, 1.2f);
    dl->AddLine(pts[6], pts[4], col, 1.2f);
    // verticals
    dl->AddLine(pts[0], pts[4], col, 1.2f);
    dl->AddLine(pts[1], pts[5], col, 1.2f);
    dl->AddLine(pts[2], pts[6], col, 1.2f);
    dl->AddLine(pts[3], pts[7], col, 1.2f);
}

namespace ollie::features {
    void tick_visuals() {
        using namespace ollie::state;
        using namespace ollie::offsets;
        auto dl = ImGui::GetBackgroundDrawList();
        auto& io = ImGui::GetIO();
        ImU32 primary = IM_COL32(accent_r, accent_g, accent_b, 255);
        float cx = io.DisplaySize.x * 0.5f, cy = io.DisplaySize.y * 0.5f;

        if (esp_crosshair) draw_crosshair(dl, cx, cy, crosshair_style, IM_COL32(255, 255, 255, 230));
        if (esp_aim_fov_ring && aimbot_enabled) {
            float r = std::tan(aimbot_fov * 0.5f * 3.14159265f / 180.f) * io.DisplaySize.y;
            dl->AddCircle(ImVec2(cx, cy), r, IM_COL32(accent_r, accent_g, accent_b, 170), 48, 1.2f);
        }

        int localPid = N::PLAYER_ID();
        int localPed = N::PLAYER_PED_ID();
        float lx = 0, ly = 0, lz = 0;
        N::GET_ENTITY_COORDS(localPed, true, &lx, &ly, &lz);

        int   closest_to_cross = -1;
        float closest_cross_d2 = 1e18f;

        for (int i = 0; i < 32; ++i) {
            if (!N::IS_PLAYER_ACTIVE(i)) continue;
            if (i == localPid) continue;
            if (friends_skip_esp && friends_set.count(i)) continue;
            int ped = N::GET_PLAYER_PED(i);
            if (ped == 0) continue;

            float px = 0, py = 0, pz = 0;
            N::GET_ENTITY_COORDS(ped, true, &px, &py, &pz);
            float dx = px - lx, dy = py - ly, dz = pz - lz;
            float d = std::sqrt(dx*dx + dy*dy + dz*dz);
            if (d > esp_max_distance) continue;

            ImVec2 head, feet;
            if (!w2s(px, py, pz + 1.0f, head)) continue;
            if (!w2s(px, py, pz - 1.0f, feet)) continue;

            float h = feet.y - head.y;
            float w = h * 0.5f;
            ImVec2 tl(head.x - w * 0.5f, head.y);
            ImVec2 br(head.x + w * 0.5f, feet.y);

            float midx = (tl.x + br.x) * 0.5f, midy = (tl.y + br.y) * 0.5f;
            float cdx = midx - cx, cdy = midy - cy;
            float cd2 = cdx*cdx + cdy*cdy;
            if (cd2 < closest_cross_d2) { closest_cross_d2 = cd2; closest_to_cross = i; }

            ImU32 col = (i == aimbot_target_id) ? IM_COL32(255, 230, 60, 255) : primary;
            bool  friendly = friends_set.count(i) > 0;
            if (friendly) col = IM_COL32(60, 220, 120, 255);

            if (esp_chams) {
                int a = std::clamp(esp_chams_alpha, 0, 255);
                dl->AddRectFilled(tl, br, (col & 0x00FFFFFF) | ((ImU32)a << 24), 2.f);
            }
            if (esp_box)    dl->AddRect(tl, br, col, 0.f, 0, 1.5f);
            if (esp_box_3d) draw_box_3d(dl, ped, col);
            if (esp_head_dot) dot(dl, ped, BONE_HEAD, col, 3.5f);
            if (esp_name) {
                const char* name = N::GET_PLAYER_NAME(i);
                dl->AddText(ImVec2(tl.x, tl.y - 16), col, name ? name : "player");
            }
            if (esp_dist) {
                char buf[32]; std::snprintf(buf, sizeof(buf), "%.0fm", d);
                dl->AddText(ImVec2(tl.x, br.y + 2), col, buf);
            }
            if (esp_health) {
                int hp = N::GET_ENTITY_HEALTH(ped);
                float r = (std::clamp)(hp / 200.0f, 0.f, 1.f);
                ImVec2 a(tl.x - 6, tl.y), b(tl.x - 2, tl.y + h);
                dl->AddRectFilled(ImVec2(a.x, b.y - h * r), b, IM_COL32(60, 255, 90, 255));
                dl->AddRect(a, b, IM_COL32(0, 0, 0, 255));
            }
            if (esp_tracers) {
                dl->AddLine(ImVec2(cx, io.DisplaySize.y),
                            ImVec2(midx, midy), col, 1.0f);
            }
            if (esp_skeleton) {
                ImU32 sc = IM_COL32(255, 255, 255, 220);
                seg(dl, ped, BONE_HEAD, BONE_NECK, sc);
                seg(dl, ped, BONE_NECK, BONE_SPINE3, sc);
                seg(dl, ped, BONE_SPINE3, BONE_PELVIS, sc);
                seg(dl, ped, BONE_SPINE3, BONE_L_ELBOW, sc);
                seg(dl, ped, BONE_L_ELBOW, BONE_L_HAND, sc);
                seg(dl, ped, BONE_SPINE3, BONE_R_ELBOW, sc);
                seg(dl, ped, BONE_R_ELBOW, BONE_R_HAND, sc);
                seg(dl, ped, BONE_PELVIS, BONE_L_KNEE, sc);
                seg(dl, ped, BONE_L_KNEE, BONE_L_FOOT, sc);
                seg(dl, ped, BONE_PELVIS, BONE_R_KNEE, sc);
                seg(dl, ped, BONE_R_KNEE, BONE_R_FOOT, sc);
            }
            if (esp_bones) {
                ImU32 bc = IM_COL32(60, 200, 255, 220);
                dot(dl, ped, BONE_HEAD,   bc);
                dot(dl, ped, BONE_NECK,   bc);
                dot(dl, ped, BONE_SPINE3, bc);
                dot(dl, ped, BONE_PELVIS, bc);
            }
        }

        if (esp_dist_to_cross && closest_to_cross >= 0) {
            float d = std::sqrt(closest_cross_d2);
            char buf[48];
            const char* name = N::GET_PLAYER_NAME(closest_to_cross);
            std::snprintf(buf, sizeof(buf), "%s  %.0fpx", name ? name : "?", d);
            dl->AddText(ImVec2(cx + 12, cy + 12), IM_COL32(255, 255, 255, 220), buf);
        }
    }
}
