

struct RendererMemory
{
    Mesh meshBuffer1; // tempy? right now used for transforming and clipping, respectively
    Mesh meshBuffer2; // rename these to "frame memory" or similar?

    Mesh dummyMesh;

    bitmapf depthBuffer;

    void init(memory_block *memory, int w, int h)
    {
        // TODO: what to do with MAX_VERTS.. Mesh class variable?
        meshBuffer1.Init(memory, MAX_VERTS);
        meshBuffer2.Init(memory, MAX_VERTS);
        dummyMesh.Init(memory, MAX_VERTS);

        depthBuffer.allocate(w, h, memory);
    }

};


