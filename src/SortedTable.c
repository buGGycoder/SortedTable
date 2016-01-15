#include "SortedTable.h"

#include "NonBlocking.h"
#include "RefCounter.h"
#include "SpinLock.h"
#include "SizedBuffer.h"

#define WAIT_FOR_NO_READER_TIMEOUT (30ULL * 1000000ULL)
#define REINTERPRET_TIMEOUT (30ULL * 1000000ULL)
#define CONSTRUCT_TIMEOUT (30ULL * 1000000ULL)
#define REBUILD_TIMEOUT (30ULL * 1000000ULL)
#define COPY_TO_TIMEOUT (30ULL * 1000000ULL)
#define SET_READ_ONLY_TIMEOUT (30ULL * 1000000ULL)


#pragma pack(push)
#pragma pack(1)
PLAIN_OLD_DATA(_SortedTableHeader)
{
    volatile uint64_t size;
    MEMBER_FIELD_DEFINE(volatile uint32_t, signature);
    volatile uint32_t version;
    MEMBER_FIELD_DEFINE(_RefCounter, readerCounter);
    MEMBER_FIELD_DEFINE(_RefCounter, writerCounter);
    MEMBER_FIELD_DEFINE(NonBlockingStateMachine, tableStateMachine);
    MEMBER_FIELD_DEFINE(NonBlockingStateMachine, dataStateMachine);
    MEMBER_FIELD_DEFINE(NonBlockingStateMachine, indexStateMachine);
    volatile uint64_t dataBegin;
    volatile uint64_t dataEnd;
    volatile uint64_t indexBegin;
    volatile uint64_t indexEnd;
    volatile uint64_t scale;
    volatile uint64_t wasted;
    volatile uint64_t initialized : 1;
    volatile uint64_t initializationFailed : 1;
    volatile uint64_t readOnly : 1;
};

PLAIN_OLD_DATA(_SortedTable)
{
    MEMBER_FIELD_DEFINE(_SortedTableHeader, header);
};
#pragma pack(pop)

#define SORTTABLE_SIGNATURE 'LBTS'
#define SORTTABLE_VERSION (1)

typedef bool TYPE_CALLBACK(void* context);

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _CopyArray, bool, void* source, INTEGER_SCALE sourceScale, void* target, INTEGER_SCALE targetScale, uint64_t count)
{
    if (sourceScale > targetScale)
    {
        return false;
    }
    else if (sourceScale == targetScale)
    {
        if (source == target)
        {
            return true;
        }
    }
    else if (source > target)
    {
        return false;
    }

    uint64_t counter = 0;
    uint64_t iter = 0;
    uint16_t pattern = (((uint16_t)-1 & sourceScale) << 8) | ((uint16_t)-1 & targetScale);
    switch(pattern)
    {
    case 0x0000:
    case 0x0101:
    case 0x0202:
    case 0x0303:
        I_MEMMOVE(target, source, (1ULL << sourceScale) * count);
        break;

    case 0x0001:
    {
        uint8_t* src = (uint8_t*)source;
        uint16_t* tgt = (uint16_t*)target;
        for (; counter < count; ++counter)
        {
            iter = count - counter - 1;
            tgt[iter] = src[iter];
        }

        break;
    }

    case 0x0002:
    {
        uint8_t* src = (uint8_t*)source;
        uint32_t* tgt = (uint32_t*)target;
        for (; counter < count; ++counter)
        {
            iter = count - counter - 1;
            tgt[iter] = src[iter];
        }

        break;
    }

    case 0x0003:
    {
        uint8_t* src = (uint8_t*)source;
        uint64_t* tgt = (uint64_t*)target;
        for (; counter < count; ++counter)
        {
            iter = count - counter - 1;
            tgt[iter] = src[iter];
        }

        break;
    }

    case 0x0102:
    {
        uint16_t* src = (uint16_t*)source;
        uint32_t* tgt = (uint32_t*)target;
        for (; counter < count; ++counter)
        {
            iter = count - counter - 1;
            tgt[iter] = src[iter];
        }

        break;
    }

    case 0x0103:
    {
        uint16_t* src = (uint16_t*)source;
        uint64_t* tgt = (uint64_t*)target;
        for (; counter < count; ++counter)
        {
            iter = count - counter - 1;
            tgt[iter] = src[iter];
        }

        break;
    }

    case 0x0203:
    {
        uint32_t* src = (uint32_t*)source;
        uint64_t* tgt = (uint64_t*)target;
        for (; counter < count; ++counter)
        {
            iter = count - counter - 1;
            tgt[iter] = src[iter];
        }

        break;
    }

    default:
        return false;
    }

    return true;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, StateMachine, NonBlockingStateMachine*, _SortedTable* table)
{
    return table->header->tableStateMachine;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, Size, uint64_t, _SortedTable* table)
{
    return table->header->size;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, AvaliableMemory, uint64_t, _SortedTable* table)
{
    return table->header->indexBegin > table->header->dataEnd ? table->header->indexBegin - table->header->dataEnd : 0;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, WastedMemory, uint64_t, _SortedTable* table)
{
    return table->header->wasted;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, ReaderCount, uint32_t, _SortedTable* table)
{
    return table->header->readerCounter->counter - 1;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, WriterCount, uint32_t, _SortedTable* table)
{
    return table->header->writerCounter->counter - 1;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _ScaleInBytes, uint8_t, _SortedTable* table)
{
    return ((POINTER_DISTANCE(1, 0)) << table->header->scale);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _GetDataBegin, void*, _SortedTable* table)
{
    return POINTER_OFFSET(table, table->header->dataBegin);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _SetDataBegin, bool, _SortedTable* table, void* dataBegin)
{
    if (!ASSERT((
                    dataBegin >= POINTER_OFFSET(table, sizeof(_SortedTableHeader))
                    && dataBegin <= POINTER_OFFSET(table, table->header->dataEnd)
                    && dataBegin <= POINTER_OFFSET(table, table->header->indexBegin))
                , "DEV BUG!!! _SetDataBegin out of boundary!\n"))
    {
        return false;
    }

    table->header->dataBegin = POINTER_DISTANCE(dataBegin, table);
    return true;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _GetDataEnd, void*, _SortedTable* table)
{
    return POINTER_OFFSET(table, table->header->dataEnd);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _SetDataEnd, bool, _SortedTable* table, void* dataEnd)
{
    if (!ASSERT((
                    dataEnd >= POINTER_OFFSET(table, sizeof(_SortedTableHeader))
                    && dataEnd >= POINTER_OFFSET(table, table->header->dataBegin)
                    && dataEnd <= POINTER_OFFSET(table, table->header->indexBegin))
                , "DEV BUG!!! _SetDataEnd out of boundary!\n"))
    {
        return false;
    }

    table->header->dataEnd = POINTER_DISTANCE(dataEnd, table);
    return true;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _MaximumPossibleDataEnd, void*, _SortedTable* table)
{
    return POINTER_OFFSET(table, POINTER_ROUND_DOWN_TO_SCALE(table->header->size, table->header->scale));
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _GetIndexBegin, void*, _SortedTable* table)
{
    return POINTER_OFFSET(table, table->header->indexBegin);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _SetIndexBegin, bool, _SortedTable* table, void* indexBegin)
{
    if (!ASSERT((
                    indexBegin >= POINTER_OFFSET(table, table->header->dataEnd)
                    && indexBegin <= POINTER_OFFSET(table, table->header->indexEnd)
                    && indexBegin <= POINTER_OFFSET(table, POINTER_ROUND_DOWN_TO_SCALE(table->header->size, table->header->scale)))
                , "DEV BUG!!! _SetIndexBegin out of boundary! %d, %d, %d\n",
                indexBegin >= POINTER_OFFSET(table, table->header->dataEnd),
                indexBegin <= POINTER_OFFSET(table, table->header->indexEnd),
                indexBegin <= POINTER_OFFSET(table, POINTER_ROUND_DOWN_TO_SCALE(table->header->size, table->header->scale))))
    {
        return false;
    }

    table->header->indexBegin = POINTER_DISTANCE(indexBegin, table);
    return true;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _GetIndexEnd, void*, _SortedTable* table)
{
    return POINTER_OFFSET(table, table->header->indexEnd);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _SetIndexEnd, bool, _SortedTable* table, void* indexEnd)
{
    if (!ASSERT((
                    indexEnd >= POINTER_OFFSET(table, table->header->dataEnd)
                    && indexEnd >= POINTER_OFFSET(table, table->header->indexBegin)
                    && indexEnd <= POINTER_OFFSET(table, POINTER_ROUND_DOWN_TO_SCALE(table->header->size, table->header->scale)))
                , "DEV BUG!!! _SetIndexBegin out of boundary! %d, %d, %d\n",
                indexEnd >= POINTER_OFFSET(table, table->header->dataEnd),
                indexEnd >= POINTER_OFFSET(table, table->header->indexBegin),
                indexEnd <= POINTER_OFFSET(table, POINTER_ROUND_DOWN_TO_SCALE(table->header->size, table->header->scale))))
    {
        return false;
    }

    table->header->indexEnd = POINTER_DISTANCE(indexEnd, table);
    return true;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _Count, uint64_t, _SortedTable* table)
{
    return (table->header->indexEnd - table->header->indexBegin) / ((POINTER_DISTANCE(1, 0)) << table->header->scale);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _GetIndex, uint64_t, _SortedTable* table, void* indexPointer)
{
    switch(table->header->scale)
    {
    case INTEGER_SCALE_0:
        return *(uint8_t*)indexPointer;

    case INTEGER_SCALE_1:
        return *(uint16_t*)indexPointer;

    case INTEGER_SCALE_2:
        return *(uint32_t*)indexPointer;

    case INTEGER_SCALE_3:
        return *(uint64_t*)indexPointer;

    default:
        /* maybe we should return error??? */
        return *(uint64_t*)indexPointer;
    }
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _GetIndexInArray, uint64_t, _SortedTable* table, void* arrayPointer, int64_t arrayIndex)
{
    void* indexPointer = POINTER_OFFSET(arrayPointer, NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _ScaleInBytes, table) * arrayIndex);
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndex, table, indexPointer);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _SetIndex, bool, _SortedTable* table, void* indexPointer, uint64_t indexValue)
{
    switch(table->header->scale)
    {
    case INTEGER_SCALE_0:
        *(uint8_t*)indexPointer = (uint8_t)indexValue;
        break;

    case INTEGER_SCALE_1:
        *(uint16_t*)indexPointer = (uint16_t)indexValue;
        break;

    case INTEGER_SCALE_2:
        *(uint32_t*)indexPointer = (uint32_t)indexValue;
        break;

    case INTEGER_SCALE_3:
        *(uint64_t*)indexPointer = (uint64_t)indexValue;
        break;

    default:
        return false;
    }

    return true;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _SetIndexInArray, uint64_t, _SortedTable* table, void* arrayPointer, int64_t arrayIndex, uint64_t indexValue)
{
    void* indexPointer = POINTER_OFFSET(arrayPointer, NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _ScaleInBytes, table) * arrayIndex);
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndex, table, indexPointer, indexValue);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _IndexPointerAt, void*, _SortedTable* table, int64_t order)
{
    void* indexPointer = POINTER_OFFSET(table, table->header->indexBegin + order * NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _ScaleInBytes, table));
    if (POINTER_OFFSET(table, POINTER_ROUND_DOWN_TO_SCALE(table->header->size, table->header->scale)) < indexPointer)
    {
        return NULL;
    }

    return indexPointer;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _GetIndexAt, _KeyValue*, _SortedTable* table, int64_t order)
{
    void* indexPointer = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _IndexPointerAt, table, order);
    if (!indexPointer)
    {
        return NULL;
    }

    return POINTER_OFFSET(table, NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndex, table, indexPointer));
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _SetIndexAt, bool, _SortedTable* table, int64_t order, _KeyValue* data)
{
    uint64_t indexValue = POINTER_DISTANCE(data, table);
    if (GET_INTEGER_SCALE(indexValue) > table->header->scale)
    {
        return false;
    }

    void* indexPointer = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _IndexPointerAt, table, order);
    if (!indexPointer)
    {
        return false;
    }

    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndex, table, indexPointer, indexValue);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _InsertIndexAt, bool, _SortedTable* table, int64_t order, _KeyValue* data)
{
    void* indexBegin = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexBegin, table);
    void* indexEnd = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexEnd, table);
    void* dataEnd = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetDataEnd, table);
    void* maximumPossibleDataEnd = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _MaximumPossibleDataEnd, table);
    uint8_t scaleInBytes = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _ScaleInBytes, table);
    uint8_t leftMovingPriority = !!(dataEnd < indexBegin);
    uint8_t rightMovingPriority = !!(indexEnd < maximumPossibleDataEnd);
    if (!(leftMovingPriority || rightMovingPriority))
    {
        return false;
    }

    leftMovingPriority <<= 1;
    rightMovingPriority <<= 1;

    uint64_t count = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _Count, table);

    /*
     * Move right when count of left items equals to the count of right items.
     * Such will make more space for data since data is in left side.
     */
    if (count / 2 > order)
    {
        leftMovingPriority |= 1;
    }
    else
    {
        rightMovingPriority |= 1;
    }

    if (leftMovingPriority > rightMovingPriority)
    {
        indexBegin = POINTER_OFFSET(indexBegin, -scaleInBytes);
        void* indexPointer = POINTER_OFFSET(indexBegin, order * scaleInBytes);

        /* step1. move the first item if possible
         *        or set the data as the first data if order is '0'
         */
        if (order)
        {
            if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndexInArray, table, indexBegin, 0,
                    NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexInArray, table, indexBegin, 1)))
            {
                return false;
            }
        }
        else
        {
            if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndex, table, indexPointer, POINTER_DISTANCE(data, table)))
            {
                return false;
            }
        }

        /* step2. modify indexBegin to append the index array */
        if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndexBegin, table, indexBegin))
        {
            return false;
        }

        /* step3. move the reset items if possible and set the data */
        if (order)
        {
            if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _CopyArray,
                    POINTER_OFFSET(indexBegin, 2 * scaleInBytes), table->header->scale,
                    POINTER_OFFSET(indexBegin, scaleInBytes), table->header->scale, order - 1))
            {
                return false;
            }

            if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndex, table, indexPointer, POINTER_DISTANCE(data, table)))
            {
                return false;
            }
        }

        return true;
    }

    indexEnd = POINTER_OFFSET(indexEnd, scaleInBytes);
    void* indexPointer = POINTER_OFFSET(indexBegin, order * scaleInBytes);

    /* step1. move the last item if possible
     *        or set the data as the last data if order is last
     */
    if (order < count)
    {
        if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndexInArray, table, indexBegin, count,
                NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexInArray, table, indexBegin, count - 1)))
        {
            return false;
        }
    }
    else
    {
        if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndex, table, indexPointer, POINTER_DISTANCE(data, table)))
        {
            return false;
        }
    }

    /* step2. modify indexEnd to append the index array */
    if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndexEnd, table, indexEnd))
    {
        return false;
    }

    /* step3. move the reset items if possible and set the data */
    if (order < count)
    {
        if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _CopyArray,
                indexPointer, table->header->scale,
                POINTER_OFFSET(indexPointer, scaleInBytes), table->header->scale, count - order - 1 /* 1 already moved */))
        {
            return false;
        }

        if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndex, table, indexPointer, POINTER_DISTANCE(data, table)))
        {
            return false;
        }
    }

    return true;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _RemoveIndexAt, bool, _SortedTable* table, int64_t order)
{
    void* indexBegin = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexBegin, table);
    void* indexEnd = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexEnd, table);
    uint8_t scaleInBytes = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _ScaleInBytes, table);
    uint8_t leftMovingPriority = 0;
    uint8_t rightMovingPriority = 0;
    uint64_t count = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _Count, table);

    /*
     * Move right when count of left items equals to the count of right items.
     * Such will make more space for data since data is in left side.
     */
    if (count / 2 > order)
    {
        leftMovingPriority |= 1;
    }
    else
    {
        rightMovingPriority |= 1;
    }

    if (leftMovingPriority > rightMovingPriority)
    {
        indexBegin = POINTER_OFFSET(indexBegin, scaleInBytes);
        /* step1. move items to remove the index item */
        if (order)
        {
            if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _CopyArray,
                    POINTER_OFFSET(indexBegin, -scaleInBytes), table->header->scale,
                    indexBegin, table->header->scale, order))
            {
                return false;
            }
        }

        /* step2. modify indexBegin to shorten the index array */
        if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndexBegin, table, indexBegin))
        {
            return false;
        }

        return true;
    }

    indexEnd = POINTER_OFFSET(indexEnd, -scaleInBytes);
    /* step1. move items to remove the index item */
    if (count > order + 1)
    {
        if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _CopyArray,
                POINTER_OFFSET(indexBegin, (order + 1) * scaleInBytes), table->header->scale,
                POINTER_OFFSET(indexBegin, order * scaleInBytes), table->header->scale, count - 1 - order))
        {
            return false;
        }
    }

    /* step2. modify indexEnd to shorten the index array */
    if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndexEnd, table, indexEnd))
    {
        return false;
    }

    return true;
}

#pragma pack(push)
#pragma pack(1)
CLASS(_SearchCancellationContext)
{
    NonBlockingState enterState;
    NonBlockingState* currentState;
};
#pragma pack(pop)

MEMBER_METHOD_IMPLEMENT(_SearchCancellationContext, _SearchCancellationContext, _SearchCancellationContext*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        break;
    case METHOD_DTOR:
        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(_SearchCancellationContext)
{
};

static bool _Search_ShouldCancel(void* context)
{
    _SearchCancellationContext* ctx = (_SearchCancellationContext*)context;
    return ctx->enterState != * ctx->currentState;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, _Search, bool, _SortedTable* table, _SizeString* key, TYPE_CALLBACK* shouldCancel, void* cancellationJustfication, int64_t* candidate)
{
    if (!candidate)
    {
        return false;
    }

    if (shouldCancel && shouldCancel(cancellationJustfication))
    {
        return false;
    }

    int64_t end = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _Count, table);
    int64_t begin = 0;
    while (begin < end)
    {
        int64_t tester = (begin + end) >> 1;
        _SizeString* candidateString = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, KeyUnsafe,
                                       NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexAt,
                                               table,
                                               tester));
        if (shouldCancel && shouldCancel(cancellationJustfication))
        {
            return false;
        }

        int comparison = SizeString.Compare(key, candidateString);
        if (0 < comparison)
        {
            begin = tester + 1;
        }
        else if (0 > comparison)
        {
            end = tester;
        }
        else
        {
            *candidate = tester;
            return true;
        }
    }

    *candidate = -begin - 1;
    return true;
}

#pragma pack(push)
#pragma pack(1)
CLASS(_SortedTableInitializeContext)
{
    void* buffer;
    uint64_t bufferSize;
    _SortedTable* table;
    uint64_t timeout;
};
#pragma pack(pop)

MEMBER_METHOD_IMPLEMENT(_SortedTableInitializeContext, _SortedTableInitializeContext, _SortedTableInitializeContext*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        break;
    case METHOD_DTOR:
        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(_SortedTableInitializeContext)
{
};

static bool _Reinterpret_MoveIndexes(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableInitializeContext* ctx = (_SortedTableInitializeContext*)context;
    INTEGER_SCALE scale = GET_INTEGER_SCALE(ctx->bufferSize);
    uint64_t count = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _Count, ctx->table);
    uint8_t scaleInBytes = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _ScaleInBytes, ctx->table);
    uint64_t indexEnd = POINTER_ROUND_DOWN_TO_SCALE(ctx->bufferSize, scale);
    uint64_t indexBegin = indexEnd - count * scaleInBytes;
    void* target = POINTER_OFFSET(ctx->buffer, indexBegin);
    void* source = POINTER_OFFSET(ctx->buffer, ctx->table->header->indexBegin);
    if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _CopyArray, source, ctx->table->header->scale, target, scale, count))
    {
        return false;
    }

    int64_t wastedAjustment = (int64_t)ctx->table->header->indexEnd - (int64_t)POINTER_ROUND_DOWN_TO_SCALE(ctx->table->header->size, ctx->table->header->scale);
    if (0 > wastedAjustment)
    {
        __sync_sub_and_fetch(&ctx->table->header->wasted, (int64_t)-wastedAjustment);
    }
    else
    {
        __sync_add_and_fetch(&ctx->table->header->wasted, (int64_t)wastedAjustment);
    }

    ctx->table->header->size = ctx->bufferSize;
    ctx->table->header->scale = scale;
    ctx->table->header->indexBegin = indexBegin;
    ctx->table->header->indexEnd = indexEnd;
    return true;
}

static bool _Reinterpret_Validate(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableInitializeContext* ctx = (_SortedTableInitializeContext*)context;
    if (ctx->bufferSize < ctx->table->header->size)
    {
        return false;
    }

    if (ctx->bufferSize == ctx->table->header->size)
    {
        return true;
    }

    if (!NonBlocking.WaitForNoReaderTimed(ctx->table->header->tableStateMachine, WAIT_FOR_NO_READER_TIMEOUT, NULL))
    {
        return false;
    }

    return NonBlocking.WriteTimed(ctx->table->header->indexStateMachine, ctx->timeout, _Reinterpret_MoveIndexes, ctx, NULL, NULL);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, Reinterpret, _SortedTable*, void* buffer, uint64_t size)
{
    if (POINTER_DISTANCE(POINTER_ROUND_UP(buffer, uint64_t), buffer))
    {
        return NULL;
    }

    if (sizeof(_SortedTableHeader) >= size)
    {
        return NULL;
    }

    _SortedTable* table = (_SortedTable*)buffer;
    if (SORTTABLE_SIGNATURE != *table->header->signature)
    {
        return NULL;
    }

    _SortedTableInitializeContext* context = I_NEW(_SortedTableInitializeContext);
    context->buffer = buffer;
    context->bufferSize = size;
    context->table = table;
    context->timeout = REINTERPRET_TIMEOUT;
    if (!NonBlocking.WriteTimed(table->header->tableStateMachine, context->timeout, _Reinterpret_Validate, context, NULL, NULL))
    {
        table = NULL;
    }

    I_DELETE(_SortedTableInitializeContext, context);
    return table;
}

static bool _Construct_InitializeData(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableInitializeContext* ctx = (_SortedTableInitializeContext*)context;
    ctx->table->header->dataBegin = ctx->table->header->dataEnd = sizeof(_SortedTableHeader);
    return true;
}

static bool _Construct_InitializeIndex(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableInitializeContext* ctx = (_SortedTableInitializeContext*)context;
    ctx->table->header->indexBegin = ctx->table->header->indexEnd = POINTER_ROUND_DOWN_TO_SCALE(ctx->bufferSize, ctx->table->header->scale);
    return true;
}

static bool _Construct_Initialize(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableInitializeContext* ctx = (_SortedTableInitializeContext*)context;
    if (!NonBlocking.WaitForNoReaderTimed(ctx->table->header->tableStateMachine, WAIT_FOR_NO_READER_TIMEOUT, NULL))
    {
        return false;
    }

    ctx->table->header->size = ctx->bufferSize;
    ctx->table->header->scale = GET_INTEGER_SCALE(ctx->bufferSize);
    ctx->table->header->version = SORTTABLE_VERSION;
    /* initialize both reader and writer counter to 1 to represent god */
    RefCounter.AddRef(ctx->table->header->readerCounter);
    RefCounter.AddRef(ctx->table->header->writerCounter);

    if (!_Construct_InitializeData(enterState, currentState, context))
    {
        return false;
    }

    if (!_Construct_InitializeIndex(enterState, currentState, context))
    {
        return false;
    }

    ctx->table->header->initialized = true;
    return !(ctx->table->header->initializationFailed = ctx->table->header->dataEnd >= ctx->table->header->indexBegin);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, Construct, _SortedTable*, void* buffer, uint64_t size, bool force, bool clean)
{
    if (POINTER_DISTANCE(POINTER_ROUND_UP(buffer, uint64_t), buffer))
    {
        return NULL;
    }

    if (sizeof(_SortedTableHeader) >= size)
    {
        return NULL;
    }

    _SortedTable* table = (_SortedTable*)buffer;
    if (SORTTABLE_SIGNATURE == __sync_lock_test_and_set(table->header->signature, SORTTABLE_SIGNATURE) && !force)
    {
        return NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, Reinterpret, buffer, size);
    }

    if (clean)
    {
        memset(table, 0, POINTER_DISTANCE(table->header->signature, table));
        void* pMemoryAfterSignature = POINTER_OFFSET(table->header->signature, sizeof(table->header->signature));
        memset(pMemoryAfterSignature, 0, size - POINTER_DISTANCE(pMemoryAfterSignature, table));
    }
    else
    {
        memset(table, 0, POINTER_DISTANCE(table->header->signature, table));
        void* pMemoryAfterSignature = POINTER_OFFSET(table->header->signature, sizeof(table->header->signature));
        memset(pMemoryAfterSignature, 0, sizeof(table->header) - POINTER_DISTANCE(pMemoryAfterSignature, table));
    }

    _SortedTableInitializeContext* context = I_NEW(_SortedTableInitializeContext);
    context->buffer = buffer;
    context->bufferSize = size;
    context->table = table;
    context->timeout = CONSTRUCT_TIMEOUT;
    if (!NonBlocking.WriteTimed(table->header->tableStateMachine, context->timeout, _Construct_Initialize, context, NULL, NULL))
    {
        table = NULL;
    }

    I_DELETE(_SortedTableInitializeContext, context);
    return table;
}

static bool _Rebuild_RewriteIndexes(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTable* table = (_SortedTable*)context;
    _KeyValue* currentData = (_KeyValue*)POINTER_OFFSET(table, table->header->dataBegin);
    _KeyValue* dataEnd = (_KeyValue*)POINTER_OFFSET(table, table->header->dataEnd);
    int64_t indexEnd = (int64_t)table->header->indexEnd;
    table->header->indexBegin = table->header->indexEnd = POINTER_ROUND_DOWN_TO_SCALE(table->header->size, table->header->scale);
    int64_t wastedAjustment = (int64_t)indexEnd - (int64_t)table->header->indexEnd;
    if (0 > wastedAjustment)
    {
        __sync_sub_and_fetch(&table->header->wasted, (int64_t)-wastedAjustment);
    }
    else
    {
        __sync_add_and_fetch(&table->header->wasted, (int64_t)wastedAjustment);
    }

    for (; currentData < dataEnd; currentData = (_KeyValue*)KeyValue.End(currentData))
    {
        if(!KeyValue.TryFixStateUnsafe(currentData))
        {
            continue;
        }

        _SizeString* currentKey = KeyValue.KeyUnsafe(currentData);
        int64_t candidate = -1;
        if(!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _Search, table, currentKey, NULL, NULL, &candidate))
        {
            continue;
        }

        _KeyValue* candidateData = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexAt, table, candidate);
        bool isCandiateDeleted = KeyValue.IsDeleted(candidateData);
        ASSERT(!isCandiateDeleted, "DEV BUG!!! The rebuilt candidate is deleted?!");
        bool isCurrentDeleted = KeyValue.IsDeleted(currentData);
        if (0 > candidate)
        {
            if (!isCurrentDeleted && !NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _InsertIndexAt, table, -candidate - 1, currentData))
            {
                /* log for insert error */
            }

            continue;
        }

        uint64_t candiateTimestamp = *KeyValue.TimeStamp(candidateData);
        uint64_t currentTimestamp = *KeyValue.TimeStamp(currentData);
        uint64_t candiateVersion = KeyValue.VersionUnsafe(candidateData);
        uint64_t currentVersion = KeyValue.VersionUnsafe(currentData);
        if (currentTimestamp < candiateTimestamp || (currentTimestamp == candiateTimestamp && currentVersion < candiateVersion))
        {
            continue;
        }

        if (isCurrentDeleted
                ? !NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _RemoveIndexAt, table, candidate)
                : !NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndexAt, table, candidate, currentData))
        {
            /* log remove error or set error */
        }
    }

    return true;
}

static bool _Rebuild_TryOnce(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTable* table = (_SortedTable*)context;
    if (!NonBlocking.WaitForNoReaderTimed(table->header->tableStateMachine, WAIT_FOR_NO_READER_TIMEOUT, NULL))
    {
        return false;
    }

    return _Rebuild_RewriteIndexes(enterState, currentState, context);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, Rebuild, _SortedTable*, _SortedTable* table)
{
    if (!NonBlocking.WriteTimed(table->header->tableStateMachine, REBUILD_TIMEOUT, _Rebuild_TryOnce, table, NULL, NULL))
    {
        table = NULL;
    }

    return table;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, IsReadOnly, bool, _SortedTable* table)
{
    return table->header->readOnly;
}

static bool _SetReadOnly_TryOnce(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTable* table = (_SortedTable*)context;
    if (!NonBlocking.WaitForNoReaderTimed(table->header->tableStateMachine, WAIT_FOR_NO_READER_TIMEOUT, NULL))
    {
        return false;
    }

    table->header->readOnly = true;
    return true;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, SetReadOnly, bool, _SortedTable* table)
{
    return NonBlocking.WriteTimed(table->header->tableStateMachine, SET_READ_ONLY_TIMEOUT, _SetReadOnly_TryOnce, table, NULL, NULL);
}

static bool _ClearReadOnly_TryOnce(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTable* table = (_SortedTable*)context;
    if (!NonBlocking.WaitForNoReaderTimed(table->header->tableStateMachine, WAIT_FOR_NO_READER_TIMEOUT, NULL))
    {
        return false;
    }

    table->header->readOnly = false;
    return true;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, ClearReadOnly, bool, _SortedTable* table)
{
    return NonBlocking.WriteTimed(table->header->tableStateMachine, SET_READ_ONLY_TIMEOUT, _ClearReadOnly_TryOnce, table, NULL, NULL);
}

#pragma pack(push)
#pragma pack(1)
CLASS(_SortedTableCopyContext)
{
    _SortedTable* source;
    _SortedTable* target;
    ERROR_CODE error;
};
#pragma pack(pop)

MEMBER_METHOD_IMPLEMENT(_SortedTableCopyContext, _SortedTableCopyContext, _SortedTableCopyContext*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        break;
    case METHOD_DTOR:
        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(_SortedTableCopyContext)
{
};

static bool _CopyTo_CopyToTarget(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableCopyContext* ctx = (_SortedTableCopyContext*)context;
    if (!NonBlocking.WaitForNoReaderTimed(ctx->target->header->tableStateMachine, WAIT_FOR_NO_READER_TIMEOUT, NULL))
    {
        ctx->error = ERROR_TIMEOUT;
        return false;
    }

    uint64_t count = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _Count, ctx->source);
    uint64_t order = 0;
    for (; order < count; ++order)
    {
        _KeyValue* kv = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexAt, ctx->source, order);
        void* buffer = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetDataEnd, ctx->target);
        uint64_t bufferSize = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, AvaliableMemory, ctx->target);
        void* end = NULL;
        uint64_t copiedSize = KeyValue.CopyUnsafe(kv, buffer, bufferSize, &end);
        if (!copiedSize)
        {
            ctx->error = ERROR_NOT_ENOUGH_MEMORY;
            return false;
        }

        if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetDataEnd, ctx->target, end))
        {
            ctx->error = ERROR_NOT_ENOUGH_MEMORY;
            return false;
        }

        if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _InsertIndexAt, ctx->target, order, buffer))
        {
            ctx->error = ERROR_NOT_ENOUGH_MEMORY;
            return false;
        }
    }

    return true;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, CopyTo, ERROR_CODE, _SortedTable* source, _SortedTable* target)
{
    if(!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, SetReadOnly, source))
    {
        return ERROR_TIMEOUT;
    }

    _SortedTableCopyContext* context = I_NEW(_SortedTableCopyContext);
    context->source = source;
    context->target = target;
    context->error = ERROR_NO;
    NonBlocking.WriteTimed(target->header->tableStateMachine, COPY_TO_TIMEOUT, _CopyTo_CopyToTarget, context, NULL, NULL);
    ERROR_CODE error = context->error;
    I_DELETE(_SortedTableCopyContext, context);
    return error;
}

#pragma pack(push)
#pragma pack(1)
CLASS(_SortedTableOpContext)
{
    _SortedTable* table;
    uint64_t timeout;
    const char* key;
    uint64_t keySize;
    const char* value;
    uint64_t valueSize;
    uint64_t version;
    bool force;
    uint64_t versionUpdated;

    _SizeString* keyString;

    bool copyExistingValue;
    int64_t candidate;
    _KeyValue* existingData;
    _SizeBuffer* existingValue;
    uint64_t existingDataVersion;
    uint64_t existingDataTimeStamp;
    uint64_t existingDataSize;
    uint64_t existingValueSize;
    _SizedString* existingValueCopy;

    _KeyValue* composingData;
    uint64_t composingDataSize;
    bool composingDataComposed;
    int64_t wastedMemroyAdjustment;

    NonBlockingState indexState;

    ERROR_CODE error;
};
#pragma pack(pop)

MEMBER_METHOD_IMPLEMENT(_SortedTableOpContext, _SortedTableOpContext, _SortedTableOpContext*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        break;
    case METHOD_DTOR:
        if (thiz->keyString)
        {
            SizeString.Delete(thiz->keyString);
            thiz->keyString = NULL;
        }

        if (thiz->existingValueCopy)
        {
            SizedString.Delete(thiz->existingValueCopy);
            thiz->existingValueCopy = NULL;
        }

        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(_SortedTableOpContext)
{
};

static bool _IsErrorEnding(ERROR_CODE* error, uint64_t* timeout, uint64_t timeElapsed)
{
    if (ERROR_NO != *error)
    {
        return true;
    }
    else if (*timeout < timeElapsed)
    {
        *error = ERROR_TIMEOUT;
        return true;
    }

    if (*timeout > timeElapsed)
    {
        *timeout -= timeElapsed;
    }
    else
    {
        *timeout = 0;
    }

    return false;
}

static bool _Search(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableOpContext* ctx = (_SortedTableOpContext*)context;
    if (enterState != *currentState)
    {
        return false;
    }

    /* Initialize to a known state for one loop */
    ctx->existingData = NULL;
    ctx->existingValue = NULL;
    ctx->existingDataVersion = 0;
    ctx->existingDataSize = 0;
    ctx->existingValueSize = 0;
    if (ctx->copyExistingValue && ctx->existingValueCopy)
    {
        SizedString.Delete(ctx->existingValueCopy);
        ctx->existingValueCopy = NULL;
    }

    _SearchCancellationContext* cancellationContext = I_NEW(_SearchCancellationContext);
    cancellationContext->enterState = enterState;
    cancellationContext->currentState = currentState;
    if(!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _Search, ctx->table, ctx->keyString, _Search_ShouldCancel, cancellationContext, &ctx->candidate))
    {
        I_DELETE(_SearchCancellationContext, cancellationContext);
        return false;
    }

    I_DELETE(_SearchCancellationContext, cancellationContext);
    if (0 > ctx->candidate)
    {
        return true;
    }

    bool deleted = false;
    ctx->existingData = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexAt, ctx->table, ctx->candidate);
    if (enterState != *currentState)
    {
        return false;
    }

    if (!KeyValue.GetValueTimed(ctx->existingData, ctx->timeout, &deleted, &ctx->existingDataVersion, &ctx->existingDataTimeStamp, NULL, ctx->copyExistingValue ? &ctx->existingValueCopy : NULL))
    {
        ctx->error = ERROR_TIMEOUT;
        return false;
    }

    if (enterState != *currentState)
    {
        return false;
    }

    ASSERT(!deleted, "DEV BUG!!! _Search got a deleted item %s => enter index: %llu, current index: %llu, value version: %llu\n", ctx->key, enterState, *currentState, ctx->existingDataVersion);
    if (!ctx->force && ctx->existingDataVersion != ctx->version)
    {
        ctx->error = ERROR_WRONG_VERSION;
        return false;
    }

    ctx->existingValue = KeyValue.ValueUnsafe(ctx->existingData);
    ctx->existingValueSize = SizeBuffer.Size(ctx->existingValue);
    ctx->existingDataSize = KeyValue.PhysicalSize(ctx->existingData) - (SizeBuffer.Capacity(ctx->existingValue) - ctx->existingValueSize);
    return enterState == *currentState;
}

static bool _Set_AppendData(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableOpContext* ctx = (_SortedTableOpContext*)context;
    if (ctx->composingData)
    {
        return true;
    }

    if (ctx->composingDataSize + NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _ScaleInBytes, ctx->table) > SortedTable.AvaliableMemory(ctx->table))
    {
        ctx->error = ERROR_NOT_ENOUGH_MEMORY;
        return false;
    }

    ctx->composingData = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetDataEnd, ctx->table);
    ctx->table->header->dataEnd += ctx->composingDataSize;
    ctx->wastedMemroyAdjustment += ctx->composingDataSize;

    if (!SortedTable.AvaliableMemory(ctx->table))
    {
        ctx->error = ERROR_NOT_ENOUGH_MEMORY;
        return false;
    }

    return true;
}

static bool _Set_InsertIndex(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableOpContext* ctx = (_SortedTableOpContext*)context;
    if (enterState != (NON_BLOCKING_STATE_CORRUPTED_MASK | ctx->indexState))
    {
        return false;
    }

    if (0 <= ctx->candidate)
    {
        return false;
    }

    if (NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _ScaleInBytes, ctx->table) > SortedTable.AvaliableMemory(ctx->table))
    {
        ctx->error = ERROR_NOT_ENOUGH_MEMORY;
        return false;
    }

    int64_t indexEnd = (int64_t)ctx->table->header->indexEnd;
    if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _InsertIndexAt, ctx->table, -ctx->candidate - 1, ctx->composingData))
    {
        return false;
    }

    ctx->wastedMemroyAdjustment -= ctx->composingDataSize;
    ctx->wastedMemroyAdjustment += (int64_t)indexEnd - (int64_t)ctx->table->header->indexEnd;

    if (!SortedTable.AvaliableMemory(ctx->table))
    {
        ctx->error = ERROR_NOT_ENOUGH_MEMORY;
        return false;
    }

    return true;
}

static bool _Set_ModifyIndex(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableOpContext* ctx = (_SortedTableOpContext*)context;
    if (enterState != (NON_BLOCKING_STATE_CORRUPTED_MASK | ctx->indexState))
    {
        return false;
    }

    if (0 > ctx->candidate)
    {
        return false;
    }

    if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _SetIndexAt, ctx->table, ctx->candidate, ctx->composingData))
    {
        return false;
    }

    ctx->wastedMemroyAdjustment -= ctx->composingDataSize;
    ctx->wastedMemroyAdjustment += ctx->existingDataSize;
    return true;
}

static bool _Set_TryOnce(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableOpContext* ctx = (_SortedTableOpContext*)context;

    uint64_t timeElapsed = 0;
    bool result = NonBlocking.ReadTimed(ctx->table->header->indexStateMachine, ctx->timeout, _Search, ctx, &ctx->indexState, &timeElapsed);
    if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
    {
        return true;
    }

    if (!result)
    {
        return false;
    }

    if (0 > ctx->candidate)
    {
        /*
         * Data doesn't exist.
         * Both data and index should be inserted.
         */
        result = NonBlocking.WriteTimed(ctx->table->header->dataStateMachine, ctx->timeout, _Set_AppendData, ctx, NULL, &timeElapsed);
        if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
        {
            return true;
        }

        if (ERROR_NO == ctx->error && !result)
        {
            ctx->error = ERROR_NOT_ENOUGH_MEMORY;
            return true;
        }

        if (!ctx->composingDataComposed)
        {
            ctx->versionUpdated = 2;
            if (!KeyValue.Initialize(ctx->composingData, ctx->composingDataSize, ctx->key, ctx->keySize, ctx->value, ctx->valueSize, ctx->versionUpdated, 0/*timeStamp*/, NULL))
            {
                ctx->error = ERROR_UNKNOWN;
                return true;
            }

            ctx->composingDataComposed = true;
        }

        bool result = NonBlocking.WriteTimed(ctx->table->header->indexStateMachine, ctx->timeout, _Set_InsertIndex, ctx, NULL, &timeElapsed);
        if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
        {
            return true;
        }

        return result;
    }
    else if (SizeBuffer.Capacity(ctx->existingValue) < ctx->valueSize)
    {
        /*
         * Data exists. The new value occupies more space than the old one.
         * Data should be inserted. Index should be modified.
         */
        result = NonBlocking.WriteTimed(ctx->table->header->dataStateMachine, ctx->timeout, _Set_AppendData, ctx, NULL, &timeElapsed);
        if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
        {
            return true;
        }

        if (ERROR_NO == ctx->error && !result)
        {
            ctx->error = ERROR_NOT_ENOUGH_MEMORY;
            return true;
        }

        NonBlockingWriteState existingDataVersion = 0;
        NonBlockingStateMachine* exisingDataStateMachine = KeyValue.StateMachine(ctx->existingData);
        if (!NonBlocking.BeginWriteTimed(exisingDataStateMachine, ctx->timeout, &existingDataVersion))
        {
            ctx->error = ERROR_TIMEOUT;
            return true;
        }

        if (existingDataVersion != (NON_BLOCKING_STATE_CORRUPTED_MASK | ctx->existingDataVersion))
        {
            NonBlocking.RevertWriteAndCheckSuccess(exisingDataStateMachine, existingDataVersion);
            return false;
        }

        ctx->versionUpdated = NON_BLOCKING_STATE_NEXT_STABLE_X2(existingDataVersion);
        if (!ctx->composingDataComposed)
        {
            if (!KeyValue.Initialize(ctx->composingData, ctx->composingDataSize, ctx->key, ctx->keySize, ctx->value, ctx->valueSize, ctx->versionUpdated, 0/*timeStamp*/, NULL))
            {
                NonBlocking.RevertWriteAndCheckSuccess(exisingDataStateMachine, existingDataVersion);
                ctx->error = ERROR_UNKNOWN;
                return true;
            }

            ctx->composingDataComposed = true;
        }

        KeyValue.SetVersionUnsafe(ctx->composingData, ctx->versionUpdated);
        result = NonBlocking.WriteTimedAlwaysRevert(ctx->table->header->indexStateMachine, ctx->timeout, _Set_ModifyIndex, ctx, NULL, &timeElapsed);
        if (result)
        {
            NonBlocking.EndWriteAndCheckSuccess(exisingDataStateMachine, existingDataVersion);
        }
        else
        {
            NonBlocking.RevertWriteAndCheckSuccess(exisingDataStateMachine, existingDataVersion);
        }

        if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
        {
            return true;
        }

        return result;
    }
    else
    {
        /*
         * Data exists. The new value occupies less space than the old one.
         * Data should be modified.
         */
        if (!KeyValue.SetValueTimed(ctx->existingData, ctx->timeout, ctx->value, ctx->valueSize, &ctx->versionUpdated))
        {
            ctx->error = ERROR_UNKNOWN;
        }
        else
        {
            ctx->wastedMemroyAdjustment += (int64_t)ctx->existingValueSize - (int64_t)ctx->valueSize;
        }

        return true;
    }
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, Set, ERROR_CODE, _SortedTable* table, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, const char* value, uint64_t valueSize, uint64_t version, bool force, uint64_t* versionUpdated)
{
    if (table->header->readOnly)
    {
        return ERROR_ACCESS_DENIED;
    }

    _SortedTableOpContext* context = I_NEW(_SortedTableOpContext);

    context->table = table;
    context->timeout = microsecondsTimeout;
    context->key = key;
    context->keySize = keySize;
    context->value = value;
    context->valueSize = valueSize;
    context->version = version;
    context->force = force;

    context->keyString = SizeString.New(key, keySize);
    context->composingDataSize = KeyValue.EvaluatePhysicalSize(context->keySize, context->valueSize);
    context->error = ERROR_NO;

    NonBlockingReadState state = NonBlocking.BeginRead(table->header->tableStateMachine);
    RefCounter.AddRef(table->header->writerCounter);
    bool result = NonBlocking.RetryTimed(microsecondsTimeout, _Set_TryOnce, context, NULL);
    RefCounter.Release(table->header->writerCounter);
    NonBlocking.EndReadAndCheckSuccess(table->header->tableStateMachine, state);

    ERROR_CODE error = context->error;
    if (ERROR_NO == error)
    {
        if (result)
        {
            versionUpdated && (*versionUpdated = context->versionUpdated);
        }
        else
        {
            error = ERROR_TIMEOUT;
        }
    }

    if (0 > context->wastedMemroyAdjustment)
    {
        __sync_sub_and_fetch(&table->header->wasted, (uint64_t)-context->wastedMemroyAdjustment);
    }
    else
    {
        __sync_add_and_fetch(&table->header->wasted, (uint64_t)context->wastedMemroyAdjustment);
    }

    I_DELETE(_SortedTableOpContext, context);
    return error;
}

static bool _Delete_RemoveIndex(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableOpContext* ctx = (_SortedTableOpContext*)context;
    if (enterState != (NON_BLOCKING_STATE_CORRUPTED_MASK | ctx->indexState))
    {
        return false;
    }

    int64_t indexEnd = (int64_t)ctx->table->header->indexEnd;
    if (!NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _RemoveIndexAt, ctx->table, ctx->candidate))
    {
        return false;
    }

    ctx->wastedMemroyAdjustment += (int64_t)indexEnd - (int64_t)ctx->table->header->indexEnd;
    return true;
}

static bool _Delete_MarkData(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableOpContext* ctx = (_SortedTableOpContext*)context;
    if (enterState != (NON_BLOCKING_STATE_CORRUPTED_MASK | ctx->existingDataVersion))
    {
        return false;
    }

    uint64_t timeElapsed = 0;
    NonBlockingWriteState indexState = 0;
    bool result = NonBlocking.WriteTimed(ctx->table->header->indexStateMachine, ctx->timeout, _Delete_RemoveIndex, ctx, &indexState, &timeElapsed);
    if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
    {
        return true;
    }

    if (!result)
    {
        return false;
    }

    KeyValue.DeleteUnsafe(ctx->existingData);
    ctx->wastedMemroyAdjustment += ctx->existingDataSize;
    return true;
}

static bool _Delete_TryOnce(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableOpContext* ctx = (_SortedTableOpContext*)context;

    uint64_t timeElapsed = 0;
    bool result = NonBlocking.ReadTimed(ctx->table->header->indexStateMachine, ctx->timeout, _Search, ctx, &ctx->indexState, &timeElapsed);
    if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
    {
        return true;
    }

    if (!result)
    {
        return false;
    }

    if (0 > ctx->candidate)
    {
        return true;
    }

    result = NonBlocking.WriteTimed(KeyValue.StateMachine(ctx->existingData), ctx->timeout, _Delete_MarkData, context, NULL, &timeElapsed);
    if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
    {
        return true;
    }

    return result;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, Delete, ERROR_CODE, _SortedTable* table, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, uint64_t version, bool force)
{
    if (table->header->readOnly)
    {
        return ERROR_ACCESS_DENIED;
    }

    _SortedTableOpContext* context = I_NEW(_SortedTableOpContext);

    context->table = table;
    context->timeout = microsecondsTimeout;
    context->key = key;
    context->keySize = keySize;
    context->version = version;
    context->force = force;

    context->keyString = SizeString.New(key, keySize);
    context->composingDataSize = KeyValue.EvaluatePhysicalSize(context->keySize, context->valueSize);
    context->error = ERROR_NO;

    NonBlockingReadState state = NonBlocking.BeginRead(table->header->tableStateMachine);
    RefCounter.AddRef(table->header->writerCounter);
    bool result = NonBlocking.RetryTimed(microsecondsTimeout, _Delete_TryOnce, context, NULL);
    RefCounter.Release(table->header->writerCounter);
    NonBlocking.EndReadAndCheckSuccess(table->header->tableStateMachine, state);

    ERROR_CODE error = context->error;
    if (ERROR_NO == context->error && !result)
    {
        error = ERROR_TIMEOUT;
    }

    if (0 > context->wastedMemroyAdjustment)
    {
        __sync_sub_and_fetch(&table->header->wasted, (uint64_t)-context->wastedMemroyAdjustment);
    }
    else
    {
        __sync_add_and_fetch(&table->header->wasted, (uint64_t)context->wastedMemroyAdjustment);
    }

    I_DELETE(_SortedTableOpContext, context);
    return error;
}

static bool _Get_TryOnce(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableOpContext* ctx = (_SortedTableOpContext*)context;

    uint64_t timeElapsed = 0;
    bool result = NonBlocking.ReadTimed(ctx->table->header->indexStateMachine, ctx->timeout, _Search, ctx, &ctx->indexState, &timeElapsed);
    if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
    {
        return true;
    }

    if (0 > ctx->candidate)
    {
        ctx->error = ERROR_NOT_FOUND;
    }

    return result;
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, Get, ERROR_CODE, _SortedTable* table, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, _SizedString** value, uint64_t* version, uint64_t* timeStamp, _KeyValue** raw)
{
    _SortedTableOpContext* context = I_NEW(_SortedTableOpContext);

    context->table = table;
    context->timeout = microsecondsTimeout;
    context->key = key;
    context->keySize = keySize;
    context->version = version;
    context->force = true;

    context->keyString = SizeString.New(key, keySize);
    context->composingDataSize = KeyValue.EvaluatePhysicalSize(context->keySize, context->valueSize);
    context->error = ERROR_NO;

    context->copyExistingValue = !!value;

    NonBlockingReadState state = NonBlocking.BeginRead(table->header->tableStateMachine);
    RefCounter.AddRef(table->header->readerCounter);
    bool result = NonBlocking.RetryTimed(microsecondsTimeout, _Get_TryOnce, context, NULL);
    RefCounter.Release(table->header->readerCounter);
    NonBlocking.EndReadAndCheckSuccess(table->header->tableStateMachine, state);

    ERROR_CODE error = context->error;
    if (ERROR_NO == error)
    {
        if (result)
        {
            version && (*version = context->existingDataVersion);
            timeStamp && (*timeStamp = context->existingDataTimeStamp);
            raw && (*raw = context->existingData);
            if (value)
            {
                *value = context->existingValueCopy;
                context->existingValueCopy = NULL;
            }
        }
        else
        {
            error = ERROR_TIMEOUT;
        }
    }

    I_DELETE(_SortedTableOpContext, context);
    return error;
}

#pragma pack(push)
#pragma pack(1)
CLASS(_SortedTableFindContext)
{
    _SortedTable* table;
    uint64_t timeout;
    const char* keyLow;
    uint64_t keyLowSize;
    bool isKeyLowClosed;
    const char* keyHigh;
    uint64_t keyHighSize;
    bool isKeyHighClosed;
    const char* path;
    uint64_t retrievingFrom;
    uint64_t desiredCount;
    uint64_t retrievedCount;
    _KeyValue** retrievedData;
    bool needData;
    bool copyData;
    bool needAll;

    _SizedBuffer* pathBuffer;
    FIND_SCOPE scope;

    _SizeString* keyLowString;
    _SizeString* keyHighString;

    int64_t tableIndexLow;
    int64_t tableIndexHigh;

    NonBlockingState indexState;

    ERROR_CODE error;
};
#pragma pack(pop)

MEMBER_METHOD_IMPLEMENT(_SortedTableFindContext, _SortedTableFindContext, _SortedTableFindContext*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        break;
    case METHOD_DTOR:
        if (thiz->pathBuffer)
        {
            SizedBuffer.Delete(thiz->pathBuffer);
            thiz->pathBuffer = NULL;
        }

        if (thiz->keyLowString)
        {
            SizeString.Delete(thiz->keyLowString);
            thiz->keyLowString = NULL;
        }

        if (thiz->keyHighString)
        {
            SizeString.Delete(thiz->keyHighString);
            thiz->keyHighString = NULL;
        }

        if (thiz->retrievedData)
        {
            if (thiz->copyData)
            {
                int64_t i = 0;
                for (; i < thiz->retrievedCount; ++i)
                {
                    I_FREE(thiz->retrievedData[i]);
                }
            }

            I_FREE(thiz->retrievedData);
            thiz->retrievedData = NULL;
        }

        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(_SortedTableFindContext)
{
};

static bool _Find_Search(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableFindContext* ctx = (_SortedTableFindContext*)context;
    _SearchCancellationContext* cancellationContext = I_NEW(_SearchCancellationContext);
    cancellationContext->enterState = enterState;
    cancellationContext->currentState = currentState;
    bool result = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _Search, ctx->table, ctx->keyLowString, _Search_ShouldCancel, cancellationContext, &ctx->tableIndexLow);
    result &= NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _Search, ctx->table, ctx->keyHighString, _Search_ShouldCancel, cancellationContext, &ctx->tableIndexHigh);
    I_DELETE(_SearchCancellationContext, cancellationContext);
    return result;
}

static bool _Find_FillData(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableFindContext* ctx = (_SortedTableFindContext*)context;

    if (ctx->indexState != ctx->table->header->indexStateMachine->state)
    {
        return false;
    }

    if (0 <= ctx->tableIndexLow /* Found */)
    {
        if (!ctx->isKeyLowClosed)
        {
            ++ctx->tableIndexLow;
        }
    }
    else
    {
        ctx->tableIndexLow = -ctx->tableIndexLow - 1;
    }

    if (0 <= ctx->tableIndexHigh /* Found */)
    {
        if (!ctx->isKeyHighClosed)
        {
            --ctx->tableIndexHigh;
        }
    }
    else
    {
        ctx->tableIndexHigh = -ctx->tableIndexHigh - 1 - 1;
    }

    if (0 < ctx->retrievingFrom)
    {
        ctx->tableIndexLow += ctx->retrievingFrom;
    }

    if (0 < ctx->desiredCount && ctx->desiredCount < ctx->tableIndexHigh - ctx->tableIndexLow + 1)
    {
        ctx->tableIndexHigh = ctx->tableIndexLow + ctx->desiredCount - 1;
    }

    ctx->retrievedCount = ctx->tableIndexHigh - ctx->tableIndexLow + 1LL;

    if (!(ctx->needData && ctx->retrievedCount))
    {
        return true;
    }

    ctx->retrievedData = (_KeyValue**)I_MALLOC(ctx->retrievedCount * sizeof(_KeyValue*));
    if (!ctx->retrievedData)
    {
        ctx->error = ERROR_NOT_ENOUGH_MEMORY;
        return true;
    }

    int64_t tableIndex = 0;
    int64_t dataIndex = 0;
    for (tableIndex = ctx->tableIndexLow, dataIndex = 0; tableIndex <= ctx->tableIndexHigh; ++tableIndex, ++dataIndex)
    {
        ctx->retrievedData[dataIndex] = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexAt, ctx->table, tableIndex);
        if (ctx->indexState != ctx->table->header->indexStateMachine->state)
        {
            I_FREE(ctx->retrievedData);
            ctx->retrievedData = NULL;
            return false;
        }
    }

    if (!ctx->copyData)
    {
        return true;
    }

    for (tableIndex = ctx->tableIndexLow, dataIndex = 0; tableIndex <= ctx->tableIndexHigh; ++tableIndex, ++dataIndex)
    {
        if (!(ctx->retrievedData[dataIndex] = KeyValue.CloneTimed(ctx->retrievedData[dataIndex], ctx->timeout)))
        {
            break;
        }
    }

    if (tableIndex <= ctx->tableIndexHigh)
    {
        ctx->retrievedCount = dataIndex;
        ctx->error = ERROR_PARTIAL_COMPLETE;
    }

    return true;
}

static bool _Find_TryOnce(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableFindContext* ctx = (_SortedTableFindContext*)context;

    uint64_t timeElapsed = 0;
    bool result = NonBlocking.ReadTimed(ctx->table->header->indexStateMachine, ctx->timeout, _Find_Search, ctx, &ctx->indexState, &timeElapsed);
    if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
    {
        return true;
    }

    if (!result)
    {
        return false;
    }

    return _Find_FillData(enterState, currentState, context);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, Find, ERROR_CODE,
                           _SortedTable* table,
                           uint64_t microsecondsTimeout,
                           const char* keyLow,
                           uint64_t keyLowSize,
                           bool isKeyLowClosed,
                           const char* keyHigh,
                           uint64_t keyHighSize,
                           bool isKeyHighClosed,
                           uint64_t retrievingFrom,
                           uint64_t desiredCount,
                           bool copyData,
                           uint64_t* retrievedCount,
                           const _KeyValue*** retrievedData)
{
    if (!retrievedCount)
    {
        return ERROR_ARGUMENT_NULL;
    }

    _SortedTableFindContext* context = I_NEW(_SortedTableFindContext);

    context->table = table;
    context->timeout = microsecondsTimeout;
    context->keyLow = keyLow;
    context->keyLowSize = keyLowSize;
    context->isKeyLowClosed = isKeyLowClosed;
    context->keyHigh = keyHigh;
    context->keyHighSize = keyHighSize;
    context->isKeyHighClosed = isKeyHighClosed;
    context->retrievingFrom = retrievingFrom;
    context->desiredCount = desiredCount;
    context->needData = !!retrievedData;
    context->copyData = copyData;

    context->keyLowString = SizeString.New(keyLow, keyLowSize);
    context->keyHighString = SizeString.New(keyHigh, keyHighSize);

    NonBlockingReadState state = NonBlocking.BeginRead(table->header->tableStateMachine);
    RefCounter.AddRef(table->header->readerCounter);
    bool result = NonBlocking.RetryTimed(microsecondsTimeout, _Find_TryOnce, context, NULL);
    RefCounter.Release(table->header->readerCounter);
    NonBlocking.EndReadAndCheckSuccess(table->header->tableStateMachine, state);

    ERROR_CODE error = context->error;
    if (ERROR_NO == error)
    {
        if (result)
        {
            retrievedCount && (*retrievedCount = context->retrievedCount);
            if (retrievedData)
            {
                *retrievedData = context->retrievedData;
                context->retrievedData = NULL;
            }
        }
        else
        {
            error = ERROR_TIMEOUT;
        }
    }

    I_DELETE(_SortedTableFindContext, context);
    return error;
}

static bool _FindAll_Search(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableFindContext* ctx = (_SortedTableFindContext*)context;
    ctx->tableIndexLow = 0;
    ctx->isKeyLowClosed = true;
    ctx->tableIndexHigh = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _Count, ctx->table);
    ctx->isKeyHighClosed = false;
    return true;
}

static bool _FindAll_TryOnce(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableFindContext* ctx = (_SortedTableFindContext*)context;

    uint64_t timeElapsed = 0;
    bool result = NonBlocking.ReadTimed(ctx->table->header->indexStateMachine, ctx->timeout, _FindAll_Search, ctx, &ctx->indexState, &timeElapsed);
    if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
    {
        return true;
    }

    if (!result)
    {
        return false;
    }

    return _Find_FillData(enterState, currentState, context);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, FindAll, ERROR_CODE,
                           _SortedTable* table,
                           uint64_t microsecondsTimeout,
                           uint64_t retrievingFrom,
                           uint64_t desiredCount,
                           bool copyData,
                           uint64_t* retrievedCount,
                           const _KeyValue*** retrievedData)
{
    if (!retrievedCount)
    {
        return ERROR_ARGUMENT_NULL;
    }

    _SortedTableFindContext* context = I_NEW(_SortedTableFindContext);

    context->table = table;
    context->timeout = microsecondsTimeout;
    context->needAll = true;
    context->retrievingFrom = retrievingFrom;
    context->desiredCount = desiredCount;
    context->needData = !!retrievedData;
    context->copyData = copyData;

    NonBlockingReadState state = NonBlocking.BeginRead(table->header->tableStateMachine);
    RefCounter.AddRef(table->header->readerCounter);
    bool result = NonBlocking.RetryTimed(microsecondsTimeout, _FindAll_TryOnce, context, NULL);
    RefCounter.Release(table->header->readerCounter);
    NonBlocking.EndReadAndCheckSuccess(table->header->tableStateMachine, state);

    ERROR_CODE error = context->error;
    if (ERROR_NO == error)
    {
        if (result)
        {
            retrievedCount && (*retrievedCount = context->retrievedCount);
            if (retrievedData)
            {
                *retrievedData = context->retrievedData;
                context->retrievedData = NULL;
            }
        }
        else
        {
            error = ERROR_TIMEOUT;
        }
    }

    I_DELETE(_SortedTableFindContext, context);
    return error;
}

_SizedBuffer* _NormalizePath(const char* path, uint64_t pathSize)
{
    _SizedBuffer* normalizedPath = SizedBuffer.New(3 + pathSize);
    if (!normalizedPath)
    {
        return NULL;
    }

    SizedBuffer.AppendAsPath(normalizedPath, "/", 1);
    SizedBuffer.AppendAsPath(normalizedPath, path, pathSize);
    SizedBuffer.AppendAsPath(normalizedPath, "/", 1);
    /*uint64_t normalizedPathSize = SizedBuffer.Size(normalizedPath);
    if (1 < normalizedPathSize)
    {
        normalizedPath->data[normalizedPathSize - 1] = 0;
        SizedBuffer.SetSize(normalizedPath, normalizedPathSize - 1);
    }*/

    return normalizedPath;
}

static bool _FindPath_FillData(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableFindContext* ctx = (_SortedTableFindContext*)context;

    if (ctx->indexState != ctx->table->header->indexStateMachine->state)
    {
        return false;
    }

    bool rootFound = 0 <= ctx->tableIndexLow;
    if (!rootFound /* Not Found */)
    {
        ctx->tableIndexLow = -ctx->tableIndexLow - 1;
    }

    if (0 > ctx->tableIndexHigh /* Not Found */)
    {
        ctx->tableIndexHigh = -ctx->tableIndexHigh - 1;
    }

    ctx->retrievedCount = 0;
    ctx->retrievedData = (_KeyValue**)I_MALLOC((ctx->tableIndexHigh - ctx->tableIndexLow) * sizeof(_KeyValue*));
    if (!ctx->retrievedData)
    {
        ctx->error = ERROR_NOT_ENOUGH_MEMORY;
        return true;
    }

    int64_t tableIndex = ctx->tableIndexLow;
    int64_t dataIndex = 0;
    switch(ctx->scope)
    {
    case FIND_SCOPE_BASE:
    {
        if (!rootFound)
        {
            I_FREE(ctx->retrievedData);
            ctx->retrievedData = NULL;
            ctx->error = ERROR_NOT_FOUND;
            return true;
        }

        ctx->retrievedCount = 1;
        ctx->retrievedData[0] = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexAt, ctx->table, ctx->tableIndexLow);
    }

    break;
    case FIND_SCOPE_ONE_LEVEL:
    {
        uint64_t retrievingFrom = ctx->retrievingFrom;
        uint64_t desiredCount = ctx->desiredCount;
        rootFound && (++tableIndex);
        for (; tableIndex < ctx->tableIndexHigh; ++tableIndex)
        {
            _KeyValue* kv = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexAt, ctx->table, tableIndex);
            _KeyValue* kv2 = NONINSTANTIAL_NAMESPACE_METHOD_CALL(SortedTable, _GetIndexAt, ctx->table, tableIndex - 1);
            _SizeString* key = KeyValue.KeyUnsafe(kv);
            int64_t keyIterator = 0;
            uint64_t keySize = SizeString.Size(key);
            for (keyIterator = SizeString.Size(ctx->keyLowString); keyIterator < keySize; ++keyIterator)
            {
                if ('/' == SizeString.Data(key)[keyIterator])
                {
                    break;
                }
            }

            if (ctx->indexState != ctx->table->header->indexStateMachine->state)
            {
                I_FREE(ctx->retrievedData);
                ctx->retrievedData = NULL;
                ctx->retrievedCount = 0;
                return false;
            }

            if (keyIterator < keySize - 1)
            {
                continue;
            }

            if (retrievingFrom)
            {
                --retrievingFrom;
                continue;
            }

            ctx->retrievedData[dataIndex++] = kv;
            if (0 < ctx->desiredCount)
            {
                if (desiredCount)
                {
                    --desiredCount;
                }

                if (!desiredCount)
                {
                    break;
                }
            }
        }
    }

    break;
    case FIND_SCOPE_SUBTREE:
    default:
        break;
    }

    ctx->retrievedCount = dataIndex;
    if (!(ctx->needData && ctx->retrievedCount))
    {
        I_FREE(ctx->retrievedData);
        ctx->retrievedData = NULL;
        return true;
    }

    if (!ctx->copyData)
    {
        return true;
    }

    for (dataIndex = 0; dataIndex < ctx->retrievedCount; ++dataIndex)
    {
        if (!(ctx->retrievedData[dataIndex] = KeyValue.CloneTimed(ctx->retrievedData[dataIndex], ctx->timeout)))
        {
            break;
        }
    }

    if (dataIndex < ctx->retrievedCount)
    {
        ctx->retrievedCount = dataIndex;
        ctx->error = ERROR_PARTIAL_COMPLETE;
    }

    return true;
}

static bool _FindPath_TryOnce(NonBlockingState enterState, NonBlockingState* currentState, void* context)
{
    _SortedTableFindContext* ctx = (_SortedTableFindContext*)context;

    uint64_t timeElapsed = 0;
    bool result = NonBlocking.ReadTimed(ctx->table->header->indexStateMachine, ctx->timeout, _Find_Search, ctx, &ctx->indexState, &timeElapsed);
    if (_IsErrorEnding(&ctx->error, &ctx->timeout, timeElapsed))
    {
        return true;
    }

    if (!result)
    {
        return false;
    }

    return _FindPath_FillData(enterState, currentState, context);
}

NAMESPACE_METHOD_IMPLEMENT(SortedTable, FindPath, ERROR_CODE,
                           _SortedTable* table,
                           uint64_t microsecondsTimeout,
                           const char* path,
                           uint64_t pathSize,
                           FIND_SCOPE scope,
                           uint64_t retrievingFrom,
                           uint64_t desiredCount,
                           bool copyData,
                           uint64_t* retrievedCount,
                           _KeyValue*** retrievedData)
{
    if (!retrievedCount)
    {
        return ERROR_ARGUMENT_NULL;
    }

    _SizedBuffer* normalizedPath = _NormalizePath(path, pathSize);
    if (!normalizedPath)
    {
        return ERROR_OUT_OF_MEMORY;
    }

    _SizeString* keyLowString = SizeString.New(SizedBuffer.Data(normalizedPath), SizedBuffer.Size(normalizedPath));
    if (!keyLowString)
    {
        SizedBuffer.Delete(normalizedPath);
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    _SizeString* keyHighString = SizeString.Clone(keyLowString);
    if (!keyHighString)
    {
        SizedBuffer.Delete(normalizedPath);
        SizeString.Delete(keyLowString);
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    ++(SizeString.Data(keyHighString)[SizeString.Size(keyHighString) - 1]);

    _SortedTableFindContext* context = I_NEW(_SortedTableFindContext);

    context->table = table;
    context->timeout = microsecondsTimeout;
    context->path = path;
    context->scope = scope;
    context->retrievingFrom = retrievingFrom;
    context->desiredCount = desiredCount;
    context->needData = !!retrievedData;
    context->copyData = copyData;

    context->pathBuffer = normalizedPath;
    context->keyLowString = keyLowString;
    context->isKeyLowClosed = true;
    context->keyHighString = keyHighString;
    context->isKeyHighClosed = false;

    NonBlockingReadState state = NonBlocking.BeginRead(table->header->tableStateMachine);
    RefCounter.AddRef(table->header->readerCounter);
    bool result = NonBlocking.RetryTimed(microsecondsTimeout, FIND_SCOPE_SUBTREE == scope ? _Find_TryOnce : _FindPath_TryOnce, context, NULL);
    RefCounter.Release(table->header->readerCounter);
    NonBlocking.EndReadAndCheckSuccess(table->header->tableStateMachine, state);

    ERROR_CODE error = context->error;
    if (ERROR_NO == error)
    {
        if (result)
        {
            retrievedCount && (*retrievedCount = context->retrievedCount);
            if (retrievedData)
            {
                *retrievedData = context->retrievedData;
                context->retrievedData = NULL;
            }
        }
        else
        {
            error = ERROR_TIMEOUT;
        }
    }

    I_DELETE(_SortedTableFindContext, context);
    return error;
}

NAMESPACE_INITIALIZE(SortedTable)
{
    NAMESPACE_METHOD_INITIALIZE(SortedTable, Reinterpret)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, Construct)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, Rebuild)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, StateMachine)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, Size)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, AvaliableMemory)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, WastedMemory)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, ReaderCount)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, WriterCount)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, IsReadOnly)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, SetReadOnly)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, ClearReadOnly)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, CopyTo)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, Set)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, Delete)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, Get)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, Find)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, FindAll)
    NAMESPACE_METHOD_INITIALIZE(SortedTable, FindPath)
};
