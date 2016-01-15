#ifndef __SIZEDSTRING_H__
#define __SIZEDSTRING_H__

#include <stdint.h>
#include "ClassSimulator.h"

#pragma pack(push)
#pragma pack(1)
/* Size Backward | Pointer | Data*/
PLAIN_OLD_DATA(_SizedString)
{
    char data[0];
};
#pragma pack(pop)

NAMESPACE_METHOD_DECLARE(SizedString, EvaluatePhysicalSize, uint64_t, uint64_t size);
NAMESPACE_METHOD_DECLARE(SizedString, Initialize, _SizedString*, void* buffer, uint64_t bufferSize, const char* data, int64_t dataSize, OUT uint64_t* physicalSize);
NAMESPACE_METHOD_DECLARE(SizedString, New, _SizedString*, const char* data, int64_t size);
NAMESPACE_METHOD_DECLARE(SizedString, Clone, _SizedString*, const _SizedString* data);
NAMESPACE_METHOD_DECLARE(SizedString, Delete, void, _SizedString* data);
NAMESPACE_METHOD_DECLARE(SizedString, Size, uint64_t, const _SizedString* data);
NAMESPACE_METHOD_DECLARE(SizedString, Data, char*, _SizedString* data);
NAMESPACE_METHOD_DECLARE(SizedString, Begin, void*, _SizedString* data);
NAMESPACE_METHOD_DECLARE(SizedString, End, void*, _SizedString* data);
NAMESPACE_METHOD_DECLARE(SizedString, MetadataSize, uint64_t, const _SizedString* data);
NAMESPACE_METHOD_DECLARE(SizedString, CopyTo, _SizedString*, const _SizedString* data, void* buffer, uint64_t bufferSize, OUT void** end);
NAMESPACE_METHOD_DECLARE(SizedString, Compare, int64_t, _SizedString* data1, _SizedString* data2);

NAMESPACE(SizedString)
{
    NAMESPACE_METHOD_DEFINE(SizedString, EvaluatePhysicalSize);
    NAMESPACE_METHOD_DEFINE(SizedString, Initialize);
    NAMESPACE_METHOD_DEFINE(SizedString, New);
    NAMESPACE_METHOD_DEFINE(SizedString, Clone);
    NAMESPACE_METHOD_DEFINE(SizedString, Delete);
    NAMESPACE_METHOD_DEFINE(SizedString, Size);
    NAMESPACE_METHOD_DEFINE(SizedString, Data);
    NAMESPACE_METHOD_DEFINE(SizedString, Begin);
    NAMESPACE_METHOD_DEFINE(SizedString, End);
    NAMESPACE_METHOD_DEFINE(SizedString, MetadataSize);
    NAMESPACE_METHOD_DEFINE(SizedString, CopyTo);
    NAMESPACE_METHOD_DEFINE(SizedString, Compare);
};

#endif /* __SIZEDSTRING_H__ */
