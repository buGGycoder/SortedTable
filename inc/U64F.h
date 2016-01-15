#ifndef __U64F_H__
#define __U64F_H__

#include <stdbool.h>
#include <stdint.h>

#include "ClassSimulator.h"

PLAIN_OLD_DATA(_U64F)
{
};

NAMESPACE_METHOD_DECLARE(U64F, EvaluatePhysicalSize, uint8_t, uint64_t value);
NAMESPACE_METHOD_DECLARE(U64F, PhysicalSize, uint8_t, const _U64F* data);
NAMESPACE_METHOD_DECLARE(U64F, Reinterpret, _U64F*, void* buffer, void** end);
NAMESPACE_METHOD_DECLARE(U64F, Construct, _U64F*, uint64_t value, void* buffer, void** end);
NAMESPACE_METHOD_DECLARE(U64F, Value, uint64_t, const _U64F* data);
NAMESPACE_METHOD_DECLARE(U64F, End, void*, _U64F* data);
NAMESPACE_METHOD_DECLARE(U64F, Compare, int64_t, const _U64F* data1, const _U64F* data2);
NAMESPACE_METHOD_DECLARE(U64F, CopyTo, int8_t, const _U64F* source, void* target, uint64_t targetSize);

NAMESPACE(U64F)
{
    NAMESPACE_METHOD_DEFINE(U64F, EvaluatePhysicalSize);
    NAMESPACE_METHOD_DEFINE(U64F, PhysicalSize);
    NAMESPACE_METHOD_DEFINE(U64F, Reinterpret);
    NAMESPACE_METHOD_DEFINE(U64F, Construct);
    NAMESPACE_METHOD_DEFINE(U64F, Value);
    NAMESPACE_METHOD_DEFINE(U64F, End);
    NAMESPACE_METHOD_DEFINE(U64F, Compare);
    NAMESPACE_METHOD_DEFINE(U64F, CopyTo);
};

#endif /* __U64F_H__ */
