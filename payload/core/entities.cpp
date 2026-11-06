#include "entities.h"
#include "../invoker/natives.h"
#include <mutex>

namespace ollie::entities {
    static std::vector<int> g_list;
    static std::mutex g_m;

    void track(int handle) {
        if (!handle) return;
        std::lock_guard<std::mutex> lk(g_m);
        g_list.push_back(handle);
    }
    int count() {
        std::lock_guard<std::mutex> lk(g_m);
        return (int)g_list.size();
    }
    void delete_all() {
        std::vector<int> copy;
        {
            std::lock_guard<std::mutex> lk(g_m);
            copy.swap(g_list);
        }
        ollie::invoker::run_on_script([copy = std::move(copy)]() mutable {
            for (int h : copy) {
                if (!h) continue;
                int local = h;
                N::DELETE_ENTITY(&local);
            }
        });
    }
}
