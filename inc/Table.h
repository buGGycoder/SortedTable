#ifndef __TABLE_H__
#define __TABLE_H__

#include <stdint.h>
#include "ClassSimulator.h"
#include "Error.h"
#include "SortedTable.h"
#include "TableFile.h"

MEMBER_METHOD_DECLARE(Table, Initialize, ERROR_CODE, const char* path);
MEMBER_METHOD_DECLARE(Table, Close, ERROR_CODE);
MEMBER_METHOD_DECLARE(Table, ActiveTable, _SortedTable*);
MEMBER_METHOD_DECLARE(Table, Set, ERROR_CODE, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, const char* value, uint64_t valueSize, uint64_t version, bool force, uint64_t* versionUpdated);
MEMBER_METHOD_DECLARE(Table, Delete, ERROR_CODE, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, uint64_t version, bool force);
MEMBER_METHOD_DECLARE(Table, Get, ERROR_CODE, uint64_t microsecondsTimeout, const char* key, uint64_t keySize, _SizedString** value, uint64_t* version, uint64_t* timeStamp, _KeyValue** raw);
MEMBER_METHOD_DECLARE(Table, Find, ERROR_CODE,
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
MEMBER_METHOD_DECLARE(Table, FindAll, ERROR_CODE,
                      uint64_t microsecondsTimeout,
                      uint64_t retrievingFrom,
                      uint64_t desiredCount,
                      bool copyData,
                      uint64_t* retrievedCount,
                      const _KeyValue*** retrievedData);
MEMBER_METHOD_DECLARE(Table, FindPath, ERROR_CODE,
                      uint64_t microsecondsTimeout,
                      const char* path,
                      uint64_t pathSize,
                      FIND_SCOPE scope,
                      uint64_t retrievingFrom,
                      uint64_t desiredCount,
                      bool copyData,
                      uint64_t* retrievedCount,
                      _KeyValue*** retrievedData);

CLASS_DECLARE(_TableSyncContext);
CLASS(Table)
{
    NAMESPACE_METHOD_DEFINE(Table, Initialize);
    NAMESPACE_METHOD_DEFINE(Table, Close);
    NAMESPACE_METHOD_DEFINE(Table, ActiveTable);
    NAMESPACE_METHOD_DEFINE(Table, Set);
    NAMESPACE_METHOD_DEFINE(Table, Delete);
    NAMESPACE_METHOD_DEFINE(Table, Get);
    NAMESPACE_METHOD_DEFINE(Table, Find);
    NAMESPACE_METHOD_DEFINE(Table, FindAll);
    NAMESPACE_METHOD_DEFINE(Table, FindPath);

    TableFile syncFile;
    TableFile dataFile[2];
    _TableSyncContext* syncContext;
};

#endif /* __TABLE_H__ */
