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

// Canvas iso_canv;
// Canvas top_canv;
// Canvas side_canv;
// Canvas front_canv;
bitmap iso;
bitmap top;
bitmap side;
bitmap front;


HWND g_hwnd;

bool running = true;


void render(float dt)
{
    if (!running) return;

    // BOOKKEEPING

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


    // UPDATE

    input_state input = input_poll_current_state(g_hwnd);

    bool walkMode = false;
    if (walkMode)
    {
        // camera.updateWithFPSControls(&input, dt);
        updateCameraWithFPSControls(&camera, &input, dt);
    }
    else
    {
        updateCameraWithCADControls(&camera, &scene, &input, dt);

    }


    // bitmap output;
    // output.allocate_with_malloc(sw, sh);

    // top_canv.updateWithPaintControls(&renderer, &input, dt);
    // side_canv.updateWithPaintControls(&renderer, &input, dt);
    // front_canv.updateWithPaintControls(&renderer, &input, dt);
    renderer.fill(&iso, 0); // for now clear here, should draw_to do it though?
    renderer.draw_to(&scene, &camera, &iso);

    // top_canv.render_to(&renderer, &output);
    // side_canv.render_to(&renderer, &output);
    // front_canv.render_to(&renderer, &output);
    // iso_canv.render_to(&renderer, &output);

    // screen.fill_tex_with_pattern(dt);
    screen.fill_tex_with_mem((u8*)iso.pixels, iso.width, iso.height);


    // RENDER

    d3d_clear(0, 0, 0);
    screen.render();
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
        // renderer.fill(&top, 0xffc0ffee);
        // renderer.fill(&front, 0xfffacade);
        // renderer.fill(&side, 0xffdecade);
        // renderer.fill(iso, 0xfffabace);

    // top_canv.init(v2{0,0}, &top);
    // side_canv.init(v2{0,h/2.0f}, &side);
    // front_canv.init(v2{w/2.0f,0}, &front);
    // iso_canv.init(v2{w/2.0f,h/2.0f}, &iso);
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

    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, "title",
        // WS_POPUP | WS_VISIBLE,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, 0, 0, hInstance, 0);
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
