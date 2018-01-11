

struct Canvas
{

    v2 pos;
    // v2 size;

    bitmap *canvasBitmap;


    bool mouseHeldL = false;
    bool mouseHeldR = false;
    v2 lastMousePos;


    void init(v2 p, bitmap *canv)
    {
        pos = p;
        // size = s;
        canvasBitmap = canv;
    }


    void drawBoxAt(Renderer *renderer, float x, float y, u32 col)
    {
        int size = 4;
        for (int i = -size; i <= size; i++)
        {
            for (int j = -size; j <= size; j++)
            {
                renderer->set_pixel(x+i, y+j, canvasBitmap, col);
            }
        }
    }

    void singleButtonPaint(Renderer *renderer, bool mouseDown, v2 mousePos,
                           bool *mouseHeld, v2 *lastMousePos, u32 col)
    {
        if (mouseDown)
        {
            float x = mousePos.x - pos.x;
            float y = mousePos.y - pos.y;
            drawBoxAt(renderer, x, y, col);

            if (*mouseHeld)
            {

                if (bool glitchyButCoolBoxLines = false)
                {
                    float minX = min(lastMousePos->x, x);
                    float maxX = max(lastMousePos->x, x);
                    float minY = min(lastMousePos->y, y);
                    float maxY = max(lastMousePos->y, y);
                    for (float ix = minX; ix < maxX; ix++)
                    {
                        for (float iy = minY; iy < maxY; iy++)
                        {
                            drawBoxAt(renderer, ix, iy, col);
                        }
                    }
                }
                else
                {
                    // line renderer loop copied to here
                    // add function to render shape from p1 to p2 in renderer?

                    v2 start = {lastMousePos->x, lastMousePos->y};
                    v2 end = mousePos - pos; // a bit awkward?
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
                        drawBoxAt(renderer, draw.x, draw.y, col);
                    }

                    if (bool forceIncludeEndPoints = false)
                    {
                        // SetPixel(start, screen, col);
                        // SetPixel(end, screen, col);
                        drawBoxAt(renderer, start.x, start.y, col);
                        drawBoxAt(renderer, end.x, end.y, col);
                    }




                }
            }
            *mouseHeld = true;
            *lastMousePos = {x,y};
        }
        if (!mouseDown)
        {
            *mouseHeld = false;
        }
    }

    void updateWithPaintControls(Renderer *renderer, input_state *input, float dt)
    {

        singleButtonPaint(renderer, input->mouseL, {input->mouseX, input->mouseY},
                          &mouseHeldL, &lastMousePos, 0xffffffff);

        singleButtonPaint(renderer, input->mouseR, {input->mouseX, input->mouseY},
                          &mouseHeldR, &lastMousePos, 0xff000000);

    }

    void render_to(Renderer *renderer, bitmap *dest)
    {
        renderer->copy_to(canvasBitmap, dest, pos, {-1,-1,-1,-1}, 1.0f);
    }

};

