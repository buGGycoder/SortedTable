#include <string.h>

#include "Path.h"
#include "SizedString.h"

NAMESPACE_METHOD_IMPLEMENT(Path, ParseNameIndex, int, const char* path, int size)
{
    int i = (size ? size : strlen(path)) - 1;
    for (; 0 < i && '/' != path[i]; --i)
        ;
    return ('/' == path[i]) + i;
}


NAMESPACE_METHOD_IMPLEMENT(Path, Equals, bool, const char* thiz, const char* other)
{
    return !strcmp(thiz, other);
}

NAMESPACE_METHOD_IMPLEMENT(Path, IsSibling, bool, const char* thiz, const char* other)
{
    int iNameThiz = 0;
    int iNameOther = 0;
    int sizeThiz = strlen(thiz);
    int sizeOther = strlen(other);
    1 < sizeThiz && (sizeThiz -= '/' == thiz[sizeThiz - 1]);
    1 < sizeOther && (sizeOther -= '/' == other[sizeOther - 1]);
    iNameThiz = NONINSTANTIAL_NAMESPACE_METHOD_CALL(Path, ParseNameIndex, thiz, sizeThiz);
    iNameOther = NONINSTANTIAL_NAMESPACE_METHOD_CALL(Path, ParseNameIndex, other, sizeOther);
    if (iNameThiz != iNameOther || !iNameThiz)
    {
        return false;
    }

    return !strncmp(thiz, other, iNameThiz);
}

NAMESPACE_METHOD_IMPLEMENT(Path, IsParentOf, bool, const char* thiz, const char* other)
{
    int iNameOther = 0;
    int sizeThiz = strlen(thiz);
    int sizeOther = strlen(other);
    1 < sizeThiz && (sizeThiz -= '/' == thiz[sizeThiz - 1]);
    1 < sizeOther && (sizeOther -= '/' == other[sizeOther - 1]);
    if (!(iNameOther = NONINSTANTIAL_NAMESPACE_METHOD_CALL(Path, ParseNameIndex, other, sizeOther)))
    {
        return false;
    }

    if (iNameOther - 1 != sizeThiz)
    {
        return false;
    }

    return !strncmp(thiz, other, sizeThiz);
}

/*NAMESPACE_METHOD_IMPLEMENT(Path, IsChildOf, bool, const char* thiz, const char* other)
{
    return Path.IsParentOf(other, thiz);
}

NAMESPACE_METHOD_IMPLEMENT(Path, IsAncestorOf, bool, const char* thiz, const char* other)
{
    int sizeThiz = strlen(thiz);
    int sizeOther = strlen(other);
    1 < sizeThiz && (sizeThiz -= '/' == thiz[sizeThiz - 1]);
    1 < sizeOther && (sizeOther -= '/' == other[sizeOther - 1]);
    if (sizeThiz >= sizeOther)
    {
        return false;
    }

    if (strncmp(thiz, other, sizeThiz))
    {
        return false;
    }

    return '/' == thiz[sizeThiz - 1] || '/' == other[sizeThiz];
}

NAMESPACE_METHOD_IMPLEMENT(Path, IsDescendantOf, bool, const char* thiz, const char* other)
{
    return Path.IsAncestorOf(other, thiz);
}

NAMESPACE_METHOD_IMPLEMENT(Path, Name, const char*, const char* thiz)
{
    return thiz + NONINSTANTIAL_NAMESPACE_METHOD_CALL(Path, ParseNameIndex, thiz, 0);
}

NAMESPACE_METHOD_IMPLEMENT(Path, NewParent, SIZED_STRING_T, const char* thiz)
{
    int iNameIndex = NONINSTANTIAL_NAMESPACE_METHOD_CALL(Path, ParseNameIndex, thiz, 0);
    return iNameIndex ? SizedString.New(thiz, iNameIndex - 1) : NULL;
}

NAMESPACE_METHOD_IMPLEMENT(Path, NewChild, SIZED_STRING_T, const char* thiz, const char* name)
{
    SIZED_STRING_T child = SizedString.New(thiz, strlen(name) + 1);
    SizedString.AppendAsPath(child, name, 0);
    return child;
}

NAMESPACE_INITIALIZE(Path)
{
    NAMESPACE_METHOD_INITIALIZE(Path, Equals)
    NAMESPACE_METHOD_INITIALIZE(Path, IsParentOf)
    NAMESPACE_METHOD_INITIALIZE(Path, IsChildOf)
    NAMESPACE_METHOD_INITIALIZE(Path, IsAncestorOf)
    NAMESPACE_METHOD_INITIALIZE(Path, IsDescendantOf)
    NAMESPACE_METHOD_INITIALIZE(Path, Name)
    NAMESPACE_METHOD_INITIALIZE(Path, NewParent)
    NAMESPACE_METHOD_INITIALIZE(Path, NewChild)
};*/
