#include <unistd.h>

#define __USE_XOPEN2K
#define __USE_BSD
#define __USE_LARGEFILE64
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "File.h"

NAMESPACE_METHOD_IMPLEMENT(File, Detect, bool, const char* dest)
{
    struct stat64 st;
    if (stat(dest, &st))
    {
        return false;
    }

    return S_ISREG(st.st_mode) && !access(dest, F_OK);
}

NAMESPACE_METHOD_IMPLEMENT(File, Size, uint64_t, const char* dest)
{
    struct stat64 st;
    if (stat(dest, &st))
    {
        return 0ULL;
    }

    return st.st_size;
}

NAMESPACE_METHOD_IMPLEMENT(File, Remove, bool, const char* dest)
{
    return !unlink(dest);
}

NAMESPACE_METHOD_IMPLEMENT(File, Link, bool, const char* src, const char* dest)
{
    return !link(src, dest);
}

NAMESPACE_METHOD_IMPLEMENT(File, Move, bool, const char* src, const char* dest)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(File, Link, src, dest) && NONINSTANTIAL_NAMESPACE_METHOD_CALL(File, Remove, src);
}

NAMESPACE_METHOD_IMPLEMENT(File, DetectDirectory, bool, const char* dest)
{
    struct stat64 st;
    if (stat(dest, &st))
    {
        return false;
    }

    return S_ISDIR(st.st_mode) && !access(dest, F_OK);
}

NAMESPACE_METHOD_IMPLEMENT(File, MakeDirectory, void, char* path)
{
    if (!access(path, F_OK))
    {
        return;
    }

    char *p = path;
    for (; *p; ++p)
    {
        if ('/' == *p)
        {
            *p = 0;
            mkdir(path, 0755);
            *p = '/';
        }
    }

    mkdir(path, 0755);
}

NAMESPACE_INITIALIZE(File)
{
    NAMESPACE_METHOD_INITIALIZE(File, Detect)
    NAMESPACE_METHOD_INITIALIZE(File, Size)
    NAMESPACE_METHOD_INITIALIZE(File, Remove)
    NAMESPACE_METHOD_INITIALIZE(File, Link)
    NAMESPACE_METHOD_INITIALIZE(File, Move)
    NAMESPACE_METHOD_INITIALIZE(File, DetectDirectory)
    NAMESPACE_METHOD_INITIALIZE(File, MakeDirectory)
};
