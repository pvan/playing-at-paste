#include <windows.h>
#include <windowsx.h>  // GET_X_LPARAM
#include <stdio.h> // basically for sprintf


// common functions / general style stuff used for most things below
#include "types.h"
#include "utils.h"

// needed for renderer and also directx.cpp
#include "math/include.h"

// for now just for rendering 2d quads to screen
#include "directx.cpp"

// things the renderer depends on...
#include "memory.cpp"
#include "input.cpp"
#include "renderer/include.h"




d3d_textured_quad screen;
void create_quads()
{
    // u32 green = 0xff00ff00;
    // screen.create((u8*)&green,1,1, -1,-1,1,1,0);
    screen.create(0,400,400, -1,-1,1,1,0);
}
void destroy_quads()
{
    screen.destroy();
}


HWND g_hwnd;

bool running = true;


void render(float dt)
{
    if (!running) return;

    RECT winRect; GetWindowRect(g_hwnd, &winRect);
    int sw = winRect.right-winRect.left;
    int sh = winRect.bottom-winRect.top;

    if (!d3d_device_is_ok()) { // probably device lost from ctrl alt delete or something
        if (D3D_DEBUG_MSG) OutputDebugString("D3D NOT OK\n");
        if (d3d_device_can_be_reset()) {
            OutputDebugString("Resetting d3d entirely...\n");
            destroy_quads();
            d3d_cleanup();
            d3d_init(g_hwnd, sw, sh);
            create_quads();
        } else {
            if (D3D_DEBUG_MSG) OutputDebugString("D3D CANNOT BE RESET\n");
        }
    }

    d3d_resize_if_change(sw, sh, g_hwnd);


    screen.fill_tex_with_pattern(dt);


    d3d_clear(0, 0, 0);
    screen.render();
    d3d_swap();

}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CLOSE) running = false;
    if (uMsg == WM_NCHITTEST) {
        RECT win; if (!GetWindowRect(hwnd, &win)) return HTNOWHERE;
        POINT pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        POINT pad = { GetSystemMetrics(SM_CXFRAME), GetSystemMetrics(SM_CYFRAME) };
        bool left   = pos.x < win.left   + pad.x;
        bool right  = pos.x > win.right  - pad.x -1;  // win.right 1 pixel beyond window, right?
        bool top    = pos.y < win.top    + pad.y;
        bool bottom = pos.y > win.bottom - pad.y -1;
        if (top && left)     return HTTOPLEFT;
        if (top && right)    return HTTOPRIGHT;
        if (bottom && left)  return HTBOTTOMLEFT;
        if (bottom && right) return HTBOTTOMRIGHT;
        if (left)            return HTLEFT;
        if (right)           return HTRIGHT;
        if (top)             return HTTOP;
        if (bottom)          return HTBOTTOM;
        return HTCAPTION;
    }
    if (uMsg == WM_SIZE) {
        d3d_swap();
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "playing at paste";
    if (!RegisterClass(&wc)) { MessageBox(0, "RegisterClass failed", 0, 0); return 1; }

    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, "title",
        // WS_POPUP | WS_VISIBLE,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, 0, 0, hInstance, 0);
    if (!hwnd) { MessageBox(0, "CreateWindowEx failed", 0, 0); return 1; }

    g_hwnd = hwnd;


    assert(d3d_load());
    assert(d3d_init(g_hwnd, 400,400));


    create_quads();


    float prev;
    float now = GetWallClockSeconds() * 1000.0;
    while(running)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        prev = now;
        now = GetWallClockSeconds() * 1000.0;

        render(now - prev);

        Sleep(16);
    }

    d3d_cleanup();
    return 0;
}
