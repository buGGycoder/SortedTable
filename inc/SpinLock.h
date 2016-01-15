#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include "ClassSimulator.h"
#include <stdbool.h>
#include <stdint.h>


NAMESPACE_METHOD_DECLARE(SpinLock, IsOwner, bool, volatile int32_t* lock);
NAMESPACE_METHOD_DECLARE(SpinLock, Lock, void, volatile int32_t* lock);
NAMESPACE_METHOD_DECLARE(SpinLock, Unlock, void, volatile int32_t* lock);

NAMESPACE(SpinLock)
{
    NAMESPACE_METHOD_DEFINE(SpinLock, IsOwner);
    NAMESPACE_METHOD_DEFINE(SpinLock, Lock);
    NAMESPACE_METHOD_DEFINE(SpinLock, Unlock);
};


#ifdef _WIN32
#include <Windows.h>
#define SCHEDULE_YIELD(...) Sleep(0)
#define gettid() GetCurrentThreadId()
#include <intrin.h>
#define SET_IF_CLEARED_I32(pi32,v) (!_InterlockedCompareExchange((pi32), (v), 0))
#define RESET_I32(pi32) (_InterlockedExchange((pi32), 0))
#else
#include <sched.h>
#define SCHEDULE_YIELD(...) sched_yield()
#define SET_IF_CLEARED_I32(pi32,v) __sync_bool_compare_and_swap((pi32), 0, (v))
#define RESET_I32(pi32) (__sync_fetch_and_and((pi32), 0))
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#define gettid() syscall(__NR_gettid)
#endif

#define SPINLOCK_LOCK(pi32) \
    do \
    { \
        uint32_t tid = gettid(); \
        do \
        { \
            while (*(pi32)) \
            { \
                SCHEDULE_YIELD(); \
            } \
        } \
        while (!SET_IF_CLEARED_I32(pi32, tid)); \
    } \
    while(0);

#define SPINLOCK_UNLOCK(pi32) { RESET_I32(pi32); }


#endif /* __SPINLOCK_H__ */
