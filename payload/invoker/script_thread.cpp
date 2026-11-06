// script_thread.cpp
// Natives must land on the game's main thread. Present hook pumps this queue.
#include "invoker.h"
#include "../core/logger.h"
#include <deque>
#include <functional>
#include <mutex>

namespace ollie::invoker {
    static std::deque<std::function<void()>> g_q;
    static std::mutex g_qm;

    void run_on_script(std::function<void()> f) {
        std::lock_guard<std::mutex> lk(g_qm);
        if (g_q.size() > 2048) return; // runaway guard
        g_q.push_back(std::move(f));
    }

    void pump() {
        std::lock_guard<std::mutex> lk(g_qm);
        while (!g_q.empty()) {
            auto f = std::move(g_q.front()); g_q.pop_front();
            try { f(); } catch (...) {}
        }
    }
}
