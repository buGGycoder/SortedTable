#include "SizeBuffer.h"
#include "StringBuilder.h"
#include "U64F.h"

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, EvaluatePhysicalSize, uint64_t, uint64_t capacity)
{
    return 2ULL * U64F.EvaluatePhysicalSize(capacity) + capacity;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Initialize, _SizeBuffer*, void* buffer, uint64_t bufferSize, OUT uint64_t* capacity)
{
    if (!buffer || !bufferSize)
    {
        return NULL;
    }

    uint8_t sizeOfCapacity = U64F.EvaluatePhysicalSize(bufferSize);
    uint8_t metadataSize = 2 * sizeOfCapacity;
    /*capacity and size occupied all space*/
    if (metadataSize >= bufferSize)
    {
        return NULL;
    }

    uint64_t realCapacity = bufferSize - metadataSize;
    capacity && (*capacity = realCapacity);
    void* end = NULL;
    if (!U64F.Construct(realCapacity, buffer, &end))
    {
        return NULL;
    }

    if (!U64F.Construct(0, end, NULL))
    {
        return NULL;
    }

    return (_SizeBuffer*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, InitializeByString, _SizeBuffer*, void* buffer, uint64_t bufferSize, const char* data, int64_t dataSize, OUT void** end)
{
    if (!data)
    {
        return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Initialize, buffer, bufferSize, NULL);
    }

    if (0 > dataSize)
    {
        dataSize = strlen(data);
    }

    uint8_t metadataSize = 2 * U64F.EvaluatePhysicalSize(dataSize);
    uint64_t physicalSize = dataSize + metadataSize;
    if (bufferSize < physicalSize)
    {
        return NULL;
    }

    void* p = NULL;
    if (!U64F.Construct(dataSize, buffer, &p))
    {
        return NULL;
    }

    if (!U64F.Construct(dataSize, p, NULL))
    {
        return NULL;
    }

    I_MEMMOVE(POINTER_OFFSET(buffer, metadataSize), data, dataSize);
    end && (*end = POINTER_OFFSET(buffer, physicalSize));
    return (_SizeBuffer*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, New, _SizeBuffer*, uint64_t capacity)
{
    if (!capacity)
    {
        return NULL;
    }

    uint8_t sizeOfCapacity = U64F.EvaluatePhysicalSize(capacity);
    uint8_t metadataSize = 2 * sizeOfCapacity;
    void* buffer = I_MALLOC(metadataSize + capacity);
    if (!buffer)
    {
        return NULL;
    }

    void* end = NULL;
    if (!U64F.Construct(capacity, buffer, &end))
    {
        return NULL;
    }

    if (!U64F.Construct(0, end, NULL))
    {
        return NULL;
    }

    return (_SizeBuffer*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Clone, _SizeBuffer*, const _SizeBuffer* data)
{
    uint8_t sizeOfCapacity = U64F.PhysicalSize((_U64F*)data);
    uint64_t physicalSize = 2 * sizeOfCapacity + U64F.Value((_U64F*)data);
    void* buffer = I_MALLOC(physicalSize);
    if (!buffer)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, data, physicalSize);
    return (_SizeBuffer*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Delete, void, _SizeBuffer* data)
{
    I_FREE(data);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Capacity, uint64_t, const _SizeBuffer* data)
{
    return U64F.Value((_U64F*)data);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Size, uint64_t, const _SizeBuffer* data)
{
    uint8_t sizeOfCapacity = U64F.PhysicalSize((_U64F*)data);
    _U64F* pSize = (_U64F*)POINTER_OFFSET(data, sizeOfCapacity);
    return 0 > U64F.Compare(pSize, (_U64F*)data) ? U64F.Value(pSize) : U64F.Value((_U64F*)data);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Data, char*, _SizeBuffer* data)
{
    return (char*)POINTER_OFFSET(data, NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, MetadataSize, data));
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Begin, void*, _SizeBuffer* data)
{
    return data;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, End, void*, _SizeBuffer* data)
{
    uint64_t physicalSize = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, MetadataSize, data)
                            + NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Capacity, data);
    return POINTER_OFFSET(data, physicalSize);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, MetadataSize, uint64_t, const _SizeBuffer* data)
{
    return 2ULL * U64F.PhysicalSize((_U64F*)data);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, SetSize, bool, _SizeBuffer* data, uint64_t size)
{
    uint64_t capacity = U64F.Value((_U64F*)data);
    if (size > capacity)
    {
        return false;
    }

    uint8_t sizeOfCapacity = U64F.PhysicalSize((_U64F*)data);
    return !!U64F.Construct(size, POINTER_OFFSET(data, sizeOfCapacity), NULL);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, AppendAsPath, bool, _SizeBuffer* data, const char* substring, int substringSize)
{
    uint64_t capacity = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Capacity, data);
    int64_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Size, data);
    char* realData = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Data, data);
    size = StringBuilder.AppendAsPath(realData, size, capacity, substring, substringSize);
    if (0 > size)
    {
        return false;
    }

    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, SetSize, data, size);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Append, bool, _SizeBuffer* data, const char* substring, int substringSize)
{
    uint64_t capacity = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Capacity, data);
    int64_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Size, data);
    char* realData = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Data, data);
    size = StringBuilder.Append(realData, size, capacity, substring, substringSize);
    if (0 > size)
    {
        return false;
    }

    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, SetSize, data, size);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, AppendFormatAsPath, bool, _SizeBuffer* data, const char* formatString, ...)
{
    uint64_t capacity = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Capacity, data);
    int64_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Size, data);
    char* realData = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Data, data);

    va_list args;
    va_start(args, formatString);
    size = StringBuilder.VFormat(realData, size, capacity, true, formatString, args);
    va_end(args);

    if (0 > size)
    {
        return false;
    }

    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, SetSize, data, size);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, AppendFormat, bool, _SizeBuffer* data, const char* formatString, ...)
{
    uint64_t capacity = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Capacity, data);
    int64_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Size, data);
    char* realData = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Data, data);

    va_list args;
    va_start(args, formatString);
    size = StringBuilder.VFormat(realData, size, capacity, false, formatString, args);
    va_end(args);

    if (0 > size)
    {
        return false;
    }

    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, SetSize, data, size);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, CopyTo, _SizeBuffer*, _SizeBuffer* data, void* buffer, uint64_t bufferSize)
{
    uint8_t sizeOfCapacity = U64F.PhysicalSize((_U64F*)data);
    uint64_t physicalSize = 2 * sizeOfCapacity + U64F.Value((_U64F*)data);
    if (bufferSize < physicalSize)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, data, physicalSize);
    return (_SizeBuffer*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Compare, int64_t, _SizeBuffer* data1, _SizeBuffer* data2)
{
    uint8_t sizeOfCapacity1 = U64F.PhysicalSize((_U64F*)data1);
    uint8_t sizeOfCapacity2 = U64F.PhysicalSize((_U64F*)data2);
    _U64F* pSize1 = (_U64F*)POINTER_OFFSET(data1, sizeOfCapacity1);
    _U64F* pSize2 = (_U64F*)POINTER_OFFSET(data2, sizeOfCapacity2);
    int64_t sizeComparison = U64F.Compare(pSize1, pSize2);
    uint64_t minimalComparisonSize = 0 > sizeComparison ? U64F.Value(pSize1) : U64F.Value(pSize2);
    char* p1 = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Data, data1);
    char* p2 = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeBuffer, Data, data2);
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

NAMESPACE_INITIALIZE(SizeBuffer)
{
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, EvaluatePhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, Initialize)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, InitializeByString)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, New)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, Clone)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, Delete)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, Capacity)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, Size)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, Data)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, Begin)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, End)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, MetadataSize)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, SetSize)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, AppendAsPath)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, Append)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, AppendFormatAsPath)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, AppendFormat)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, CopyTo)
    NAMESPACE_METHOD_INITIALIZE(SizeBuffer, Compare)
};
