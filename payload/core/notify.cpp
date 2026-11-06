#include "notify.h"
#include "imgui.h"
#include <chrono>
#include <deque>
#include <mutex>
#include <vector>
#include <string>

namespace ollie::notify {
    struct Toast { std::string text; std::chrono::steady_clock::time_point until; };
    static std::deque<Toast> g_q;
    static std::mutex g_m;

    static std::vector<std::string> g_log;        // persistent feed
    static std::mutex g_log_m;

    void push(const std::string& text) {
        {
            std::lock_guard<std::mutex> lk(g_m);
            g_q.push_back({ text, std::chrono::steady_clock::now() + std::chrono::seconds(3) });
            if (g_q.size() > 6) g_q.pop_front();
        }
        {
            std::lock_guard<std::mutex> lk(g_log_m);
            g_log.push_back(text);
            if (g_log.size() > 200) g_log.erase(g_log.begin());
        }
    }

    int  log_count() {
        std::lock_guard<std::mutex> lk(g_log_m);
        return (int)g_log.size();
    }
    const char* log_at(int i) {
        std::lock_guard<std::mutex> lk(g_log_m);
        if (i < 0 || i >= (int)g_log.size()) return "";
        return g_log[i].c_str();
    }
    void clear_log() {
        std::lock_guard<std::mutex> lk(g_log_m);
        g_log.clear();
    }

    void render() {
        std::lock_guard<std::mutex> lk(g_m);
        auto now = std::chrono::steady_clock::now();
        while (!g_q.empty() && g_q.front().until < now) g_q.pop_front();
        if (g_q.empty()) return;

        auto& io = ImGui::GetIO();
        float w = 320.f;
        float x = io.DisplaySize.x - w - 20.f;
        float y = 20.f;
        auto dl = ImGui::GetForegroundDrawList();
        for (auto& t : g_q) {
            ImVec2 tl(x, y), br(x + w, y + 32.f);
            dl->AddRectFilled(tl, br, IM_COL32(16, 16, 20, 220), 4.f);
            dl->AddRect(tl, br, IM_COL32(255, 60, 160, 255), 4.f, 0, 1.2f);
            dl->AddText(ImVec2(x + 10, y + 8), IM_COL32(255, 255, 255, 255), t.text.c_str());
            y += 40.f;
        }
    }
}
