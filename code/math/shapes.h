



// -------------------------------


struct Triangle4D
{
    v4 pts[3];
    v4 operator [](int i) { return pts[i]; }
};


struct Triangle
{
    v3 pts[3];
    v3 operator [](int i) { return pts[i]; }
};


Triangle TransformTri(Triangle t, mat4 mat)
{
    Triangle result;
    for (int i = 0; i < 3; i++)
    {
        result.pts[i] = TransformV3(t.pts[i], mat);
    }
    return result;
}


// ---------------

union Rect
{
    struct
    {
        v2 pos;
        v2 size;
    };

    struct
    {
        float x, y, w, h;
    };


    // float x;
    // float y;
    // float w;
    // float h;
};


// -----------





struct Line
{
    v2 start;
    v2 end;

    float length()
    {
        return start.dist(end);
    }

    // where y = ax + b
    float a()
    {
        return (end.y-start.y)/(end.x-start.x);
    }
    float b()
    {
        return start.y - a() * start.x;
    }
};

struct Line3
{
    v3 start;
    v3 end;
};




// -----------