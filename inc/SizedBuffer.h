#ifndef __SIZEDBUFFER_H__
#define __SIZEDBUFFER_H__

#include <stdbool.h>
#include <stdint.h>
#include "ClassSimulator.h"

#pragma pack(push)
#pragma pack(1)
/* Size Backward | Capacity Backward | Pointer | Data*/
PLAIN_OLD_DATA(_SizedBuffer)
{
    char data[0];
};
#pragma pack(pop)

NAMESPACE_METHOD_DECLARE(SizedBuffer, EvaluatePhysicalSize, uint64_t, uint64_t capacity);
NAMESPACE_METHOD_DECLARE(SizedBuffer, Initialize, _SizedBuffer*, void* buffer, uint64_t bufferSize, OUT uint64_t* capacity);
NAMESPACE_METHOD_DECLARE(SizedBuffer, InitializeByString, _SizedBuffer*, void* buffer, uint64_t bufferSize, const char* data, int64_t dataSize, OUT void** end);
NAMESPACE_METHOD_DECLARE(SizedBuffer, New, _SizedBuffer*, uint64_t capacity);
NAMESPACE_METHOD_DECLARE(SizedBuffer, Clone, _SizedBuffer*, const _SizedBuffer* data);
NAMESPACE_METHOD_DECLARE(SizedBuffer, Delete, void, _SizedBuffer* data);
NAMESPACE_METHOD_DECLARE(SizedBuffer, Capacity, uint64_t, const _SizedBuffer* data);
NAMESPACE_METHOD_DECLARE(SizedBuffer, Size, uint64_t, const _SizedBuffer* data);
NAMESPACE_METHOD_DECLARE(SizedBuffer, Data, char*, _SizedBuffer* data);
NAMESPACE_METHOD_DECLARE(SizedBuffer, Begin, void*, _SizedBuffer* data);
NAMESPACE_METHOD_DECLARE(SizedBuffer, End, void*, _SizedBuffer* data);
NAMESPACE_METHOD_DECLARE(SizedBuffer, MetadataSize, uint64_t, const _SizedBuffer* data);
NAMESPACE_METHOD_DECLARE(SizedBuffer, SetSize, bool, _SizedBuffer* data, uint64_t size);
NAMESPACE_METHOD_DECLARE(SizedBuffer, AppendAsPath, bool, _SizedBuffer* data, const char* substring, int substringSize);
NAMESPACE_METHOD_DECLARE(SizedBuffer, Append, bool, _SizedBuffer* data, const char* substring, int substringSize);
NAMESPACE_METHOD_DECLARE(SizedBuffer, AppendFormatAsPath, bool, _SizedBuffer* data, const char* formatString, ...);
NAMESPACE_METHOD_DECLARE(SizedBuffer, AppendFormat, bool, _SizedBuffer* data, const char* formatString, ...);
NAMESPACE_METHOD_DECLARE(SizedBuffer, CopyTo, _SizedBuffer*, _SizedBuffer* data, void* buffer, uint64_t bufferSize);
NAMESPACE_METHOD_DECLARE(SizedBuffer, Compare, int64_t, _SizedBuffer* data1, _SizedBuffer* data2);

NAMESPACE(SizedBuffer)
{
    NAMESPACE_METHOD_DEFINE(SizedBuffer, EvaluatePhysicalSize);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, Initialize);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, InitializeByString);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, New);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, Clone);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, Delete);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, Capacity);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, Size);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, Data);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, Begin);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, End);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, MetadataSize);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, SetSize);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, AppendAsPath);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, Append);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, AppendFormatAsPath);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, AppendFormat);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, CopyTo);
    NAMESPACE_METHOD_DEFINE(SizedBuffer, Compare);
};

#endif /* __SIZEDBUFFER_H__ */
