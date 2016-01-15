#include "SizedString.h"

#pragma pack(push)
#pragma pack(1)
PLAIN_OLD_DATA(_SizedStringMetadata)
{
    uint64_t size;
    char data[0];
    _SizedString string[0];
};
#pragma pack(pop)

NAMESPACE_METHOD_IMPLEMENT(SizedString, EvaluatePhysicalSize, uint64_t, uint64_t size)
{
    return size + sizeof(_SizedStringMetadata);
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

    uint64_t capacity = dataSize + sizeof(_SizedStringMetadata);
    physicalSize && (*physicalSize = capacity);
    if (bufferSize < capacity)
    {
        return NULL;
    }

    _SizedStringMetadata* ss = (_SizedStringMetadata*)buffer;
    ss->size = dataSize;
    I_MEMMOVE(ss->data, data, dataSize);
    return ss->string;
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
    _SizedStringMetadata* ss = (_SizedStringMetadata*)POINTER_OFFSET(data, -sizeof(_SizedStringMetadata));
    uint64_t physicalSize = ss->size + sizeof(_SizedStringMetadata);
    void* buffer = I_MALLOC(physicalSize);
    if (!buffer)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, ss, physicalSize);
    return (_SizedString*)POINTER_OFFSET(buffer, sizeof(_SizedStringMetadata));
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Delete, void, _SizedString* data)
{
    I_FREE(POINTER_OFFSET(data, -sizeof(_SizedStringMetadata)));
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Size, uint64_t, const _SizedString* data)
{
    return ((_SizedStringMetadata*)POINTER_OFFSET(data, -sizeof(_SizedStringMetadata)))->size;
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Data, char*, _SizedString* data)
{
    return data->data;
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Begin, void*, _SizedString* data)
{
    return POINTER_OFFSET(data, -NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedString, MetadataSize, data));
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, End, void*, _SizedString* data)
{
    return POINTER_OFFSET(data, NONINSTANTIAL_NAMESPACE_METHOD_CALL(SizedString, Size, data));
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, MetadataSize, uint64_t, const _SizedString* data)
{
    return sizeof(_SizedStringMetadata);
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, CopyTo, _SizedString*, const _SizedString* data, void* buffer, uint64_t bufferSize, OUT void** end)
{
    _SizedStringMetadata* ss = (_SizedStringMetadata*)POINTER_OFFSET(data, -sizeof(_SizedStringMetadata));
    uint64_t physicalSize = ss->size + sizeof(_SizedStringMetadata);
    if (bufferSize < physicalSize)
    {
        return NULL;
    }

    I_MEMMOVE(buffer, ss, physicalSize);
    end && (*end = POINTER_OFFSET(buffer, physicalSize));
    return (_SizedString*)POINTER_OFFSET(buffer, sizeof(_SizedStringMetadata));
}

NAMESPACE_METHOD_IMPLEMENT(SizedString, Compare, int64_t, _SizedString* data1, _SizedString* data2)
{
    _SizedStringMetadata* ss1 = (_SizedStringMetadata*)POINTER_OFFSET(data1, -sizeof(_SizedStringMetadata));
    _SizedStringMetadata* ss2 = (_SizedStringMetadata*)POINTER_OFFSET(data2, -sizeof(_SizedStringMetadata));
    int64_t sizeComparison = ss1->size - ss2->size;
    uint64_t minimalComparisonSize = 0 > sizeComparison ? ss1->size : ss2->size;
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
