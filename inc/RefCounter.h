#ifndef REFCOUNTER_H_INCLUDED
#define REFCOUNTER_H_INCLUDED

#include <stdint.h>
#include "ClassSimulator.h"

#pragma pack(push)
#pragma pack(1)
PLAIN_OLD_DATA(_RefCounter)
{
    /* _WIN32 InterlockedCompareExchange requires long as type of the lock */
    volatile int32_t lock;
    volatile int32_t counter;
    volatile int8_t state;
};
#pragma pack(pop)


NAMESPACE_METHOD_DECLARE(RefCounter, AddRef, int32_t, _RefCounter* counter);
NAMESPACE_METHOD_DECLARE(RefCounter, Release, int32_t, _RefCounter* counter);
NAMESPACE_METHOD_DECLARE(RefCounter, AddRefUnsafe, int32_t, _RefCounter* counter);
NAMESPACE_METHOD_DECLARE(RefCounter, ReleaseUnsafe, int32_t, _RefCounter* counter);


NAMESPACE(RefCounter)
{
    NAMESPACE_METHOD_DEFINE(RefCounter, AddRef);
    NAMESPACE_METHOD_DEFINE(RefCounter, Release);
    NAMESPACE_METHOD_DEFINE(RefCounter, AddRefUnsafe);
    NAMESPACE_METHOD_DEFINE(RefCounter, ReleaseUnsafe);
};


#endif /* REFCOUNTER_H_INCLUDED */
