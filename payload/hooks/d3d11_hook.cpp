// Hooks IDXGISwapChain::Present (vtbl index 8) via a one-shot dummy device.
// Drives ImGui + script queue pump.
#include "../core/logger.h"
#include "MinHook.h"
#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "../gui/menu.h"
#include "hooking.h"
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <atomic>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace ollie::invoker { void pump(); }

using Present_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*, UINT, UINT);
static Present_t oPresent = nullptr;

static bool g_init = false;
static ID3D11Device* g_device = nullptr;
static ID3D11DeviceContext* g_ctx = nullptr;
static ID3D11RenderTargetView* g_rtv = nullptr;
static HWND g_hwnd = nullptr;
static WNDPROC g_orig_wnd = nullptr;

extern LRESULT CALLBACK ollie_wndproc(HWND, UINT, WPARAM, LPARAM, WNDPROC original);
extern void ollie_theme();

static LRESULT CALLBACK wndproc_proxy(HWND h, UINT msg, WPARAM w, LPARAM l) {
    return ollie_wndproc(h, msg, w, l, g_orig_wnd);
}

static void init_imgui(IDXGISwapChain* sc) {
    if (g_init) return;
    if (FAILED(sc->GetDevice(__uuidof(ID3D11Device), (void**)&g_device))) return;
    g_device->GetImmediateContext(&g_ctx);
    DXGI_SWAP_CHAIN_DESC sd{}; sc->GetDesc(&sd);
    g_hwnd = sd.OutputWindow;

    ID3D11Texture2D* back = nullptr;
    sc->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back);
    if (back) {
        g_device->CreateRenderTargetView(back, nullptr, &g_rtv);
        back->Release();
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImGui_ImplWin32_Init(g_hwnd);
    ImGui_ImplDX11_Init(g_device, g_ctx);
    ollie_theme();

    g_orig_wnd = (WNDPROC)SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC, (LONG_PTR)wndproc_proxy);
    g_init = true;
    ollie::log::line("imgui live");
}

static HRESULT STDMETHODCALLTYPE hkPresent(IDXGISwapChain* sc, UINT si, UINT flags) {
    init_imgui(sc);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    // Feed mouse state before NewFrame so this frame's clicks register.
    // FiveM swallows WM messages; polling GetCursorPos + GetAsyncKeyState is reliable.
    {
        auto& io = ImGui::GetIO();
        if (ollie::menu::visible()) {
            io.MouseDrawCursor = true;
            ClipCursor(nullptr);
            while (ShowCursor(TRUE) < 0) {}

            POINT p{};
            if (GetCursorPos(&p) && g_hwnd) {
                ScreenToClient(g_hwnd, &p);
                io.AddMousePosEvent((float)p.x, (float)p.y);
            }
            io.AddMouseButtonEvent(0, (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0);
            io.AddMouseButtonEvent(1, (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0);
            io.AddMouseButtonEvent(2, (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0);
        } else {
            io.MouseDrawCursor = false;
            // release imgui's grip so game gets the pointer back
            io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
            io.AddMouseButtonEvent(0, false);
            io.AddMouseButtonEvent(1, false);
            io.AddMouseButtonEvent(2, false);
        }
    }

    ImGui::NewFrame();

    ollie::menu::render();
    ollie::invoker::pump();

    ImGui::Render();
    if (g_rtv) g_ctx->OMSetRenderTargets(1, &g_rtv, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(sc, si, flags);
}

static void* get_present_from_dummy() {
    WNDCLASSEXW wc{ sizeof(wc), CS_HREDRAW | CS_VREDRAW, DefWindowProcW, 0, 0,
                   GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr, L"ollie_dummy", nullptr };
    RegisterClassExW(&wc);
    HWND hw = CreateWindowW(L"ollie_dummy", L"x", WS_OVERLAPPEDWINDOW, 0, 0, 2, 2, nullptr, nullptr, wc.hInstance, nullptr);

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hw;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL fl[] = { D3D_FEATURE_LEVEL_11_0 };
    IDXGISwapChain* sc = nullptr; ID3D11Device* dev = nullptr; ID3D11DeviceContext* ctx = nullptr;
    D3D_FEATURE_LEVEL got;
    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, fl, 1,
        D3D11_SDK_VERSION, &sd, &sc, &dev, &got, &ctx))) {
        DestroyWindow(hw); UnregisterClassW(L"ollie_dummy", wc.hInstance);
        return nullptr;
    }

    void** vtbl = *(void***)sc;
    void* present = vtbl[8];
    if (sc)  sc->Release();
    if (ctx) ctx->Release();
    if (dev) dev->Release();
    DestroyWindow(hw); UnregisterClassW(L"ollie_dummy", wc.hInstance);
    return present;
}

bool install_d3d11() {
    void* target = get_present_from_dummy();
    if (!target) return false;
    if (MH_CreateHook(target, &hkPresent, (void**)&oPresent) != MH_OK) return false;
    if (MH_EnableHook(target) != MH_OK) return false;
    ollie::log::line("Present hooked @ %p", target);
    return true;
}
