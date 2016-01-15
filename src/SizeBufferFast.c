#include "SizeBuffer.h"
#include "StringBuilder.h"

#pragma pack(push)
#pragma pack(1)
PLAIN_OLD_DATA(_SizeBufferMetadata)
{
    _SizeBuffer buffer[0];
    uint64_t capacity;
    uint64_t size;
    char data[0];
};
#pragma pack(pop)

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, EvaluatePhysicalSize, uint64_t, uint64_t capacity)
{
    return capacity + sizeof(_SizeBufferMetadata);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Initialize, _SizeBuffer*, void* buffer, uint64_t bufferSize, OUT uint64_t* capacity)
{
    if (!buffer || !bufferSize)
    {
        return NULL;
    }

    /*capacity and size occupied all space*/
    if (sizeof(_SizeBufferMetadata) >= bufferSize)
    {
        return NULL;
    }

    uint64_t realCapacity = bufferSize - sizeof(_SizeBufferMetadata);
    capacity && (*capacity = realCapacity);
    _SizeBufferMetadata* sb = (_SizeBufferMetadata*)buffer;
    sb->capacity = realCapacity;
    sb->size = 0;
    return sb->buffer;
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

    if (bufferSize < dataSize + sizeof(_SizeBufferMetadata))
    {
        return NULL;
    }

    _SizeBufferMetadata* sb = (_SizeBufferMetadata*)buffer;
    sb->capacity = sb->size = dataSize;
    I_MEMMOVE(sb->data, data, dataSize);
    end && (*end = POINTER_OFFSET(sb->data, dataSize));
    return (_SizeBuffer*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, New, _SizeBuffer*, uint64_t capacity)
{
    if (!capacity)
    {
        return NULL;
    }

    _SizeBufferMetadata* sb = (_SizeBufferMetadata*)I_MALLOC(capacity + sizeof(_SizeBufferMetadata));
    if (!sb)
    {
        return NULL;
    }

    sb->capacity = capacity;
    sb->size = 0;
    return sb->buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Clone, _SizeBuffer*, const _SizeBuffer* data)
{
    _SizeBufferMetadata* sb = (_SizeBufferMetadata*)data;
    uint64_t physicalSize = sb->capacity + sizeof(_SizeBufferMetadata);
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
    return ((_SizeBufferMetadata*)data)->capacity;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Size, uint64_t, const _SizeBuffer* data)
{
    return ((_SizeBufferMetadata*)data)->size;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Data, char*, _SizeBuffer* data)
{
    return ((_SizeBufferMetadata*)data)->data;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Begin, void*, _SizeBuffer* data)
{
    return data;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, End, void*, _SizeBuffer* data)
{
    return POINTER_OFFSET(data, ((_SizeBufferMetadata*)data)->capacity + sizeof(_SizeBufferMetadata));
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, MetadataSize, uint64_t, const _SizeBuffer* data)
{
    return sizeof(_SizeBufferMetadata);
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, SetSize, bool, _SizeBuffer* data, uint64_t size)
{
    _SizeBufferMetadata* sb = (_SizeBufferMetadata*)data;
    if (size > sb->capacity)
    {
        return false;
    }

    sb->size = size;
    return true;
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
    _SizeBufferMetadata* sb = (_SizeBufferMetadata*)data;
    uint64_t physicalSize = sb->capacity + sizeof(_SizeBufferMetadata);
    if (bufferSize < physicalSize)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, sb, physicalSize);
    return (_SizeBuffer*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeBuffer, Compare, int64_t, _SizeBuffer* data1, _SizeBuffer* data2)
{
    _SizeBufferMetadata* sb1 = (_SizeBufferMetadata*)data1;
    _SizeBufferMetadata* sb2 = (_SizeBufferMetadata*)data2;
    int64_t sizeComparison = sb1->size - sb2->size;
    uint64_t minimalComparisonSize = 0 > sizeComparison ? sb1->size : sb2->size;
    char* p1 = sb1->data;
    char* p2 = sb2->data;
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
