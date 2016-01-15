#ifndef __U64_H__
#define __U64_H__

#include <stdbool.h>
#include <stdint.h>

#include "ClassSimulator.h"

NAMESPACE_METHOD_DECLARE(U64, EvaluatePhysicalSize, uint8_t, uint64_t value);
NAMESPACE_METHOD_DECLARE(U64, PhysicalSize, uint8_t, const void* data);
NAMESPACE_METHOD_DECLARE(U64, Reinterpret, void*, void* buffer, bool backward, void** end);
NAMESPACE_METHOD_DECLARE(U64, Construct, void*, uint64_t value, void* buffer, bool backward, void** end);
NAMESPACE_METHOD_DECLARE(U64, Value, uint64_t, const void* data, bool backward);
NAMESPACE_METHOD_DECLARE(U64, Compare, int64_t, const void* data1, bool data1Backward, const void* data2, bool data2Backward);
NAMESPACE_METHOD_DECLARE(U64, CopyTo, uint8_t, const void* source, bool sourceBackward, void* target, bool targetBackward, uint64_t targetSize);

NAMESPACE(U64)
{
    NAMESPACE_METHOD_DEFINE(U64, EvaluatePhysicalSize);
    NAMESPACE_METHOD_DEFINE(U64, PhysicalSize);
    NAMESPACE_METHOD_DEFINE(U64, Reinterpret);
    NAMESPACE_METHOD_DEFINE(U64, Construct);
    NAMESPACE_METHOD_DEFINE(U64, Value);
    NAMESPACE_METHOD_DEFINE(U64, Compare);
    NAMESPACE_METHOD_DEFINE(U64, CopyTo);
};


#endif /* __U64_H__ */
