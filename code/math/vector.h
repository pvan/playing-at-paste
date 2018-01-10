


struct v2
{
    float x;
    float y;

    float mag() {
        return sqrtf(x*x + y*y);
    }
    float dist(v2 other) {
        return sqrtf( (x-other.x)*(x-other.x) + (y-other.y)*(y-other.y) );
    }
    v2 rotate(float a) {
        v2 result;
        result.x = x*cos(a) + y*sin(a);  // note the rotate goofy
        result.y = -x*sin(a) + y*cos(a);
        // result.x = x*cos(a) - y*sin(a);  //regular aka RH system
        // result.y = x*sin(a) + y*cos(a);
        return result;
    }
    float dot(v2 other)
    {
        return (x * other.x + y * other.y);
    }
    v2 normalized()
    {
        v2 result;
        result.x = x/mag();
        result.y = y/mag();
        return result;
    }
};

v2 operator-(v2 a, v2 b)
{
    v2 result = {a.x-b.x, a.y-b.y};
    return result;
}
v2 operator+(v2 a, v2 b)
{
    v2 result = {a.x+b.x, a.y+b.y};
    return result;
}
v2 operator*(v2 v, float s)
{
    v2 result = {v.x*s, v.y*s};
    return result;
}
v2 operator*(float s, v2 v)
{
    return v*s;
}


bool Testv2()
{

    v2 origin = {0.0f, 0.0f};
    v2 one = {1.0f, 1.0f};
    v2 test1 = {.7071f, .7071f};

    float t2 = test1.mag();
    float t3 = origin.dist(test1);

    v2 t4 = test1 * 1.0f;
    v2 t5 = 2.0f * test1;
    v2 t6 = 0.5f * one;

    int a = 1+1;
    return true;
}




struct v3
{
    float x;
    float y;
    float z;

    float mag() {
        return sqrtf(x*x + y*y + z*z);
    }
    float dist(v3 o) {
        return sqrtf( (x-o.x)*(x-o.x) + (y-o.y)*(y-o.y) + (z-o.z)*(z-o.z) );
    }
    void normalize() {
        float mag2 = mag();
        x /= mag2;
        y /= mag2;
        z /= mag2;
    }
    v3 normalized() {
        v3 result;
        float mag2 = mag();
        result.x = x / mag2;
        result.y = y / mag2;
        result.z = z / mag2;
        return result;
    }
    v3 cross(v3 o) {
        return v3{ y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x };
    }
    float dot(v3 o) {
        return x*o.x + y*o.y + z*o.z;
    }

    v2 xz() { return v2{x, z}; }
    v2 xy() { return v2{x, y}; }
    v2 yz() { return v2{y, z}; }

    v3 rotateAroundY(float a) {
        v2 comps = xz();
        comps = comps.rotate(a);
        return v3{comps.x, y, comps.y};
    }
    v3 rotateAroundZ(float a) {
        v2 comps = xy();
        comps = comps.rotate(a);
        return v3{comps.x, comps.y, z};
    }
    v3 rotateAroundX(float a) {
        v2 comps = yz();
        comps = comps.rotate(a);
        return v3{x, comps.x, comps.y};
    }

    v3 rotateAroundAxis(v3 axis, float a);

    float get(int i)
    {
        if (i == 0) return x;
        else if (i == 1) return y;
        else return z;
    }
    float operator[](int i) { return get(i); }

    v3 SnapToGrid(float resolution)
    {
        v3 result;
        result.x = round_nearest(x / resolution) * resolution;
        result.y = round_nearest(y / resolution) * resolution;
        result.z = round_nearest(z / resolution) * resolution;
        return result;
    }

};


v3 operator-(v3 a)
{
    v3 result = {-a.x, -a.y, -a.z};
    return result;
}
v3 operator-(v3 a, v3 b)
{
    v3 result = {a.x-b.x, a.y-b.y, a.z-b.z};
    return result;
}
v3 operator+(v3 a, v3 b)
{
    v3 result = {a.x+b.x, a.y+b.y, a.z+b.z};
    return result;
}
v3 operator*(v3 v, float s)
{
    v3 result = {v.x*s, v.y*s, v.z*s};
    return result;
}
v3 operator*(float s, v3 v)
{
    return v*s;
}
bool operator==(v3 l, v3 r)
{
    float epsilon = 0.001f;
    if (absf(l.x - r.x) > epsilon) return false;
    if (absf(l.y - r.y) > epsilon) return false;
    if (absf(l.z - r.z) > epsilon) return false;
    return true;
}


//  v4 --------------------


struct v4
{
    float x;
    float y;
    float z;
    float w;

    v2 xz() { return v2{x, z}; }
    v2 xy() { return v2{x, y}; }
    v2 yz() { return v2{y, z}; }
    v3 xyz() { return v3{x, y, z}; }

    float get(int i)
    {
        if (i == 0) return x;
        else if (i == 1) return y;
        else if (i == 2) return z;
        else return w;
    }
    float operator[](int i) { return get(i); }
};

v4 operator-(v4 a, v4 b)
{
    v4 result = {a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w};
    return result;
}
v4 operator+(v4 a, v4 b)
{
    v4 result = {a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};
    return result;
}
v4 operator*(v4 v, float s)
{
    v4 result = {v.x*s, v.y*s, v.z*s, v.w*s};
    return result;
}
v4 operator*(float s, v4 v)
{
    return v*s;
}



v4 v4w1(v3 p) { return v4{p.x,p.y,p.z,1}; }

//





