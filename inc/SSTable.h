#ifndef __SSTABLE_H__
#define __SSTABLE_H__

#include <stdbool.h>
#include <stdint.h>
#include "ClassSimulator2.h"
#include "Error.h"


CLASS_DECLARE(_SSTable);

NAMESPACE_METHOD_DECLARE(SSTable, Reinterpret, const _SSTable*, uint8_t* start, uint64_t size);
NAMESPACE_METHOD_DECLARE(SSTable, Construct, _SSTable*, uint8_t* start, uint64_t size);
NAMESPACE_METHOD_DECLARE(SSTable, IsCompact, bool, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, SetCompact, bool, _SSTable* thiz, bool compact);
NAMESPACE_METHOD_DECLARE(SSTable, IsReferenced, bool, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, IsGreedy, bool, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, SetGreedy, bool, _SSTable* thiz, bool greedy);
NAMESPACE_METHOD_DECLARE(SSTable, IsCorrupted, bool, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, IsContinuouslySorted, bool, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, Wasted, uint64_t, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, MemoryStart, uint8_t*, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, Count, uint64_t, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, Capacity, uint64_t, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, AvaliableMemory, uint64_t, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, PhysicalSize, uint64_t, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, CopyTo, ERROR_CODE, const _SSTable* thiz, uint8_t* start, uint64_t size, _SSTable** table);
NAMESPACE_METHOD_DECLARE(SSTable, Set, ERROR_CODE, _SSTable* thiz, const char* key, uint64_t keySize, const char* value, uint64_t valueSize);
NAMESPACE_METHOD_DECLARE(SSTable, Delete, ERROR_CODE, _SSTable* thiz, const char* key, uint64_t keySize);
NAMESPACE_METHOD_DECLARE(SSTable, DeleteAt, ERROR_CODE, _SSTable* thiz, int64_t index);
NAMESPACE_METHOD_DECLARE(SSTable, GetValue, ERROR_CODE, _SSTable* thiz, const char* key, uint64_t keySize, const char** value, uint64_t* valueSize);
NAMESPACE_METHOD_DECLARE(SSTable, GetValueAt, ERROR_CODE, _SSTable* thiz, int64_t index, const char** value, uint64_t* valueSize);
NAMESPACE_METHOD_DECLARE(SSTable, GetKeyAt, ERROR_CODE, _SSTable* thiz, int64_t index, const char** key, uint64_t* keySize);
NAMESPACE_METHOD_DECLARE(SSTable, GetKeyValuePairAt, ERROR_CODE, _SSTable* thiz, int64_t index, const _String** key, const _String** value);
NAMESPACE_METHOD_DECLARE(SSTable, Find, int64_t,
                         _SSTable* thiz,
                         const char* keyLow,
                         uint64_t keyLowSize,
                         bool isKeyLowClosed,
                         const char* keyHigh,
                         uint64_t keyHighSize,
                         bool isKeyHighClosed,
                         int start,
                         int count,
                         IUint64** iuint64,
                         const _String*** keyArray,
                         const _String*** valueArray,
                         uint64_t* arraySize);
NAMESPACE_METHOD_DECLARE(SSTable, Search, ERROR_CODE, const _SSTable* thiz, const char* key, uint64_t keySize);
NAMESPACE_METHOD_DECLARE(SSTable, DerefResource, ERROR_CODE, _SSTable* thiz);

NAMESPACE_METHOD_DECLARE(SSTable, _FirstIndex, uint64_t*, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, _NextData, _String*, const _SSTable* thiz);
NAMESPACE_METHOD_DECLARE(SSTable, _Search, int64_t, const _SSTable* thiz, _String* key);
NAMESPACE_METHOD_DECLARE(SSTable, _Get, uint64_t*, const _SSTable* thiz, const char* key, uint64_t keySize);
NAMESPACE_METHOD_DECLARE(SSTable, _GetAt, uint64_t*, const _SSTable* thiz, int64_t index);
NAMESPACE_METHOD_DECLARE(SSTable, _IUint64, IUint64*, const _SSTable* thiz);

NAMESPACE(SSTable)
{
    NAMESPACE_METHOD_DEFINE(SSTable, Reinterpret);
    NAMESPACE_METHOD_DEFINE(SSTable, Construct);
    NAMESPACE_METHOD_DEFINE(SSTable, IsCompact);
    NAMESPACE_METHOD_DEFINE(SSTable, SetCompact);
    NAMESPACE_METHOD_DEFINE(SSTable, IsReferenced);
    NAMESPACE_METHOD_DEFINE(SSTable, IsGreedy);
    NAMESPACE_METHOD_DEFINE(SSTable, SetGreedy);
    NAMESPACE_METHOD_DEFINE(SSTable, IsCorrupted);
    NAMESPACE_METHOD_DEFINE(SSTable, IsContinuouslySorted);
    NAMESPACE_METHOD_DEFINE(SSTable, Wasted);
    NAMESPACE_METHOD_DEFINE(SSTable, MemoryStart);
    NAMESPACE_METHOD_DEFINE(SSTable, Count);
    NAMESPACE_METHOD_DEFINE(SSTable, Capacity);
    NAMESPACE_METHOD_DEFINE(SSTable, AvaliableMemory);
    NAMESPACE_METHOD_DEFINE(SSTable, PhysicalSize);
    NAMESPACE_METHOD_DEFINE(SSTable, CopyTo);
    NAMESPACE_METHOD_DEFINE(SSTable, Set);
    NAMESPACE_METHOD_DEFINE(SSTable, Delete);
    NAMESPACE_METHOD_DEFINE(SSTable, DeleteAt);
    NAMESPACE_METHOD_DEFINE(SSTable, GetValue);
    NAMESPACE_METHOD_DEFINE(SSTable, GetValueAt);
    NAMESPACE_METHOD_DEFINE(SSTable, GetKeyAt);
    NAMESPACE_METHOD_DEFINE(SSTable, GetKeyValuePairAt);
    NAMESPACE_METHOD_DEFINE(SSTable, Find);
    NAMESPACE_METHOD_DEFINE(SSTable, Search);
    NAMESPACE_METHOD_DEFINE(SSTable, DerefResource);

    NAMESPACE_METHOD_DEFINE(SSTable, _FirstIndex);
    NAMESPACE_METHOD_DEFINE(SSTable, _NextData);
    NAMESPACE_METHOD_DEFINE(SSTable, _Search);
    NAMESPACE_METHOD_DEFINE(SSTable, _Get);
    NAMESPACE_METHOD_DEFINE(SSTable, _GetAt);
    NAMESPACE_METHOD_DEFINE(SSTable, _IUint64);
};

CLASS_DECLARE(_SSTableShareHeader);

NAMESPACE_METHOD_DECLARE(SSTableShareHeader, Construct, _SSTableShareHeader*, uint8_t* start, uint64_t size);
NAMESPACE_METHOD_DECLARE(SSTableShareHeader, Table, _SSTable*, _SSTableShareHeader* thiz);
NAMESPACE_METHOD_DECLARE(SSTableShareHeader, Lock, void, _SSTableShareHeader* thiz);
NAMESPACE_METHOD_DECLARE(SSTableShareHeader, Unlock, void, _SSTableShareHeader* thiz);

NAMESPACE(SSTableShareHeader)
{
    NAMESPACE_METHOD_DEFINE(SSTableShareHeader, Construct);
    NAMESPACE_METHOD_DEFINE(SSTableShareHeader, Table);
    NAMESPACE_METHOD_DEFINE(SSTableShareHeader, Lock);
    NAMESPACE_METHOD_DEFINE(SSTableShareHeader, Unlock);
};


MEMBER_METHOD_DECLARE(DualSSTable, Initialize, ERROR_CODE, uint8_t* syncSpace, uint64_t syncSize, uint8_t* data0Space, uint64_t data0Size, uint8_t* data1Space, uint64_t data1Size);
MEMBER_METHOD_DECLARE(DualSSTable, ResetData, ERROR_CODE, bool active);
MEMBER_METHOD_DECLARE(DualSSTable, Switch, ERROR_CODE, bool force);
MEMBER_METHOD_DECLARE(DualSSTable, LockActive, _SSTableShareHeader*);
MEMBER_METHOD_DECLARE(DualSSTable, ReplacePassive, ERROR_CODE, uint8_t* dataSpace, uint64_t dataSize, uint8_t** originalDataSpace, uint64_t* originalDataSize);

MEMBER_METHOD_DECLARE(DualSSTable, _MoveContentUnsafe, ERROR_CODE, int currentIndex);
MEMBER_METHOD_DECLARE(DualSSTable, _ResetTableUnsafe, ERROR_CODE, int index);

CLASS(DualSSTable)
{
    MEMBER_METHOD_DEFINE(DualSSTable, Initialize);
    MEMBER_METHOD_DEFINE(DualSSTable, ResetData);
    MEMBER_METHOD_DEFINE(DualSSTable, Switch);
    MEMBER_METHOD_DEFINE(DualSSTable, LockActive);
    MEMBER_METHOD_DEFINE(DualSSTable, ReplacePassive);

    MEMBER_METHOD_DEFINE(DualSSTable, _MoveContentUnsafe);
    MEMBER_METHOD_DEFINE(DualSSTable, _ResetTableUnsafe);

    _SSTableShareHeader* sync;
    int64_t syncSize;
    _SSTableShareHeader* data[2];
    int64_t dataSize[2];
};


MEMBER_METHOD_DECLARE(SimpleSSTable, Initialize, ERROR_CODE, bool safe, uint8_t* syncSpace, uint64_t syncSize, uint8_t* data0Space, uint64_t data0Size, uint8_t* data1Space, uint64_t data1Size);
MEMBER_METHOD_DECLARE(SimpleSSTable, Set, ERROR_CODE, const char* key, uint64_t keySize, const char* value, uint64_t valueSize);
MEMBER_METHOD_DECLARE(SimpleSSTable, Delete, ERROR_CODE, const char* key, uint64_t keySize);
MEMBER_METHOD_DECLARE(SimpleSSTable, GetValue, ERROR_CODE, const char* key, uint64_t keySize, const char** value, uint64_t* valueSize);
MEMBER_METHOD_DECLARE(SimpleSSTable, Find, ERROR_CODE,
                      const char* keyLow,
                      uint64_t keyLowSize,
                      bool isKeyLowClosed,
                      const char* keyHigh,
                      uint64_t keyHighSize,
                      bool isKeyHighClosed,
                      int start,
                      int count,
                      IUint64** iuint64,
                      const _String*** keyArray,
                      const _String*** valueArray,
                      uint64_t* arraySize);
MEMBER_METHOD_DECLARE(SimpleSSTable, DerefResource, ERROR_CODE);
MEMBER_METHOD_DECLARE(SimpleSSTable, ReplacePassive, ERROR_CODE, uint8_t* dataSpace, uint64_t dataSize, uint8_t** originalDataSpace, uint64_t* originalDataSize);
MEMBER_METHOD_DECLARE(SimpleSSTable, DualTable, DualSSTable*);

CLASS(SimpleSSTable)
{
    MEMBER_METHOD_DEFINE(SimpleSSTable, Initialize);
    MEMBER_METHOD_DEFINE(SimpleSSTable, Set);
    MEMBER_METHOD_DEFINE(SimpleSSTable, Delete);
    MEMBER_METHOD_DEFINE(SimpleSSTable, GetValue);
    MEMBER_METHOD_DEFINE(SimpleSSTable, Find);
    MEMBER_METHOD_DEFINE(SimpleSSTable, DerefResource);
    MEMBER_METHOD_DEFINE(SimpleSSTable, ReplacePassive);
    MEMBER_METHOD_DEFINE(SimpleSSTable, DualTable);

    DualSSTable dualTable;
    bool safe;
};

#endif /* __SSTABLE_H__ */
