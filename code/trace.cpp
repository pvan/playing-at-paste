


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