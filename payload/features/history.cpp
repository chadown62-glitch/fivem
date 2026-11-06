// history.cpp -- teleport history + undo + coord-to-clipboard
#include "../invoker/natives.h"
#include "../core/notify.h"
#include <Windows.h>
#include <deque>
#include <mutex>
#include <cstdio>
#include <string>

namespace ollie::state {
    extern float teleport_coords[3];
}

namespace ollie::features {

    struct TP { float x, y, z; };
    static std::deque<TP> g_hist;
    static std::mutex g_m;

    void push_history(float x, float y, float z) {
        std::lock_guard<std::mutex> lk(g_m);
        g_hist.push_front({ x, y, z });
        if (g_hist.size() > 10) g_hist.pop_back();
    }

    int history_count() {
        std::lock_guard<std::mutex> lk(g_m);
        return (int)g_hist.size();
    }

    bool history_at(int i, float* x, float* y, float* z) {
        std::lock_guard<std::mutex> lk(g_m);
        if (i < 0 || i >= (int)g_hist.size()) return false;
        *x = g_hist[i].x; *y = g_hist[i].y; *z = g_hist[i].z;
        return true;
    }

    void tp_undo() {
        float x, y, z;
        if (!history_at(0, &x, &y, &z)) return;
        {
            std::lock_guard<std::mutex> lk(g_m);
            g_hist.pop_front();
        }
        ollie::invoker::run_on_script([x, y, z] {
            N::SET_ENTITY_COORDS_NO_OFFSET(N::PLAYER_PED_ID(), x, y, z, false, false, false);
        });
    }

    void copy_current_coords() {
        ollie::invoker::run_on_script([] {
            int ped = N::PLAYER_PED_ID();
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            char buf[96];
            std::snprintf(buf, sizeof(buf), "%.3f, %.3f, %.3f", x, y, z);

            // paint them into the world-tab teleport box too
            ollie::state::teleport_coords[0] = x;
            ollie::state::teleport_coords[1] = y;
            ollie::state::teleport_coords[2] = z;

            if (OpenClipboard(nullptr)) {
                EmptyClipboard();
                size_t len = std::strlen(buf) + 1;
                HGLOBAL g = GlobalAlloc(GMEM_MOVEABLE, len);
                if (g) {
                    void* p = GlobalLock(g);
                    if (p) { std::memcpy(p, buf, len); GlobalUnlock(g); }
                    SetClipboardData(CF_TEXT, g);
                }
                CloseClipboard();
            }
            ollie::notify::push(std::string("coords copied: ") + buf);
        });
    }

    // intercept teleport_to to record history (menu.cpp will call the recording wrapper)
    void teleport_record_to(float x, float y, float z) {
        ollie::invoker::run_on_script([x, y, z] {
            int ped = N::PLAYER_PED_ID();
            float cx = 0, cy = 0, cz = 0;
            N::GET_ENTITY_COORDS(ped, true, &cx, &cy, &cz);
            push_history(cx, cy, cz);
            N::SET_ENTITY_COORDS_NO_OFFSET(ped, x, y, z, false, false, false);
        });
    }
}
