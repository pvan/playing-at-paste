

// todo: switch over to (always?) using Color isntead of u32?
struct Color
{
    union
    {
        u32 hex;
        struct
        {
            u8 a;
            u8 r;
            u8 g;
            u8 b;
        };
    };
};

static Color MakeColor(u32 in)
{
    Color out;
    out.hex = in;
    return out;
}

static u32 MakeColor(u8 r, u8 g, u8 b, u8 a)
{
    uint32_t result = (r<<16) | (g<<8) | (b<<0) | (a<<24);
    return result;
}
static u32 MakeColor(u32 r, u32 g, u32 b)
{
    return MakeColor(r, g, b, 255);
    // uint32_t Result = (r<<16) | (g<<8) | (b<<0);
    // return Result;
}

static u8 ColorR(u32 col) { return col >> 16; }
static u8 ColorG(u32 col) { return col >> 8; }
static u8 ColorB(u32 col) { return col >> 0; }
static u8 ColorA(u32 col) { return col >> 24; }

static u32 SetAlpha(u32 col, u8 a) { return (col | (a<<24)); }

static u32 RandomColor()
{
    //u32 rando = randu32();
    //rando = SetAlpha(rando, 255);
    //return rando;
    return 0xff0077ff;  // todo fix this
}


static u32 AlphaMixColors(Color a, Color b, float p1, float p2)
{
    Color result;
    result.a = (a.a*p1 + b.a*p2);
    result.r = (a.r*p1 + b.r*p2);
    result.b = (a.b*p1 + b.b*p2);
    result.g = (a.g*p1 + b.g*p2);
    return result.hex;
}

static u32 ColorMix3(Color a, Color b, Color c, float w0, float w1, float w2)
{
    Color result;
    result.a = (a.a*w0 + b.a*w1 + c.a*w2);
    result.r = (a.r*w0 + b.r*w1 + c.r*w2);
    result.b = (a.b*w0 + b.b*w1 + c.b*w2);
    result.g = (a.g*w0 + b.g*w1 + c.g*w2);
    return result.hex;
}

static Color ColorMix2(u8 *c0, u8 *c1, u8 *c2, u8 p0, u8 p1, u8 p2)
{
    Color result;
    result.a = (c0[0]*p0 + c1[0]*p1 + c2[0]*p2) >> 8;
    result.r = (c0[1]*p0 + c1[1]*p1 + c2[1]*p2) >> 8;
    result.b = (c0[2]*p0 + c1[2]*p1 + c2[2]*p2) >> 8;
    result.g = (c0[3]*p0 + c1[3]*p1 + c2[3]*p2) >> 8;
    return result;
}

static u32 ColorMix(u32 c0, u32 c1, u32 c2, float p0, float p1, float p2)
{
    // u32 wc0 = MakeColor(ColorR(c0)*p0, ColorG(c0)*p0, ColorB(c0)*p0, ColorA(c0)*p0);
    // u32 wc1 = MakeColor(ColorR(c1)*p1, ColorG(c1)*p1, ColorB(c1)*p1, ColorA(c1)*p1);
    // u32 wc2 = MakeColor(ColorR(c2)*p2, ColorG(c2)*p2, ColorB(c2)*p2, ColorA(c2)*p2);

    u8 r = (float)ColorR(c0)*p0 + (float)ColorR(c1)*p1 + (float)ColorR(c2)*p2;
    u8 g = (float)ColorG(c0)*p0 + (float)ColorG(c1)*p1 + (float)ColorG(c2)*p2;
    u8 b = (float)ColorB(c0)*p0 + (float)ColorB(c1)*p1 + (float)ColorB(c2)*p2;
    u8 a = (float)ColorA(c0)*p0 + (float)ColorA(c1)*p1 + (float)ColorA(c2)*p2;

    // u8 r = ColorR(c0)*p0 + ColorR(c1)*p1 + ColorR(c2)*p2;
    // u8 g = ColorG(c0)*p0 + ColorG(c1)*p1 + ColorG(c2)*p2;
    // u8 b = ColorB(c0)*p0 + ColorB(c1)*p1 + ColorB(c2)*p2;
    // u8 a = ColorA(c0)*p0 + ColorA(c1)*p1 + ColorA(c2)*p2;

    // return c0*p0 + c1*p1 + c2*p2;

    return MakeColor(r, g, b, a);
}


static u32 LerpCol_slow(u32 c1, u32 c2, float p)
{
    u8 newR1 = ColorR(c1)*p;
    u8 newR2 = ColorR(c2)*(1-p);

    return MakeColor(
        clamp((float)ColorR(c1)*(1-p) + (float)ColorR(c2)*(p), 0, 255),
        clamp((float)ColorG(c1)*(1-p) + (float)ColorG(c2)*(p), 0, 255),
        clamp((float)ColorB(c1)*(1-p) + (float)ColorB(c2)*(p), 0, 255),
        clamp((float)ColorA(c1)*(1-p) + (float)ColorA(c2)*(p), 0, 255)
    );
}

// seems only slightly faster than below
static u32 LerpCol5050(u32 c1, u32 c2)
{
    // shift this one over to preserve the most sig bit when combining
    u32 a = ((0xff000000 & c1)>>8) + ((0xff000000 & c2)>>8);
    u32 r = (0x00ff0000 & c1) + (0x00ff0000 & c2);
    u32 g = (0x0000ff00 & c1) + (0x0000ff00 & c2);
    u32 b = (0x000000ff & c1) + (0x000000ff & c2);
    return (0xff000000 & a<<7) | (0x00ff0000 & r>>1) | (0x0000ff00 & g>>1) | (0x000000ff & b>>1);
    // re have to re-mask these so the least sig. bit doesn't shift into the next spot
}

static u32 LerpCol(u32 c1, u32 c2, float p)
{
    // the idea with q here:
    // c1 *    p    +  c2 *     (1-p)
    // c1 *  %/100  +  c2 * (100-%) / 100
    // c1 *  q/256  +  c2 * (256-q) / 256
    u8 q = u8(p * 256);

    // // >>8 instead of /256 seems actually ever so slightly slower? maybe not, prolly equiv?
    // u32 a = ((((0xff000000 & c1)>>24)*q)>>8) + ((((0xff000000 & c2)>>24)*(256-q))>>8);
    // u32 r = ((((0x00ff0000 & c1)>>16)*q)>>8) + ((((0x00ff0000 & c2)>>16)*(256-q))>>8);
    // u32 g = ((((0x0000ff00 & c1)>> 8)*q)>>8) + ((((0x0000ff00 & c2)>> 8)*(256-q))>>8);
    // u32 b = ((((0x000000ff & c1)>> 0)*q)>>8) + ((((0x000000ff & c2)>> 0)*(256-q))>>8);

    u32 a = ((((0xff000000 & c1)>>24)*q)/256) + ((((0xff000000 & c2)>>24)*(256-q))/256);
    u32 r = ((((0x00ff0000 & c1)>>16)*q)/256) + ((((0x00ff0000 & c2)>>16)*(256-q))/256);
    u32 g = ((((0x0000ff00 & c1)>> 8)*q)/256) + ((((0x0000ff00 & c2)>> 8)*(256-q))/256);
    u32 b = ((((0x000000ff & c1)>> 0)*q)/256) + ((((0x000000ff & c2)>> 0)*(256-q))/256);
    return a<<24 | r<<16 | g<<8 | b;

}

static u32 FloatToGreyValueColor(float valueBetweenMinMax, float min, float max)
{
    float p = (valueBetweenMinMax-min) / (max-min);
    return LerpCol(0xffffffff, 0x0, p);
}