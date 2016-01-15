#include "U64.h"

int LEADING_ONE_MAP[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8
};

NAMESPACE_METHOD_IMPLEMENT(U64, Anylize, void, uint64_t value, int* countOfDataBytes, int* countOfDataBytesAdjustment, int* countOfExclusiveDataBytes)
{
    uint64_t tester = value;
    /* Anyway, 1 byte at least is required to represent the value. */
    *countOfDataBytes = 1;
    for (tester >>= 8; tester; (tester >>= 8), ++*countOfDataBytes)
        ;

    *countOfDataBytesAdjustment = 0;
    if (8 > *countOfDataBytes)
    {
        uint8_t mask = ~(-1 << (8 - *countOfDataBytes /* -1 shared bit + 1 delimiter*/));
        uint8_t lastDataByte = value >> ((*countOfDataBytes - 1) << 3);
        if ((lastDataByte & mask) == lastDataByte)
        {
            *countOfDataBytesAdjustment = -1;
        }
    }

    *countOfExclusiveDataBytes = *countOfDataBytes + *countOfDataBytesAdjustment;
}

NAMESPACE_METHOD_IMPLEMENT(U64, EvaluatePhysicalSize, uint8_t, uint64_t value)
{
    if (0x80LL > value)
    {
        return 1;
    }

    int countOfDataBytes = 0;
    int countOfDataBytesAdjustment = 0;
    int countOfExclusiveDataBytes = 0;
    NONINSTANTIAL_NAMESPACE_METHOD_CALL(U64, Anylize, value, &countOfDataBytes, &countOfDataBytesAdjustment, &countOfExclusiveDataBytes);
    return 1 + (8 == countOfExclusiveDataBytes) + countOfExclusiveDataBytes;
}

NAMESPACE_METHOD_IMPLEMENT(U64, PhysicalSize, uint8_t, const void* data)
{
    if (0 <= VALUE_OF_POINTER_I8(data))
    {
        return 1;
    }

    int countOfDataBytes = LEADING_ONE_MAP[VALUE_OF_POINTER_U8(data)];
    int countOfControlBits = 1 + countOfDataBytes;
    return countOfDataBytes + (countOfControlBits >> 3) + !!(7 & countOfControlBits);
}

NAMESPACE_METHOD_IMPLEMENT(U64, Reinterpret, void*, void* buffer, bool backward, void** end)
{
    uint8_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(U64, PhysicalSize, buffer);
    end && (*end = POINTER_OFFSET(buffer, size * (backward ? DIRECTIONAL_ONE_STEP_BACKWARD : DIRECTIONAL_ONE_STEP_FORWARD)));
    return buffer;
}

NAMESPACE_METHOD_IMPLEMENT(U64, Construct, void*, uint64_t value, void* buffer, bool backward, void** end)
{
    DIRECTIONAL_ONE_STEP oneStep = backward ? DIRECTIONAL_ONE_STEP_BACKWARD : DIRECTIONAL_ONE_STEP_FORWARD;
    if (0x80LL > value)
    {
        *POINTER_U8(buffer) = (uint8_t)value;
        end && (*end = POINTER_OFFSET(buffer, oneStep));
        return buffer;
    }

    uint8_t* source = NULL;
    int countOfDataBytes = 0;
    int countOfDataBytesAdjustment = 0;
    int countOfExclusiveDataBytes = 0;

    NONINSTANTIAL_NAMESPACE_METHOD_CALL(U64, Anylize, value, &countOfDataBytes, &countOfDataBytesAdjustment, &countOfExclusiveDataBytes);

    uint8_t* iterator = buffer;
    uint8_t* controlByte = iterator;
    uint8_t* shareDataByteWithControl = NULL;
    *controlByte = 0;
    if (8 == countOfExclusiveDataBytes)
    {
        iterator += oneStep;
        *iterator = 0;
    }

    if (countOfDataBytesAdjustment)
    {
        shareDataByteWithControl = iterator;
    }

    iterator += oneStep;
    end && (*end = iterator + countOfExclusiveDataBytes * oneStep);
    int count = countOfExclusiveDataBytes;
    for(source = (uint8_t*)&value; count; --count)
    {
        *iterator = *source++;
        iterator += oneStep;
    }

    /*
     * Do these two steps as the last step in order to make sure any temporary interpretation of
     * the number won't be greater than it real value
     */
    *controlByte |= ((uint8_t)-1 << (8 - countOfExclusiveDataBytes));
    shareDataByteWithControl && (*shareDataByteWithControl |= ((uint8_t*)&value)[countOfExclusiveDataBytes]);
    return buffer;
}

NAMESPACE_METHOD_IMPLEMENT(U64, Value, uint64_t, const void* data, bool backward)
{
    if (0 <= VALUE_OF_POINTER_I8(data))
    {
        return VALUE_OF_POINTER_U8(data);
    }

    DIRECTIONAL_ONE_STEP oneStep = backward ? DIRECTIONAL_ONE_STEP_BACKWARD : DIRECTIONAL_ONE_STEP_FORWARD;
    int countOfDataBytes = LEADING_ONE_MAP[VALUE_OF_POINTER_U8(data)];
    if (8 == countOfDataBytes)
    {
        /* adjust control byte since the first byte is filled by 1 */
        data = POINTER_OFFSET(data, oneStep);
    }

    uint64_t value = ((uint8_t)-1 >> countOfDataBytes) & VALUE_OF_POINTER_U8(data);
    for (; countOfDataBytes; --countOfDataBytes)
    {
        value <<= 8;
        value |= POINTER_U8(data)[countOfDataBytes * oneStep];
    }

    return value;
}

NAMESPACE_METHOD_IMPLEMENT(U64, Compare, int64_t, const void* data1, bool data1Backward, const void* data2, bool data2Backward)
{
    uint8_t data1ControlByte = VALUE_OF_POINTER_U8(data1);
    uint8_t data2ControlByte = VALUE_OF_POINTER_U8(data2);
    int64_t result = 0LL + data1ControlByte - data2ControlByte;
    if (result)
    {
        return result;
    }

    DIRECTIONAL_ONE_STEP data1OneStep = data1Backward ? DIRECTIONAL_ONE_STEP_BACKWARD : DIRECTIONAL_ONE_STEP_FORWARD;
    DIRECTIONAL_ONE_STEP data2OneStep = data2Backward ? DIRECTIONAL_ONE_STEP_BACKWARD : DIRECTIONAL_ONE_STEP_FORWARD;
    int countOfDataBytes = LEADING_ONE_MAP[data1ControlByte];
    if (8 == countOfDataBytes)
    {
        /* adjust control byte since the first byte is filled by 1 */
        data1 = POINTER_OFFSET(data1, data1OneStep);
        data2 = POINTER_OFFSET(data2, data2OneStep);
    }


    uint64_t value1 = ((uint8_t)-1 >> countOfDataBytes) & data1ControlByte;
    uint64_t value2 = ((uint8_t)-1 >> countOfDataBytes) & data2ControlByte;
    for (; value1 == value2 && countOfDataBytes; --countOfDataBytes)
    {
        value1 = POINTER_U8(data1)[countOfDataBytes * data1OneStep];
        value2 = POINTER_U8(data2)[countOfDataBytes * data2OneStep];
    }

    return value1 - value2;
}

NAMESPACE_METHOD_IMPLEMENT(U64, CopyTo, uint8_t, const void* source, bool sourceBackward, void* target, bool targetBackward, uint64_t targetSize)
{
    uint8_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(U64, PhysicalSize, source);
    if (size > targetSize)
    {
        return 0;
    }

    int sourceStep = sourceBackward ? DIRECTIONAL_ONE_STEP_BACKWARD : DIRECTIONAL_ONE_STEP_FORWARD;
    int targetStep = targetBackward ? DIRECTIONAL_ONE_STEP_BACKWARD : DIRECTIONAL_ONE_STEP_FORWARD;
    uint8_t* from = POINTER_U8(source);
    uint8_t* to = POINTER_U8(target);
    uint8_t copyCounter = size;
    for (; copyCounter; --copyCounter)
    {
        *from = *to;
        from += sourceStep;
        to += targetStep;
    }

    return size;
}

NAMESPACE_INITIALIZE(U64)
{
    NAMESPACE_METHOD_INITIALIZE(U64, EvaluatePhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(U64, PhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(U64, Reinterpret)
    NAMESPACE_METHOD_INITIALIZE(U64, Construct)
    NAMESPACE_METHOD_INITIALIZE(U64, Value)
    NAMESPACE_METHOD_INITIALIZE(U64, Compare)
    NAMESPACE_METHOD_INITIALIZE(U64, CopyTo)
};
