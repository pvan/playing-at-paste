

struct input_state
{
    bool w;
    bool a;
    bool s;
    bool d;
    bool up;
    bool down;
    bool left;
    bool right;

    bool space;
    bool shift;
    bool ctrl;

    bool tab;

    bool escape;

    bool z;
    bool x;
    bool c;

    bool q;
    bool e;
    bool r;
    bool t;
    bool y;

    bool row[12];

    bool n1;
    bool n2;
    bool n3;
    bool n4;
    bool n5;
    bool n6;
    bool n7;
    bool n8;
    bool n9;
    bool n0;
    bool nMinus;
    bool nEquals;


    bool mouseL;
    bool mouseM;
    bool mouseR;
    float mouseX;
    float mouseY;

    float deltaMouseX;
    float deltaMouseY;

    int mouseWheelDelta;

    // bool lockMouseToCenterThisFrame;
    bool allowOSToMoveMouseBeforeNextFrame;

    bool tellOSToHideCursor;


    bool squareL;
    bool squareR;

    bool carrotL;
    bool carrotR;
    bool questionMark;

    bool m;

};
