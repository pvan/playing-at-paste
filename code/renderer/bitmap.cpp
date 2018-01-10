

struct bitmap
{
    u32 *pixels;
    int width;
    int height;

    void allocate(int w, int h, memory_block *mem)
    {
        width = w;
        height = h;
        pixels = (u32*)mem->allocate(w*sizeof(u32) * h*sizeof(u32));
    }

    // float operator [](int i) { return m[i]; }
    // float operator ()(int c, int r) { return ge(c,r); }
    // float ge(int c, int r)
    // {
    //     ASSERT(c+r*4 >= 0 && c+r*4 < 16);
    //     return m[c + r*4];
    // }
    // void se(int c, int r, float v)
    // {
    //     ASSERT(c+r*4 >= 0 && c+r*4 < 16);
    //     m[c + r*4] = v;
    // }

};

struct bitmapf
{
    float *pixels;
    int width;
    int height;

    void allocate(int w, int h, memory_block *mem)
    {
        width = w;
        height = h;
        pixels = (float*)mem->allocate(w*sizeof(float) * h*sizeof(float));
    }
};