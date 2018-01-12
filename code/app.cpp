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
#include "renderer/include.h"

// need renderer
#include "input.cpp"


Scene scene;
Camera camera;
Renderer renderer;
input_state previnput;


d3d_textured_quad screenquad;
void create_quads()
{
    // u32 green = 0xff00ff00;
    // screen.create((u8*)&green,1,1, -1,-1,1,1,0);
    screenquad.create(0,400,400, -1,-1,1,1,0);
}
void destroy_quads()
{
    screenquad.destroy();
}

bitmap iso;
bitmap top;
bitmap side;
bitmap front;
bitmap screen;


HWND g_hwnd;

bool running = true;


void render(float dt)
{
    if (!running) return;

    // BOOKKEEPING

    RECT winRect; GetClientRect(g_hwnd, &winRect);
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


    // UPDATE

    input_state input = input_poll_current_state(g_hwnd);

    bool walkMode = false;
    if (walkMode) {
        updateCameraWithFPSControls(&camera, &input, dt);
    } else {
        updateCameraWithCADControls(&camera, &scene, &input, dt);
    }


    renderer.fill(&iso, 0); // for now clear here, should draw_to do it though?
    renderer.draw_to(&scene, &camera, &iso);
    updateBitmapWithPaintControls(&renderer, &top,   {0,0}      , &input, &previnput, dt);
    updateBitmapWithPaintControls(&renderer, &side,  {sw/2.0f,0}, &input, &previnput, dt);
    updateBitmapWithPaintControls(&renderer, &front, {0,sh/2.0f}, &input, &previnput, dt);

    renderer.fill(&screen, 0); // for now clear here
    renderer.copy_to(&top,   &screen, {0,0}            , {-1,-1,-1,-1}, 1);
    renderer.copy_to(&side,  &screen, {sw/2.0f,0}      , {-1,-1,-1,-1}, 1);
    renderer.copy_to(&front, &screen, {0,sh/2.0f}      , {-1,-1,-1,-1}, 1);
    renderer.copy_to(&iso,   &screen, {sw/2.0f,sh/2.0f}, {-1,-1,-1,-1}, 1);


    screenquad.fill_tex_with_mem((u8*)screen.pixels, screen.width, screen.height);

    previnput = input;


    // RENDER

    d3d_clear(0, 0, 0);
    screenquad.render();
    // top.render();
    // right.render();
    // front.render();
    d3d_swap();

}

void init(int w, int h)
{

    assert(d3d_load());
    assert(d3d_init(g_hwnd, w,h));


    create_quads();


    memory_block app_memory;
    app_memory.create(MEGABYTES(512), (void*)TERABYTES(2));


    renderer.init_with_backend(&app_memory, w, h, SOFTWARE);

    scene.init(&app_memory);
    scene.add(game_object::Make(&app_memory, CUBE, v3{0,0,10}, 1));

    camera.Init(v3{0,0,0}, 0, 0, 1, 500, 90, w,h);



    top.allocate(w/2, h/2, &app_memory);
    side.allocate(w/2, h/2, &app_memory);
    front.allocate(w/2, h/2, &app_memory);
    iso.allocate(w/2, h/2, &app_memory);
    screen.allocate(w, h, &app_memory);

    previnput = input_poll_current_state(g_hwnd);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CLOSE) running = false;
    if (uMsg == WM_SIZE) {
        d3d_swap();
    }
    input_read_win_msg(hwnd, uMsg, wParam, lParam);
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

    RECT startrect = {0,0,400,400};
    AdjustWindowRectEx(&startrect, WS_OVERLAPPEDWINDOW, 0, 0);

    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, "title",
        // WS_POPUP | WS_VISIBLE,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        startrect.right-startrect.left, startrect.bottom-startrect.top,
        0, 0, hInstance, 0);
    if (!hwnd) { MessageBox(0, "CreateWindowEx failed", 0, 0); return 1; }

    g_hwnd = hwnd;


    init(400,400);


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
