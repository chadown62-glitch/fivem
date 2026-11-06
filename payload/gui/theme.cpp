// theme.cpp -- neon-pink glass palette with optional rainbow breath
#include "imgui.h"
#include <cmath>

namespace ollie::state {
    extern bool accent_rainbow;
    extern int  accent_r, accent_g, accent_b;
}

static ImVec4 col(int r, int g, int b, int a = 255) {
    return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

void ollie_theme() {
    auto& s = ImGui::GetStyle();

    s.WindowRounding    = 14.f;
    s.ChildRounding     = 10.f;
    s.FrameRounding     = 8.f;
    s.GrabRounding      = 10.f;
    s.PopupRounding     = 10.f;
    s.ScrollbarRounding = 10.f;
    s.TabRounding       = 8.f;

    s.WindowPadding     = ImVec2(16, 16);
    s.FramePadding      = ImVec2(10, 7);
    s.ItemSpacing       = ImVec2(10, 8);
    s.ItemInnerSpacing  = ImVec2(8, 6);
    s.IndentSpacing     = 18.f;
    s.ScrollbarSize     = 12.f;

    s.WindowBorderSize  = 1.0f;
    s.FrameBorderSize   = 0.0f;
    s.ChildBorderSize   = 1.0f;
    s.TabBorderSize     = 0.0f;

    s.WindowTitleAlign  = ImVec2(0.5f, 0.5f);
    s.ButtonTextAlign   = ImVec2(0.5f, 0.5f);
    s.GrabMinSize       = 14.f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_Text]                 = col(242, 243, 247);
    c[ImGuiCol_TextDisabled]         = col(120, 120, 135);
    c[ImGuiCol_WindowBg]             = col(14, 14, 20, 245);
    c[ImGuiCol_ChildBg]              = col(20, 20, 28, 220);
    c[ImGuiCol_PopupBg]              = col(18, 18, 26, 240);
    c[ImGuiCol_Border]               = col(255, 60, 170, 160);
    c[ImGuiCol_BorderShadow]         = col(0, 0, 0, 0);

    c[ImGuiCol_FrameBg]              = col(28, 28, 38, 255);
    c[ImGuiCol_FrameBgHovered]       = col(44, 30, 55, 255);
    c[ImGuiCol_FrameBgActive]        = col(66, 36, 86, 255);

    c[ImGuiCol_TitleBg]              = col(14, 14, 20, 255);
    c[ImGuiCol_TitleBgActive]        = col(14, 14, 20, 255);
    c[ImGuiCol_TitleBgCollapsed]     = col(14, 14, 20, 180);

    c[ImGuiCol_MenuBarBg]            = col(18, 18, 26, 255);
    c[ImGuiCol_ScrollbarBg]          = col(20, 20, 28, 0);
    c[ImGuiCol_ScrollbarGrab]        = col(52, 52, 66, 255);
    c[ImGuiCol_ScrollbarGrabHovered] = col(255, 60, 170, 200);
    c[ImGuiCol_ScrollbarGrabActive]  = col(255, 90, 200, 255);

    c[ImGuiCol_CheckMark]            = col(255, 90, 200, 255);
    c[ImGuiCol_SliderGrab]           = col(255, 90, 200, 255);
    c[ImGuiCol_SliderGrabActive]     = col(255, 140, 220, 255);

    c[ImGuiCol_Button]               = col(38, 30, 52, 255);
    c[ImGuiCol_ButtonHovered]        = col(255, 60, 170, 230);
    c[ImGuiCol_ButtonActive]         = col(255, 90, 200, 255);

    c[ImGuiCol_Header]               = col(44, 30, 55, 255);
    c[ImGuiCol_HeaderHovered]        = col(255, 60, 170, 180);
    c[ImGuiCol_HeaderActive]         = col(255, 90, 200, 220);

    c[ImGuiCol_Separator]            = col(70, 40, 92, 255);
    c[ImGuiCol_SeparatorHovered]     = col(255, 60, 170, 255);
    c[ImGuiCol_SeparatorActive]      = col(255, 90, 200, 255);

    c[ImGuiCol_ResizeGrip]           = col(255, 60, 170, 80);
    c[ImGuiCol_ResizeGripHovered]    = col(255, 60, 170, 160);
    c[ImGuiCol_ResizeGripActive]     = col(255, 90, 200, 220);

    c[ImGuiCol_Tab]                  = col(28, 28, 38, 255);
    c[ImGuiCol_TabHovered]           = col(255, 60, 170, 230);
    c[ImGuiCol_TabActive]            = col(255, 90, 200, 255);
    c[ImGuiCol_TabUnfocused]         = col(28, 28, 38, 255);
    c[ImGuiCol_TabUnfocusedActive]   = col(44, 30, 55, 255);

    c[ImGuiCol_NavHighlight]         = col(255, 90, 200, 255);
    c[ImGuiCol_NavWindowingHighlight]= col(255, 90, 200, 180);
    c[ImGuiCol_NavWindowingDimBg]    = col(0, 0, 0, 100);
    c[ImGuiCol_ModalWindowDimBg]     = col(0, 0, 0, 160);

    c[ImGuiCol_PlotLines]            = col(255, 90, 200, 255);
    c[ImGuiCol_PlotLinesHovered]     = col(255, 140, 220, 255);
    c[ImGuiCol_PlotHistogram]        = col(255, 90, 200, 255);
    c[ImGuiCol_PlotHistogramHovered] = col(255, 140, 220, 255);

    c[ImGuiCol_TableHeaderBg]        = col(28, 28, 38, 255);
    c[ImGuiCol_TableBorderStrong]    = col(70, 40, 92, 255);
    c[ImGuiCol_TableBorderLight]     = col(40, 30, 55, 255);
    c[ImGuiCol_TableRowBg]           = col(0, 0, 0, 0);
    c[ImGuiCol_TableRowBgAlt]        = col(255, 255, 255, 8);
}

static void hsv2rgb(float h, float s, float v, int& r, int& g, int& b) {
    float c = v * s;
    float x = c * (1.f - std::fabs(std::fmod(h / 60.f, 2.f) - 1.f));
    float m = v - c;
    float rf = 0, gf = 0, bf = 0;
    if      (h <  60) { rf = c; gf = x; }
    else if (h < 120) { rf = x; gf = c; }
    else if (h < 180) { gf = c; bf = x; }
    else if (h < 240) { gf = x; bf = c; }
    else if (h < 300) { rf = x; bf = c; }
    else              { rf = c; bf = x; }
    r = (int)((rf + m) * 255);
    g = (int)((gf + m) * 255);
    b = (int)((bf + m) * 255);
}

ImU32 ollie_accent_breath() {
    if (ollie::state::accent_rainbow) {
        float t = (float)ImGui::GetTime();
        int r, g, b;
        hsv2rgb(std::fmod(t * 40.f, 360.f), 0.9f, 1.f, r, g, b);
        ollie::state::accent_r = r;
        ollie::state::accent_g = g;
        ollie::state::accent_b = b;
        return IM_COL32(r, g, b, 255);
    }
    int r = ollie::state::accent_r;
    int g = ollie::state::accent_g;
    int b = ollie::state::accent_b;
    float t = (float)ImGui::GetTime();
    float s = 0.5f + 0.5f * std::sin(t * 1.6f);
    r = (int)(r * (0.85f + 0.15f * s));
    g = (int)(g * (0.85f + 0.15f * s));
    b = (int)(b * (0.85f + 0.15f * s));
    return IM_COL32(r, g, b, 255);
}
