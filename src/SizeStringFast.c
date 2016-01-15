#include "SizeString.h"

#pragma pack(push)
#pragma pack(1)
PLAIN_OLD_DATA(_SizeStringMetadata)
{
    _SizeString string[0];
    uint64_t size;
    char data[0];
};
#pragma pack(pop)

NAMESPACE_METHOD_IMPLEMENT(SizeString, EvaluatePhysicalSize, uint64_t, uint64_t size)
{
    return size + sizeof(_SizeStringMetadata);
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

    uint64_t capacity = dataSize + sizeof(_SizeStringMetadata);
    physicalSize && (*physicalSize = capacity);
    if (bufferSize < capacity)
    {
        return NULL;
    }

    _SizeStringMetadata* ss = (_SizeStringMetadata*)buffer;
    ss->size = dataSize;
    I_MEMMOVE(ss->data, data, dataSize);
    return ss->string;
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
    _SizeStringMetadata* ss = (_SizeStringMetadata*)data;
    uint64_t physicalSize = ss->size + sizeof(_SizeStringMetadata);
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
    return ((_SizeStringMetadata*)data)->size;
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, Data, char*, _SizeString* data)
{
    return ((_SizeStringMetadata*)data)->data;
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, Begin, void*, _SizeString* data)
{
    return data;
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, End, void*, _SizeString* data)
{
    return POINTER_OFFSET(data, NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizeString, MetadataSize, data));
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, MetadataSize, uint64_t, const _SizeString* data)
{
    return sizeof(_SizeStringMetadata);
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, CopyTo, _SizeString*, const _SizeString* data, void* buffer, uint64_t bufferSize, OUT void** end)
{
    _SizeStringMetadata* ss = (_SizeStringMetadata*)data;
    uint64_t physicalSize = ss->size + sizeof(_SizeStringMetadata);
    if (bufferSize < physicalSize)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, ss, physicalSize);
    end && (*end = POINTER_OFFSET(buffer, physicalSize));
    return (_SizeString*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(SizeString, Compare, int64_t, _SizeString* data1, _SizeString* data2)
{
    _SizeStringMetadata* ss1 = (_SizeStringMetadata*)data1;
    _SizeStringMetadata* ss2 = (_SizeStringMetadata*)data2;
    int64_t sizeComparison = ss1->size - ss2->size;
    uint64_t minimalComparisonSize = 0 > sizeComparison ? ss1->size : ss2->size;
    char* p1 = ss1->data;
    char* p2 = ss2->data;
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
