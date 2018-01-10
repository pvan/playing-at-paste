

// -----------

struct Quaternion
{
    float x;
    float y;
    float z;
    float w;

    Quaternion norm()
    {
        Quaternion q;
        float n = sqrtf(x*x+y*y+z*z+w*w);
        q.x = x/n;
        q.y = y/n;
        q.z = z/n;
        q.w = w/n;
        return q;
    }


    mat4 RotationMatrix()
    {
        mat4 result = {
            1.0f - 2.0f*y*y - 2.0f*z*z, 2.0f*x*y - 2.0f*z*w, 2.0f*x*z + 2.0f*y*w, 0.0f,
            2.0f*x*y + 2.0f*z*w, 1.0f - 2.0f*x*x - 2.0f*z*z, 2.0f*y*z - 2.0f*x*w, 0.0f,
            2.0f*x*z - 2.0f*y*w, 2.0f*y*z + 2.0f*x*w, 1.0f - 2.0f*x*x - 2.0f*y*y, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        return result;
    }

    v3 RotateV3(v3 in)
    {
        return TransformV3(in, RotationMatrix());
    }
};


Quaternion QuaternionFromAxisAngle(v3 ax, float rads)
{
    Quaternion q = {sin(rads/2)*ax.x, sin(rads/2)*ax.y, sin(rads/2)*ax.z, cos(rads/2)};
    return q;
}


