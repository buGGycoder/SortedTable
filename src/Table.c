#include "Table.h"
#include <pthread.h>
#include "SpinLock.h"
#include <linux/limits.h>
#include "File.h"
#include "SizedBuffer.h"
#include "NonBlocking.h"

#define SYNC_TABLE_INITIAL_SIZE (512ULL * 1024ULL)
#define DATA_TABLE_INITIAL_SIZE (1024ULL * 1024ULL)
#define SYNC_CONTEXT_KEY "sync_context"
#define SYNC_CONTEXT_SET_TIMEOUT (1000000ULL)
#define SYNC_CONTEXT_GET_TIMEOUT (1000000ULL)

#pragma pack(push)
#pragma pack(1)
PLAIN_OLD_DATA(_TableSyncContext)
{
    volatile uint64_t activeTableSelector : 1;
    volatile int32_t spinLock;
    MEMBER_FIELD_DEFINE(NonBlockingStateMachine, stateMachine);
};
#pragma pack(pop)

MEMBER_METHOD_IMPLEMENT(Table, Initialize, ERROR_CODE, const char* path)
{
    if (!File.DetectDirectory(path))
    {
        return ERROR_NOT_FOUND;
    }

    _SizedBuffer* buffer = SizedBuffer.New(PATH_MAX);
    ERROR_CODE error = ERROR_NO;
    do
    {
        SizedBuffer.AppendAsPath(buffer, path, 0);
        SizedBuffer.AppendAsPath(buffer, "table-sync-context.stf", 0);
        TableFile* file = &thiz->syncFile;
        file->Initialize(file, buffer, 0);
        if (!file->Size(file))
        {
            error = file->Initialize(file, buffer, SYNC_TABLE_INITIAL_SIZE);
        }

        if (ERROR_NO != error)
        {
            break;
        }

        _TableSyncContext c = {0};
        error = SortedTable.Set(file->Table(file), SYNC_CONTEXT_SET_TIMEOUT, SYNC_CONTEXT_KEY, sizeof(SYNC_CONTEXT_KEY), &c, sizeof(c), 0, false, NULL);
        if (ERROR_NO != error && ERROR_WRONG_VERSION != error)
        {
            break;
        }

        bool setByMe = ERROR_NO == error;
        _KeyValue* kv = NULL;
        error = SortedTable.Get(file->Table(file), SYNC_CONTEXT_GET_TIMEOUT, SYNC_CONTEXT_KEY, sizeof(SYNC_CONTEXT_KEY), NULL, NULL, NULL, &kv);
        if (ERROR_NO != error)
        {
            break;
        }

        thiz->syncContext = SizeBuffer.Data(KeyValue.ValueUnsafe(kv));
        SpinLock.Lock(&thiz->syncContext->spinLock);

        int i = 0;
        for(; i < sizeof(thiz->dataFile) / sizeof(thiz->dataFile[0]); ++i)
        {
            SizedBuffer.SetSize(buffer, 0);
            SizedBuffer.AppendAsPath(buffer, path, 0);
            SizedBuffer.AppendFormatAsPath(buffer, "table-data-storage-%d.stf", i);
            file = &thiz->dataFile[i];
            file->Initialize(file, buffer, 0);
            if (!file->Size(file))
            {
                error = file->Initialize(file, buffer, DATA_TABLE_INITIAL_SIZE);
            }

            if (ERROR_NO != error)
            {
                break;
            }
        }

        SpinLock.Unlock(&thiz->syncContext->spinLock);
    }
    while(false);
    SizedBuffer.Delete(buffer);
    return error;
}

MEMBER_METHOD_IMPLEMENT(Table, Close, ERROR_CODE)
{
    thiz->syncFile.Close(&thiz->syncFile);
    thiz->dataFile[0].Close(&thiz->dataFile[0]);
    thiz->dataFile[1].Close(&thiz->dataFile[1]);
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(Table, ActiveTable, _SortedTable*)
{
    int activeSelector = !!thiz->syncContext->activeTableSelector;
    TableFile* activeFile = &thiz->dataFile[activeSelector];
    _SortedTable* activeTable = activeFile->Table(activeFile);
    if (SortedTable.Size(activeTable) > activeFile->Size(activeFile))
    {
        SpinLock.Lock(&thiz->syncContext->spinLock);
        if (SortedTable.Size(activeTable) > activeFile->Size(activeFile))
        {
            activeFile->ExtendSizeTo(activeFile, SortedTable.Size(activeTable));
        }

        activeTable = activeFile->Table(activeFile);
        SpinLock.Unlock(&thiz->syncContext->spinLock);
    }

    return activeTable;
}

MEMBER_METHOD_IMPLEMENT(Table, Set, ERROR_CODE, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, const char* value, uint64_t valueSize, uint64_t version, bool force, uint64_t* versionUpdated)
{
    NonBlockingStateMachine* syncStateMachine = thiz->syncContext->stateMachine;
    NonBlockingReadState state = NonBlocking.BeginRead(syncStateMachine);
    ERROR_CODE error = SortedTable.Set(thiz->ActiveTable(thiz), microsecondsTimeout, key, keySize, value, valueSize, version, force, versionUpdated);
    NonBlocking.EndReadAndCheckSuccess(syncStateMachine, state);
    if (ERROR_NOT_ENOUGH_MEMORY != error)
    {
        return error;
    }

    SpinLock.Lock(&thiz->syncContext->spinLock);

    do
    {
        state = NonBlocking.BeginRead(syncStateMachine);
        error = SortedTable.Set(thiz->ActiveTable(thiz), microsecondsTimeout, key, keySize, value, valueSize, version, force, versionUpdated);
        NonBlocking.EndReadAndCheckSuccess(syncStateMachine, state);

        if (ERROR_NOT_ENOUGH_MEMORY != error)
        {
            break;
        }

        state = NonBlocking.BeginWrite(syncStateMachine);
        NonBlocking.WaitForNoReader(syncStateMachine);

        int activeSelector = !!thiz->syncContext->activeTableSelector;
        int inactiveSelector = !activeSelector;
        TableFile* activeFile = &thiz->dataFile[activeSelector];
        TableFile* inactiveFile = &thiz->dataFile[inactiveSelector];
        _SortedTable* activeTable = thiz->ActiveTable(thiz);
        if (SortedTable.Size(activeTable) / 2 <= SortedTable.WastedMemory(activeTable))
        {
            inactiveFile->ExtendSizeTo(inactiveFile, SortedTable.Size(activeTable));
            _SortedTable* inactiveTable = inactiveFile->Table(inactiveFile);
            SortedTable.Construct(inactiveTable, inactiveFile->Size(inactiveFile), true, false);
            SortedTable.CopyTo(activeFile->Table(activeFile), inactiveFile->Table(inactiveFile));
            thiz->syncContext->activeTableSelector ^= 1;
        }
        else
        {
            activeFile->ExtendSizeTo(activeFile, 2 * SortedTable.Size(activeTable));
        }

        NonBlocking.EndWriteAndCheckSuccess(syncStateMachine, state);

        state = NonBlocking.BeginRead(syncStateMachine);
        error = SortedTable.Set(thiz->ActiveTable(thiz), microsecondsTimeout, key, keySize, value, valueSize, version, force, versionUpdated);
        NonBlocking.EndReadAndCheckSuccess(syncStateMachine, state);
    }
    while(false);

    SpinLock.Unlock(&thiz->syncContext->spinLock);
    return error;
}

MEMBER_METHOD_IMPLEMENT(Table, Delete, ERROR_CODE, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, uint64_t version, bool force)
{
    NonBlockingStateMachine* syncStateMachine = thiz->syncContext->stateMachine;
    NonBlockingReadState state = NonBlocking.BeginRead(syncStateMachine);
    ERROR_CODE error = SortedTable.Delete(thiz->ActiveTable(thiz), microsecondsTimeout, key, keySize, version, force);
    NonBlocking.EndReadAndCheckSuccess(syncStateMachine, state);
    return error;
}

MEMBER_METHOD_IMPLEMENT(Table, Get, ERROR_CODE, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, _SizedString** value, uint64_t* version, uint64_t* timeStamp, _KeyValue** raw)
{
    NonBlockingStateMachine* syncStateMachine = thiz->syncContext->stateMachine;
    NonBlockingReadState state = NonBlocking.BeginRead(syncStateMachine);
    ERROR_CODE error = SortedTable.Get(thiz->ActiveTable(thiz), microsecondsTimeout, key, keySize, value, version, timeStamp, raw);
    NonBlocking.EndReadAndCheckSuccess(syncStateMachine, state);
    return error;
}

MEMBER_METHOD_IMPLEMENT(Table, Find, ERROR_CODE,
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
    NonBlockingStateMachine* syncStateMachine = thiz->syncContext->stateMachine;
    NonBlockingReadState state = NonBlocking.BeginRead(syncStateMachine);
    ERROR_CODE error = SortedTable.Find(thiz->ActiveTable(thiz),
                                        microsecondsTimeout,
                                        keyLow,
                                        keyLowSize,
                                        isKeyLowClosed,
                                        keyHigh,
                                        keyHighSize,
                                        isKeyHighClosed,
                                        retrievingFrom,
                                        desiredCount,
                                        copyData,
                                        retrievedCount,
                                        retrievedData);
    NonBlocking.EndReadAndCheckSuccess(syncStateMachine, state);
    return error;
}

MEMBER_METHOD_IMPLEMENT(Table, FindAll, ERROR_CODE,
                        uint64_t microsecondsTimeout,
                        uint64_t retrievingFrom,
                        uint64_t desiredCount,
                        bool copyData,
                        uint64_t* retrievedCount,
                        const _KeyValue*** retrievedData)
{
    NonBlockingStateMachine* syncStateMachine = thiz->syncContext->stateMachine;
    NonBlockingReadState state = NonBlocking.BeginRead(syncStateMachine);
    ERROR_CODE error = SortedTable.FindAll(thiz->ActiveTable(thiz),
                                           microsecondsTimeout,
                                           retrievingFrom,
                                           desiredCount,
                                           copyData,
                                           retrievedCount,
                                           retrievedData);
    NonBlocking.EndReadAndCheckSuccess(syncStateMachine, state);
    return error;
}

MEMBER_METHOD_IMPLEMENT(Table, FindPath, ERROR_CODE,
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
    NonBlockingStateMachine* syncStateMachine = thiz->syncContext->stateMachine;
    NonBlockingReadState state = NonBlocking.BeginRead(syncStateMachine);
    ERROR_CODE error = SortedTable.FindPath(thiz->ActiveTable(thiz),
                                            microsecondsTimeout,
                                            path,
                                            pathSize,
                                            scope,
                                            retrievingFrom,
                                            desiredCount,
                                            copyData,
                                            retrievedCount,
                                            retrievedData);
    NonBlocking.EndReadAndCheckSuccess(syncStateMachine, state);
    return error;
}

MEMBER_METHOD_IMPLEMENT(Table, Table, Table*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        I_INIT(TableFile, &thiz->syncFile);
        I_INIT(TableFile, &thiz->dataFile[0]);
        I_INIT(TableFile, &thiz->dataFile[1]);
        break;
    case METHOD_DTOR:
        I_DISPOSE(TableFile, &thiz->syncFile);
        I_DISPOSE(TableFile, &thiz->dataFile[0]);
        I_DISPOSE(TableFile, &thiz->dataFile[1]);
        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(Table)
{
    NAMESPACE_METHOD_INITIALIZE(Table, Initialize)
    NAMESPACE_METHOD_INITIALIZE(Table, Close)
    NAMESPACE_METHOD_INITIALIZE(Table, ActiveTable)
    NAMESPACE_METHOD_INITIALIZE(Table, Set)
    NAMESPACE_METHOD_INITIALIZE(Table, Delete)
    NAMESPACE_METHOD_INITIALIZE(Table, Get)
    NAMESPACE_METHOD_INITIALIZE(Table, Find)
    NAMESPACE_METHOD_INITIALIZE(Table, FindAll)
    NAMESPACE_METHOD_INITIALIZE(Table, FindPath)
};
