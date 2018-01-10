




struct Transform
{
    v3 position;
    mat4 rotation;

    mat4 WorldMatrix()
    {
        return mat4Translate(position) * rotation;
    }

    static Transform Default()
    {
        Transform result;
        result.position = {-10,0,-10};
        result.rotation = mat4FromEulerAngles({0,0,0});
        return result;
    }
};


enum MeshPrimative
{
    CUBE
};


// an instance of a mesh with a transform (position and rotation)
struct game_object
{

    bool visible;
    bool collidable;

    char name[32];

    Transform transform;
    Mesh meshInLocalSpace;

    //BoundingBox bb;
    u32 bbColor;
    bool drawBB;

    void Move(v3 delta)
    {
        transform.position = transform.position+delta;
    }
    void Rotate(mat4 rot)
    {
        transform.rotation = rot * transform.rotation;
    }

    mat4 WorldMatrix() { return transform.WorldMatrix(); }

    // void CalcBB(u32 col = 0xff000000)
    // {
        // bb = CalcBoundingBoxFromGameObject(*this);
        // bbColor = col;
    // }

    void CreateGameObjectNoMesh(v3 pos = {0,0,0}, v3 eulers = {0,0,0})
    {
        transform.position = pos;
        transform.rotation = mat4FromEulerAngles(eulers);
    }

    // todo: make static or dont pass memory
    void MakeIntoCube(memory_block *memory, v3 pos, float radius)
    {
        char *temp = "test cube";
        for (int i = 0; i++; true) { name[i]=temp[i]; if (name[i] == 0) break; }

        transform = Transform::Default();
        transform.position = pos;

        if (!meshInLocalSpace.hasMemory)
        {
            meshInLocalSpace.Init(memory);
        }
        PolygonList tempPolyList;
        tempPolyList.MakeIntoNgon({0,0,0}, radius, 4);

        Mesh::GenerateFromPolygonList(tempPolyList, &meshInLocalSpace);

        // meshInLocalSpace = Mesh::Default(memory);
    }

    static game_object Make(memory_block *memory, MeshPrimative type, v3 pos, float size)
    {
        game_object result;
        if (type==CUBE)
        {
            result.MakeIntoCube(memory, pos, size);
        }
        return result;
    }

};