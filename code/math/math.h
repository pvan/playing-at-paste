

// general math stuff & trig (mostly replaced by math.h now)


// quite odd
namespace ext_math {
    #include <math.h>
}


#define PI 3.14159265359f

#define DEG2RAD (PI/180.0f)
#define RAD2DEG (180.0f/PI)

#define RADIANS(x) (x*DEG2RAD)
#define DEGREES(x) (x*RAD2DEG)




// our janky trig approximations weren't accurate enough for 3D work, for now use math.h

float sinf(float x)
{
    return ext_math::sinf(x);
}
float cosf(float x)
{
    return ext_math::cosf(x);
}
float tanf(float x)
{
    return ext_math::tanf(x);
}
float atan2f(float a, float b)
{
    return ext_math::atan2f(a, b);
}

float sqrtf(float x)
{
    return ext_math::sqrtf(x);
}

float sin(float x)
{
    return sinf(x);
}
float cos(float x)
{
    return cosf(x);
}
float tan(float x)
{
    return tanf(x);
}
float atan2(float a, float b)
{
    return atan2f(a, b);
}

// float max(float x, float y)
// {
    // if (x > y) return x;
    // else return y;
// }
// float min(float x, float y)
// {
    // if (x > y) return y;
    // else return x;
// }

float lerp(float a, float b, float p)
{
    float result = (p*(b-a) + a);
    // pb - pa + a
    // (1-p)a + pb
    if (p>=1.0f) result = b;
    if (p<=0.0f) result = a;
    return result;
}


// // note this won't work if min > max?
float clamp(float f, float min, float max)
{
    ASSERT(min <= max);
    float result = f>=max? max : (f<=min? min : f);
    return result;
}


int floor(float f)
{
    return ext_math::floor(f);
    // int result = (int)f;
    // if (f < 0 && (float)(int)f != f) result--;
    // return result;
}


int ceilf(float f)
{
    return ext_math::ceil(f);
    // int result = (int)f;
    // if (f > 0 && (float)(int)f != f) result++;
    // return result;
}
int ceil(float f)
{
    return ceilf(f);
}

int round_nearest(float f)  //nearest
{
    int result;
    if (f >= 0) result = (int)(f + 0.5f);
    else result = (int)(f - 0.5f);
    return result;
}
int nearestInt(float in)  // todo: consolidate with above
{
    return floor(in + 0.5);
}
i64 nearestI64(float in)
{
    return floor(in + 0.5);
}

float absf(float f)
{
    if (f < 0) return -f; else return f;
}
float signf(float f)
{
    if (f < 0) return -1.0f; else return 1.0f;
}





// in C: -1 % 12 == -1
// want: -1 % 12 == 11 (this func)
int mod_wrap(int n, int m)
{
    int result = ((n % m) + m) % m;
    return result;
}


void TestMath()
{
    ASSERT(ceil(0) == 0);
    ASSERT(ceil(1) == 1);
    ASSERT(ceil(2) == 2);
    ASSERT(ceil(3) == 3);
    ASSERT(ceil(-1) == -1);
    ASSERT(ceil(-2) == -2);
    ASSERT(ceil(-100) == -100);

    ASSERT(ceil(3.01f) == 4);
    ASSERT(ceil(3.99f) == 4);
    ASSERT(ceil(-3.01f) == -3);
    ASSERT(ceil(-3.99f) == -3);
    ASSERT(ceil(.01f) == 1);
    ASSERT(ceil(.99f) == 1);
    ASSERT(ceil(-.01f) == 0);
    ASSERT(ceil(-.99f) == 0);
    ASSERT(ceil(1.01f) == 2);
    ASSERT(ceil(1.99f) == 2);


    ASSERT(floor(0) == 0);
    ASSERT(floor(1) == 1);
    ASSERT(floor(2) == 2);
    ASSERT(floor(3) == 3);
    ASSERT(floor(-1) == -1);
    ASSERT(floor(-2) == -2);
    ASSERT(floor(-100) == -100);

    ASSERT(floor(3.01f) == 3);
    ASSERT(floor(3.99f) == 3);
    ASSERT(floor(-3.01f) == -4);
    ASSERT(floor(-3.99f) == -4);
    ASSERT(floor(.01f) == 0);
    ASSERT(floor(.99f) == 0);
    ASSERT(floor(-.01f) == -1);
    ASSERT(floor(-.99f) == -1);
    ASSERT(floor(1.01f) == 1);
    ASSERT(floor(1.99f) == 1);


    ASSERT(mod_wrap(-1, 12) == 11);
    ASSERT(mod_wrap(-13, 12) == 11);
    ASSERT(mod_wrap(1, 12) == 1);
    ASSERT(mod_wrap(13, 12) == 1);
    ASSERT(mod_wrap(12, 12) == 0);
    ASSERT(mod_wrap(-12, 12) == 0);
    ASSERT(mod_wrap(0, 12) == 0);

    float t1;
    t1 = sin(0);
    t1 = sin(PI/2);
    t1 = sin(PI);
    t1 = sin(PI*2/3);
    t1 = sin(PI*3/4);
    t1 = sin(2*PI);  // does not seem to work over PI? erp?
    t1 = sin(PI+1);
    t1 = sin(-PI);

    t1 = cos(0);
    t1 = cos(PI/2);
    t1 = cos(PI);
    t1 = cos(PI*2/3);
    t1 = cos(PI*3/4);
    t1 = cos(2*PI);
    t1 = cos(PI+1);
    t1 = cos(-PI);

    int a = 1+1;
}







