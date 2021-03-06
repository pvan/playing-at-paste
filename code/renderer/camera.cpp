
// why didn't we need these before? did we include this before windows.h?
#undef near
#undef far

struct Camera
{

    // todo: use a transform for this?
    v3 pos;
    float heading;
    float pitch;
    float near;
    float far;

    float fovDegrees;
    float aspectRatio;

    bool init = false;

    // general transformation matrix form:
    // xi, yi, zi, tx,
    // xj, yj, zj, ty,
    // xk, yk, zk, tz,
    //  0,  0,  0,  1  // 0 instead of 1 in BR to ignore translations

    //  where xi, xj, xk is the vector in the old system that is now the X axis
    //  and tx, ty, tz is the translations in each of the new axes
    //  (to get translations in the old axes, make sep. matrix and combine?)



    // void SetRotByPitchHeading(float pitch, float heading)
    // {
    //     look = v3{0,0,1}.rotateAroundX(pitch).rotateAroundY(heading);
    //     up = v3{0,1,0}.rotateAroundX(pitch).rotateAroundY(heading);
    // }

    mat4 GenerateLookMatrix()
    {

        mat4 translate = {
            1, 0, 0, -pos.x,
            0, 1, 0, -pos.y,//-playerHeight,
            0, 0, 1, -pos.z,
            0, 0, 0, 1
        };

        // rotate around Y axis
        mat4 rotateY = {
            cosf(-heading), 0, sinf(-heading), 0,
            0,          1,  0, 0,
           -sinf(-heading), 0, cosf(-heading), 0,
            0, 0, 0, 1
        };
        // rotate around X axis
        mat4 rotateX = {
            1,            0,            0, 0,
            0, cosf(-pitch), sinf(-pitch), 0,
            0,-sinf(-pitch), cosf(-pitch), 0,
            0, 0, 0, 1
        };


        return rotateX * rotateY * translate;

    }

    mat4 GeneratePerspectiveMatrix()
    {
        float fovRads = RADIANS(fovDegrees);
        float xi = 1.0f/tanf(fovRads/2.0f);
        float yj = xi * aspectRatio;
        mat4 viewToClip = {
            xi,  0,             0,                  0,
            0,  yj,             0,                  0,
            0,  0,   (far+near)/(far-near),  -(2*far*near)/(far-near),
            0,  0,              1,                  0
        };
        return viewToClip;
    }

    mat4 GenerateOrthoMatrix(bitmap *screen, float scale = 1.0f)
    {
        float sw = screen->width/2;
        float sh = screen->height/2;
        sw /= scale;
        sh /= scale;
        mat4 ortho = {
            1/sw,   0,      0,                  0,
            0,      1/sh,   0,                  0,
            0,      0,    2/(far-near),  -(far+near)/(far-near),
            0,      0,      0,                  1
        };
        return ortho;
    }


    v3 CalcWorldUp()
    {
        v4 localUp = {0, 1, 0, 0};
        mat4 transform = GenerateLookMatrix().inv();
        return TransformV4(localUp, transform).xyz();
    }
    v3 CalcWorldRight()
    {
        v4 localRight = {1, 0, 0, 0};  // correct? or z?
        mat4 transform = GenerateLookMatrix().inv();
        return TransformV4(localRight, transform).xyz();
    }

    void setFov(float newFov)
    {
        fovDegrees = newFov; //todo cap this at 180?
    }
    void setAspectRatio(float newRatio)
    {
        aspectRatio = newRatio;
    }

    void Init(v3 position, float h, float p, float n, float f, float fov, int dw, int dh)
    {
        pos = position;
        heading = h;
        pitch = p;
        near = n;
        far = f;

        fovDegrees = fov;
        aspectRatio = (float)dw/(float)dh;

        init = true;
    }
    void Init(float n, float f, float fov, int dw, int dh)
    {
        Init(v3{0,0,0}, 0, 0, n, f, fov, dw, dh);
    }


    // just keep the args flat here so we don't need any weird structs to call this
    void updateWithFPSControls(bool left, bool right, bool up, bool down, bool shift,
                               bool w, bool s, bool a, bool d, bool space, bool ctrl,
                               bool fovM, bool fovP, float deltaMouseX, float deltaMouseY, float dt)
    {
        ASSERT(init);

        float xSens = 0.5f;
        float ySens = 0.5f;

        // if (lockMouseToCenter && mouseLookOn) {
            heading += (deltaMouseX*xSens/100);
            pitch -= (deltaMouseY*ySens/50);
        // }

        if (left) heading -= xSens/3;
        if (right) heading += xSens/3;
        if (up) pitch += ySens/3;
        if (down) pitch -= ySens/3;


        float moveSpeed = 40.0f;
        if (shift) moveSpeed *= 3;
        float posDelta = moveSpeed * dt;

        if (heading > PI) heading -= 2*PI;
        if (heading < -PI) heading += 2*PI;

        v3 lookXZ = v3{0,0,1}.rotateAroundY(heading);
        v3 rightXZ = v3{1,0,0}.rotateAroundY(heading);
        v3 upDir = v3{0,1,0};//.rotateAroundY(heading);

        if (w) { pos = pos+(lookXZ*posDelta); }
        if (s) { pos = pos-(lookXZ*posDelta); }
        if (a) { pos = pos-(rightXZ*posDelta); }
        if (d) { pos = pos+(rightXZ*posDelta); }
        if (space) { pos = pos+(upDir*posDelta); }
        if (ctrl) { pos = pos-(upDir*posDelta); }


        // if (playerPos.y < -playerHeight) playerPos.y = -playerHeight;
        if (pitch < -PI/2) pitch = -PI/2;
        if (pitch > PI/2) pitch = PI/2;

        if (fovM) setFov(fovDegrees-1);
        if (fovP) setFov(fovDegrees+1);

    }

};


struct key
{
    char code;
    bool pressed;
};
struct mInput
{
    char *code;
    float amount;
};

struct fps_bindings
{
    key forward;
    key back;
    key left;
    key right;

    key turnL;
    key turnR;
    key turnUp;
    key turnDown;

    mInput lookTurn;
    mInput lookTilt;
};
