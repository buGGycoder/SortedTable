#ifndef __U64B_H__
#define __U64B_H__

#include <stdbool.h>
#include <stdint.h>

#include "ClassSimulator.h"

PLAIN_OLD_DATA(_U64B)
{
};

NAMESPACE_METHOD_DECLARE(U64B, EvaluatePhysicalSize, uint8_t, uint64_t value);
NAMESPACE_METHOD_DECLARE(U64B, PhysicalSize, uint8_t, const _U64B* data);
NAMESPACE_METHOD_DECLARE(U64B, Reinterpret, _U64B*, void* buffer, void** end);
NAMESPACE_METHOD_DECLARE(U64B, Construct, _U64B*, uint64_t value, void* buffer, void** end);
NAMESPACE_METHOD_DECLARE(U64B, Value, uint64_t, const _U64B* data);
NAMESPACE_METHOD_DECLARE(U64B, End, void*, _U64B* data);
NAMESPACE_METHOD_DECLARE(U64B, Compare, int64_t, const _U64B* data1, const _U64B* data2);
NAMESPACE_METHOD_DECLARE(U64B, CopyTo, int8_t, const _U64B* source, void* target, uint64_t targetSize);

NAMESPACE(U64B)
{
    NAMESPACE_METHOD_DEFINE(U64B, EvaluatePhysicalSize);
    NAMESPACE_METHOD_DEFINE(U64B, PhysicalSize);
    NAMESPACE_METHOD_DEFINE(U64B, Reinterpret);
    NAMESPACE_METHOD_DEFINE(U64B, Construct);
    NAMESPACE_METHOD_DEFINE(U64B, Value);
    NAMESPACE_METHOD_DEFINE(U64B, End);
    NAMESPACE_METHOD_DEFINE(U64B, Compare);
    NAMESPACE_METHOD_DEFINE(U64B, CopyTo);
};

#endif /* __U64B_H__ */
