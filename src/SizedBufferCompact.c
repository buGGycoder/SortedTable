#include "SizedBuffer.h"
#include "StringBuilder.h"
#include "U64B.h"

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, EvaluatePhysicalSize, uint64_t, uint64_t capacity)
{
    return 2ULL * U64B.EvaluatePhysicalSize(capacity) + capacity;
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Initialize, _SizedBuffer*, void* buffer, uint64_t bufferSize, OUT uint64_t* capacity)
{
    if (!buffer || !bufferSize)
    {
        return NULL;
    }

    uint8_t sizeOfCapacity = U64B.EvaluatePhysicalSize(bufferSize);
    uint8_t metadataSize = 2 * sizeOfCapacity;
    /*capacity and size occupied all space*/
    if (metadataSize >= bufferSize)
    {
        return NULL;
    }

    uint64_t realCapacity = bufferSize - metadataSize;
    capacity && (*capacity = realCapacity);
    void* p = POINTER_OFFSET(buffer, metadataSize);
    void* end = NULL;
    if (!U64B.Construct(realCapacity, POINTER_OFFSET(p, -1), &end))
    {
        return NULL;
    }

    if (!U64B.Construct(0, end, NULL))
    {
        return NULL;
    }

    return (_SizedBuffer*)p;
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, InitializeByString, _SizedBuffer*, void* buffer, uint64_t bufferSize, const char* data, int64_t dataSize, OUT void** end)
{
    if (!data)
    {
        return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, Initialize, buffer, bufferSize, NULL);
    }

    if (0 > dataSize)
    {
        dataSize = strlen(data);
    }

    uint8_t metadataSize = 2 * U64B.EvaluatePhysicalSize(dataSize);
    uint64_t physicalSize = dataSize + metadataSize;
    if (bufferSize < physicalSize)
    {
        return NULL;
    }

    void* p = POINTER_OFFSET(buffer, metadataSize);
    if (!U64B.Construct(dataSize, POINTER_OFFSET(p, -1), &p))
    {
        return NULL;
    }

    if (!U64B.Construct(dataSize, p, NULL))
    {
        return NULL;
    }

    I_MEMMOVE(POINTER_OFFSET(buffer, metadataSize), data, dataSize);
    end && (*end = POINTER_OFFSET(buffer, physicalSize));
    return (_SizedBuffer*)p;
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, New, _SizedBuffer*, uint64_t capacity)
{
    if (!capacity)
    {
        return NULL;
    }

    uint8_t sizeOfCapacity = U64B.EvaluatePhysicalSize(capacity);
    uint8_t metadataSize = 2 * sizeOfCapacity;
    void* buffer = I_MALLOC(metadataSize + capacity);
    if (!buffer)
    {
        return NULL;
    }

    void* p = POINTER_OFFSET(buffer, metadataSize);
    void* end = NULL;
    if (!U64B.Construct(capacity, POINTER_OFFSET(p, -1), &end))
    {
        return NULL;
    }

    if (!U64B.Construct(0, end, NULL))
    {
        return NULL;
    }

    return (_SizedBuffer*)p;
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Clone, _SizedBuffer*, const _SizedBuffer* data)
{
    uint8_t sizeOfCapacity = U64B.PhysicalSize((_U64B*)POINTER_OFFSET(data, -1));
    uint64_t physicalSize = 2 * sizeOfCapacity + U64B.Value((_U64B*)POINTER_OFFSET(data, -1));
    void* buffer = I_MALLOC(physicalSize);
    if (!buffer)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, POINTER_OFFSET(data, -2 * sizeOfCapacity), physicalSize);
    return (_SizedBuffer*)POINTER_OFFSET(buffer, 2 * sizeOfCapacity);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Delete, void, _SizedBuffer* data)
{
    uint8_t sizeOfCapacity = U64B.PhysicalSize((_U64B*)POINTER_OFFSET(data, -1));
    I_FREE(POINTER_OFFSET(data, -2 * sizeOfCapacity));
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Capacity, uint64_t, const _SizedBuffer* data)
{
    return U64B.Value((_U64B*)POINTER_OFFSET(data, -1));
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Size, uint64_t, const _SizedBuffer* data)
{
    _U64B* pCapacity = (_U64B*)POINTER_OFFSET(data, -1);
    uint8_t sizeOfCapacity = U64B.PhysicalSize(pCapacity);
    _U64B* pSize = (_U64B*)POINTER_OFFSET(data, -1 - sizeOfCapacity);
    return 0 > U64B.Compare(pSize, pCapacity) ? U64B.Value(pSize) : U64B.Value(pCapacity);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Data, char*, _SizedBuffer* data)
{
    return data->data;
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Begin, void*, _SizedBuffer* data)
{
    uint8_t sizeOfCapacity = U64B.PhysicalSize((_U64B*)POINTER_OFFSET(data, -1));
    return POINTER_OFFSET(data, -2 * sizeOfCapacity);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, End, void*, _SizedBuffer* data)
{
    return POINTER_OFFSET(data, U64B.Value((_U64B*)POINTER_OFFSET(data, -1)));
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, MetadataSize, uint64_t, const _SizedBuffer* data)
{
    return 2 * U64B.PhysicalSize((_U64B*)POINTER_OFFSET(data, -1));
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, SetSize, bool, _SizedBuffer* data, uint64_t size)
{
    _U64B* pCapacity = (_U64B*)POINTER_OFFSET(data, -1);
    uint64_t capacity = U64B.Value(pCapacity);
    if (size > capacity)
    {
        return false;
    }

    uint8_t sizeOfCapacity = U64B.PhysicalSize(pCapacity);
    return !!U64B.Construct(size, POINTER_OFFSET(data, -1 - sizeOfCapacity), NULL);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, AppendAsPath, bool, _SizedBuffer* data, const char* substring, int substringSize)
{
    uint64_t capacity = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, Capacity, data);
    int64_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, Size, data);
    size = StringBuilder.AppendAsPath(data->data, size, capacity, substring, substringSize);
    if (0 > size)
    {
        return false;
    }

    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, SetSize, data, size);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Append, bool, _SizedBuffer* data, const char* substring, int substringSize)
{
    uint64_t capacity = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, Capacity, data);
    int64_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, Size, data);
    size = StringBuilder.Append(data->data, size, capacity, substring, substringSize);
    if (0 > size)
    {
        return false;
    }

    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, SetSize, data, size);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, AppendFormatAsPath, bool, _SizedBuffer* data, const char* formatString, ...)
{
    uint64_t capacity = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, Capacity, data);
    int64_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, Size, data);

    va_list args;
    va_start(args, formatString);
    size = StringBuilder.VFormat(data->data, size, capacity, true, formatString, args);
    va_end(args);

    if (0 > size)
    {
        return false;
    }

    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, SetSize, data, size);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, AppendFormat, bool, _SizedBuffer* data, const char* formatString, ...)
{
    uint64_t capacity = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, Capacity, data);
    int64_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, Size, data);

    va_list args;
    va_start(args, formatString);
    size = StringBuilder.VFormat(data->data, size, capacity, false, formatString, args);
    va_end(args);

    if (0 > size)
    {
        return false;
    }

    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, SetSize, data, size);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, CopyTo, _SizedBuffer*, _SizedBuffer* data, void* buffer, uint64_t bufferSize)
{
    uint8_t sizeOfCapacity = U64B.PhysicalSize((_U64B*)POINTER_OFFSET(data, -1));
    uint64_t physicalSize = 2 * sizeOfCapacity + U64B.Value((_U64B*)POINTER_OFFSET(data, -1 -sizeOfCapacity));
    if (bufferSize < physicalSize)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, POINTER_OFFSET(data, -2 * sizeOfCapacity), physicalSize);
    return (_SizedBuffer*)POINTER_OFFSET(buffer, 2 * sizeOfCapacity);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Compare, int64_t, _SizedBuffer* data1, _SizedBuffer* data2)
{
    _U64B* pCapacity1 = (_U64B*)POINTER_OFFSET(data1, -1);
    _U64B* pCapacity2 = (_U64B*)POINTER_OFFSET(data2, -1);
    uint8_t sizeOfCapacity1 = U64B.PhysicalSize(pCapacity1);
    uint8_t sizeOfCapacity2 = U64B.PhysicalSize(pCapacity2);
    _U64B* pSize1 = (_U64B*)POINTER_OFFSET(pCapacity1, -sizeOfCapacity1);
    _U64B* pSize2 = (_U64B*)POINTER_OFFSET(pCapacity2, -sizeOfCapacity2);
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

NAMESPACE_INITIALIZE(SizedBuffer)
{
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, EvaluatePhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, Initialize)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, InitializeByString)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, New)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, Clone)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, Delete)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, Capacity)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, Size)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, Data)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, Begin)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, End)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, MetadataSize)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, SetSize)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, AppendAsPath)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, Append)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, AppendFormatAsPath)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, AppendFormat)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, CopyTo)
    NAMESPACE_METHOD_INITIALIZE(SizedBuffer, Compare)
};
