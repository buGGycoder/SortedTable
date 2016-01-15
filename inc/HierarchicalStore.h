#ifndef __HIERARCHICALSTORE_H__
#define __HIERARCHICALSTORE_H__

#include <stdint.h>
#include "ClassSimulator.h"
#include "Error.h"
#include "SortedTable.h"
#include "TableFile.h"

PLAIN_OLD_DATA(Identity)
{
    uint64_t u128LELow;
    uint64_t u128LEHigh;
};

PLAIN_OLD_DATA(Capsule)
{
    uint64_t size;
    void* payload;
};

PLAIN_OLD_DATA(SearchKey)
{
    MEMBER_FIELD_DEFINE(Identity, identity);
    _SizedString* path;
    uint64_t version;
};

PLAIN_OLD_DATA(Node)
{
    MEMBER_FIELD_DEFINE(SearchKey, key);
    MEMBER_FIELD_DEFINE(Capsule, capsule);
    struct Node* node;
};


MEMBER_METHOD_DECLARE(HierarchicalStore, Initialize, ERROR_CODE, const char* path);
MEMBER_METHOD_DECLARE(HierarchicalStore, Close, ERROR_CODE);
MEMBER_METHOD_DECLARE(HierarchicalStore, ActiveTable, _SortedTable*);
MEMBER_METHOD_DECLARE(HierarchicalStore, Set, ERROR_CODE, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, const char* value, uint64_t valueSize, uint64_t version, bool force, uint64_t* versionUpdated);
MEMBER_METHOD_DECLARE(HierarchicalStore, Delete, ERROR_CODE, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, uint64_t version, bool force);
MEMBER_METHOD_DECLARE(HierarchicalStore, Get, ERROR_CODE, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, _SizedString** value, uint64_t* version, uint64_t* timeStamp, _KeyValue** raw);
MEMBER_METHOD_DECLARE(HierarchicalStore, Find, ERROR_CODE,
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
MEMBER_METHOD_DECLARE(HierarchicalStore, FindAll, ERROR_CODE,
                      uint64_t microsecondsTimeout,
                      uint64_t retrievingFrom,
                      uint64_t desiredCount,
                      bool copyData,
                      uint64_t* retrievedCount,
                      const _KeyValue*** retrievedData);

CLASS_DECLARE(_TableSyncContext);
CLASS(HierarchicalStore)
{
    NAMESPACE_METHOD_DEFINE(HierarchicalStore, Initialize);
    NAMESPACE_METHOD_DEFINE(HierarchicalStore, Close);
    NAMESPACE_METHOD_DEFINE(HierarchicalStore, ActiveTable);
    NAMESPACE_METHOD_DEFINE(HierarchicalStore, Set);
    NAMESPACE_METHOD_DEFINE(HierarchicalStore, Delete);
    NAMESPACE_METHOD_DEFINE(HierarchicalStore, Get);
    NAMESPACE_METHOD_DEFINE(HierarchicalStore, Find);
    NAMESPACE_METHOD_DEFINE(HierarchicalStore, FindAll);

    TableFile syncFile;
    TableFile dataFile[2];
    _TableSyncContext* syncContext;
};

#endif /* __HIERARCHICALSTORE_H__ */
