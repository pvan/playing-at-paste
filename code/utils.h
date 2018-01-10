#ifndef UTILS_H
#define UTILS_H



#define PRINT(...) { char buf[256]; sprintf(buf, __VA_ARGS__); OutputDebugString(buf); }



#define LENGTH(array) (sizeof(array) / sizeof((array)[0]))

#define KILOBYTES(x) (1024LL*(x))
#define MEGABYTES(x) (1024LL*KILOBYTES(x))
#define GIGABYTES(x) (1024LL*MEGABYTES(x))
#define TERABYTES(x) (1024LL*GIGABYTES(x))


// for now just use this
#include <assert.h>
#define ASSERT assert


float GetWallClockSeconds()
{
    LARGE_INTEGER counter; QueryPerformanceCounter(&counter);
    LARGE_INTEGER freq; QueryPerformanceFrequency(&freq);
    return (float)counter.QuadPart / (float)freq.QuadPart;
}



// STRING


#define NBSP '\u00A0'


// this is case sensitive
bool StringBeginsWith(const char *str, const char *front)
{
    while (*front)
    {
        if (!*str)
            return false;

        if (*front != *str)
            return false;

        front++;
        str++;
    }
    return true;
}
// this is case sensitive
bool StringEndsWith(const char *str, const char *end)
{
    int strLen = strlen(str);
    int endLen = strlen(end);
    for (int i = 0; i < endLen; i++)
    {
        // recall str[len-1] is the last character of the string
        if (str[(strLen-1)-i] != end[(endLen-1)-i]) return false;
    }
    return true;
}
bool Test_StringEndsWith()
{
    assert(StringEndsWith("test.test.txt", ".txt"));
    assert(StringEndsWith("test.txt", ".txt"));
    assert(StringEndsWith("test.txt", "t"));
    assert(StringEndsWith("test.txt\n", "t\n"));
    assert(StringEndsWith("test.txt", ""));
    assert(StringEndsWith("", ""));
    assert(!StringEndsWith("test.txt", ".txt2"));
    assert(!StringEndsWith("test.txt2", ".txt"));
    assert(!StringEndsWith("", "txt"));
    assert(!StringEndsWith("test.txt\n", ".txt"));
    return true;
}

void DirectoryFromPath(char *path)
{
    char *new_end = 0;
    for (char *c = path; *c; c++)
    {
        if (*c == '\\' || *c == '/')
            new_end = c+1;
    }
    if (new_end != 0)
        *new_end = '\0';
}



#endif