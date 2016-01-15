#include "U64.h"
#include "U64F.h"

NAMESPACE_METHOD_IMPLEMENT(U64F, EvaluatePhysicalSize, uint8_t, uint64_t value)
{
    return U64.EvaluatePhysicalSize(value);
}

NAMESPACE_METHOD_IMPLEMENT(U64F, PhysicalSize, uint8_t, const _U64F* data)
{
    return U64.PhysicalSize(data);
}

NAMESPACE_METHOD_IMPLEMENT(U64F, Reinterpret, _U64F*, void* buffer, void** end)
{
    return (_U64F*)U64.Reinterpret(buffer, false, end);
}

NAMESPACE_METHOD_IMPLEMENT(U64F, Construct, _U64F*, uint64_t value, void* buffer, void** end)
{
    return (_U64F*)U64.Construct(value, buffer, false, end);
}

NAMESPACE_METHOD_IMPLEMENT(U64F, Value, uint64_t, const _U64F* data)
{
    return U64.Value(data, false);
}

NAMESPACE_METHOD_IMPLEMENT(U64F, End, void*, _U64F* data)
{
    uint64_t size = U64.PhysicalSize(data);
    return POINTER_OFFSET(data, size);
}

NAMESPACE_METHOD_IMPLEMENT(U64F, Compare, int64_t, const _U64F* data1, const _U64F* data2)
{
    return U64.Compare(data1, false, data2, false);
}

NAMESPACE_METHOD_IMPLEMENT(U64F, CopyTo, int8_t, const _U64F* source, void* target, uint64_t targetSize)
{
    return U64.CopyTo(source, false, target, false, targetSize);
}

NAMESPACE_INITIALIZE(U64F)
{
    NAMESPACE_METHOD_INITIALIZE(U64F, EvaluatePhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(U64F, PhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(U64F, Reinterpret)
    NAMESPACE_METHOD_INITIALIZE(U64F, Construct)
    NAMESPACE_METHOD_INITIALIZE(U64F, Value)
    NAMESPACE_METHOD_INITIALIZE(U64F, End)
    NAMESPACE_METHOD_INITIALIZE(U64F, Compare)
    NAMESPACE_METHOD_INITIALIZE(U64F, CopyTo)
};
