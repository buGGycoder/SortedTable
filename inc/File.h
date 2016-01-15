#ifndef __FILE_H__
#define __FILE_H__

#include <stdbool.h>
#include <stdint.h>

#include "ClassSimulator.h"

NAMESPACE_METHOD_DECLARE(File, Detect, bool, const char* dest);
NAMESPACE_METHOD_DECLARE(File, Size, uint64_t, const char* dest);
NAMESPACE_METHOD_DECLARE(File, Remove, bool, const char* dest);
NAMESPACE_METHOD_DECLARE(File, Link, bool, const char* src, const char* dest);
NAMESPACE_METHOD_DECLARE(File, Move, bool, const char* src, const char* dest);
NAMESPACE_METHOD_DECLARE(File, DetectDirectory, bool, const char* path);
NAMESPACE_METHOD_DECLARE(File, MakeDirectory, void, char* path);

NAMESPACE(File)
{
    NAMESPACE_METHOD_DEFINE(File, Detect);
    NAMESPACE_METHOD_DEFINE(File, Size);
    NAMESPACE_METHOD_DEFINE(File, Remove);
    NAMESPACE_METHOD_DEFINE(File, Link);
    NAMESPACE_METHOD_DEFINE(File, Move);
    NAMESPACE_METHOD_DEFINE(File, DetectDirectory);
    NAMESPACE_METHOD_DEFINE(File, MakeDirectory);
};

#endif /* __FILE_H__ */
