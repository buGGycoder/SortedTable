#include "SizedString.h"
#include "U64B.h"

NAMESPACE_METHOD_IMPLEMENT(SizedString, EvaluatePhysicalSize, uint64_t, uint64_t size)
{
    return 2ULL * U64B.EvaluatePhysicalSize(size) + size;
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Initialize, _SizedString*, void* buffer, uint64_t bufferSize, const char* data, int64_t dataSize, OUT uint64_t* physicalSize)
{
    if (!data)
    {
        return NULL;
    }

    if (0 > dataSize)
    {
        dataSize = strlen(data);
    }

    uint8_t sizeOfSize = U64B.EvaluatePhysicalSize(dataSize);
    uint64_t capacity = dataSize + sizeOfSize;
    physicalSize && (*physicalSize = capacity);
    if (bufferSize < capacity)
    {
        return NULL;
    }

    void* p = POINTER_OFFSET(buffer, sizeOfSize);
    if (!U64B.Construct(dataSize, POINTER_OFFSET(p, -1), NULL))
    {
        return NULL;
    }

    I_MEMMOVE(p, data, dataSize);
    return (_SizedString*)p;
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, New, _SizedString*, const char* data, int64_t size)
{
    uint64_t physicalSize = 0;
    NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedString, Initialize, NULL, 0, data, size, &physicalSize);
    void* buffer = I_MALLOC(physicalSize);
    if (!buffer)
    {
        return NULL;
    }

    _SizedString* p = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedString, Initialize, buffer, physicalSize, data, size, NULL);
    if (!p)
    {
        I_FREE(buffer);
        return NULL;
    }

    return p;
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Clone, _SizedString*, const _SizedString* data)
{
    void* pU64 = POINTER_OFFSET(data, -1);
    uint8_t sizeU64 = U64B.PhysicalSize(pU64);
    uint64_t physicalSize = sizeU64 + U64B.Value(pU64);
    void* buffer = I_MALLOC(physicalSize);
    if (!buffer)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, POINTER_OFFSET(data, -sizeU64), physicalSize);
    return (_SizedString*)POINTER_OFFSET(buffer, sizeU64);
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Delete, void, _SizedString* data)
{
    I_FREE(POINTER_OFFSET(data, -U64B.PhysicalSize(POINTER_OFFSET(data, -1))));
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Size, uint64_t, const _SizedString* data)
{
    return U64B.Value(POINTER_OFFSET(data, -1));
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Data, char*, _SizedString* data)
{
    return data->data;
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Begin, void*, _SizedString* data)
{
    return POINTER_OFFSET(data, -U64B.PhysicalSize(POINTER_OFFSET(data, -1)));
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, End, void*, _SizedString* data)
{
    return POINTER_OFFSET(data, U64B.Value(POINTER_OFFSET(data, -1)));
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, MetadataSize, uint64_t, const _SizedString* data)
{
    return U64B.PhysicalSize(POINTER_OFFSET(data, -1));
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, CopyTo, _SizedString*, const _SizedString* data, void* buffer, uint64_t bufferSize, OUT void** end)
{
    void* pU64 = POINTER_OFFSET(data, -1);
    uint8_t sizeU64 = U64B.PhysicalSize(pU64);
    uint64_t physicalSize = sizeU64 + U64B.Value(pU64);
    if (bufferSize < physicalSize)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, POINTER_OFFSET(data, -sizeU64), physicalSize);
    end && (*end = POINTER_OFFSET(buffer, physicalSize));
    return (_SizedString*)POINTER_OFFSET(buffer, sizeU64);
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Compare, int64_t, _SizedString* data1, _SizedString* data2)
{
    _U64B* pSize1 = (_U64B*)POINTER_OFFSET(data1, -1);
    _U64B* pSize2 = (_U64B*)POINTER_OFFSET(data2, -1);
    int64_t sizeComparison = U64B.Compare(pSize1, pSize2);
    uint64_t minimalComparisonSize = 0 > sizeComparison ? U64B.Value(pSize1) : U64B.Value(pSize2);
    char* p1 = data1->data;
    char* p2 = data2->data;
    int i = 0;
    int result = 0;
    for (i = 0; i < minimalComparisonSize; ++i)
    {
        if ((result = p1[i] - p2[i]))
        {
            return result;
        }
    }

    return sizeComparison;
}

NAMESPACE_INITIALIZE(SizedString)
{
    NAMESPACE_METHOD_INITIALIZE(SizedString, EvaluatePhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(SizedString, Initialize)
    NAMESPACE_METHOD_INITIALIZE(SizedString, New)
    NAMESPACE_METHOD_INITIALIZE(SizedString, Clone)
    NAMESPACE_METHOD_INITIALIZE(SizedString, Delete)
    NAMESPACE_METHOD_INITIALIZE(SizedString, Size)
    NAMESPACE_METHOD_INITIALIZE(SizedString, Data)
    NAMESPACE_METHOD_INITIALIZE(SizedString, Begin)
    NAMESPACE_METHOD_INITIALIZE(SizedString, End)
    NAMESPACE_METHOD_INITIALIZE(SizedString, MetadataSize)
    NAMESPACE_METHOD_INITIALIZE(SizedString, CopyTo)
    NAMESPACE_METHOD_INITIALIZE(SizedString, Compare)
};
