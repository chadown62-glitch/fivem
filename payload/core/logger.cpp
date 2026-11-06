#include "logger.h"
#include <Windows.h>
#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <string>

namespace ollie::log {
    static FILE* g_f = nullptr;
    static std::mutex g_m;
    static std::string g_tag = "ollie";

    void init(const char* tag) {
        g_tag = tag ? tag : "ollie";
        wchar_t temp[MAX_PATH]{}; GetTempPathW(MAX_PATH, temp);
        std::wstring p = std::wstring(temp) + L"ollie.log";
        _wfopen_s(&g_f, p.c_str(), L"w");
    }

    void line(const char* fmt, ...) {
        std::lock_guard<std::mutex> lk(g_m);
        if (!g_f) return;
        va_list ap; va_start(ap, fmt);
        std::fprintf(g_f, "[%s] ", g_tag.c_str());
        std::vfprintf(g_f, fmt, ap);
        std::fprintf(g_f, "\n");
        std::fflush(g_f);
        va_end(ap);
    }
}
