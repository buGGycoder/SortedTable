#include "SizeString.h"
#include "U64F.h"

NAMESPACE_METHOD_IMPLEMENT(SizeString, EvaluatePhysicalSize, uint64_t, uint64_t size)
{
    return U64F.EvaluatePhysicalSize(size) + size;
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, Initialize, _SizeString*, void* buffer, uint64_t bufferSize, const char* data, int64_t dataSize, OUT uint64_t* physicalSize)
{
    if (!data)
    {
        return NULL;
    }

    if (0 > dataSize)
    {
        dataSize = strlen(data);
    }

    uint8_t sizeOfSize = U64F.EvaluatePhysicalSize(dataSize);
    uint64_t capacity = dataSize + sizeOfSize;
    physicalSize && (*physicalSize = capacity);
    if (bufferSize < capacity)
    {
        return NULL;
    }

    void* p = NULL;
    if (!U64F.Construct(dataSize, buffer, &p))
    {
        return NULL;
    }

    I_MEMMOVE(p, data, dataSize);
    return (_SizeString*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, New, _SizeString*, const char* data, int64_t size)
{
    uint64_t physicalSize = 0;
    NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeString, Initialize, NULL, 0, data, size, &physicalSize);
    void* buffer = I_MALLOC(physicalSize);
    if (!buffer)
    {
        return NULL;
    }

    _SizeString* p = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeString, Initialize, buffer, physicalSize, data, size, NULL);
    if (!p)
    {
        I_FREE(buffer);
        return NULL;
    }

    return p;
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, Clone, _SizeString*, const _SizeString* data)
{
    uint64_t physicalSize = U64F.PhysicalSize((_U64F*)data) + U64F.Value((_U64F*)data);
    void* buffer = I_MALLOC(physicalSize);
    if (!buffer)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, data, physicalSize);
    return (_SizeString*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, Delete, void, _SizeString* data)
{
    I_FREE(data);
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, Size, uint64_t, const _SizeString* data)
{
    return U64F.Value((_U64F*)data);
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, Data, char*, _SizeString* data)
{
    return (char*)POINTER_OFFSET(data, U64F.PhysicalSize((_U64F*)data));
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, Begin, void*, _SizeString* data)
{
    return data;
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, End, void*, _SizeString* data)
{
    return POINTER_OFFSET(data, U64F.PhysicalSize((_U64F*)data) + U64F.Value((_U64F*)data));
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, MetadataSize, uint64_t, const _SizeString* data)
{
    return U64F.PhysicalSize((_U64F*)data);
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, CopyTo, _SizeString*, const _SizeString* data, void* buffer, uint64_t bufferSize, OUT void** end)
{
    uint64_t physicalSize = U64F.PhysicalSize((_U64F*)data) + U64F.Value((_U64F*)data);
    if (bufferSize < physicalSize)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, data, physicalSize);
    end && (*end = POINTER_OFFSET(buffer, physicalSize));
    return (_SizeString*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, Compare, int64_t, _SizeString* data1, _SizeString* data2)
{
    int64_t sizeComparison = U64F.Compare((_U64F*)data1, (_U64F*)data2);
    uint64_t minimalComparisonSize = 0 > sizeComparison ? U64F.Value((_U64F*)data1) : U64F.Value((_U64F*)data2);
    char* p1 = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeString, Data, data1);
    char* p2 = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeString, Data, data2);
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

NAMESPACE_INITIALIZE(SizeString)
{
    NAMESPACE_METHOD_INITIALIZE(SizeString, EvaluatePhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(SizeString, Initialize)
    NAMESPACE_METHOD_INITIALIZE(SizeString, New)
    NAMESPACE_METHOD_INITIALIZE(SizeString, Clone)
    NAMESPACE_METHOD_INITIALIZE(SizeString, Delete)
    NAMESPACE_METHOD_INITIALIZE(SizeString, Size)
    NAMESPACE_METHOD_INITIALIZE(SizeString, Data)
    NAMESPACE_METHOD_INITIALIZE(SizeString, Begin)
    NAMESPACE_METHOD_INITIALIZE(SizeString, End)
    NAMESPACE_METHOD_INITIALIZE(SizeString, MetadataSize)
    NAMESPACE_METHOD_INITIALIZE(SizeString, CopyTo)
    NAMESPACE_METHOD_INITIALIZE(SizeString, Compare)
};
