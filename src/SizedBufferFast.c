#include "SizedBuffer.h"
#include "StringBuilder.h"

#pragma pack(push)
#pragma pack(1)
PLAIN_OLD_DATA(_SizedBufferMetadata)
{
    uint64_t size;
    uint64_t capacity;
    char data[0];
    _SizedBuffer buffer[0];
};
#pragma pack(pop)

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, EvaluatePhysicalSize, uint64_t, uint64_t capacity)
{
    return capacity + sizeof(_SizedBufferMetadata);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Initialize, _SizedBuffer*, void* buffer, uint64_t bufferSize, OUT uint64_t* capacity)
{
    if (!buffer || !bufferSize)
    {
        return NULL;
    }

    /*capacity and size occupied all space*/
    if (sizeof(_SizedBufferMetadata) >= bufferSize)
    {
        return NULL;
    }

    uint64_t realCapacity = bufferSize - sizeof(_SizedBufferMetadata);
    capacity && (*capacity = realCapacity);
    _SizedBufferMetadata* sb = (_SizedBufferMetadata*)buffer;
    sb->capacity = realCapacity;
    sb->size = 0;
    return sb->buffer;
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

    if (bufferSize < dataSize + sizeof(_SizedBufferMetadata))
    {
        return NULL;
    }

    _SizedBufferMetadata* sb = (_SizedBufferMetadata*)buffer;
    sb->capacity = sb->size = dataSize;
    I_MEMMOVE(sb->data, data, dataSize);
    end && (*end = POINTER_OFFSET(sb->data, dataSize));
    return sb->buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, New, _SizedBuffer*, uint64_t capacity)
{
    if (!capacity)
    {
        return NULL;
    }

    _SizedBufferMetadata* sb = I_MALLOC(capacity + sizeof(_SizedBufferMetadata));
    if (!sb)
    {
        return NULL;
    }

    sb->capacity = capacity;
    sb->size = 0;
    return sb->buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Clone, _SizedBuffer*, const _SizedBuffer* data)
{
    _SizedBufferMetadata* sb = (_SizedBufferMetadata*)POINTER_OFFSET(data, -sizeof(_SizedBufferMetadata));
    uint64_t physicalSize = sb->capacity + sizeof(_SizedBufferMetadata);
    void* buffer = I_MALLOC(physicalSize);
    if (!buffer)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, sb, physicalSize);
    return (_SizedBuffer*)POINTER_OFFSET(buffer, sizeof(_SizedBufferMetadata));
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Delete, void, _SizedBuffer* data)
{
    I_FREE(POINTER_OFFSET(data, -sizeof(_SizedBufferMetadata)));
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Capacity, uint64_t, const _SizedBuffer* data)
{
    return ((_SizedBufferMetadata*)POINTER_OFFSET(data, -sizeof(_SizedBufferMetadata)))->capacity;
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Size, uint64_t, const _SizedBuffer* data)
{
    return ((_SizedBufferMetadata*)POINTER_OFFSET(data, -sizeof(_SizedBufferMetadata)))->size;
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Data, char*, _SizedBuffer* data)
{
    return data->data;
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Begin, void*, _SizedBuffer* data)
{
    return POINTER_OFFSET(data, -NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, MetadataSize, data));
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, End, void*, _SizedBuffer* data)
{
    return POINTER_OFFSET(data, NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedBuffer, Capacity, data));
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, MetadataSize, uint64_t, const _SizedBuffer* data)
{
    return sizeof(_SizedBufferMetadata);
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, SetSize, bool, _SizedBuffer* data, uint64_t size)
{
    _SizedBufferMetadata* sb = (_SizedBufferMetadata*)POINTER_OFFSET(data, -sizeof(_SizedBufferMetadata));
    if (size > sb->capacity)
    {
        return false;
    }

    sb->size = size;
    return true;
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
    _SizedBufferMetadata* sb = (_SizedBufferMetadata*)POINTER_OFFSET(data, -sizeof(_SizedBufferMetadata));
    uint64_t physicalSize = sb->capacity + sizeof(_SizedBufferMetadata);
    if (bufferSize < physicalSize)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, sb, physicalSize);
    return (_SizedBuffer*)POINTER_OFFSET(buffer, sizeof(_SizedBufferMetadata));
}

NAMESPACE_METHOD_IMPLEMENT(SizedBuffer, Compare, int64_t, _SizedBuffer* data1, _SizedBuffer* data2)
{
    _SizedBufferMetadata* sb1 = (_SizedBufferMetadata*)POINTER_OFFSET(data1, -sizeof(_SizedBufferMetadata));
    _SizedBufferMetadata* sb2 = (_SizedBufferMetadata*)POINTER_OFFSET(data2, -sizeof(_SizedBufferMetadata));
    int64_t sizeComparison = sb1->size - sb2->size;
    uint64_t minimalComparisonSize = 0 > sizeComparison ? sb1->size : sb2->size;
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
