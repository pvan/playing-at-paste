

struct Canvas
{

    v2 pos;
    // v2 size;

    bitmap *canvasBitmap;


    // bool mouseHeldL = false;
    // bool mouseHeldR = false;
    v2 lastMousePos;


    void init(v2 p, bitmap *canv)
    {
        pos = p;
        // size = s;
        canvasBitmap = canv;
    }


    void render_to(Renderer *renderer, bitmap *dest)
    {
        renderer->copy_to(canvasBitmap, dest, pos, {-1,-1,-1,-1}, 1.0f);
    }

};

