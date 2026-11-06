#pragma once
#include "imgui.h"
#include <string>

namespace ollie::ui {
    // breathing accent colour, driven from theme.cpp
    ImU32 accent_breath();

    // drop shadow rect under the window-level content (call once before any widget)
    void window_shadow();

    // left sidebar nav. returns the picked index. `icons` and `labels` same count.
    int  sidebar_nav(float width, const char* const* labels, const char* const* icons, int count, int current);

    // hero strip that lives at the top of the content column
    void hero(const char* title, const char* subtitle);

    // card frame -- call Begin / End. Begin returns true to render.
    bool begin_card(const char* id, float width = 0.f, float height = 0.f);
    void end_card();

    // section title with accent chip
    void section(const char* label);

    // iOS-style toggle switch. returns true on flip.
    bool toggle(const char* label, bool* v);

    // pill button
    bool pill(const char* label, const ImVec2& size = ImVec2(0, 0));

    // small tag chip (status / info)
    void chip(const char* text, ImU32 col);
}
