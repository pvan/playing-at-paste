




const int MAX_VERTS = 1000;

const int MAX_MATERIALS_PER_MESH = 12;

struct Mesh
{
    bool hasMemory = false;
    int vertCountAllocated = 0;

    // recall vert has attributes, eg color, uv, etc
    vert *verts;
    int vertCount;

    int *tris;
    int triCount;

    // doin this for now instead of submeshes
    // for easy changing of texture on individ surfaces
    int textureCount;
    bitmap textures[MAX_MATERIALS_PER_MESH];  // only this many for now i guess...


    void Init(memory_block *memory)
    {
        Init(memory, MAX_VERTS);  // todo: what to do about MAX_VERTS
    }

    void Init(memory_block *memory, int maxVerts)
    {
        verts = (vert*)memory->allocate(maxVerts * sizeof(vert) * 4); // why *4??? todo
        tris = (int*)memory->allocate(maxVerts * sizeof(vert) * 4);

        vertCount = 0;
        triCount = 0;

        vertCountAllocated = maxVerts; // is this used??

        hasMemory = true;
    }


    static void Copy(Mesh in, Mesh *out)
    {
        for (int i = 0; i < in.vertCount; i++)
            out->verts[i] = in.verts[i];

        for (int i = 0; i < in.triCount; i++)
            out->tris[i] = in.tris[i];

        out->vertCount = in.vertCount;
        out->triCount = in.triCount;

        out->textureCount = in.textureCount;
        for (int i = 0; i < in.textureCount; i++)
            out->textures[i] = in.textures[i];
    }


    static void Transform(Mesh *mesh, mat4 matrix)
    {
        for (int i = 0; i < mesh->vertCount; i++)
        {
            mesh->verts[i] = matrix * mesh->verts[i];
        }
    }


    static void GenerateFromPolygonList(PolygonList inBrush, Mesh *outMesh)
    {
        int vertCount = 0;
        int triCount = 0;
        for (int i = 0; i < inBrush.polygonCount; i++)
        {
            for (int j = 1; j < inBrush.polygons[i].vertCount-1; j++)
            {
                outMesh->verts[vertCount++] = inBrush.polygons[i].verts[0];
                outMesh->verts[vertCount++] = inBrush.polygons[i].verts[(j+0)%inBrush.polygons[i].vertCount];
                outMesh->verts[vertCount++] = inBrush.polygons[i].verts[(j+1)%inBrush.polygons[i].vertCount];

                outMesh->tris[triCount++] = vertCount-3;
                outMesh->tris[triCount++] = vertCount-2;
                outMesh->tris[triCount++] = vertCount-1;
            }

        }
        outMesh->vertCount = vertCount;
        outMesh->triCount = triCount;

        outMesh->textureCount = inBrush.polygonCount;
        ASSERT(outMesh->textureCount <= MAX_MATERIALS_PER_MESH);
        if (outMesh->textureCount > MAX_MATERIALS_PER_MESH) outMesh->textureCount = MAX_MATERIALS_PER_MESH;
        for (int i = 0; i < outMesh->textureCount; i++) {
            // TODO: mesh might not support as many textures as brush
            outMesh->textures[i] = inBrush.polygons[i].texture;
        }
    }

    static Mesh Default(memory_block *memory)
    {
        Mesh result;
        result.Init(memory, MAX_VERTS);
        return result;
    }

};

