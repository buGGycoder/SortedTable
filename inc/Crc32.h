#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdint.h>
#include "ClassSimulator.h"


NAMESPACE_METHOD_DECLARE(Crc32, Accumulate, uint32_t, uint32_t accumulated, const char* source, uint64_t size);
NAMESPACE_METHOD_DECLARE(Crc32, Finish, uint32_t, uint32_t accumulated, uint64_t accumulatedSize);

NAMESPACE(Crc32)
{
    NAMESPACE_METHOD_DEFINE(Crc32, Accumulate);
    NAMESPACE_METHOD_DEFINE(Crc32, Finish);
};

#endif /* __CRC32_H__ */
