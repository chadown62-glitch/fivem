#pragma once
#include <string>
namespace ollie::notify {
    void push(const std::string& text);
    void render();

    int         log_count();
    const char* log_at(int i);
    void        clear_log();
}
