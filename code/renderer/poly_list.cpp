


// vert is a v4 with attributes (color, uv, etc)
struct vert
{
    #pragma pack(push)
    #pragma pack(1)

    union
    {
        v4 pos;
        struct {
            float x;
            float y;
            float z;
            float w;
        };
    };

    u32 c;
    u32 origC;

    // union
    // {
    //     v2 uv;
    //     struct {
            float u;
            float v;
    //     };
    // };

    int mat;

    #pragma pack(pop)


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

    void Move(v3 delta)
    {
        x += delta.x;
        y += delta.y;
        z += delta.z;
    }
};

vert VertFromV3(v3 in)
{
    vert result = {0};
    result.x = in.x;
    result.y = in.y;
    result.z = in.z;
    result.w = 1;
    result.c = 0xff0000ff;
    result.origC = 0xff0000ff;
    return result;
}

// do these even make sense for verts? how do you combine the attributes?
// vert operator-(vert a, vert b)
// {
//     vert result = {a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w, a.c-b.c};
//     return result;
// }
// vert operator+(vert a, vert b)
// {
//     vert result = {a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w, a.c+b.c};
//     return result;
// }
// vert operator*(vert v, float s)
// {
//     v.pos = v.pos * s;
//     return v;
// }
// vert operator*(float s, vert v)
// {
//     return v*s;
// }


vert TransformVert(vert p, mat4 mat)
{
    vert result = p;
    result.x = p.x*mat[ 0] + p.y*mat[ 1] + p.z*mat[ 2] + p.w*mat[ 3];
    result.y = p.x*mat[ 4] + p.y*mat[ 5] + p.z*mat[ 6] + p.w*mat[ 7];
    result.z = p.x*mat[ 8] + p.y*mat[ 9] + p.z*mat[10] + p.w*mat[11];
    result.w = p.x*mat[12] + p.y*mat[13] + p.z*mat[14] + p.w*mat[15];
    return result;
}
vert operator *(mat4 mat, vert v) { return TransformVert(v, mat); }




// not sure how number of sides of poly is related to num of verts in each side
const int MAX_POLY_VERTS = 8;
const int MAX_POLYLIST_POLYS = 12;  //eh..?

// individual plane/quad/etc
struct Polygon2
{
    vert verts[MAX_POLY_VERTS];
    int vertCount;

    bitmap texture; //replace with textureId at some point?

    // or this?
    // vert center;
    // float width;
    // float length;
    // float angle;
    // actually, need full transform?

    v3 CalcNormal()
    {
        // not sure if correct
        return (verts[0].pos.xyz() - verts[1].pos.xyz()).cross(verts[1].pos.xyz() - verts[2].pos.xyz());
    }


    // TODO: use quaternions to rotat these planes to the correct normal
    void MakeNgon(v3 pos, float polyRad, v3 norm, int sides, u32 col, float uScale = 1, float vScale = 1, int matIndex = 0)
    {

        // basically we want one texture length per side (before u or v scaling)
        float oneSideLength = 2 * polyRad * sin(PI / (float)sides);
        float uPerWorldUnit = uScale / oneSideLength;
        float VPerWorldUnit = vScale / oneSideLength;

        vertCount = sides;


        // build points...

        ASSERT(sides <= 8);

        v3 p[8];

        float radiansPerVert = -2*PI / (float)sides; // negative so we create ccw and have norm of -Z

        mat4 rotInc = mat4RotZ(radiansPerVert);
        mat4 rotHalfInit = mat4RotZ(radiansPerVert / 2);

        p[0] = v3{0,-1,0} * polyRad;

        p[0] = TransformV3(p[0], rotHalfInit);  // start half way, so we're square to the sides


        for (int i = 1; i < vertCount; i++)
        {
            p[i] = TransformV3(p[i-1], rotInc);
        }

        // todo: maybe: add offset to uv so center of texture is center of polygon?
        // something like: float uOffset = 0.5f; ??

        // build into verts with color and uv...
        for (int i = 0; i < vertCount; i++)
        {
            verts[i] = vert{p[i].x, p[i].y, p[i].z, 1, col, col, p[i].x*uPerWorldUnit, p[i].y*VPerWorldUnit, matIndex};
        }


        // rotate to correct normal...

        // XZ angle from 0,0,-1 to norm
        // recall in a normal 2D system angle from 1,0 to vec is atan2(y,x)
        // in this case, world -Z is our +X, and world +X is our +Y
        float yAngle = atan2(norm.x, -norm.z);
        mat4 rotY = mat4RotY(yAngle);

        if (norm.x > 0.0001f || norm.x < -0.0001f || norm.z > 0.0001f || norm.z < -0.0001f) {
            for (int i = 0; i < vertCount; i++)
            {
                verts[i] = TransformVert(verts[i], rotY);
            }
        }


        if (norm.z < 0.999f) {  // special case when angle is 180 from start (don't perform both y and z rot)

            float xAngle = atan2(norm.y, -norm.z);
            mat4 rotX = mat4RotX(xAngle);

            // things with a > z were getting rotated back toward -z.. this is startnig to get messy..
            if (norm.y > 0.0001f || norm.y < -0.0001f /*|| norm.z > 0.0001f*/ || norm.z < -0.0001f) {
                for (int i = 0; i < vertCount; i++)
                {
                    verts[i] = TransformVert(verts[i], rotX);
                }
            }

        }

        if (norm.y < 0) {
            mat4 rot180 = mat4RotY(RADIANS(180));
            for (int i = 0; i < vertCount; i++)
            {
                verts[i] = TransformVert(verts[i], rot180);
            }
        }


        for (int i = 0; i < vertCount; i++)
        {
            verts[i] = TransformVert(verts[i], mat4Translate(pos));
        }




    }

    void MakeIntoRectFromPositionRadiusNormal(v3 pos, float widRad, float heiRad, v3 norm, u32 col, float uScale = 1, float vScale = 1, int matIndex = 0)
    {
        vertCount = 4;


        // winding these ccw is a 0 0 -1 normal
        v3 p1 = v3{-widRad,-heiRad,0};
        v3 p2 = v3{ widRad,-heiRad,0};
        v3 p3 = v3{ widRad, heiRad,0};
        v3 p4 = v3{-widRad, heiRad,0};


        // XZ angle from 0,0,-1 to norm
        // recall in a normal 2D system angle from 1,0 to vec is atan2(y,x)
        // in this case, world -Z is our +X, and world +X is our +Y
        float yAngle = atan2(norm.x, -norm.z);
        mat4 rotY = mat4RotY(yAngle);

        if (norm.x > 0.0001f || norm.x < -0.0001f || norm.z > 0.0001f || norm.z < -0.0001f) {
            p1 = TransformV3(p1, rotY);
            p2 = TransformV3(p2, rotY);
            p3 = TransformV3(p3, rotY);
            p4 = TransformV3(p4, rotY);
        }


        if (norm.z < 0.999f) {  // special case when angle is 180 from start (don't beform both y and z rot)

            float xAngle = atan2(norm.y, -norm.z);
            mat4 rotX = mat4RotX(xAngle);

            // things with a > z were getting rotated back toward -z.. this is startnig to get messy..
            if (norm.y > 0.0001f || norm.y < -0.0001f /*|| norm.z > 0.0001f*/ || norm.z < -0.0001f) {
                p1 = TransformV3(p1, rotX);
                p2 = TransformV3(p2, rotX);
                p3 = TransformV3(p3, rotX);
                p4 = TransformV3(p4, rotX);
            }

        }


        p1 = TransformV3(p1, mat4Translate(pos));
        p2 = TransformV3(p2, mat4Translate(pos));
        p3 = TransformV3(p3, mat4Translate(pos));
        p4 = TransformV3(p4, mat4Translate(pos));



        verts[0] = vert{p1.x, p1.y, p1.z, 1, col, col, 0,      0,      matIndex};
        verts[1] = vert{p2.x, p2.y, p2.z, 1, col, col, uScale, 0,      matIndex};
        verts[2] = vert{p3.x, p3.y, p3.z, 1, col, col, uScale, vScale, matIndex};
        verts[3] = vert{p4.x, p4.y, p4.z, 1, col, col, 0,      vScale, matIndex};

    }

    void MakeIntoSquareFromPositionRadiusNormal(v3 pos, float rad, v3 norm, u32 col, float uScale = 1, float vScale = 1, int matIndex = 0)
    {
        vertCount = 4;


        // winding these ccw is a 0 0 -1 normal
        v3 p1 = v3{-1,-1,0} * rad;
        v3 p2 = v3{ 1,-1,0} * rad;
        v3 p3 = v3{ 1, 1,0} * rad;
        v3 p4 = v3{-1, 1,0} * rad;


        // XZ angle from 0,0,-1 to norm
        // recall in a normal 2D system angle from 1,0 to vec is atan2(y,x)
        // in this case, world -Z is our +X, and world +X is our +Y
        float yAngle = atan2(norm.x, -norm.z);
        mat4 rotY = mat4RotY(yAngle);

        if (norm.x > 0.0001f || norm.x < -0.0001f || norm.z > 0.0001f || norm.z < -0.0001f) {
            p1 = TransformV3(p1, rotY);
            p2 = TransformV3(p2, rotY);
            p3 = TransformV3(p3, rotY);
            p4 = TransformV3(p4, rotY);
        }


        if (norm.z < 0.999f) {  // special case when angle is 180 from start (don't beform both y and z rot)

            float xAngle = atan2(norm.y, -norm.z);
            mat4 rotX = mat4RotX(xAngle);

            // things with a > z were getting rotated back toward -z.. this is startnig to get messy..
            if (norm.y > 0.0001f || norm.y < -0.0001f /*|| norm.z > 0.0001f*/ || norm.z < -0.0001f) {
                p1 = TransformV3(p1, rotX);
                p2 = TransformV3(p2, rotX);
                p3 = TransformV3(p3, rotX);
                p4 = TransformV3(p4, rotX);
            }

        }


        p1 = TransformV3(p1, mat4Translate(pos));
        p2 = TransformV3(p2, mat4Translate(pos));
        p3 = TransformV3(p3, mat4Translate(pos));
        p4 = TransformV3(p4, mat4Translate(pos));



        verts[0] = vert{p1.x, p1.y, p1.z, 1, col, col, 0,      0,      matIndex};
        verts[1] = vert{p2.x, p2.y, p2.z, 1, col, col, uScale, 0,      matIndex};
        verts[2] = vert{p3.x, p3.y, p3.z, 1, col, col, uScale, vScale, matIndex};
        verts[3] = vert{p4.x, p4.y, p4.z, 1, col, col, 0,      vScale, matIndex};

    }

};



const v3 allDirs[6] = {
    v3{ 0, 0, 1},
    v3{ 0, 0,-1},
    v3{ 1, 0, 0},
    v3{-1, 0, 0},
    v3{ 0, 1, 0},
    v3{ 0,-1, 0}
};


// collection of inter-related surfaces
struct PolygonList
{
    Polygon2 polygons[MAX_POLYLIST_POLYS];
    int polygonCount;

    // todo: could just pass negative rad to get an inside-out box!
    void MakeIntoCubeAt(v3 pos, float rad, bitmap texture, bool inFacingWalls = false)
    {
        polygonCount = 6;

        float factor = 1;
        if (inFacingWalls) { factor = -1; }

        for (int i = 0; i < 6; i++)
        {

            // static int colCount = 0;
            // colCount = (colCount + 1) % 100;
            u32 col = 0xff777777;//randCols[colCount];

            polygons[i].MakeIntoSquareFromPositionRadiusNormal(pos+(allDirs[i]*rad), rad, factor*allDirs[i], col);
            polygons[i].texture = texture;
        }
    }

    void MakeIntoNgon(v3 pos, float polyRad, /*bitmap texture,*/ int sides)
    {
        ASSERT(sides <= 8);

        float sidesf = (float)sides;

        polygonCount = sides + 2;  // 2 extra for top and bottom

        float factor = 1;
        //if (inFacingWalls) { factor = -1; }


        float wallWidRad = polyRad * sin(PI/sidesf);
        float apothem = wallWidRad / tan(PI/sidesf);
        // float apothem = polyRad * Cos(PI/sidesf);  // should also work


        // we can set this ratio so it sort of makes sense / most roundish
        // actually should we pass in the wall height / wallheightrad?
        // float wallHeiRad = polyRad / 2.0f;   // more rounded 8side, but flat 4side
        float wallHeiRad = wallWidRad;  // this will make 4sided a cube, but a very flat 8sided



        static int colCount = 0;
        u32 col;

        v3 dir = {0, 0, -1};
        float angleStep = (2.0f*PI) / (float)sides;
        float angle = 0;
        for (int i = 0; i < sides; i++)
        {

            // colCount = (colCount + 1) % 100;
            // col = randCols[colCount];
            col = 0xff333333;

            polygons[i].MakeIntoRectFromPositionRadiusNormal(pos+(dir*apothem), wallWidRad, wallHeiRad, factor*dir, col);
            // polygons[i].MakeIntoSquareFromPositionRadiusNormal(pos+(dir*apothem), newSideLength, factor*dir);
            // polygons[i].texture = texture;

            angle += angleStep;
            mat4 rotY = mat4RotY(angle);
            dir = TransformV3({0, 0, -1}, rotY).normalized();
        }


        // top and bottom...

        // float polyRadius = apothem / Cos(PI/sidesf);

        // colCount = (colCount + 1) % 100;
        // col = randCols[colCount];
        col = 0xff3377ff;

        dir = v3{0, 1, 0};
        polygons[sides].MakeNgon(pos+(dir*wallHeiRad), polyRad, factor*dir, sides, col);
        // polygons[sides].texture = texture;

        // colCount = (colCount + 1) % 100;
        // col = randCols[colCount];
        col = 0xffff7733;


        dir = v3{0, -1, 0};
        polygons[sides+1].MakeNgon(pos+(dir*wallHeiRad), polyRad, factor*dir, sides, col);
        // polygons[sides+1].texture = texture;

    }

};


void TransformPolygonList(PolygonList *brush, mat4 matrix)
{
    for (int i = 0; i < brush->polygonCount; i++)
    {
        for (int j = 0; j < brush->polygons[i].vertCount; j++)
        {
            brush->polygons[i].verts[j] = matrix * brush->polygons[i].verts[j];
        }
    }
}