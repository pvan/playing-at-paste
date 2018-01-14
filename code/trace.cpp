


// accessor that returns black outside the image bounds
u32 pixelat(int x, int y, u32 *img, int iw, int ih)
{
    if (x < 0 || x > iw) return 0;
    if (y < 0 || y > ih) return 0;
    return img[y*iw+x];
}

void FindBoundryPoints(u32 *img, int iw, int ih, v2i *outList, int maxOut, int *outCount)
{
    *outCount = 0;

    v2i start = {-1,-1};
    v2i current;
    v2i backtrace;

    // find a start point
    for (int x = 0; x < iw; x++)
    {
        for (int y = 0; y < ih; y++)
        {
            if (pixelat(x,y, img,iw,ih) != 0)
            {
                start = {x,y};
                current = {x,y};
                if (x == 0) backtrace = {x, y-1};
                else backtrace = {x-1, y};
                goto foundstart;
            }
        }
    }
    foundstart:

    // check if we found any start point (if not, probably blank bitmap)
    if (start.x == -1)
    {
        *outCount = -1; //todo: just return 0 probably better
        return;
    }

    // PRINT("start %i, %i\n", start.x, start.y);

    // these need to be in clockwise order
    v2i checkpixels[] = {{-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}};
    int checkcount = 8;

    while (current != start || *outCount == 0)
    {
        outList[(*outCount)++] = current;
        if (*outCount >= maxOut) break;

        int backtrace_index = 0;
        for (int i = 0; i < checkcount; i++)
        {
            v2i delta = backtrace - current;
            if (delta.x == checkpixels[i].x && delta.y == checkpixels[i].y)
            {
                backtrace_index = i;
                break;
            }
        }

        for (int i = 0; i < checkcount; i++)
        {
            int checki = (backtrace_index+i+1) % checkcount; // note start 1 clockwise from backtrace
            v2i check = current+checkpixels[checki];
            if (pixelat(check.x,check.y, img,iw,ih) != 0)
            {
                backtrace = current;
                current = check;
                break;
            }
        }
    }

}


float perpdist(v2 a, v2 b, v2 p)
{
    float num = absf( (b.y-a.y)*p.x - (b.x-a.x)*p.y + b.x*a.y - b.y*a.x );
    float den = sqrtf( (b.y-a.y)*(b.y-a.y) + (b.x-a.x)*(b.x-a.x) );
    return num / den;
}
float perpdisti(v2i a, v2i b, v2i p)
{
    return perpdist({(float)a.x,(float)a.y}, {(float)b.x,(float)b.y}, {(float)p.x,(float)p.y});
}

// note *outcount must be 0 !
// note in and out cannot be the same memory
void DouglasPeuckerSimplify(v2i *in, int incount, v2i *out, int *outcount, float threshold)
{
    if (incount <= 0) return;

    // find point max dist from line formed by start/end points
    float dmax = 0;
    int imax;
    for (int i = 0; i < incount; i++)
    {
        float d = perpdisti(in[0], in[incount-1], in[i]);
        if (d > dmax)
        {
            dmax = d;
            imax = i;
        }
    }

    // recurse if distance is too large
    if (dmax > threshold)
    {
        v2i *seg1 = in;
        int seg1count = imax;
        DouglasPeuckerSimplify(seg1, seg1count, out, outcount, threshold);

        v2i *seg2 = in+imax;
        int seg2count = incount-imax;
        DouglasPeuckerSimplify(seg2, seg2count, out, outcount, threshold);
    }
    else
    {
        out[(*outcount)++] = in[0];
        out[(*outcount)++] = in[incount-1];
    }
}