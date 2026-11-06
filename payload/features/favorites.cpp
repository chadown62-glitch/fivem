// favorites.cpp -- named coord list saved to config
#include "../invoker/natives.h"
#include "../core/notify.h"
#include <vector>
#include <string>
#include <mutex>

namespace ollie::favs {
    struct Fav { std::string name; float x, y, z; };
    static std::vector<Fav> g_list;
    static std::mutex g_m;

    void add(const std::string& name, float x, float y, float z) {
        std::lock_guard<std::mutex> lk(g_m);
        if (g_list.size() >= 64) return;
        g_list.push_back({ name, x, y, z });
    }
    void remove(int i) {
        std::lock_guard<std::mutex> lk(g_m);
        if (i < 0 || i >= (int)g_list.size()) return;
        g_list.erase(g_list.begin() + i);
    }
    int  count() { std::lock_guard<std::mutex> lk(g_m); return (int)g_list.size(); }
    bool get(int i, std::string& name, float* x, float* y, float* z) {
        std::lock_guard<std::mutex> lk(g_m);
        if (i < 0 || i >= (int)g_list.size()) return false;
        name = g_list[i].name;
        *x = g_list[i].x; *y = g_list[i].y; *z = g_list[i].z;
        return true;
    }
    void clear() { std::lock_guard<std::mutex> lk(g_m); g_list.clear(); }

    // for config serialization
    void serialize(std::string& out) {
        std::lock_guard<std::mutex> lk(g_m);
        out.clear();
        for (auto& f : g_list) {
            out += f.name; out += "|";
            char buf[64]; std::snprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", f.x, f.y, f.z);
            out += buf;
            out += ";";
        }
    }
    void deserialize(const std::string& s) {
        std::lock_guard<std::mutex> lk(g_m);
        g_list.clear();
        size_t i = 0;
        while (i < s.size()) {
            size_t semi = s.find(';', i);
            if (semi == std::string::npos) break;
            auto entry = s.substr(i, semi - i);
            i = semi + 1;
            size_t bar = entry.find('|');
            if (bar == std::string::npos) continue;
            std::string nm = entry.substr(0, bar);
            std::string tail = entry.substr(bar + 1);
            float x = 0, y = 0, z = 0;
            if (sscanf_s(tail.c_str(), "%f,%f,%f", &x, &y, &z) == 3) {
                g_list.push_back({ nm, x, y, z });
            }
        }
    }
}

namespace ollie::features {
    void fav_add_here(const char* name) {
        ollie::invoker::run_on_script([n = std::string(name)] {
            int ped = N::PLAYER_PED_ID();
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            ollie::favs::add(n.empty() ? "unnamed" : n, x, y, z);
            ollie::notify::push(std::string("fav + ") + (n.empty() ? "unnamed" : n));
        });
    }
    void fav_tp(int i) {
        std::string nm; float x, y, z;
        if (!ollie::favs::get(i, nm, &x, &y, &z)) return;
        ollie::invoker::run_on_script([x, y, z] {
            N::SET_ENTITY_COORDS_NO_OFFSET(N::PLAYER_PED_ID(), x, y, z, false, false, false);
        });
    }
}
