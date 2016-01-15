#include "FileMapping.h"

#include <errno.h>
#define __USE_XOPEN2K
#define __USE_BSD
#define __USE_LARGEFILE64
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <openssl/sha.h>

#include "Crc32.h"

#define TAG "FileMapping"
#define LOG_W
#define LOG_E
#define LOG_I

MEMBER_METHOD_IMPLEMENT(FileMapping, Ok, bool)
{
    return NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, FdOk, thiz) && NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, MmOk, thiz);
}

MEMBER_METHOD_IMPLEMENT(FileMapping, FdOk, bool)
{
    return -1 != thiz->fd;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, MmOk, bool)
{
    return (void*)-1 != thiz->mm;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, Sha1, const char*)
{
    return thiz->sha1;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, Crc32, uint32_t)
{
    return thiz->crc32;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, Size, uint64_t)
{
    return thiz->size;
}

STATIC_METHOD_IMPLEMENT(FileMapping, CalculateSha1, void, const void* source, size_t size, char sum[LENGTH_OF_SHA1_BINARY])
{
    memset(sum, 0, LENGTH_OF_SHA1_BINARY);
    SHA_CTX context = {0};
    SHA1_Init(&context);
    SHA1_Update(&context, source, size);
    SHA1_Final((unsigned char*)sum, &context);
}

STATIC_METHOD_IMPLEMENT(FileMapping, CalculateCrc32, uint32_t, const char* source, size_t size)
{
    return Crc32.Finish(Crc32.Accumulate(0, source, size), size);
}

STATIC_METHOD_IMPLEMENT(FileMapping, GetSize, bool, int fd, uint64_t* size)
{
    if (-1 == fd)
    {
        LOG_W(TAG, "DEV BUG: GetSize is called for unopened file.");
        return false;
    }

    if (!size)
    {
        LOG_W(TAG, "DEV BUG: GetSize is called in a non-zero size state.");
        return false;
    }

    struct stat64 st;
    if (fstat(fd, &st))
    {
        LOG_W(TAG, "fstat failed. Error: %d", errno);
        return false;
    }

    *size = st.st_size;
    return true;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, StampSha1, bool)
{
    if ((void*)-1 == thiz->mm)
    {
        LOG_W(TAG, "DEV BUG: CalculateSha1 is called for unmapped file.");
        return false;
    }

    NONINSTANTIAL_STATIC_METHOD_CALL(FileMapping, CalculateSha1, thiz->mm, thiz->size, thiz->sha1);
    return true;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, StampCrc32, bool)
{
    if ((void*)-1 == thiz->mm)
    {
        LOG_W(TAG, "DEV BUG: CalculateCrc32 is called for unmapped file.");
        return false;
    }

    thiz->crc32 = NONINSTANTIAL_STATIC_METHOD_CALL(FileMapping, CalculateCrc32, thiz->mm, thiz->size);
    return true;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, StampSize, bool)
{
    return NONINSTANTIAL_STATIC_METHOD_CALL(FileMapping, GetSize, thiz->fd, &thiz->size);
}

MEMBER_METHOD_IMPLEMENT(FileMapping, CheckSha1, bool)
{
    if ((void*)-1 == thiz->mm)
    {
        LOG_W(TAG, "DEV BUG: CheckSha1 is called for unmapped file.");
        return false;
    }

    uint8_t sum[LENGTH_OF_SHA1_BINARY] = {0};
    thiz->CalculateSha1(thiz->mm, thiz->size, sum);
    int i = 0;
    for (; i < LENGTH_OF_SHA1_BINARY; ++i)
    {
        if (sum[i] != thiz->sha1[i])
        {
            return false;
        }
    }

    return true;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, CheckCrc32, bool)
{
    if ((void*)-1 == thiz->mm)
    {
        LOG_W(TAG, "DEV BUG: CheckCrc32 is called for unmapped file.");
        return false;
    }

    return  thiz->crc32 == thiz->CalculateCrc32(thiz->mm, thiz->size);;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, CheckSize, bool)
{
    uint64_t size = 0;
    return thiz->GetSize(thiz->fd, &size) && (thiz->size == size);
}

MEMBER_METHOD_IMPLEMENT(FileMapping, FileDescriptor, int)
{
    return thiz->fd;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, MemoryMap, void*)
{
    return thiz->mm;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, Write, bool, uint64_t offset, const void* fragment, uint64_t fragmentSize)
{
    if (thiz->readOnly)
    {
        return false;
    }

    if ((void*)-1 == thiz->mm)
    {
        LOG_W(TAG, "DEV BUG: Write is called for unmapped file.");
        return false;
    }

    if (!fragmentSize)
    {
        LOG_W(TAG, "DEV BUG: Write is called with fragmentSize 0.");
        return true;
    }

    if (offset >= thiz->size)
    {
        LOG_E(TAG, "Failed to write. offset is too big. Offset: ", offset,  ", Size: ", thiz->size);
        return false;
    }

    if (offset + fragmentSize < offset || offset + fragmentSize > thiz->size)
    {
        LOG_E(TAG, "Failed to write. offset + fragmentSize is too big. Offset: ", offset, ", FragmentSize: ", fragmentSize, ", Size: ", thiz->size);
        /* fragmentSize = thiz->size - offset; */
        return false;
    }

    memmove(thiz->mm + offset, fragment, fragmentSize);
    return true;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, Read, bool, uint64_t offset, void* fragment, uint64_t* fragmentSize)
{
    if ((void*)-1 == thiz->mm)
    {
        LOG_W(TAG, "DEV BUG: Read is called for unmapped file.");
        return false;
    }

    if (!fragmentSize)
    {
        LOG_E(TAG, "DEV BUG: NULL fragmentSize.");
        return false;
    }

    if (!*fragmentSize)
    {
        LOG_W(TAG, "DEV BUG: Read is called with fragmentSize 0.");
        return true;
    }

    if (offset >= thiz->size)
    {
        LOG_E(TAG, "Failed to read. offset is too big. Offset: ", offset, ", Size: ", thiz->size);
        return false;
    }

    if (offset + *fragmentSize > thiz->size)
    {
        *fragmentSize = thiz->size - offset;
    }

    memmove(fragment, thiz->mm + offset, *fragmentSize);
    return true;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, CanRead, bool)
{
    struct flock fileLock = {0};
    fileLock.l_start = 0;
    fileLock.l_len = 0;
    fileLock.l_pid = 0;
    fileLock.l_type = F_RDLCK;
    fileLock.l_whence = SEEK_SET;
    return -1 != fcntl(thiz->fd, F_GETLK, &fileLock)
            && (F_UNLCK == fileLock.l_type || F_RDLCK == fileLock.l_type);
}

MEMBER_METHOD_IMPLEMENT(FileMapping, LockForRead, bool)
{
    struct flock fileLock = {0};
    fileLock.l_start = 0;
    fileLock.l_len = 0;
    fileLock.l_pid = 0;
    fileLock.l_type = F_RDLCK;
    fileLock.l_whence = SEEK_SET;
    return -1 != fcntl(thiz->fd, F_SETLK, &fileLock);
}

MEMBER_METHOD_IMPLEMENT(FileMapping, LockForWrite, bool)
{
    if (thiz->readOnly)
    {
        return false;
    }

    struct flock fileLock = {0};
    fileLock.l_start = 0;
    fileLock.l_len = 0;
    fileLock.l_pid = 0;
    fileLock.l_type = F_WRLCK;
    fileLock.l_whence = SEEK_SET;
    return -1 != fcntl(thiz->fd, F_SETLK, &fileLock);
}

MEMBER_METHOD_IMPLEMENT(FileMapping, Unlock, bool)
{
    struct flock fileLock = {0};
    fileLock.l_start = 0;
    fileLock.l_len = 0;
    fileLock.l_pid = 0;
    fileLock.l_type = F_UNLCK;
    fileLock.l_whence = SEEK_SET;
    return -1 != fcntl(thiz->fd, F_SETLK, &fileLock);
}

MEMBER_METHOD_IMPLEMENT(FileMapping, Sync, bool)
{
    if (thiz->readOnly)
    {
        if (NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, MmOk, thiz) && madvise(thiz->mm, thiz->size, MADV_DONTNEED))
        {
            LOG_W(TAG, "Cannot discard memory for mmap. Error: ", errno);
        }

        if (NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, FdOk, thiz) && posix_fadvise64(thiz->fd, 0, 0, POSIX_FADV_DONTNEED))
        {
            LOG_W(TAG, "Cannot discard memory for fd. Error: ", errno);
        }
    }
    else
    {
#ifdef FILEOP_SAFE_MODE
        if (NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, MmOk, thiz) && msync(thiz->mm, thiz->size, MS_ASYNC))
        {
            LOG_I(TAG, "Failed to msync(). fd: ", thiz->fd, ", Error: ", errno);
            return false;
        }

        if (NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, FdOk, thiz) && fsync(thiz->fd))
        {
            LOG_I(TAG, "Failed to fsync(). fd: ", thiz->fd, ", Error: ", errno);
            return false;
        }
#endif
    }

    return true;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, Close, bool)
{
    bool succeeded = true;;
    if ((void*)-1 != thiz->mm)
    {
        if (!NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, Sync, thiz))
        {
            succeeded = false;
        }

        if (munmap(thiz->mm, thiz->size))
        {
            LOG_I(TAG, "Failed to munmap(). fd: ", thiz->fd, ", Error: ", errno);
            succeeded = false;
        }

        thiz->mm = (void*)-1;
    }

    if (-1 != thiz->fd)
    {
        if (close(thiz->fd))
        {
            LOG_I(TAG, "Failed to close(). fd: ", thiz->fd, ", Error: ", errno);
            succeeded = false;
        }

        thiz->fd = -1;
    }

    return succeeded;
}



MEMBER_METHOD_IMPLEMENT(FileMapping, Initialize, bool, const char* path, const char sha1[LENGTH_OF_SHA1_BINARY], uint32_t crc32, uint64_t size, bool readOnly, bool lockForWrite)
{
    if (!(readOnly || size))
    {
        LOG_E(TAG, "Invalid size specified for writable file ", path);
        return false;;
    }

    if (sha1)
    {
        memcpy(thiz->sha1, sha1, LENGTH_OF_SHA1_BINARY);
    }

    thiz->crc32 = crc32;
    thiz->size = size;
    thiz->readOnly = readOnly;
    bool succeeded = false;
    do
    {
        thiz->fd = open(path, (readOnly ? O_RDONLY : (O_RDWR | O_CREAT /*| O_DSYNC*/)), 0644);
        if (-1 == thiz->fd)
        {
            LOG_E(TAG, "Cannot open file ", path, (readOnly ? " for read. Error: " : "for write. Error:"), errno);
            break;
        }

        if (readOnly)
        {
            if (!(thiz->size || NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, StampSize, thiz)))
            {
                LOG_E(TAG, "Cannot get the file size for readonly file ", path, " Error: ", errno);
                break;
            }
        }
        else
        {
            if (lockForWrite && !NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, LockForWrite, thiz))
            {
                LOG_E(TAG, "Cannot lock file ", path, " for write. Error: ", errno);
                break;
            }

            if (thiz->size && posix_fallocate(thiz->fd, 0, thiz->size))
            {
                LOG_E(TAG, "Cannot posix_fallocate file ", path, " to size ", thiz->size, ". Error: ", errno);
                break;
            }

            if (ftruncate(thiz->fd, thiz->size))
            {
                LOG_E(TAG, "Cannot ftruncate file ", path, " to size ", thiz->size, ". Error: ", errno);
                break;
            }
        }

        if (thiz->size)
        {
            thiz->mm = (char*)mmap(NULL, thiz->size, (thiz->readOnly ? PROT_READ : (PROT_READ | PROT_WRITE)), MAP_SHARED, thiz->fd, 0);
            if ((void*)-1 == thiz->mm)
            {
                LOG_E(TAG, "Cannot mmap file ", path, ". Error: ", errno);
                break;
            }
        }

        succeeded = true;
    }
    while(0);

    if (!succeeded)
    {
        NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, Close, thiz);
        return false;
    }

    return true;
}

MEMBER_METHOD_IMPLEMENT(FileMapping, FileMapping, FileMapping*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        thiz->fd = -1;
        thiz->mm = (void*)-1;
        break;
    case METHOD_DTOR:
        NONINSTANTIAL_MEMBER_METHOD_CALL(FileMapping, Close, thiz);
        break;
    default:
        return NULL;

    }

    return thiz;
}

CLASS_INITIALIZE(FileMapping)
{
    MEMBER_METHOD_INITIALIZE(FileMapping, Initialize)
    MEMBER_METHOD_INITIALIZE(FileMapping, Ok)
    MEMBER_METHOD_INITIALIZE(FileMapping, FdOk)
    MEMBER_METHOD_INITIALIZE(FileMapping, MmOk)
    MEMBER_METHOD_INITIALIZE(FileMapping, Sha1)
    MEMBER_METHOD_INITIALIZE(FileMapping, Crc32)
    MEMBER_METHOD_INITIALIZE(FileMapping, Size)
    STATIC_METHOD_INITIALIZE(FileMapping, CalculateSha1)
    STATIC_METHOD_INITIALIZE(FileMapping, CalculateCrc32)
    STATIC_METHOD_INITIALIZE(FileMapping, GetSize)
    MEMBER_METHOD_INITIALIZE(FileMapping, StampSha1)
    MEMBER_METHOD_INITIALIZE(FileMapping, StampCrc32)
    MEMBER_METHOD_INITIALIZE(FileMapping, StampSize)
    MEMBER_METHOD_INITIALIZE(FileMapping, CheckSha1)
    MEMBER_METHOD_INITIALIZE(FileMapping, CheckCrc32)
    MEMBER_METHOD_INITIALIZE(FileMapping, CheckSize)
    MEMBER_METHOD_INITIALIZE(FileMapping, FileDescriptor)
    MEMBER_METHOD_INITIALIZE(FileMapping, MemoryMap)
    MEMBER_METHOD_INITIALIZE(FileMapping, Write)
    MEMBER_METHOD_INITIALIZE(FileMapping, Read)
    MEMBER_METHOD_INITIALIZE(FileMapping, CanRead)
    MEMBER_METHOD_INITIALIZE(FileMapping, LockForRead)
    MEMBER_METHOD_INITIALIZE(FileMapping, LockForWrite)
    MEMBER_METHOD_INITIALIZE(FileMapping, Unlock)
    MEMBER_METHOD_INITIALIZE(FileMapping, Sync)
    MEMBER_METHOD_INITIALIZE(FileMapping, Close)
};
