#ifndef __LOCK_H__
#define __LOCK_H__

#define __USE_XOPEN2K
#include <pthread.h>
#include <stdbool.h>

#include "ClassSimulator.h"

NAMESPACE_METHOD_DECLARE(Lock, Initialize, bool, pthread_mutex_t* mutex);
NAMESPACE_METHOD_DECLARE(Lock, Lock, bool, pthread_mutex_t* mutex);
NAMESPACE_METHOD_DECLARE(Lock, Unlock, bool, pthread_mutex_t* mutex);

NAMESPACE(Lock)
{
    NAMESPACE_METHOD_DEFINE(Lock, Initialize);
    NAMESPACE_METHOD_DEFINE(Lock, Lock);
    NAMESPACE_METHOD_DEFINE(Lock, Unlock);
};

#endif /* __LOCK_H__ */
