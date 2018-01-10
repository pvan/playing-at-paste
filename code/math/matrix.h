
// matrix--------------


struct mat4
{
    float m[16];
    float operator [](int i) { return m[i]; }
    float operator ()(int c, int r) { return ge(c,r); }
    float ge(int c, int r)
    {
        ASSERT(c+r*4 >= 0 && c+r*4 < 16);
        return m[c + r*4];
    }
    void se(int c, int r, float v)
    {
        ASSERT(c+r*4 >= 0 && c+r*4 < 16);
        m[c + r*4] = v;
    }
    // mat4 identity()  // untested
    // {
    //     mat4 result;
    //     result.m[0] = 1;  result.m[4] = 0;  result.m[8] = 0;   result.m[12] = 0;
    //     result.m[1] = 0;  result.m[5] = 1;  result.m[9] = 0;   result.m[13] = 0;
    //     result.m[2] = 0;  result.m[6] = 0;  result.m[10] = 1;  result.m[14] = 0;
    //     result.m[3] = 0;  result.m[7] = 0;  result.m[11] = 0;  result.m[15] = 1;
    // }
    mat4 mul(mat4 o)
    {
        mat4 result;

        result.m[ 0] = m[ 0]*o.m[ 0] + m[ 1]*o.m[ 4] + m[ 2]*o.m[ 8] + m[ 3]*o.m[12];
        result.m[ 1] = m[ 0]*o.m[ 1] + m[ 1]*o.m[ 5] + m[ 2]*o.m[ 9] + m[ 3]*o.m[13];
        result.m[ 2] = m[ 0]*o.m[ 2] + m[ 1]*o.m[ 6] + m[ 2]*o.m[10] + m[ 3]*o.m[14];
        result.m[ 3] = m[ 0]*o.m[ 3] + m[ 1]*o.m[ 7] + m[ 2]*o.m[11] + m[ 3]*o.m[15];

        result.m[ 4] = m[ 4]*o.m[ 0] + m[ 5]*o.m[ 4] + m[ 6]*o.m[ 8] + m[ 7]*o.m[12];
        result.m[ 5] = m[ 4]*o.m[ 1] + m[ 5]*o.m[ 5] + m[ 6]*o.m[ 9] + m[ 7]*o.m[13];
        result.m[ 6] = m[ 4]*o.m[ 2] + m[ 5]*o.m[ 6] + m[ 6]*o.m[10] + m[ 7]*o.m[14];
        result.m[ 7] = m[ 4]*o.m[ 3] + m[ 5]*o.m[ 7] + m[ 6]*o.m[11] + m[ 7]*o.m[15];

        result.m[ 8] = m[ 8]*o.m[ 0] + m[ 9]*o.m[ 4] + m[10]*o.m[ 8] + m[11]*o.m[12];
        result.m[ 9] = m[ 8]*o.m[ 1] + m[ 9]*o.m[ 5] + m[10]*o.m[ 9] + m[11]*o.m[13];
        result.m[10] = m[ 8]*o.m[ 2] + m[ 9]*o.m[ 6] + m[10]*o.m[10] + m[11]*o.m[14];
        result.m[11] = m[ 8]*o.m[ 3] + m[ 9]*o.m[ 7] + m[10]*o.m[11] + m[11]*o.m[15];

        result.m[12] = m[12]*o.m[ 0] + m[13]*o.m[ 4] + m[14]*o.m[ 8] + m[15]*o.m[12];
        result.m[13] = m[12]*o.m[ 1] + m[13]*o.m[ 5] + m[14]*o.m[ 9] + m[15]*o.m[13];
        result.m[14] = m[12]*o.m[ 2] + m[13]*o.m[ 6] + m[14]*o.m[10] + m[15]*o.m[14];
        result.m[15] = m[12]*o.m[ 3] + m[13]*o.m[ 7] + m[14]*o.m[11] + m[15]*o.m[15];

        return result;
    }
    mat4 inv()
    {
        // copied from SO

        mat4 inv;

        inv.m[0] = m[5]  * m[10] * m[15] -
                   m[5]  * m[11] * m[14] -
                   m[9]  * m[6]  * m[15] +
                   m[9]  * m[7]  * m[14] +
                   m[13] * m[6]  * m[11] -
                   m[13] * m[7]  * m[10];

        inv.m[4] = -m[4]  * m[10] * m[15] +
                    m[4]  * m[11] * m[14] +
                    m[8]  * m[6]  * m[15] -
                    m[8]  * m[7]  * m[14] -
                    m[12] * m[6]  * m[11] +
                    m[12] * m[7]  * m[10];

        inv.m[8] = m[4]  * m[9] * m[15] -
                   m[4]  * m[11] * m[13] -
                   m[8]  * m[5] * m[15] +
                   m[8]  * m[7] * m[13] +
                   m[12] * m[5] * m[11] -
                   m[12] * m[7] * m[9];

        inv.m[12] = -m[4]  * m[9] * m[14] +
                     m[4]  * m[10] * m[13] +
                     m[8]  * m[5] * m[14] -
                     m[8]  * m[6] * m[13] -
                     m[12] * m[5] * m[10] +
                     m[12] * m[6] * m[9];

        inv.m[1] = -m[1]  * m[10] * m[15] +
                    m[1]  * m[11] * m[14] +
                    m[9]  * m[2] * m[15] -
                    m[9]  * m[3] * m[14] -
                    m[13] * m[2] * m[11] +
                    m[13] * m[3] * m[10];

        inv.m[5] = m[0]  * m[10] * m[15] -
                   m[0]  * m[11] * m[14] -
                   m[8]  * m[2] * m[15] +
                   m[8]  * m[3] * m[14] +
                   m[12] * m[2] * m[11] -
                   m[12] * m[3] * m[10];

        inv.m[9] = -m[0]  * m[9] * m[15] +
                    m[0]  * m[11] * m[13] +
                    m[8]  * m[1] * m[15] -
                    m[8]  * m[3] * m[13] -
                    m[12] * m[1] * m[11] +
                    m[12] * m[3] * m[9];

        inv.m[13] = m[0]  * m[9] * m[14] -
                    m[0]  * m[10] * m[13] -
                    m[8]  * m[1] * m[14] +
                    m[8]  * m[2] * m[13] +
                    m[12] * m[1] * m[10] -
                    m[12] * m[2] * m[9];

        inv.m[2] = m[1]  * m[6] * m[15] -
                   m[1]  * m[7] * m[14] -
                   m[5]  * m[2] * m[15] +
                   m[5]  * m[3] * m[14] +
                   m[13] * m[2] * m[7] -
                   m[13] * m[3] * m[6];

        inv.m[6] = -m[0]  * m[6] * m[15] +
                    m[0]  * m[7] * m[14] +
                    m[4]  * m[2] * m[15] -
                    m[4]  * m[3] * m[14] -
                    m[12] * m[2] * m[7] +
                    m[12] * m[3] * m[6];

        inv.m[10] = m[0]  * m[5] * m[15] -
                    m[0]  * m[7] * m[13] -
                    m[4]  * m[1] * m[15] +
                    m[4]  * m[3] * m[13] +
                    m[12] * m[1] * m[7] -
                    m[12] * m[3] * m[5];

        inv.m[14] = -m[0]  * m[5] * m[14] +
                     m[0]  * m[6] * m[13] +
                     m[4]  * m[1] * m[14] -
                     m[4]  * m[2] * m[13] -
                     m[12] * m[1] * m[6] +
                     m[12] * m[2] * m[5];

        inv.m[3] = -m[1] * m[6] * m[11] +
                    m[1] * m[7] * m[10] +
                    m[5] * m[2] * m[11] -
                    m[5] * m[3] * m[10] -
                    m[9] * m[2] * m[7] +
                    m[9] * m[3] * m[6];

        inv.m[7] = m[0] * m[6] * m[11] -
                   m[0] * m[7] * m[10] -
                   m[4] * m[2] * m[11] +
                   m[4] * m[3] * m[10] +
                   m[8] * m[2] * m[7] -
                   m[8] * m[3] * m[6];

        inv.m[11] = -m[0] * m[5] * m[11] +
                     m[0] * m[7] * m[9] +
                     m[4] * m[1] * m[11] -
                     m[4] * m[3] * m[9] -
                     m[8] * m[1] * m[7] +
                     m[8] * m[3] * m[5];

        inv.m[15] = m[0] * m[5] * m[10] -
                    m[0] * m[6] * m[9] -
                    m[4] * m[1] * m[10] +
                    m[4] * m[2] * m[9] +
                    m[8] * m[1] * m[6] -
                    m[8] * m[2] * m[5];

        float det = m[0] * inv[0] +
                    m[1] * inv[4] +
                    m[2] * inv[8] +
                    m[3] * inv[12];


        if (det == 0) return mat4{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
        ASSERT(det != 0);

        for (int i = 0; i < 16; i++)
            inv.m[i] = inv.m[i] / det;

        return inv;
    }
};


mat4 operator *(mat4 m1, mat4 m2) { return m1.mul(m2); }



// mat4 mat4RotAxis(float radians)
// {
// }
mat4 mat4FromNewAxes(v3 x, v3 y, v3 z)
{
    mat4 result = {
        x.x, y.x, z.x,
        x.y, y.y, z.y,
        x.z, y.z, z.z,
    };
    return result;
}
mat4 mat4RotY(float radians)
{
    // rotate around Y axis
    mat4 rotateY = {
        cos(-radians), 0,  sin(-radians),   0,
        0,             1,  0,               0,
       -sin(-radians), 0,  cos(-radians),   0,
        0,             0,  0,               1
    };
    return rotateY;
}
mat4 mat4RotX(float radians)
{
    // rotate around X axis
    mat4 rotateX = {
        1,         0,               0,           0,
        0,     cos(-radians), sin(-radians),     0,
        0,    -sin(-radians), cos(-radians),     0,
        0,         0,              0,            1
    };
    return rotateX;
}
mat4 mat4RotZ(float radians)
{
    // rotate around Z axis
    mat4 rotateZ = {
        cos(-radians),   -sin(-radians),  0,      0,
        sin(-radians),    cos(-radians),  0,      0,
        0,                 0,             1,      0,
        0,                 0,             0,      1
    };
    return rotateZ;
}
mat4 mat4Translate(v3 delta)
{
    // rotate around Z axis
    mat4 translate = {
        1,   0,   0,   delta.x,
        0,   1,   0,   delta.y,
        0,   0,   1,   delta.z,
        0,   0,   0,   1
    };
    return translate;
}

mat4 mat4FromEulerAngles(v3 angles)
{
    return  mat4RotY(angles.y) * mat4RotX(angles.x) * mat4RotZ(angles.z);
}



//----------------



struct mat3
{
    float m[9];




    float ge(int c, int r)
    {
        ASSERT(c+r*3 >= 0 && c+r*3 < 9);
        return m[c + r*3];
    }
    void se(int c, int r, float v)
    {
        ASSERT(c+r*3 >= 0 && c+r*3 < 9);
        m[c + r*3] = v;
    }

    bool equ(mat3 other) {
        for (int i = 0; i<9; i++)
            if (m[i] != other.m[i])
                return false;
        return true;
    }

    mat3 mul(mat3 other)
    {
        mat3 result;
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                result.se(c, r,
                    ge(0,r)*other.ge(c,0) +
                    ge(1,r)*other.ge(c,1) +
                    ge(2,r)*other.ge(c,2));
        return result;
    }

    mat3 operator *(mat3 other) { return mul(other); }

    bool operator ==(mat3 other) { return equ(other); }

    bool operator !=(mat3 other) { return !equ(other); }

    float operator ()(int c, int r) { return ge(c,r); }

    float operator [](int i) { return m[i]; }


    mat3 tra() {
        mat3 result;
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                result.se(c, r, ge(r,c));
        return result;
    }

};


bool TestMatrix3() {

    mat3 test1 = {0,1,2,3,4,5,6,7,8};
    mat3 test2 = {6,7,8,9,1,2,3,4,5};
    mat3 resultGOOD = {15, 9, 12, 69, 45, 57, 123, 81, 102};
    mat3 resultBAD = {15, 9, 12, 69, 45, 57, 123, 81, 101};

    if (!resultGOOD.equ(test1.mul(test2)))
        return false;

    if (resultBAD.equ(test1.mul(test2)))
        return false;

    mat3 result1 = test1 * test2;
    if (resultGOOD != result1)
        return false;
    if (!(resultGOOD == result1))
        return false;
    if (resultBAD == result1)
        return false;
    if (!(resultBAD != result1))
        return false;


    mat3 result2 = test2 * test1;

    mat3 traTest1 = test1.tra();

    int a = 1 + 1;

    return true;  // all tests pass

}




// todo: better way to do this?
// mat functions on vectors


v3 TransformV3(v3 p, mat4 mat)
{
    v3 result;
    result.x = p.x*mat[ 0] + p.y*mat[ 1] + p.z*mat[ 2] + 1.0f*mat[ 3];
    result.y = p.x*mat[ 4] + p.y*mat[ 5] + p.z*mat[ 6] + 1.0f*mat[ 7];
    result.z = p.x*mat[ 8] + p.y*mat[ 9] + p.z*mat[10] + 1.0f*mat[11];
    return result;
}
v4 TransformV4(v4 p, mat4 mat)
{
    v4 result = p;
    result.x = p.x*mat[ 0] + p.y*mat[ 1] + p.z*mat[ 2] + p.w*mat[ 3];
    result.y = p.x*mat[ 4] + p.y*mat[ 5] + p.z*mat[ 6] + p.w*mat[ 7];
    result.z = p.x*mat[ 8] + p.y*mat[ 9] + p.z*mat[10] + p.w*mat[11];
    result.w = p.x*mat[12] + p.y*mat[13] + p.z*mat[14] + p.w*mat[15];
    return result;
}



// v3 v3::transform(mat4 mat)
// {
//   v3 orig = {x, y, z};
//   return TransformV3(orig, mat);
// }

v3 v3::rotateAroundAxis(v3 axis, float a) {
  v3 anyVectorNotParallel;
  anyVectorNotParallel.x = axis.y;
  anyVectorNotParallel.y = axis.z;
  anyVectorNotParallel.z = axis.x;
  v3 ortho1 = axis.cross(anyVectorNotParallel);
  v3 ortho2 = axis.cross(ortho1);

  mat4 covertSoAxisIsX = mat4FromNewAxes(axis, ortho1, ortho2);
  v3 pointTransformedSoAxisIsX = {x, y, z};
  pointTransformedSoAxisIsX = TransformV3(pointTransformedSoAxisIsX, covertSoAxisIsX);

  v3 rotatedButWrongCoordSystem = pointTransformedSoAxisIsX.rotateAroundX(a);
  v3 result = TransformV3(rotatedButWrongCoordSystem, covertSoAxisIsX.inv());

  return result;
}