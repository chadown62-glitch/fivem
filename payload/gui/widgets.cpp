#include "widgets.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstring>
#include <algorithm>

extern ImU32 ollie_accent_breath();

namespace ollie::ui {

    ImU32 accent_breath() { return ollie_accent_breath(); }

    void window_shadow() {
        // soft outer glow drawn behind the window frame
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        ImVec2 p0 = ImGui::GetWindowPos();
        ImVec2 sz = ImGui::GetWindowSize();
        for (int i = 0; i < 12; ++i) {
            float g = (float)i;
            ImU32 c = IM_COL32(255, 60, 170, (int)(60.f * (1.f - g / 12.f)));
            dl->AddRect(ImVec2(p0.x - g, p0.y - g), ImVec2(p0.x + sz.x + g, p0.y + sz.y + g),
                        c, 18.f + g, 0, 1.0f);
        }
    }

    int sidebar_nav(float width, const char* const* labels, const char* const* icons, int count, int current) {
        ImGui::BeginChild("sidebar", ImVec2(width, 0), true,
                          ImGuiWindowFlags_NoScrollbar);
        auto dl = ImGui::GetWindowDrawList();
        ImVec2 start = ImGui::GetCursorScreenPos();

        // vertical accent stripe at left edge
        ImVec2 ws = ImGui::GetWindowSize();
        dl->AddRectFilled(start, ImVec2(start.x + 3.f, start.y + ws.y), accent_breath(), 2.f);
        ImGui::Dummy(ImVec2(0, 4));

        for (int i = 0; i < count; ++i) {
            ImGui::PushID(i);
            bool selected = (i == current);
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 size = ImVec2(width - 18.f, 40.f);

            bool hovered = ImGui::IsMouseHoveringRect(pos, ImVec2(pos.x + size.x, pos.y + size.y));
            bool clicked = ImGui::InvisibleButton("##row", size);

            ImU32 bg = IM_COL32(0, 0, 0, 0);
            if (selected) bg = IM_COL32(255, 60, 170, 70);
            else if (hovered) bg = IM_COL32(255, 60, 170, 40);
            dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bg, 10.f);

            if (selected) {
                dl->AddRectFilled(pos, ImVec2(pos.x + 3.f, pos.y + size.y), accent_breath(), 2.f);
            }

            // icon
            if (icons && icons[i]) {
                dl->AddText(ImVec2(pos.x + 14.f, pos.y + 11.f),
                            selected ? IM_COL32(255, 255, 255, 255) : IM_COL32(200, 200, 215, 255),
                            icons[i]);
            }
            // label
            dl->AddText(ImVec2(pos.x + 44.f, pos.y + 11.f),
                        selected ? IM_COL32(255, 255, 255, 255) : IM_COL32(210, 210, 225, 255),
                        labels[i]);

            if (clicked) current = i;
            ImGui::PopID();
            ImGui::Dummy(ImVec2(0, 2));
        }

        ImGui::EndChild();
        return current;
    }

    void hero(const char* title, const char* subtitle) {
        ImVec2 start = ImGui::GetCursorScreenPos();
        float w = ImGui::GetContentRegionAvail().x;
        float h = 72.f;
        auto dl = ImGui::GetWindowDrawList();

        // gradient bar
        ImU32 a = IM_COL32( 60, 20,  70, 255);
        ImU32 b = IM_COL32(120, 30, 110, 255);
        dl->AddRectFilledMultiColor(start, ImVec2(start.x + w, start.y + h), a, b, b, a);
        dl->AddRect(start, ImVec2(start.x + w, start.y + h),
                    accent_breath(), 12.f, 0, 1.2f);

        // thin breathing stripe at bottom
        dl->AddRectFilled(ImVec2(start.x + 1, start.y + h - 2),
                          ImVec2(start.x + w - 1, start.y + h),
                          accent_breath(), 12.f);

        dl->AddText(ImVec2(start.x + 18, start.y + 14), IM_COL32(255, 255, 255, 255), title);
        if (subtitle) {
            dl->AddText(ImVec2(start.x + 18, start.y + 40), IM_COL32(225, 225, 240, 230), subtitle);
        }

        ImGui::Dummy(ImVec2(w, h + 6));
    }

    bool begin_card(const char* id, float width, float height) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 10));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(26, 26, 36, 230));
        ImGui::PushStyleColor(ImGuiCol_Border,  IM_COL32(60, 40, 80, 255));
        return ImGui::BeginChild(id, ImVec2(width, height), true,
                                 ImGuiWindowFlags_NoScrollbar);
    }

    void end_card() {
        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(1);
    }

    void section(const char* label) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        auto dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(p, ImVec2(p.x + 3.f, p.y + 18.f), accent_breath(), 2.f);
        ImGui::Dummy(ImVec2(6, 0)); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0, 2));
    }

    bool toggle(const char* label, bool* v) {
        ImGuiStyle& style = ImGui::GetStyle();
        float h = ImGui::GetFrameHeight();
        float w = h * 2.0f;
        float r = h * 0.5f;

        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);

        ImGui::InvisibleButton(label, ImVec2(w + style.ItemInnerSpacing.x + label_size.x, h));
        bool clicked = ImGui::IsItemClicked();
        if (clicked) *v = !*v;

        auto dl = ImGui::GetWindowDrawList();
        ImU32 off = IM_COL32(60, 60, 80, 255);
        ImU32 on  = accent_breath();
        float t = *v ? 1.f : 0.f;
        ImU32 col = ImGui::IsItemHovered()
                      ? (*v ? on : IM_COL32(90, 90, 110, 255))
                      : (*v ? on : off);
        dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), col, r);
        float cx = *v ? (p.x + w - r) : (p.x + r);
        dl->AddCircleFilled(ImVec2(cx, p.y + r), r - 3.f, IM_COL32(255, 255, 255, 255));

        // label
        ImGui::SameLine();
        ImGui::SetCursorScreenPos(ImVec2(p.x + w + style.ItemInnerSpacing.x, p.y + (h - label_size.y) * 0.5f));
        ImGui::TextUnformatted(label);
        return clicked;
    }

    bool pill(const char* label, const ImVec2& size) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 999.f);
        bool r = ImGui::Button(label, size);
        ImGui::PopStyleVar();
        return r;
    }

    void chip(const char* text, ImU32 col) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 ts = ImGui::CalcTextSize(text);
        float padx = 8.f, pady = 4.f;
        float h = ts.y + pady * 2.f;
        float w = ts.x + padx * 2.f;
        auto dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), col, h * 0.5f);
        dl->AddText(ImVec2(p.x + padx, p.y + pady), IM_COL32(255, 255, 255, 255), text);
        ImGui::Dummy(ImVec2(w, h));
    }
}
