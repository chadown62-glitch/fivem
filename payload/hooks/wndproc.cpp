#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "../gui/menu.h"
#include "hooking.h"
#include <Windows.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK ollie_wndproc(HWND h, UINT msg, WPARAM w, LPARAM l, WNDPROC original) {
    if (msg == WM_KEYDOWN && w == VK_INSERT) {
        ollie::menu::toggle();
        return 0;
    }
    if (msg == WM_KEYDOWN && w == VK_END) {
        ollie::hooking::request_unload();
    }
    if (ollie::menu::visible()) {
        ImGui_ImplWin32_WndProcHandler(h, msg, w, l);
        if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MOUSEMOVE ||
            msg == WM_KEYDOWN     || msg == WM_KEYUP       || msg == WM_CHAR ||
            msg == WM_LBUTTONUP   || msg == WM_RBUTTONUP   || msg == WM_MOUSEWHEEL) {
            return 1;
        }
    }
    return CallWindowProcW(original, h, msg, w, l);
}

bool install_wndproc() { return true; }
