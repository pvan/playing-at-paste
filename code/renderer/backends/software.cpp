



struct RendererMemory
{
    Mesh meshBuffer1; // tempy? right now used for transforming and clipping, respectively
    Mesh meshBuffer2; // rename these to "frame memory" or similar?

    Mesh dummyMesh;

    bitmapf depthBuffer;

    void Init(memory_block *memory, int w, int h)
    {
        // TODO: what to do with MAX_VERTS.. Mesh class variable?
        meshBuffer1.Init(memory, MAX_VERTS);
        meshBuffer2.Init(memory, MAX_VERTS);
        dummyMesh.Init(memory, MAX_VERTS);

        depthBuffer.allocate(w, h, memory);
    }

};



namespace Software {


static void RenderBitmapOnBitmap(bitmap *source, bitmap *dest,
    v2 destPos = {0,0}, Rect subRect = {-1,-1,-1,-1}, float shrinkFactor = 1)
{

    if (subRect.x == -1)
    {
        subRect.x = 0;
        subRect.y = 0;
        subRect.w = (float)source->width;
        subRect.h = (float)source->height;
    }

    v2 screenPos = destPos;

    float posX = screenPos.x; //worldPos.x - cameraPos.x;
    float posY = screenPos.y; //worldPos.y - cameraPos.y;


    // not a complete check...
    if (subRect.w > source->width) subRect.w = source->width;
    if (subRect.h > source->height) subRect.h = source->height;

    // just assert for now..
    ASSERT(subRect.w + subRect.x <= source->width);
    ASSERT(subRect.h + subRect.y <= source->height);


    // clip source....

    int startingX = subRect.x;
    int startingY = subRect.y;
    if (posX < 0)
    {
        int pixelsOverScreen = -posX;
        startingX += pixelsOverScreen;
    }
    if (posY < 0)
    {
        int pixelsOverScreen = -posY;
        startingY += pixelsOverScreen;
    }

    int endX = subRect.x + subRect.w; //source->width;
    int endY = subRect.y + subRect.h; //source->height;

    if (posX + subRect.w > dest->width)
    {
        int pixelsOverScreen = (posX+subRect.w) - dest->width;
        endX -= pixelsOverScreen;
    }
    if (posY + subRect.h > dest->height)
    {
        int pixelsOverScreen = (posY+subRect.h) - dest->height;
        endY -= pixelsOverScreen;
    }


    // clip destination...

    int realDestX = posX;
    int realDestY = posY;

    if (realDestX < 0) realDestX = 0;
    if (realDestY < 0) realDestY = 0;


    int dy = realDestY;
    float floatsy = startingY;
    while (floatsy < endY)
    {
        int dx = realDestX;
        float floatsx = startingX;
        while (floatsx < endX)
        {
            int sx = (int)floatsx;
            int sy = (int)floatsy;

            u32 *src = ((u32*)source->pixels) + sx + (sy*source->width);
            u32 *dst = ((u32*)dest->pixels) + dx + (dy*dest->width);

            float srcA = (float)((*src>>24) & 0xFF);

            // this "should" not be needed, if we clip corrrectly
            // if (dx < dest->width && dy < dest->height && dx >= 0 && dy >= 0 &&
            if (srcA > 0)  // only draw if there's some alpha...
            {
                if (srcA == 255)
                {
                    *dst = *src;
                }
                else
                {
                    float srcR = (float)((*src>>16) & 0xFF);
                    float srcG = (float)((*src>>8) & 0xFF);
                    float srcB = (float)((*src>>0) & 0xFF);

                    float dstR = (float)((*dst>>16) & 0xFF);
                    float dstG = (float)((*dst>>8) & 0xFF);
                    float dstB = (float)((*dst>>0) & 0xFF);

                    float alpha = srcA / 255.0f;
                    float mixR = (alpha*(srcR-dstR)) + dstR;
                    float mixG = (alpha*(srcG-dstG)) + dstG;
                    float mixB = (alpha*(srcB-dstB)) + dstB;

                    *dst = MakeColor((u8)(mixR+0.5f), (u8)(mixG+0.5f), (u8)(mixB+0.5f), 255);
                }
            }
            dx++;
            floatsx = floatsx+shrinkFactor;
        }
        dy++;
        // sy+=shrinkFactor;
        floatsy = floatsy+shrinkFactor;
    }
}



static void FillBufferWithColor(bitmap *dest, u32 col)
{
    //if (col == 0x0)
    //    app.plat.MsgBox("black");

    for (int y = 0; y < dest->height; y++)
    {
        for (int x = 0; x < dest->width; x++)
        {
            int pixelOffset = y*dest->width + x;
            *(((u32*)dest->pixels) + pixelOffset) = col;
        }
    }
}



static u32 GetPixel(int x, int y, bitmap *source)
{
    // todo: check x y as received? (to look for calling code bugs)

    int wrappedX = mod_wrap(x, source->width);
    int wrappedY = mod_wrap(y, source->height);

    ASSERT(wrappedX >= 0 && wrappedY >= 0);
    ASSERT(wrappedX < source->width && wrappedY < source->height);

    int result = source->pixels[ wrappedX + wrappedY*source->width ];
    return result;
}
static u32 GetPixel(v2 pos, bitmap *source)
{
    return GetPixel(pos.x, pos.y, source);
}


static void SetPixel(int x, int y, bitmap *source, u32 col)
{
    if (x<source->width && y<source->height && x>=0 && y>=0)
    {
        int pixelOffset = y*source->width + x;
        *(((u32*)source->pixels) + pixelOffset) = col;
    }
}
static void SetPixel(v2 pos, bitmap *source, u32 col)
{
    SetPixel(pos.x, pos.y, source, col);
}




static u32 SampleTextureAt(float u, float v, bitmap *texture)
{
    int x = texture->width * u;
    // if (x > texture->width) x = x % texture->width;
    // if (x < 0) x = (x+texture->width+texture->width) % texture->width;

    int y = texture->height * v;
    // if (y > texture->height) y = y % texture->height;
    // if (y < 0) y = (y+texture->height+texture->height) % texture->height;

    return GetPixel(x, y, texture);
}





// determinant of
// ax bx cx
// ay by cy
// 1  1  1
static float Orient2D(v2 a, v2 b, v2 c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}





// sutherland-hodgman clipping algo
// structure based on benny's tutorial

static void ClipPolygonAtThisComponentValue(
        vert *inVerts, int inVertNum,
        vert *outVerts, int *outVertNum,
        int compoment, float componentCutoffValue)
{
    *outVertNum = 0;

    vert prev = inVerts[inVertNum-1];
    for (int v = 0; v < inVertNum; v++)
    {
        vert curr = inVerts[v];

        bool currIn = curr[compoment]*componentCutoffValue < curr.w;
        bool prevIn = prev[compoment]*componentCutoffValue < prev.w;
        if (currIn ^ prevIn)  // horray for xor, thx benny
        {
            // this finds proportionally how far along the line we are
            // going from prev vert to curr vert.. then lerps everything by that proportion
            float dPrev = prev.w - prev[compoment]*componentCutoffValue;
            float dCurr = curr.w - curr[compoment]*componentCutoffValue;
            float p = dPrev / (dPrev-dCurr);
            vert lerpP = {
                lerp(prev.x, curr.x, p),
                lerp(prev.y, curr.y, p),
                lerp(prev.z, curr.z, p),
                lerp(prev.w, curr.w, p),
                LerpCol(prev.c, curr.c, p),
                LerpCol(prev.origC, curr.origC, p), // shouldnt actually be used on the clipped tri
                lerp(prev.u, curr.u, p),
                lerp(prev.v, curr.v, p),
                prev.mat
            };
            outVerts[(*outVertNum)++] = lerpP;

            ASSERT(prev.mat == curr.mat);
        }
        if (currIn)
        {
            outVerts[(*outVertNum)++] = curr;
        }

        prev = curr;
    }
}



static void ClipTriangleInAxis(vert *verts, int *vertCount, int component)
{
    const int MAX_VERTS = 5*5 * 6 * 4 * 4;

    vert tempVerts[MAX_VERTS];
    int tempVertCount = 0;

    ClipPolygonAtThisComponentValue(
        verts, *vertCount,
        tempVerts, &tempVertCount,
        component, 1);

    ClipPolygonAtThisComponentValue(
        tempVerts, tempVertCount,
        verts, vertCount,
        component, -1);
}

static bool IsVertIn(vert v) {
    return !(v.x > v.w || v.x < -v.w ||
             v.y > v.w || v.y < -v.w ||
             v.z > v.w || v.z < -v.w);
}

static void ClipAllTrisToW(Mesh *inList, Mesh *outList)
{
    // just set to 0? na
    ASSERT(outList->vertCount == 0);
    ASSERT(outList->triCount == 0);

    // if (inList->vertCount

    for (int i = 0; i < inList->triCount; i+=3)
    {
        vert singleTriMesh[24]; // what max to use??
        singleTriMesh[0] = inList->verts[inList->tris[i+0]];
        singleTriMesh[1] = inList->verts[inList->tris[i+1]];
        singleTriMesh[2] = inList->verts[inList->tris[i+2]];
        int singleTriVertCount = 3;

        // don't clip if all verts inside, doesn't seem to help much actually
        if (!IsVertIn(inList->verts[inList->tris[i+0]]) ||
            !IsVertIn(inList->verts[inList->tris[i+1]]) ||
            !IsVertIn(inList->verts[inList->tris[i+2]]))
        {
            ClipTriangleInAxis(singleTriMesh, &singleTriVertCount, 0);
            ClipTriangleInAxis(singleTriMesh, &singleTriVertCount, 1);
            ClipTriangleInAxis(singleTriMesh, &singleTriVertCount, 2);
        }

        int startTriIndex = outList->vertCount;

        for (int j = 0; j < singleTriVertCount; j++)
            outList->verts[(outList->vertCount)++] = singleTriMesh[j];

        for (int j = 1; j < singleTriVertCount-1; j++)
        {
            outList->tris[(outList->triCount)++] = startTriIndex;
            outList->tris[(outList->triCount)++] = startTriIndex+j;
            outList->tris[(outList->triCount)++] = startTriIndex+j+1;
        }
    }

    outList->textureCount = inList->textureCount;
    for (int i = 0; i < inList->textureCount; i++)
        outList->textures[i] = inList->textures[i];
}


// bool IsTopLeft(vert v1, vert v2)
// {
    // // in a ccw trangle, left edges will always be going down
    // // (thx https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/)
    // if (v2.y < v1.y) // is left edge
    // {
        // return true;
    // }
    // else // not left but maybe top
    // {
        // // top edges must be horizontal
        // if (v1.y == v2.y)
        // {
            // // horizontal edges going left are def top edges (in ccw tris)
            // if (v2.x < v1.x)
            // {
                // return true;
            // }
        // }

        // // if we get here either it wasn't horizontal, or not going left (and not a left edge)
        // return false;
    // }
// }

static void RasterizeTriangles(Mesh *screenSpace, bitmap *screen, bitmapf *depthBuffer)
{
    vert *verts = screenSpace->verts;
    int vertCount = screenSpace->vertCount;
    int *tris = screenSpace->tris;
    int triCount = screenSpace->triCount;


    // float maxDepth = -100000.0f;
    // float minDepth = 100000.0f;

    int pixelsDrawn = 0;
    for (int ti = 0; ti < triCount; ti+=3)
    {
        vert t[3] = {
            verts[tris[ti+0]],
            verts[tris[ti+1]],
            verts[tris[ti+2]]
        };

        // flip the vertical uv so uv is always bottom left origin
        // if (!BOTTOM_LEFT_BITMAP_ORIGIN)
        {
            // t[0].v *= -1;
            // t[1].v *= -1;
            // t[2].v *= -1;
            t[0].v = 1 - t[0].v;
            t[1].v = 1 - t[1].v;
            t[2].v = 1 - t[2].v;
        }

        // use first vert to find material..
        // if they mismatch we could be in trouble
        int materialIndex = t[0].mat;
        ASSERT(t[0].mat == t[1].mat && t[1].mat == t[2].mat);


        // u32 baseCol = 0x66666666;
        // u32 baseCol = 0xffffffff;
        u32 baseCol = 0x00000000;

        // the traditional method
        // see chris hecker's articles for a thorough rundown of this method
        if (bool SCANLINE_METHOD = false) {

            // calc gradients for this tri..
            // ie, 1/z varies linearly over x and y.. so by how much does it?
            // taking chris hecker article's word for this for now, prove later
            // note c = 1/z or u/z or whatever you want to interpolate

            float c0;
            float c1;
            float c2;
            float y0 = t[0].y;
            float y1 = t[1].y;
            float y2 = t[2].y;
            float x0 = t[0].x;
            float x1 = t[1].x;
            float x2 = t[2].x;

            // i think this calc is related to the area of the triangle
            // compare Orient2D calc and this:
            //orient (2x-1x) * (3y-1y) - (2y-1y) * (3x-1x);
            //below  (2x-3x) * (1y-3y) - (2y-3y) * (1x-3x)
            // (yes, they are both determinants, right?)
            // (yes, they appear to calc the same thnig, but maybe cw ccw opposite???)
            float denom = (x1-x2)*(y0-y2)-(x0-x2)*(y1-y2);
            if (denom == 0) continue; // degenerate tri amirite, also /0 possible

            if (bool BACKFACE_CULLING = true) {
                // + or - is cw, other is ccw.. not sure which is which
                // i think we should be culling cw
                if (denom <= 0) continue;
            }

            c0 = 1.0f / t[0].z;
            c1 = 1.0f / t[1].z;
            c2 = 1.0f / t[2].z;
            float dc_dx = (((c1-c2)*(y0-y2))-((c0-c2)*(y1-y2)))/denom;
            float dc_dy = (((c1-c2)*(x0-x2))-((c0-c2)*(x1-x2)))/(-denom);

            c0 = t[0].u / t[0].z;
            c1 = t[1].u / t[1].z;
            c2 = t[2].u / t[2].z;
            float duc_dx = (((c1-c2)*(y0-y2))-((c0-c2)*(y1-y2)))/denom;
            float duc_dy = (((c1-c2)*(x0-x2))-((c0-c2)*(x1-x2)))/(-denom);

            c0 = t[0].v / t[0].z;
            c1 = t[1].v / t[1].z;
            c2 = t[2].v / t[2].z;
            float dvc_dx = (((c1-c2)*(y0-y2))-((c0-c2)*(y1-y2))) / denom;
            float dvc_dy = (((c1-c2)*(x0-x2))-((c0-c2)*(x1-x2))) / (-denom);



            // find top and bottom and mid verts
            // (we will go from top to bottom in the y,
            // creating x scan lines between each tri leg
            int topV = 0;
            float minY = t[topV].y;
            if (t[1].y < minY) { topV = 1; minY = t[topV].y; }
            if (t[2].y < minY) { topV = 2; minY = t[topV].y; }

            int botV = 0;
            float maxY = t[botV].y;
            if (t[1].y > maxY) { botV = 1; maxY = t[botV].y; }
            if (t[2].y > maxY) { botV = 2; maxY = t[botV].y; }

            int midV = 0;
            if (topV != 2 && botV != 2) midV = 2;
            if (topV != 1 && botV != 1) midV = 1;
            if (topV != 0 && botV != 0) midV = 0;
            float midY = t[midV].y;

            Line hypLin = {t[topV].xy(), t[botV].xy()};
            Line topLin = {t[topV].xy(), t[midV].xy()};
            Line botLin = {t[midV].xy(), t[botV].xy()};

            // linear components for each line
            // in the form x = ay + b
            // _a = x slope of edge
            // _b = x intercept

            float ha;
            // if (hypLin.end.y != hypLin.start.y)
                ha = (hypLin.end.x-hypLin.start.x)/(hypLin.end.y-hypLin.start.y);
            // else
            //     ha = 999999999.0f;
            float hb = hypLin.start.x - ha * hypLin.start.y;

            float ta;
            // if (topLin.end.y != topLin.start.y)
                ta = (topLin.end.x-topLin.start.x)/(topLin.end.y-topLin.start.y);
            // else
            //     ta = 999999999.0f;
            float tb = topLin.start.x - ta * topLin.start.y;

            float ba;
            // if (botLin.end.y != botLin.start.y)
                ba = (botLin.end.x-botLin.start.x)/(botLin.end.y-botLin.start.y);
            // else
            //     ba = 999999999.0f;
            float bb = botLin.start.x - ba * botLin.start.y;


            // // these are y-slopes, not x-slopes
            // float ha = hypLin.a();
            // float ta = topLin.a();
            // float ba = botLin.a();

            // float hb = hypLin.b();
            // float tb = topLin.b();
            // float bb = botLin.b();



            bool hypOnL = true;
            if (t[midV].x > t[botV].x) hypOnL = false;


            int startY = ceilf(t[topV].y);
            int endY = ceilf(t[botV].y); // -0 here means < in loop (-1 would be <=)

            float deltaStart = startY - t[topV].y;

            float cRow = 1.0f / t[topV].z;
            cRow += deltaStart * dc_dy;

            float ucRow = t[topV].u / t[topV].z;
            ucRow += deltaStart * duc_dy;

            float vcRow = t[topV].v / t[topV].z;
            vcRow += deltaStart * dvc_dy;

            // float wRow = 1.0f / t[topV].w;
            // wRow += deltaStart * dw_dy;

            // limit our loop to screen size
            if (startY<0) startY=0;  if (startY >= screen->height) startY = screen->height;
            if (endY<0) endY=0;  if (endY >= screen->height) endY = screen->height;

            // pretty much ignoring edge conditions / fill conventions for now,
            // you can see occational pixels missing on seams (or is that a bug from elsewhere?)
            for (int y = startY; y < endY; y++) {

                // int hx = Ceil(ha * y + hb);
                // int tx = Ceil(ta * y + tb);
                // int bx = Ceil(ba * y + bb);

                // int minX = 0;
                // int maxX = 0;
                // if (y < t[midV].y)
                // {
                //     minX = (tx<hx)?tx:hx;
                //     maxX = (tx>hx)?tx:hx;
                // }
                // else
                // {
                //     minX = (bx<hx)?bx:hx;
                //     maxX = (bx>hx)?bx:hx;
                // }

                int minX = 0;
                int maxX = 0;
                if (y < t[midV].y)
                {
                    float hypX = ceil(ha * y + hb);
                    float topX = ceil(ta * y + tb);
                    if (hypX < topX) {
                        minX = hypX;
                        maxX = topX;
                    } else {
                        minX = topX;
                        maxX = hypX;
                    }
                }
                else
                {
                    float hypX = ceil(ha * y + hb);
                    float botX = ceil(ba * y + bb);
                    if (hypX < botX) {
                        minX = hypX;
                        maxX = botX;
                    } else {
                        minX = botX;
                        maxX = hypX;
                    }
                }

                // limit our loop to screen size
                if (minX<0) minX=0;  if (minX >= screen->width) minX = screen->width;
                if (maxX<0) maxX=0;  if (maxX >= screen->width) maxX = screen->width;

                float c = cRow + (minX-t[topV].x)*dc_dx;
                float uc = ucRow + (minX-t[topV].x)*duc_dx;
                float vc = vcRow + (minX-t[topV].x)*dvc_dx;
                // float w = wRow + (minX-t[topV].x)*dw_dx;
                for (int x = minX; x < maxX; x++) {

                    ASSERT(x < screen->width && x>=0);
                    ASSERT(y < screen->height && y>=0);


                    float depth = 1.0f / c;

                    if (depth > depthBuffer->pixels[x + y*screen->width])
                    {

                        // TODO: pull out this color mixing / pixel set
                        // so it's the same on both rasterizing methods


                        depthBuffer->pixels[x + y*screen->width] = depth;



                        // baseCol = t[0].c;
                        // ColorMix3


                        float u = (uc*depth);
                        float v = (vc*depth);
                        u32 texCol = SampleTextureAt(u, v, &screenSpace->textures[materialIndex]);

                        // float grey = (1-depth ) * (255.0f/2.0f);
                        float grey = (-depth) * (255.0f);
                        // float grey = -(c+3) * (255.0f/2);
                        u32 depthCol = MakeColor(grey, grey, grey);

                        // u32 col = texCol;
                        // u32 col = depthCol;
                        u32 col = LerpCol(texCol, baseCol, 0.5f);
                        // u32 col = lerpCol5050(texCol, baseCol);

                        SetPixel(x, y, screen, col);

                        // // hacky mouse picking...
                        // if (x == globalMouseX && y == globalMouseY) {
                        //     globalMouseZ = depth;
                        //     baseCol = 0x00ffff00;
                        // }
                        pixelsDrawn++;
                    }

                    c += dc_dx;
                    uc += duc_dx;
                    vc += dvc_dx;
                    // w += dw_dx;
                }
                cRow += dc_dy;
                ucRow += duc_dy;
                vcRow += dvc_dy;
            }

        }

        // barycentric method, a more modern approach
        // niavely this is slower(ithink), but the idea is to use parallelism
        // e.g. process four pixels at once using simd, and/or multiple threads?
        // and maybe there's a way to utilize the cache better with this method?
        // best info: https://fgiesen.wordpress.com/2013/02/06/the-barycentric-conspirac/
        else {

            float twiceAreaOfTri = Orient2D(t[0].xy(), t[1].xy(), t[2].xy());
            if (twiceAreaOfTri == 0) continue; // degenerate tri amirite

            if (bool BACKFACE_CULLING = true) {
                // + or - is cw, other is ccw.. not sure which is which
                if (twiceAreaOfTri >= 0) continue;
            }


            // // check type of edge for fill conventions
            // // (draw pixel only if it lands on top left edge or strictly inside tri)
            // int bias0 = IsTopLeft(t[1], t[2]) ? 0 : -1;
            // int bias1 = IsTopLeft(t[2], t[0]) ? 0 : -1;
            // int bias2 = IsTopLeft(t[0], t[1]) ? 0 : -1;


            float xstep01 = t[1].x - t[0].x;
            float xstep12 = t[2].x - t[1].x;
            float xstep20 = t[0].x - t[2].x;
            float ystep01 = t[0].y - t[1].y;
            float ystep12 = t[1].y - t[2].y;
            float ystep20 = t[2].y - t[0].y;

            float maxX = t[0].x;
            if (t[1].x > maxX) maxX = t[1].x;
            if (t[2].x > maxX) maxX = t[2].x;
            float minX = t[0].x;
            if (t[1].x < minX) minX = t[1].x;
            if (t[2].x < minX) minX = t[2].x;

            float maxY = t[0].y;
            if (t[1].y > maxY) maxY = t[1].y;
            if (t[2].y > maxY) maxY = t[2].y;
            float minY = t[0].y;
            if (t[1].y < minY) minY = t[1].y;
            if (t[2].y < minY) minY = t[2].y;

            // check entire bounding box (widest possible interp)
            minY = floor(minY);
            minX = floor(minX);
            maxY = ceil(maxY);
            maxX = ceil(maxX);

            // doing this below for now (not any more)
            // limit our loop to at most the screen soze
            if (maxX >= screen->width) maxX = screen->width-1;
            if (maxX < 0) maxX = 0;
            if (minX >= screen->width) minX = screen->width-1;
            if (minX < 0) minX = 0;
            if (maxY >= screen->height) maxY = screen->height-1;
            if (maxY < 0) maxY = 0;
            if (minY >= screen->height) minY = screen->height-1;
            if (minY < 0) minY = 0;

            // barycentric coords at minX, minY
            float w0_row = Orient2D(t[1].xy(), t[2].xy(), v2{minX,minY}) ;// / twiceAreaOfTri;
            float w1_row = Orient2D(t[2].xy(), t[0].xy(), v2{minX,minY}) ;// / twiceAreaOfTri;
            float w2_row = Orient2D(t[0].xy(), t[1].xy(), v2{minX,minY}) ;// / twiceAreaOfTri;


            for (int y = minY; y <= maxY; y++)
            {
                // bary coords at start of this row...
                float w0 = w0_row;
                float w1 = w1_row;
                float w2 = w2_row;

                for (int x = minX; x <= maxX; x++)
                {

                    ASSERT(x < screen->width);
                    ASSERT(y < screen->height);
                    ASSERT(x >= 0);
                    ASSERT(y >= 0);

                    // >= here means if inside (>) or ON the edge (=)
                    // (for now just draw if on any edge, we aught to onyl get overdraw then, right?)
                    // can change to >0 and use bias0-2 added above to change >0 to >-1 (same as >=0)
                    // as needed (if line is not TL line)
                    // also note < or > depends on if determ (orient2d) is + or - for this winding order
                    if (w0<=0 && w1<=0 && w2<=0) {

                        float ww0 = w0 / twiceAreaOfTri;
                        float ww1 = w1 / twiceAreaOfTri;
                        float ww2 = w2 / twiceAreaOfTri;

                        // float ww0 = Orient2D(t[1].xy(), t[2].xy(), v2{x,y}) / twiceAreaOfTri;
                        // float ww1 = Orient2D(t[2].xy(), t[0].xy(), v2{x,y}) / twiceAreaOfTri;
                        // float ww2 = Orient2D(t[0].xy(), t[1].xy(), v2{x,y}) / twiceAreaOfTri;

                        float depth = (ww0*t[0].z + ww1*t[1].z + ww2*t[2].z);
                        if (depth > depthBuffer->pixels[x + y*screen->width])
                        {

                            // u32 vertCol = ColorMix2(
                            //    (u8*)&t[0].c, (u8*)&t[1].c, (u8*)&t[2].c,
                            //    ww0*255.999, ww1*255.999, ww2*255.999).hex;
                            // u32 vertCol = ColorMix(t[0].c,t[1].c,t[2].c,ww0, ww1, ww2);
                            // u32 vertCol;

                            u32 vertCol;

                            Color c1 = Color{t[0].c};
                            Color c2 = Color{t[1].c};
                            Color c3 = Color{t[2].c};

                            vertCol = ColorMix3(c1, c2, c3, ww0, ww1, ww2);

                            // perspective correct...
                            // (interp u/w and divide by 1/w)
                            // works because a / w is linear?
                            float u = ww0*(t[0].u/t[0].z) + ww1*(t[1].u/t[1].z) + ww2*(t[2].u/t[2].z);
                            float v = ww0*(t[0].v/t[0].z) + ww1*(t[1].v/t[1].z) + ww2*(t[2].v/t[2].z);
                            float divisor = ww0*(1/t[0].z) + ww1*(1/t[1].z) + ww2*(1/t[2].z);
                            //u32 texCol = SampleTextureAt(u/divisor, v/divisor, &screenSpace->textures[materialIndex]);


                            // // // for now just use tex until we can speed up mixing
                            // // u32 finalCol = texCol;
                            //  u32 finalCol = LerpCol(texCol, vertCol, 0.5);
                            //u32 finalCol = LerpCol(FloatToGreyValueColor(depth, 1, 500), 0xffffffff, 0.5f);
                            u32 finalCol = FloatToGreyValueColor(depth, 1, 20); // TODO: add texture support back in
                            // todo: here: finish adapting this.. add tex back?
                            // weight color by something else? (dist to verts or something?)

                            // u32 finalCol = 0xff888888;

                            // if (depth > maxDepth) maxDepth = depth;
                            // if (depth < minDepth) minDepth = depth;

                            // so i think depth is just in units at the moment
                            // so min/max is near/far planes
                            depthBuffer->pixels[x + y*screen->width] = depth;
                            // SetPixel(x, y, screen, cols[ti/3]);


                            //if (randf() < 0.5f)
                            SetPixel(x, y, screen, finalCol);

                            pixelsDrawn++;
                        }
                    }

                    // also note if we start using this type of bias trick,
                    // we have to account for the off-by-1 coords we'll end up with
                    w0 += ystep12;// + bias0;
                    w1 += ystep20;// + bias1;
                    w2 += ystep01;// + bias2;

                }


                w0_row += xstep12;
                w1_row += xstep20;
                w2_row += xstep01;

            }
        } // end barycentric method
    } // end of triangle triangle loop

    // // overdraw (actually just all pixels drawn now)
    // DebugPrintString("pixelsDraw %i", pixelsDrawn);

    // app.plat.PrintFloat(maxDepth);
    // app.plat.PrintFloat(minDepth);

}




static void CovertMeshNDCToScreenSpace(Mesh *meshNDC, Mesh *meshScreenSpace, bitmap *screen)
{
    ASSERT(meshNDC->vertCount == meshScreenSpace->vertCount);

    float sw = screen->width;
    float sh = screen->height;

    // rot 180 around x and offset by screen size
    mat4 screenTrns = {
        sw/2, 0, 0, sw/2,//screenCenter.x,
        0,-sh/2, 0, sh/2,//screenCenter.y,
        0, 0,-1, 0,
        0, 0, 0, 1
    };

    for (int v = 0; v < meshScreenSpace->vertCount; v++)
    {
        meshScreenSpace->verts[v] = screenTrns * meshNDC->verts[v];
    }
}

static void CovertMeshClipSpaceToNDC(Mesh *meshClipSpace, Mesh *meshNDC)
{

    ASSERT(meshClipSpace->vertCount == meshNDC->vertCount);

    for (int v = 0; v < meshNDC->vertCount; v++)
    {
        meshNDC->verts[v].x = meshClipSpace->verts[v].x / meshClipSpace->verts[v].w;
        meshNDC->verts[v].y = meshClipSpace->verts[v].y / meshClipSpace->verts[v].w;

        // z=w fixes a number of issue in the rasterer
        // but maybe we should be using w in there instead of z??
        // (we'd have to preserve the original ws which feels weird)
        // clipToNDC->verts[v].z = clipToNDC->verts[v].z / clipToNDC->verts[v].w;
        meshNDC->verts[v].z = meshClipSpace->verts[v].w;

        // definitely not right, e.g. uv=0,0 should be 0,0 regardless of coord space
        // also think colors: same at the verticies no matter what
        // clipToNDC->verts[v].u = clipToNDC->verts[v].u / clipToNDC->verts[v].w;
        // clipToNDC->verts[v].v = clipToNDC->verts[v].v / clipToNDC->verts[v].w;

        meshNDC->verts[v].w = 1; // set to 1 so later transforms work ok
    }

}






// // should demonstrate our bitmaps are all origin in the top left...
// void DebugKindaIfyDoubleCheckTexture(bitmap *tex, bitmap *screen)
// {
    // for (float u = 0; u < 1; u+=0.01f)
    // {
        // for (float v = 0; v < 1; v+=0.01f)
        // {
            // u32 texCol = SampleTextureAt(u, v, tex);
            // SetPixel(u*100, v*100, screen, texCol);
        // }
    // }
    // // RenderBitmapSubRect(&assets->testUv, {0, 0, 200, 200}, {50,50}, screen, PIXEL_SIZE);
// }


static void DrawMeshDestructive(Mesh *mesh, Camera cam, bitmap *screen, RendererMemory *renderer)
{

    mat4 worldToView = cam.GenerateLookMatrix();
    mat4 viewToClip = cam.GeneratePerspectiveMatrix();
    // mat4 viewToClip = cam.GenerateOrthoMatrix(screen, cam.pos.z+10);
    mat4 worldViewPerspective = viewToClip * worldToView;
    Mesh::Transform(mesh, worldViewPerspective);


    //Mesh::Copy(*mesh, &clippedMesh);  // no clipping


    // clip to W if clip before perspective divide
    // otherwise clip to 1 i would think
    Mesh *clippedMesh = &renderer->meshBuffer2;
    ClipAllTrisToW(mesh, clippedMesh);


    Mesh *meshNDC = clippedMesh;
    CovertMeshClipSpaceToNDC(clippedMesh, meshNDC);


    Mesh *meshScreenSpace = meshNDC;
    CovertMeshNDCToScreenSpace(meshNDC, meshScreenSpace, screen);



    // note all the depths are -, with more negative beign further away
    // 0 meaning uninitialized
    // anything with an actual depth of 0 should have been clipped earlier (ithink)
    // (actually, now a depth of 0 means in the exact middle??

    RasterizeTriangles(meshNDC, screen, &renderer->depthBuffer);



    // if (bool displayDepthAtMouse = false)
    // {
    //     for (int x = 0; x < screen->width; x++)
    //     {
    //         for (int y = 0; y < screen->height; y++)
    //         {
    //             if (app.input.mouseX == x && app.input.mouseY == y)
    //             {
    //                 float depthV = renderer->depthBuffer.pixels[x + y*screen->width];
    //                 app.plat.PrintFloat(depthV);
    //             }
    //         }
    //     }
    // }


    // if (bool debugDrawDepth = false)
    // {
    //     for (int x = 0; x < screen->width; x++)
    //     {
    //         for (int y = 0; y < screen->height; y++)
    //         {
    //             float depthV = renderer->depthBuffer.pixels[x + y*screen->width];
    //             float grey = (depthV + 1) * (255.0f/2);
    //             SetPixel(x, y, screen, MakeColor(-grey, -grey, -grey));
    //         }
    //     }
    // }




}

// void DrawMesh(Mesh origMesh, Camera cam, bitmap *gameScreen, RendererMemory *renderer)
// {
    // Mesh tempMesh = renderer->meshBuffer1;
    // CopyMesh(origMesh, &tempMesh);

    // DrawMeshDestructive(tempMesh, cam, gameScreen, renderer);
// }

static void DrawGameObject(game_object obj, Camera cam, bitmap *gameScreen, RendererMemory *renderer)
{
    Mesh worldSpaceMesh = renderer->meshBuffer1;
    Mesh::Copy(obj.meshInLocalSpace, &worldSpaceMesh);

    Mesh::Transform(&worldSpaceMesh, obj.transform.WorldMatrix());

    DrawMeshDestructive(&worldSpaceMesh, cam, gameScreen, renderer);
}





// if we reset this between drawing two things, the second thign will be strictly drawn over the previous
static void ResetDepthBuffer(RendererMemory *renderer)
{
    // reset depth buffer, note + is towards us, so start with very -
    for (int x = 0; x < renderer->depthBuffer.width; x++) {
        for (int y = 0; y < renderer->depthBuffer.height; y++) {
           renderer->depthBuffer.pixels[x + y*renderer->depthBuffer.width] = -100000.0f;
        }
    }
}


// // ----------------





// // i dont think this does any clipping amirite?
// v2 WorldPointToScreen(v3 worldPoint, Camera cam, bitmap *screen)
// {

    // mat4 worldToView = cam.GenerateLookMatrix();
    // mat4 viewToClip = cam.GeneratePerspectiveMatrix();

    // mat4 worldViewPerspective = viewToClip * worldToView;


    // v4 result = TransformV4(v4w1(worldPoint), worldViewPerspective);

    // // perspective divide
    // result.x = result.x / result.w;
    // result.y = result.y / result.w;
    // result.z = result.w;
    // result.w = 1;

    // // todo: check if behind near plane
    // if (result.z < 0)
    // {
        // return {0,0};
    // }

    // // NDC to "actual device coords"
    // // (actually, "whatever bitmap we're rendering to coords")


    // float sw = screen->width;
    // float sh = screen->height;
    // // rot 180 around x and offset by screen size
    // mat4 screenTrns = {
        // sw/2, 0, 0, sw/2,//screenCenter.x,
        // 0,-sh/2, 0, sh/2,//screenCenter.y,
        // 0, 0,-1, 0,
        // 0, 0, 0, 1
    // };

    // result = TransformV4(result, screenTrns);

    // return result.xy();

// }


// Ray CalcRayForMousePicking(float mx, float my, Camera cam, bitmap *screen)
// {

    // mat4 worldToView = cam.GenerateLookMatrix();
    // mat4 viewToClip = cam.GeneratePerspectiveMatrix();

    // float sw = screen->width;
    // float sh = screen->height;
    // // rot 180 around x and offset by screen size
    // mat4 screenTrns = {
        // sw/2, 0, 0, sw/2,//screenCenter.x,
        // 0,-sh/2, 0, sh/2,//screenCenter.y,
        // 0, 0,-1, 0,
        // 0, 0, 0, 1
    // };



    // v4 mouseDevicePos = {mx, my, 1, 1};
    // v4 mouseClipPos = TransformV4(mouseDevicePos, screenTrns.inv());
    // v4 mouseViewPos = TransformV4(mouseClipPos, viewToClip.inv());
    // v4 mouseWorldPos = TransformV4(mouseViewPos, worldToView.inv());

    // // DebugPrintString("devc: %f, %f, %f", mouseDevicePos.x, mouseDevicePos.y, mouseDevicePos.z);
    // // DebugPrintString("clip: %f, %f, %f", mouseClipPos.x, mouseClipPos.y, mouseClipPos.z);
    // // DebugPrintString("view: %f, %f, %f", mouseViewPos.x, mouseViewPos.y, mouseViewPos.z);
    // // DebugPrintString("wrld: %f, %f, %f", mouseWorldPos.x, mouseWorldPos.y, mouseWorldPos.z);


    // Ray result = {cam.pos, (mouseWorldPos.xyz() - cam.pos).normalized()};

    // // DebugPrintString("ray: %f, %f, %f", result.direction.x, result.direction.y, result.direction.z);

    // return result;
// }




// void RenderLine3D(v3 start, v3 end, Camera cam, bitmap *screen, u32 col)
// {
    // v2 screenStart = WorldPointToScreen(start, cam, screen);
    // v2 screenEnd = WorldPointToScreen(end, cam, screen);

    // // just clip the whole line if one point is 0,0
    // // (WorldPointToScreen snaps to 0,0 for now if point is behind camera)
    // if ((screenStart.x == 0 && screenStart.y == 0) ||
        // (screenEnd.x == 0 && screenEnd.y == 0))
    // {
    // }
    // else
    // {
        // RenderLine(screenStart, screenEnd, screen, col);
    // }
// }


// // void RenderBB(BoundingBox bb, Camera cam, bitmap *gameScreen, RendererMemory *renderer, u32 col)
// // {
    // // v3 p1 = {bb.min.x, bb.min.y, bb.min.z};
    // // v3 p2 = {bb.max.x, bb.min.y, bb.min.z};
    // // RenderLine3D(p1, p2, cam, gameScreen, col);
    // // p1 = {bb.min.x, bb.min.y, bb.min.z};
    // // p2 = {bb.min.x, bb.max.y, bb.min.z};
    // // RenderLine3D(p1, p2, cam, gameScreen, col);
    // // p1 = {bb.min.x, bb.min.y, bb.min.z};
    // // p2 = {bb.min.x, bb.min.y, bb.max.z};
    // // RenderLine3D(p1, p2, cam, gameScreen, col);

    // // p1 = {bb.max.x, bb.max.y, bb.max.z};
    // // p2 = {bb.min.x, bb.max.y, bb.max.z};
    // // RenderLine3D(p1, p2, cam, gameScreen, col);
    // // p1 = {bb.max.x, bb.max.y, bb.max.z};
    // // p2 = {bb.max.x, bb.min.y, bb.max.z};
    // // RenderLine3D(p1, p2, cam, gameScreen, col);
    // // p1 = {bb.max.x, bb.max.y, bb.max.z};
    // // p2 = {bb.max.x, bb.max.y, bb.min.z};
    // // RenderLine3D(p1, p2, cam, gameScreen, col);
// // }


// void RenderWireFrameMesh(Mesh *mesh, Camera cam, bitmap *gameScreen, RendererMemory *renderer, u32 col)
// {
    // for (int i = 0; i < mesh->triCount; i+=3)
    // {
        // v3 p1 = mesh->verts[mesh->tris[i+0]].xyz();
        // v3 p2 = mesh->verts[mesh->tris[i+1]].xyz();
        // v3 p3 = mesh->verts[mesh->tris[i+2]].xyz();
        // RenderLine3D(p1, p2, cam, gameScreen, col);
        // RenderLine3D(p2, p3, cam, gameScreen, col);
        // RenderLine3D(p3, p1, cam, gameScreen, col);
    // }
// }
// void RenderWireFrameGO(GameObject *go, Camera cam, bitmap *gameScreen, RendererMemory *renderer, u32 col)
// {
    // Mesh *mesh = &go->meshInLocalSpace;
    // mat4 worldMat = go->WorldMatrix();
    // for (int i = 0; i < mesh->triCount; i+=3)
    // {
        // v3 p1 = TransformV3(mesh->verts[mesh->tris[i+0]].xyz(), worldMat);
        // v3 p2 = TransformV3(mesh->verts[mesh->tris[i+1]].xyz(), worldMat);
        // v3 p3 = TransformV3(mesh->verts[mesh->tris[i+2]].xyz(), worldMat);
        // RenderLine3D(p1, p2, cam, gameScreen, col);
        // RenderLine3D(p2, p3, cam, gameScreen, col);
        // RenderLine3D(p3, p1, cam, gameScreen, col);
    // }
// }

// void RenderGOBB(GameObject go, Camera cam, bitmap *screen, RendererMemory *renderer)
// {
    // if (go.visible)
        // RenderBB(go.bb, cam, screen, renderer, go.bbColor);
// }


// void AddHitToHighlight(MeshHit hit, Highlight *hl)
// {

    // hl->AddTriangleVerts(hit.go, hit.triIndex);

    // // the rest of this becomes harder if we dont store the tri index (intead of a vert*)
    // // hl->main.ExpandToConnectedVerts(mesh); // adds any tris that share already hl verts
    // // hl->secondary.
// }


// // -------------




// void RenderQueue::Add(RenderPrimative newType, u32 color, v3 np1, v3 np2, v3 np3)
// {
    // if (newType == LINE3D)
    // {
        // RenderQueueEntry entry = {newType, color, np1, np2, np3};
        // entries[entryCount++] = entry;
    // }
// }
// void RenderQueue::Render(Camera camera, bitmap *gameScreen)
// {
    // for (int i = 0; i < entryCount; i++)
    // {
        // if (entries[i].type == LINE3D) RenderLine3D(entries[i].p1, entries[i].p2, camera, gameScreen, entries[i].c);
    // }
// }
// void RenderQueue::Reset()
// {
    // entryCount = 0;
// }


// -----------------------

// todo: move to better spot
// struct Font
// {
    // // add letter size and things here
// };

// enum RenderType
// {
    // POINT,
    // LINE2D,
    // TEXT,
    // MESH
// }

// struct RenderCommand
// {
    // RenderType type;
    // v3 location;
    // v2 start;
    // v2 end;
    // bitmap *dest;
    // Font *font;
// };

// void RenderPoint(RenderCommand cmd)
// {
    // //RenderPo
// }

// void RenderLine2D(RenderCommand cmd)
// {
    // RenderLine(cmd.start, cmd.end, cmd.dest, cmd.col);
// }

// void RenderText(RenderCommand cmd)
// {
    // RenderText(cmd.msg, cmd.start, cmd.end, cmd.font);
// }

// void RenderMesh(RenderCommand cmd)
// {
    // DrawGameObject(cmd.obj, cmd.cam, cmd.dest);
// }

// void RenderRenderQueue(RenderQueue queue)
// {
    // for (int i = 0; i < queue.count; i++)
    // {
        // RenderCommand command = queue.commands[i];

        // switch(command.type)
        // {
            // case POINT:   RenderPoint(command);
            // case LINE2D:  RenderLine2D(command);
            // case TEXT:    RenderText(command);
            // case MESH:    RenderMesh(command);
        // }

    // }
// }


// }


static void DrawScene(Scene *scene, Camera *cam, bitmap *dest, RendererMemory *mem)
{
    ResetDepthBuffer(mem);

    // sort objects here?

    for (int i = 0; i < scene->gameObjectCount; i++)
    {
        game_object go2 = scene->gameObjects[i];
        DrawGameObject(go2, *cam, dest, mem);
    }

}


};



