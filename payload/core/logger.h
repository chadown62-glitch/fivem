#pragma once
#include <string>
namespace ollie::log {
    void init(const char* tag);
    void line(const char* fmt, ...);
}
