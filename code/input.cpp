

struct input_state
{
    bool w;
    bool a;
    bool s;
    bool d;
    bool up;
    bool down;
    bool left;
    bool right;

    bool space;
    bool shift;
    bool ctrl;

    bool tab;

    bool escape;

    bool z;
    bool x;
    bool c;

    bool q;
    bool e;
    bool r;
    bool t;
    bool y;

    bool row[12];

    bool n1;
    bool n2;
    bool n3;
    bool n4;
    bool n5;
    bool n6;
    bool n7;
    bool n8;
    bool n9;
    bool n0;
    bool nMinus;
    bool nEquals;


    bool mouseL;
    bool mouseM;
    bool mouseR;
    float mouseX;
    float mouseY;

    float deltaMouseX;
    float deltaMouseY;

    int mouseWheelDelta;

    // bool lockMouseToCenterThisFrame;
    bool allowOSToMoveMouseBeforeNextFrame;

    bool tellOSToHideCursor;


    bool squareL;
    bool squareR;

    bool carrotL;
    bool carrotR;
    bool questionMark;

    bool m;

};



int input_mousewheel_last_delta = 0;  // mousewheel from last wndproc


// call from wndproc, doesn't change return value atm but needed for reading some input
void input_read_win_msg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_MOUSEWHEEL) {
        input_mousewheel_last_delta = GET_WHEEL_DELTA_WPARAM(wParam); // 1 notch = 120 +up -down
    }
}


static POINT input_lastMousePoint = {-1,-1};

// could miss a keypress here if it's faster than the framerate
static input_state input_poll_current_state(HWND window)
{
    // input_state result;
    // memset(&result, 0, sizeof(input_state));

    input_state result = {};  // todo: vs {0} ?

    if (GetAsyncKeyState('W'))      result.w = true;
    if (GetAsyncKeyState('A'))      result.a = true;
    if (GetAsyncKeyState('S'))      result.s = true;
    if (GetAsyncKeyState('D'))      result.d = true;
    if (GetAsyncKeyState(VK_UP))    result.up = true;
    if (GetAsyncKeyState(VK_DOWN))  result.down = true;
    if (GetAsyncKeyState(VK_LEFT))  result.left = true;
    if (GetAsyncKeyState(VK_RIGHT)) result.right = true;

    if (GetAsyncKeyState(VK_SPACE))   result.space = true;
    if (GetAsyncKeyState(VK_SHIFT))   result.shift = true;
    if (GetAsyncKeyState(VK_CONTROL)) result.ctrl = true;

    if (GetAsyncKeyState(VK_TAB))     result.tab = true;
    if (GetAsyncKeyState(VK_ESCAPE))  result.escape = true;

    if (GetAsyncKeyState('Z'))      result.z = true;
    if (GetAsyncKeyState('X'))      result.x = true;
    if (GetAsyncKeyState('C'))      result.c = true;

    if (GetAsyncKeyState('Q'))      result.q = true;
    if (GetAsyncKeyState('E'))      result.e = true;
    if (GetAsyncKeyState('R'))      result.r = true;
    if (GetAsyncKeyState('T'))      result.t = true;
    if (GetAsyncKeyState('Y'))      result.y = true;


    if (GetAsyncKeyState('1'))               result.row[0] = true;
    if (GetAsyncKeyState('2'))               result.row[1] = true;
    if (GetAsyncKeyState('3'))               result.row[2] = true;
    if (GetAsyncKeyState('4'))               result.row[3] = true;
    if (GetAsyncKeyState('5'))               result.row[4] = true;
    if (GetAsyncKeyState('6'))               result.row[5] = true;
    if (GetAsyncKeyState('7'))               result.row[6] = true;
    if (GetAsyncKeyState('8'))               result.row[7] = true;
    if (GetAsyncKeyState('9'))               result.row[8] = true;
    if (GetAsyncKeyState('0'))               result.row[9] = true;
    if (GetAsyncKeyState(VK_OEM_MINUS))      result.row[10] = true;
    if (GetAsyncKeyState(VK_OEM_PLUS))       result.row[11] = true;


    if (GetAsyncKeyState(VK_OEM_COMMA))      result.carrotL = true;
    if (GetAsyncKeyState(VK_OEM_PERIOD))       result.carrotR = true;
    if (GetAsyncKeyState(VK_OEM_2))       result.questionMark = true;
    if (GetAsyncKeyState('M'))       result.m = true;

    // not checking if mouse buttons have been swapped
    if (GetAsyncKeyState(VK_LBUTTON))       result.mouseL = true;
    if (GetAsyncKeyState(VK_MBUTTON))       result.mouseM = true;
    if (GetAsyncKeyState(VK_RBUTTON))       result.mouseR = true;


    result.mouseWheelDelta = input_mousewheel_last_delta;  // 1 notch = 120 +up -down
    input_mousewheel_last_delta = 0;



    bool winActiveThisFrame = false;
    HWND activeWin = GetForegroundWindow();
    if (activeWin == window)
        winActiveThisFrame = true;

    static bool wasWinActiveLastFrame = false;

    static bool wasLockedLastFrame = false;

    bool allowMouseDelta = wasWinActiveLastFrame && !wasLockedLastFrame;


    // set mouse info for game to use...
    POINT p;
    if (GetCursorPos(&p))
    {
        if (ScreenToClient(window, &p))
        {
            int PIXEL_SIZE = 1;
            result.mouseX = p.x / PIXEL_SIZE;
            result.mouseY = p.y / PIXEL_SIZE;

            // only set delta if lastP is initialized
            if (input_lastMousePoint.x != -1 && input_lastMousePoint.y != -1 && wasWinActiveLastFrame)
            {
                result.deltaMouseX = p.x - input_lastMousePoint.x;
                result.deltaMouseY = p.y - input_lastMousePoint.y;
            }
            else
            {
                result.deltaMouseX = 0;
                result.deltaMouseY = 0;
            }

            input_lastMousePoint = p;
        }
    }

    // // rset mouse back to center if told so by game
    // if (lastInput.allowOSToMoveMouseBeforeNextFrame)
    // {
    //     if (!winActiveThisFrame)
    //     {
    //         result.deltaMouseX = 0;
    //         result.deltaMouseY = 0;
    //         // ShowCursor(true); // errr, won't it show it evr frame?
    //     }
    //     else
    //     {

    //         RECT windowRect;
    //         GetClientRect(window, &windowRect);
    //         int windowW = windowRect.right - windowRect.left;
    //         int windowH = windowRect.bottom - windowRect.top;
    //         POINT winCenter = {windowW/2, windowH/2};
    //         input_lastMousePoint = winCenter;
    //         ClientToScreen(window, &winCenter);

    //         SetCursorPos(winCenter.x, winCenter.y);
    //         // ShowCursor(false);
    //     }
    // }

    // // static HCURSOR lastCursor;
    // static bool mouseWasHidden;
    // if(lastInput.tellOSToHideCursor)// && mouseWasHidden)
    // {
    //     // ShowCursor(false); // doesn't work quite like this, it's actually a counter?

    //     // lastCursor = SetCursor(0);  // a bit glitchy

    //     while(ShowCursor(false) >= 0);
    // }
    // if(!lastInput.tellOSToHideCursor && !mouseWasHidden)
    // {
    //     // ShowCursor(true); // doesn't work quite like this, it's actually a counter?

    //     // SetCursor(lastCursor);  // a bit glitchy

    //     while(ShowCursor(true) < 0);
    // }
    // mouseWasHidden = lastInput.tellOSToHideCursor;


    wasWinActiveLastFrame = winActiveThisFrame;
    // wasLockedLastFrame = !lastInput.allowOSToMoveMouseBeforeNextFrame;


    if (GetAsyncKeyState(VK_OEM_4)) result.squareL = true;
    if (GetAsyncKeyState(VK_OEM_6)) result.squareR = true;

    return result;
}





// note sure the best home for these...

void updateCameraWithCADControls(Camera *c, Scene *s, bool mouseM, float mdx, float mdy, int mdWheel, float dt)
{
    v3 lookXZ = v3{0,0,1}.rotateAroundY(c->heading);
    v3 rightXZ = v3{1,0,0}.rotateAroundY(c->heading);
    v3 upDir = v3{0,1,0};//.rotateAroundY(state->playerDir);

    // todo: rotate around clicked point, not origin
    // todo: rotate entire scene together, not each obj individually
    if (mouseM)
    {
        for (int i = 0; i < s->gameObjectCount; i++)
        {
            s->gameObjects[i].transform.rotation =
            mat4RotY(mdx/50.f) * s->gameObjects[i].transform.rotation;

            s->gameObjects[i].transform.rotation =
            mat4RotX(-mdy/50.f) * s->gameObjects[i].transform.rotation;
        }
    }

    if (mdWheel != 0)
        c->pos.z += (float)mdWheel / 120.0f;
}
void updateCameraWithCADControls(Camera *camera, Scene *scene, input_state *i, float dt)
{
    updateCameraWithCADControls(camera, scene, i->mouseM, i->deltaMouseX, i->deltaMouseY, i->mouseWheelDelta, dt);
}



void updateCameraWithFPSControls(Camera *camera, input_state *i, float dt)//, fps_bindings bindings)
{
    camera->updateWithFPSControls(i->left, i->right, i->up, i->down, i->shift,
                                  i->w, i->s, i->a, i->d, i->space, i->ctrl,
                                  i->squareL, i->squareR, i->deltaMouseX, i->deltaMouseY, dt);
}

void singleButtonPaint(Renderer *renderer, bitmap *dest, bool mDown, bool mWasDown, v2 pos, v2 prevPos, u32 col)
{
    if (mDown)
    {
        // float x = mPos.x - pos.x;
        // float y = mPos.y - pos.y;
        float x = pos.x;
        float y = pos.y;
        renderer->draw_box_at(dest, x, y, col);

        if (mWasDown)
        {

            if (bool glitchyButCoolBoxLines = false)
            {
                float minX = min(prevPos.x, x);
                float maxX = max(prevPos.x, x);
                float minY = min(prevPos.y, y);
                float maxY = max(prevPos.y, y);
                for (float ix = minX; ix < maxX; ix++)
                {
                    for (float iy = minY; iy < maxY; iy++)
                    {
                        renderer->draw_box_at(dest, ix, iy, col);
                    }
                }
            }
            else
            {
                // line renderer loop copied to here
                // add render_shape from p1 to p2 in renderer?

                v2 start = prevPos;
                // v2 end = mousePos - pos; // a bit awkward?
                v2 end = pos;
                v2 delta = end-start;


                int steps = ceil(max(absf(delta.x), absf(delta.y)));

                if (steps == 0)
                {
                    //SetPixel(start, screen, col);
                    return;
                }

                float xInc = delta.x/(float)steps;
                float yInc = delta.y/(float)steps;

                v2 draw = start;
                for (int i = 0; i < steps; i++)
                {
                    draw.x += xInc;
                    draw.y += yInc;

                    // SetPixel(draw, screen, col);
                    renderer->draw_box_at(dest, draw.x, draw.y, col);
                }

                if (bool forceIncludeEndPoints = false)
                {
                    // SetPixel(start, screen, col);
                    // SetPixel(end, screen, col);
                    renderer->draw_box_at(dest, start.x, start.y, col);
                    renderer->draw_box_at(dest, end.x, end.y, col);
                }




            }
        }
    }
}

void updateBitmapWithPaintControls(Renderer *renderer, bitmap *dest, input_state *i, input_state *pi, float dt)
{
    singleButtonPaint(renderer, dest, i->mouseL, pi->mouseL, {i->mouseX,i->mouseY}, {pi->mouseX,pi->mouseY}, 0xffffffff);
    singleButtonPaint(renderer, dest, i->mouseR, pi->mouseR, {i->mouseX,i->mouseY}, {pi->mouseX,pi->mouseY}, 0xff000000);
}



