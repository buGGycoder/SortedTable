#ifndef __SORTEDTABLE_H__
#define __SORTEDTABLE_H__

#include <stdbool.h>
#include <stdint.h>
#include "ClassSimulator.h"

#include "Error.h"
#include "KeyValue.h"

typedef enum FIND_SCOPE
{
    FIND_SCOPE_BASE = 0,
    FIND_SCOPE_ONE_LEVEL = 1,
    FIND_SCOPE_SUBTREE = 2,
} FIND_SCOPE;

CLASS_DECLARE(_SortedTable);

NAMESPACE_METHOD_DECLARE(SortedTable, Reinterpret, _SortedTable*, void* buffer, uint64_t size);
NAMESPACE_METHOD_DECLARE(SortedTable, Construct, _SortedTable*, void* buffer, uint64_t size, bool force, bool clean);
NAMESPACE_METHOD_DECLARE(SortedTable, Rebuild, _SortedTable*, _SortedTable* table);
NAMESPACE_METHOD_DECLARE(SortedTable, StateMachine, NonBlockingStateMachine*, _SortedTable* table);
NAMESPACE_METHOD_DECLARE(SortedTable, Size, uint64_t, _SortedTable* table);
NAMESPACE_METHOD_DECLARE(SortedTable, AvaliableMemory, uint64_t, _SortedTable* table);
NAMESPACE_METHOD_DECLARE(SortedTable, WastedMemory, uint64_t, _SortedTable* table);
NAMESPACE_METHOD_DECLARE(SortedTable, ReaderCount, uint32_t, _SortedTable* table);
NAMESPACE_METHOD_DECLARE(SortedTable, WriterCount, uint32_t, _SortedTable* table);
NAMESPACE_METHOD_DECLARE(SortedTable, IsReadOnly, bool, _SortedTable* table);
NAMESPACE_METHOD_DECLARE(SortedTable, SetReadOnly, bool, _SortedTable* table);
NAMESPACE_METHOD_DECLARE(SortedTable, ClearReadOnly, bool, _SortedTable* table);
NAMESPACE_METHOD_DECLARE(SortedTable, CopyTo, ERROR_CODE, _SortedTable* source, _SortedTable* target);
NAMESPACE_METHOD_DECLARE(SortedTable, Set, ERROR_CODE, _SortedTable* table, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, const char* value, uint64_t valueSize, uint64_t version, bool force, uint64_t* versionUpdated);
NAMESPACE_METHOD_DECLARE(SortedTable, Delete, ERROR_CODE, _SortedTable* table, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, uint64_t version, bool force);
NAMESPACE_METHOD_DECLARE(SortedTable, Get, ERROR_CODE, _SortedTable* table, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, _SizedString** value, uint64_t* version, uint64_t* timeStamp, _KeyValue** raw);
NAMESPACE_METHOD_DECLARE(SortedTable, Find, ERROR_CODE,
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
                         const _KeyValue*** retrievedData);
NAMESPACE_METHOD_DECLARE(SortedTable, FindAll, ERROR_CODE,
                         _SortedTable* table,
                         uint64_t microsecondsTimeout,
                         uint64_t retrievingFrom,
                         uint64_t desiredCount,
                         bool copyData,
                         uint64_t* retrievedCount,
                         const _KeyValue*** retrievedData);
NAMESPACE_METHOD_DECLARE(SortedTable, FindPath, ERROR_CODE,
                         _SortedTable* table,
                         uint64_t microsecondsTimeout,
                         const char* path,
                         uint64_t pathSize,
                         FIND_SCOPE scope,
                         uint64_t retrievingFrom,
                         uint64_t desiredCount,
                         bool copyData,
                         uint64_t* retrievedCount,
                         _KeyValue*** retrievedData);

NAMESPACE(SortedTable)
{
    NAMESPACE_METHOD_DEFINE(SortedTable, Reinterpret);
    NAMESPACE_METHOD_DEFINE(SortedTable, Construct);
    NAMESPACE_METHOD_DEFINE(SortedTable, Rebuild);
    NAMESPACE_METHOD_DEFINE(SortedTable, StateMachine);
    NAMESPACE_METHOD_DEFINE(SortedTable, Size);
    NAMESPACE_METHOD_DEFINE(SortedTable, AvaliableMemory);
    NAMESPACE_METHOD_DEFINE(SortedTable, WastedMemory);
    NAMESPACE_METHOD_DEFINE(SortedTable, ReaderCount);
    NAMESPACE_METHOD_DEFINE(SortedTable, WriterCount);
    NAMESPACE_METHOD_DEFINE(SortedTable, IsReadOnly);
    NAMESPACE_METHOD_DEFINE(SortedTable, SetReadOnly);
    NAMESPACE_METHOD_DEFINE(SortedTable, ClearReadOnly);
    NAMESPACE_METHOD_DEFINE(SortedTable, CopyTo);
    NAMESPACE_METHOD_DEFINE(SortedTable, Set);
    NAMESPACE_METHOD_DEFINE(SortedTable, Delete);
    NAMESPACE_METHOD_DEFINE(SortedTable, Get);
    NAMESPACE_METHOD_DEFINE(SortedTable, Find);
    NAMESPACE_METHOD_DEFINE(SortedTable, FindAll);
    NAMESPACE_METHOD_DEFINE(SortedTable, FindPath);
};

#endif /* __SORTEDTABLE_H__ */
