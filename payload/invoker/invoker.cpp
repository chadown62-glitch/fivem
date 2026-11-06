// Invoker — binds to scripthookv.dll's native API (nativeInit/nativePush64/nativeCall).
// scripthookv.dll is bundled by FiveM at the .app root, loaded in-process, and
// exposes a stable set of ordinals/mangled names since Alexander Blade's release.
// This bypasses the GTA5 native-registration-table sig scan entirely.

#include "invoker.h"
#include "../core/pointers.h"
#include "../core/logger.h"
#include <Windows.h>
#include <atomic>

namespace ollie::invoker {

    using fn_nativeInit    = void(*)(std::uint64_t hash);
    using fn_nativePush64  = void(*)(std::uint64_t arg);
    using fn_nativeCall    = std::uint64_t*(*)();

    static HMODULE          g_shv = nullptr;
    static fn_nativeInit    p_nativeInit   = nullptr;
    static fn_nativePush64  p_nativePush64 = nullptr;
    static fn_nativeCall    p_nativeCall   = nullptr;

    // Current-call state, held across the fluent arg stacking before call<R>().
    static thread_local std::uint64_t g_last_return[3] = {};

    bool init() {
        // --- FiveM-aware scripthookv acquisition ---
        // FiveM auto-loads scripthookv.dll from its .app folder ONLY when it exists
        // there AND the user has single-player mode ("FXDK"/"story") OR the server
        // allows it. On many public servers FiveM strips scripthookv on boot.
        // Try, in order:
        //   1. Already in-process (FiveM loaded it at boot)
        //   2. Load from %LOCALAPPDATA%\FiveM\FiveM.app\scripthookv.dll (standard path)
        //   3. Load from the FiveM process root (next to FiveM_GTAProcess.exe)
        //   4. Bare LoadLibrary (search current dir + PATH)
        g_shv = GetModuleHandleA("scripthookv.dll");
        if (g_shv) log::line("invoker: scripthookv already loaded in-process @ %p", (void*)g_shv);
        if (!g_shv) {
            g_shv = GetModuleHandleA("ScriptHookV.dll");
            if (g_shv) log::line("invoker: ScriptHookV (capital) already in-process @ %p", (void*)g_shv);
        }

        if (!g_shv) {
            // #2 FiveM.app absolute path
            char local[MAX_PATH]{};
            if (GetEnvironmentVariableA("LOCALAPPDATA", local, MAX_PATH)) {
                char p[MAX_PATH]{};
                _snprintf_s(p, sizeof(p), _TRUNCATE,
                            "%s\\FiveM\\FiveM.app\\scripthookv.dll", local);
                g_shv = LoadLibraryA(p);
                if (g_shv) log::line("invoker: loaded scripthookv from %s", p);
                else      log::line("invoker: LoadLibrary(%s) failed err=%lu", p, GetLastError());
            }
        }
        if (!g_shv) {
            // #3 beside FiveM_GTAProcess.exe
            char exe[MAX_PATH]{};
            if (GetModuleFileNameA(nullptr, exe, MAX_PATH)) {
                char* slash = strrchr(exe, '\\');
                if (slash) {
                    *(slash + 1) = 0;
                    char p[MAX_PATH]{};
                    _snprintf_s(p, sizeof(p), _TRUNCATE, "%sscripthookv.dll", exe);
                    g_shv = LoadLibraryA(p);
                    if (g_shv) log::line("invoker: loaded scripthookv from %s", p);
                    else      log::line("invoker: LoadLibrary(%s) failed err=%lu", p, GetLastError());
                }
            }
        }
        if (!g_shv) {
            g_shv = LoadLibraryA("scripthookv.dll");
            if (g_shv) log::line("invoker: loaded scripthookv via PATH");
            else       log::line("invoker: LoadLibrary(scripthookv.dll) failed err=%lu", GetLastError());
        }
        if (!g_shv) {
            log::line("invoker: scripthookv.dll not in process -- natives disabled");
            return true; // keep menu running
        }
        // Mangled names from Alexander Blade's ScriptHookV public ABI (stable since 2015).
        p_nativeInit   = (fn_nativeInit)  GetProcAddress(g_shv, "?nativeInit@@YAX_K@Z");
        p_nativePush64 = (fn_nativePush64)GetProcAddress(g_shv, "?nativePush64@@YAX_K@Z");
        p_nativeCall   = (fn_nativeCall)  GetProcAddress(g_shv, "?nativeCall@@YAPEA_KXZ");

        log::line("invoker: shv=%p init=%p push=%p call=%p",
                  (void*)g_shv, (void*)p_nativeInit, (void*)p_nativePush64, (void*)p_nativeCall);

        if (!p_nativeInit || !p_nativePush64 || !p_nativeCall) {
            log::line("invoker: scripthookv exports missing -- natives disabled");
            return true;
        }
        log::line("invoker: wired to scripthookv.dll -- natives LIVE");
        return true;
    }

    bool natives_ready() {
        return p_nativeInit != nullptr && p_nativePush64 != nullptr && p_nativeCall != nullptr;
    }

    void shutdown() {
        if (g_shv) FreeLibrary(g_shv);
        g_shv = nullptr;
        p_nativeInit = nullptr;
        p_nativePush64 = nullptr;
        p_nativeCall = nullptr;
    }

    // Legacy stub: no handler-table in this path. Return a sentinel so Call::call()
    // falls through to our scripthookv path (see below).
    NativeHandler get(std::uint64_t) {
        if (!p_nativeInit) return nullptr;
        // non-null sentinel so Call::call sees "we can invoke"
        return reinterpret_cast<NativeHandler>(1);
    }

    // scripthookv-based invoke: hash + raw 64-bit args already stacked by Call::a<T>.
    std::uint64_t* invoke_via_shv(std::uint64_t hash, const std::uint64_t* args, std::uint32_t argc) {
        if (!p_nativeInit) return g_last_return;
        p_nativeInit(hash);
        for (std::uint32_t i = 0; i < argc; ++i) p_nativePush64(args[i]);
        std::uint64_t* r = p_nativeCall();
        if (r) {
            g_last_return[0] = r[0];
            g_last_return[1] = r[1];
            g_last_return[2] = r[2];
        }
        return g_last_return;
    }
}
