#ifndef __SIZESTRING_H__
#define __SIZESTRING_H__

#include <stdint.h>
#include "ClassSimulator.h"

#pragma pack(push)
#pragma pack(1)
/* Pointer | Size Forward | Data*/
PLAIN_OLD_DATA(_SizeString)
{
};
#pragma pack(pop)

NAMESPACE_METHOD_DECLARE(SizeString, EvaluatePhysicalSize, uint64_t, uint64_t size);
NAMESPACE_METHOD_DECLARE(SizeString, Initialize, _SizeString*, void* buffer, uint64_t bufferSize, const char* data, int64_t dataSize, OUT uint64_t* physicalSize);
NAMESPACE_METHOD_DECLARE(SizeString, New, _SizeString*, const char* data, int64_t size);
NAMESPACE_METHOD_DECLARE(SizeString, Clone, _SizeString*, const _SizeString* data);
NAMESPACE_METHOD_DECLARE(SizeString, Delete, void, _SizeString* data);
NAMESPACE_METHOD_DECLARE(SizeString, Size, uint64_t, const _SizeString* data);
NAMESPACE_METHOD_DECLARE(SizeString, Data, char*, _SizeString* data);
NAMESPACE_METHOD_DECLARE(SizeString, Begin, void*, _SizeString* data);
NAMESPACE_METHOD_DECLARE(SizeString, End, void*, _SizeString* data);
NAMESPACE_METHOD_DECLARE(SizeString, MetadataSize, uint64_t, const _SizeString* data);
NAMESPACE_METHOD_DECLARE(SizeString, CopyTo, _SizeString*, const _SizeString* data, void* buffer, uint64_t bufferSize, OUT void** end);
NAMESPACE_METHOD_DECLARE(SizeString, Compare, int64_t, _SizeString* data1, _SizeString* data2);

NAMESPACE(SizeString)
{
    NAMESPACE_METHOD_DEFINE(SizeString, EvaluatePhysicalSize);
    NAMESPACE_METHOD_DEFINE(SizeString, Initialize);
    NAMESPACE_METHOD_DEFINE(SizeString, New);
    NAMESPACE_METHOD_DEFINE(SizeString, Clone);
    NAMESPACE_METHOD_DEFINE(SizeString, Delete);
    NAMESPACE_METHOD_DEFINE(SizeString, Size);
    NAMESPACE_METHOD_DEFINE(SizeString, Data);
    NAMESPACE_METHOD_DEFINE(SizeString, Begin);
    NAMESPACE_METHOD_DEFINE(SizeString, End);
    NAMESPACE_METHOD_DEFINE(SizeString, MetadataSize);
    NAMESPACE_METHOD_DEFINE(SizeString, CopyTo);
    NAMESPACE_METHOD_DEFINE(SizeString, Compare);
};

#endif /* __SIZESTRING_H__ */
