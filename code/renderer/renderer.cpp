



enum backend_name
{
    SOFTWARE,
    OPEN_GL,
    DIRECTX
};



struct Renderer
{

    RendererMemory workingMem;


    void (*copy_to)(bitmap*, bitmap*, v2, Rect, float);
    void (*fill)(bitmap*, u32);
    void (*set_pixel)(int, int, bitmap*, u32);

    void (*draw_to_p)(Scene*, Camera*, bitmap*, RendererMemory*);

    // a little awkward but w/e, keep it simple
    void draw_to(Scene *scene, Camera *cam, bitmap *dest)
    {
        draw_to_p(scene, cam, dest, &workingMem);
    }


    void init_with_backend(memory_block *memory, int w, int h, backend_name name)
    {
        workingMem.init(memory, w, h);

        if (name == SOFTWARE)
        {
            copy_to = &Software::RenderBitmapOnBitmap;
            fill = &Software::FillBufferWithColor;
            set_pixel = &Software::SetPixel;

            draw_to_p = &Software::DrawScene;
        }
        else
        {
            ASSERT(false); // not implemented
        }
    }


    void draw_box_at(bitmap *dest, float x, float y, u32 col)
    {
        int size = 4;
        for (int i = -size; i <= size; i++)
        {
            for (int j = -size; j <= size; j++)
            {
                set_pixel(x+i, y+j, dest, col);
            }
        }
    }


};


