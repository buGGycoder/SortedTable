#include <errno.h>
#include "Lock.h"

#define LOG(x,...) printf(__VA_ARGS__)

NAMESPACE_METHOD_IMPLEMENT(Lock, Initialize, bool, pthread_mutex_t* mutex)
{
    if (!mutex)
    {
        LOG(ERROR, "DEV BUG: mutex is null.");
        return false;
    }

    pthread_mutexattr_t mutexattr;
    if (pthread_mutexattr_init(&mutexattr))
    {
        LOG(ERROR, "cannot init mutexattr");
        return false;
    }

    do
    {
        if (pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED))
        {
            LOG(ERROR, "cannot set p shared");
            break;
        }

        if (pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP))
        {
            LOG(ERROR, "cannot set type recursive");
            break;
        }

        if (pthread_mutexattr_setrobust_np(&mutexattr, PTHREAD_MUTEX_ROBUST))
        {
            LOG(ERROR, "cannot set robust");
            break;
        }

        if (pthread_mutex_init(mutex, &mutexattr))
        {
            LOG(ERROR, "cannot init mutex");
            break;
        }

        return true;
    }
    while (false);

    if (pthread_mutexattr_destroy(&mutexattr))
    {
        LOG(ERROR, "cannot destory mutexattr");
    }

    return false;
}

NAMESPACE_METHOD_IMPLEMENT(Lock, Lock, bool, pthread_mutex_t* mutex)
{
    if (!mutex)
    {
        return false;
    }

    int e = pthread_mutex_lock(mutex);
    if (!e)
    {
        return true;
    }

    if (EOWNERDEAD != e)
    {
        return false;
    }

    LOG(ERROR, "EOWNERDEAD");

    if (pthread_mutex_consistent_np(mutex))
    {
        return true;
    }

    LOG(ERROR, "consistent_np failed");
    pthread_mutex_unlock(mutex);
    return false;
}

NAMESPACE_METHOD_IMPLEMENT(Lock, Unlock, bool, pthread_mutex_t* mutex)
{
    if (mutex)
    {
        return false;
    }

    return !pthread_mutex_unlock(mutex);
}

NAMESPACE_INITIALIZE(Lock)
{
    NAMESPACE_METHOD_INITIALIZE(Lock, Initialize)
    NAMESPACE_METHOD_INITIALIZE(Lock, Lock)
    NAMESPACE_METHOD_INITIALIZE(Lock, Unlock)
};
