#include "RefCounter.h"
#include "SpinLock.h"

#include <stdbool.h>

enum
{
    REF_COUNTER_STATE_CREATED = 0,
    REF_COUNTER_STATE_STARTED = 1,
    REF_COUNTER_STATE_STOPPED = 2,
};

NAMESPACE_METHOD_IMPLEMENT(RefCounter, _AddRef, int32_t, _RefCounter* counter, bool lock)
{
    int32_t count = -1;
    if (REF_COUNTER_STATE_STOPPED == counter->state)
    {
        return -1;
    }

    if (lock)
    {
        SpinLock.Lock(&counter->lock);
    }

    if (REF_COUNTER_STATE_STOPPED != counter->state)
    {
        count = ++counter->counter;
        if (REF_COUNTER_STATE_CREATED == counter->state)
        {
            counter->state = REF_COUNTER_STATE_STARTED;
        }
    }

    if (lock)
    {
        SpinLock.Unlock(&counter->lock);
    }

    return count;
}

NAMESPACE_METHOD_IMPLEMENT(RefCounter, _Release, int32_t, _RefCounter* counter, bool lock)
{
    int32_t count = -1;
    if (REF_COUNTER_STATE_STARTED != counter->state)
    {
        if (REF_COUNTER_STATE_CREATED == counter->state)
        {
            /* DEV BUG: why release an object w/o add ref??? */
        }

        return -1;
    }

    if (lock)
    {
        SpinLock.Lock(&counter->lock);
    }

    if (REF_COUNTER_STATE_STARTED == counter->state && !(count = --counter->counter))
    {
        counter->state = REF_COUNTER_STATE_STOPPED;
    }

    if (lock)
    {
        SpinLock.Unlock(&counter->lock);
    }

    return count;
}

NAMESPACE_METHOD_IMPLEMENT(RefCounter, AddRef, int32_t, _RefCounter* counter)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(RefCounter, _AddRef, counter, true);
}

NAMESPACE_METHOD_IMPLEMENT(RefCounter, Release, int32_t, _RefCounter* counter)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(RefCounter, _Release, counter, true);
}

NAMESPACE_METHOD_IMPLEMENT(RefCounter, AddRefUnsafe, int32_t, _RefCounter* counter)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(RefCounter, _AddRef, counter, false);
}

NAMESPACE_METHOD_IMPLEMENT(RefCounter, ReleaseUnsafe, int32_t, _RefCounter* counter)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(RefCounter, _Release, counter, false);
}


NAMESPACE_INITIALIZE(RefCounter)
{
    NAMESPACE_METHOD_INITIALIZE(RefCounter, AddRef)
    NAMESPACE_METHOD_INITIALIZE(RefCounter, Release)
    NAMESPACE_METHOD_INITIALIZE(RefCounter, AddRefUnsafe)
    NAMESPACE_METHOD_INITIALIZE(RefCounter, ReleaseUnsafe)
};
