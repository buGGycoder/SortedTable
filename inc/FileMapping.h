#ifndef __FILEMAPPING_H__
#define __FILEMAPPING_H__

#include <stdbool.h>
#include <stdint.h>

#include "ClassSimulator.h"

#define LENGTH_OF_SHA1_STRING (40)
#define LENGTH_OF_SHA1_BINARY (20)

MEMBER_METHOD_DECLARE(FileMapping, Initialize, bool, const char* path, const char sha1[LENGTH_OF_SHA1_BINARY], uint32_t crc32, uint64_t size, bool readOnly, bool lockForWrite);
MEMBER_METHOD_DECLARE(FileMapping, Ok, bool);
MEMBER_METHOD_DECLARE(FileMapping, FdOk, bool);
MEMBER_METHOD_DECLARE(FileMapping, MmOk, bool);
MEMBER_METHOD_DECLARE(FileMapping, Sha1, const char*);
MEMBER_METHOD_DECLARE(FileMapping, Crc32, uint32_t);
MEMBER_METHOD_DECLARE(FileMapping, Size, uint64_t);
STATIC_METHOD_DECLARE(FileMapping, CalculateSha1, void, const void* source, size_t size, char sum[LENGTH_OF_SHA1_BINARY]);
STATIC_METHOD_DECLARE(FileMapping, CalculateCrc32, uint32_t, const char* source, size_t size);
STATIC_METHOD_DECLARE(FileMapping, GetSize, bool, int fd, uint64_t* size);
MEMBER_METHOD_DECLARE(FileMapping, StampSha1, bool);
MEMBER_METHOD_DECLARE(FileMapping, StampCrc32, bool);
MEMBER_METHOD_DECLARE(FileMapping, StampSize, bool);
MEMBER_METHOD_DECLARE(FileMapping, CheckSha1, bool);
MEMBER_METHOD_DECLARE(FileMapping, CheckCrc32, bool);
MEMBER_METHOD_DECLARE(FileMapping, CheckSize, bool);
MEMBER_METHOD_DECLARE(FileMapping, FileDescriptor, int);
MEMBER_METHOD_DECLARE(FileMapping, MemoryMap, void*);
MEMBER_METHOD_DECLARE(FileMapping, Write, bool, uint64_t offset, const void* fragment, uint64_t fragmentSize);
MEMBER_METHOD_DECLARE(FileMapping, Read, bool, uint64_t offset, void* fragment, uint64_t* fragmentSize);
MEMBER_METHOD_DECLARE(FileMapping, CanRead, bool);
MEMBER_METHOD_DECLARE(FileMapping, LockForRead, bool);
MEMBER_METHOD_DECLARE(FileMapping, LockForWrite, bool);
MEMBER_METHOD_DECLARE(FileMapping, Unlock, bool);
MEMBER_METHOD_DECLARE(FileMapping, Sync, bool);
MEMBER_METHOD_DECLARE(FileMapping, Close, bool);

CLASS(FileMapping)
{
    MEMBER_METHOD_DEFINE(FileMapping, Initialize);
    MEMBER_METHOD_DEFINE(FileMapping, Ok);
    MEMBER_METHOD_DEFINE(FileMapping, FdOk);
    MEMBER_METHOD_DEFINE(FileMapping, MmOk);
    MEMBER_METHOD_DEFINE(FileMapping, Sha1);
    MEMBER_METHOD_DEFINE(FileMapping, Crc32);
    MEMBER_METHOD_DEFINE(FileMapping, Size);
    STATIC_METHOD_DEFINE(FileMapping, CalculateSha1);
    STATIC_METHOD_DEFINE(FileMapping, CalculateCrc32);
    STATIC_METHOD_DEFINE(FileMapping, GetSize);
    MEMBER_METHOD_DEFINE(FileMapping, StampSha1);
    MEMBER_METHOD_DEFINE(FileMapping, StampCrc32);
    MEMBER_METHOD_DEFINE(FileMapping, StampSize);
    MEMBER_METHOD_DEFINE(FileMapping, CheckSha1);
    MEMBER_METHOD_DEFINE(FileMapping, CheckCrc32);
    MEMBER_METHOD_DEFINE(FileMapping, CheckSize);
    MEMBER_METHOD_DEFINE(FileMapping, FileDescriptor);
    MEMBER_METHOD_DEFINE(FileMapping, MemoryMap);
    MEMBER_METHOD_DEFINE(FileMapping, Write);
    MEMBER_METHOD_DEFINE(FileMapping, Read);
    MEMBER_METHOD_DEFINE(FileMapping, CanRead);
    MEMBER_METHOD_DEFINE(FileMapping, LockForRead);
    MEMBER_METHOD_DEFINE(FileMapping, LockForWrite);
    MEMBER_METHOD_DEFINE(FileMapping, Unlock);
    MEMBER_METHOD_DEFINE(FileMapping, Sync);
    MEMBER_METHOD_DEFINE(FileMapping, Close);

    int         fd;
    uint8_t     sha1[LENGTH_OF_SHA1_BINARY];
    uint32_t    crc32;
    uint64_t    size;
    bool        readOnly;
    void*       mm;
};

#endif /* __FILEMAPPING_H__ */
