#ifndef __SIZEBUFFER_H__
#define __SIZEBUFFER_H__

#include <stdbool.h>
#include <stdint.h>
#include "ClassSimulator.h"

#pragma pack(push)
#pragma pack(1)
/* Pointer | Capacity Forward | Size Forward | Data*/
PLAIN_OLD_DATA(_SizeBuffer)
{
};
#pragma pack(pop)

NAMESPACE_METHOD_DECLARE(SizeBuffer, EvaluatePhysicalSize, uint64_t, uint64_t capacity);
NAMESPACE_METHOD_DECLARE(SizeBuffer, Initialize, _SizeBuffer*, void* buffer, uint64_t bufferSize, OUT uint64_t* capacity);
NAMESPACE_METHOD_DECLARE(SizeBuffer, InitializeByString, _SizeBuffer*, void* buffer, uint64_t bufferSize, const char* data, int64_t dataSize, OUT void** end);
NAMESPACE_METHOD_DECLARE(SizeBuffer, New, _SizeBuffer*, uint64_t capacity);
NAMESPACE_METHOD_DECLARE(SizeBuffer, Clone, _SizeBuffer*, const _SizeBuffer* data);
NAMESPACE_METHOD_DECLARE(SizeBuffer, Delete, void, _SizeBuffer* data);
NAMESPACE_METHOD_DECLARE(SizeBuffer, Capacity, uint64_t, const _SizeBuffer* data);
NAMESPACE_METHOD_DECLARE(SizeBuffer, Size, uint64_t, const _SizeBuffer* data);
NAMESPACE_METHOD_DECLARE(SizeBuffer, Data, char*, _SizeBuffer* data);
NAMESPACE_METHOD_DECLARE(SizeBuffer, Begin, void*, _SizeBuffer* data);
NAMESPACE_METHOD_DECLARE(SizeBuffer, End, void*, _SizeBuffer* data);
NAMESPACE_METHOD_DECLARE(SizeBuffer, MetadataSize, uint64_t, const _SizeBuffer* data);
NAMESPACE_METHOD_DECLARE(SizeBuffer, SetSize, bool, _SizeBuffer* data, uint64_t size);
NAMESPACE_METHOD_DECLARE(SizeBuffer, AppendAsPath, bool, _SizeBuffer* data, const char* substring, int substringSize);
NAMESPACE_METHOD_DECLARE(SizeBuffer, Append, bool, _SizeBuffer* data, const char* substring, int substringSize);
NAMESPACE_METHOD_DECLARE(SizeBuffer, AppendFormatAsPath, bool, _SizeBuffer* data, const char* formatString, ...);
NAMESPACE_METHOD_DECLARE(SizeBuffer, AppendFormat, bool, _SizeBuffer* data, const char* formatString, ...);
NAMESPACE_METHOD_DECLARE(SizeBuffer, CopyTo, _SizeBuffer*, _SizeBuffer* data, void* buffer, uint64_t bufferSize);
NAMESPACE_METHOD_DECLARE(SizeBuffer, Compare, int64_t, _SizeBuffer* data1, _SizeBuffer* data2);

NAMESPACE(SizeBuffer)
{
    NAMESPACE_METHOD_DEFINE(SizeBuffer, EvaluatePhysicalSize);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, Initialize);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, InitializeByString);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, New);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, Clone);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, Delete);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, Capacity);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, Size);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, Data);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, Begin);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, End);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, MetadataSize);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, SetSize);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, AppendAsPath);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, Append);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, AppendFormatAsPath);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, AppendFormat);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, CopyTo);
    NAMESPACE_METHOD_DEFINE(SizeBuffer, Compare);
};

#endif /* __SIZEBUFFER_H__ */
