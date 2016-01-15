#include "U64.h"
#include "U64B.h"

NAMESPACE_METHOD_IMPLEMENT(U64B, EvaluatePhysicalSize, uint8_t, uint64_t value)
{
    return U64.EvaluatePhysicalSize(value);
}

NAMESPACE_METHOD_IMPLEMENT(U64B, PhysicalSize, uint8_t, const _U64B* data)
{
    return U64.PhysicalSize(data);
}

NAMESPACE_METHOD_IMPLEMENT(U64B, Reinterpret, _U64B*, void* buffer, void** end)
{
    return (_U64B*)U64.Reinterpret(buffer, true, end);
}

NAMESPACE_METHOD_IMPLEMENT(U64B, Construct, _U64B*, uint64_t value, void* buffer, void** end)
{
    return (_U64B*)U64.Construct(value, buffer, true, end);
}

NAMESPACE_METHOD_IMPLEMENT(U64B, Value, uint64_t, const _U64B* data)
{
    return U64.Value(data, true);
}

NAMESPACE_METHOD_IMPLEMENT(U64B, End, void*, _U64B* data)
{
    uint64_t size = U64.PhysicalSize(data);
    return POINTER_OFFSET(data, -size);
}

NAMESPACE_METHOD_IMPLEMENT(U64B, Compare, int64_t, const _U64B* data1, const _U64B* data2)
{
    return U64.Compare(data1, true, data2, true);
}

NAMESPACE_METHOD_IMPLEMENT(U64B, CopyTo, int8_t, const _U64B* source, void* target, uint64_t targetSize)
{
    return U64.CopyTo(source, true, target, true, targetSize);
}

NAMESPACE_INITIALIZE(U64B)
{
    NAMESPACE_METHOD_INITIALIZE(U64B, EvaluatePhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(U64B, PhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(U64B, Reinterpret)
    NAMESPACE_METHOD_INITIALIZE(U64B, Construct)
    NAMESPACE_METHOD_INITIALIZE(U64B, Value)
    NAMESPACE_METHOD_INITIALIZE(U64B, End)
    NAMESPACE_METHOD_INITIALIZE(U64B, Compare)
    NAMESPACE_METHOD_INITIALIZE(U64B, CopyTo)
};
