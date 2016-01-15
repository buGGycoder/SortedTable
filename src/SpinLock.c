#include "SpinLock.h"

NAMESPACE_METHOD_IMPLEMENT(SpinLock, IsOwner, bool, volatile int32_t* lock)
{
    return gettid() == *lock;
}

NAMESPACE_METHOD_IMPLEMENT(SpinLock, Lock, void, volatile int32_t* lock)
{
    SPINLOCK_LOCK(lock);
}

NAMESPACE_METHOD_IMPLEMENT(SpinLock, Unlock, void, volatile int32_t* lock)
{
    SPINLOCK_UNLOCK(lock);
}

NAMESPACE_INITIALIZE(SpinLock)
{
    NAMESPACE_METHOD_INITIALIZE(SpinLock, IsOwner)
    NAMESPACE_METHOD_INITIALIZE(SpinLock, Lock)
    NAMESPACE_METHOD_INITIALIZE(SpinLock, Unlock)
};
