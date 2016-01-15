#include "NonBlocking.h"
#include "SpinLock.h"
#include "CalendarConstants.h"
#include "DateTime.h"

#define TIMEOUT_CHECKING_FREQUENCY (1000ULL)
#define SPINLOOP_RESCHEDULE_COUNT (10ULL)
#define SPINLOOP_NANOSLEEP_TIMEOUT (1000ULL)

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, BeginRead, NonBlockingReadState, NonBlockingStateMachine* pStateMachine)
{
    /* NOTE: Not calling timed version is to get better performance */
    NonBlockingReadState state = 0;
    uint64_t loopCounter = 0ULL;
    struct timespec ts = {0};
    ts.tv_nsec = SPINLOOP_NANOSLEEP_TIMEOUT;
    while (NON_BLOCKING_STATE_CORRUPTED_MASK & (state = pStateMachine->state))
    {
        if (SPINLOOP_RESCHEDULE_COUNT > loopCounter++)
        {
            SCHEDULE_YIELD();
        }
        else
        {
            nanosleep(&ts, NULL);
        }
    }

    __sync_add_and_fetch(&pStateMachine->readerCounter, 1);
    return state;
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, BeginReadTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, OUT NonBlockingReadState* pState)
{
    if (!pState)
    {
        return false;
    }

    const uint64_t frequency = 1 + (microsecondsTimeout < TIMEOUT_CHECKING_FREQUENCY ? microsecondsTimeout : TIMEOUT_CHECKING_FREQUENCY);
    NonBlockingReadState state = 0;
    uint64_t enterTime = NONINSTANTIAL_STATIC_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    uint64_t now = 0;
    uint64_t loopCounter = 0ULL;
    struct timespec ts = {0};
    ts.tv_nsec = 1000ULL * microsecondsTimeout < SPINLOOP_NANOSLEEP_TIMEOUT ? 1000ULL * microsecondsTimeout : SPINLOOP_NANOSLEEP_TIMEOUT;
    while (NON_BLOCKING_STATE_CORRUPTED_MASK & (state = pStateMachine->state))
    {
        ++loopCounter;
        if ((uint64_t)-1LL != microsecondsTimeout && !(loopCounter % frequency))
        {
            now = NONINSTANTIAL_STATIC_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
            if (microsecondsTimeout < now - enterTime)
            {
                return false;
            }
        }

        if (SPINLOOP_RESCHEDULE_COUNT > loopCounter)
        {
            SCHEDULE_YIELD();
        }
        else
        {
            nanosleep(&ts, NULL);
        }
    }

    __sync_add_and_fetch(&pStateMachine->readerCounter, 1);
    *pState = state;
    return true;
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, EndReadAndCheckSuccess, bool, NonBlockingStateMachine* pStateMachine, NonBlockingReadState state)
{
    __sync_sub_and_fetch(&pStateMachine->readerCounter, 1);
    return pStateMachine->state == state;
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, BeginWrite, NonBlockingWriteState, NonBlockingStateMachine* pStateMachine)
{
    /* NOTE: Not calling timed version is to get better performance */
    NonBlockingWriteState state = 0;
    uint64_t loopCounter = 0;
    struct timespec ts = {0};
    ts.tv_nsec = SPINLOOP_NANOSLEEP_TIMEOUT;
    while(true)
    {
        if (NON_BLOCKING_STATE_CORRUPTED_MASK & (state = pStateMachine->state))
        {
            if (SPINLOOP_RESCHEDULE_COUNT > loopCounter++)
            {
                SCHEDULE_YIELD();
            }
            else
            {
                nanosleep(&ts, NULL);
            }

            continue;
        }

        if (__sync_bool_compare_and_swap(&pStateMachine->state, state, 1 + state))
        {
            ++state;
            break;
        }
    }

    return state;
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, BeginWriteTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, OUT NonBlockingWriteState* pState)
{
    if (!pState)
    {
        return false;
    }

    const uint64_t frequency = 1 + (microsecondsTimeout < TIMEOUT_CHECKING_FREQUENCY ? microsecondsTimeout : TIMEOUT_CHECKING_FREQUENCY);
    NonBlockingWriteState state = 0;
    uint64_t enterTime = NONINSTANTIAL_STATIC_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    uint64_t now = 0;
    uint64_t loopCounter = 0ULL;
    struct timespec ts = {0};
    ts.tv_nsec = 1000ULL * microsecondsTimeout < SPINLOOP_NANOSLEEP_TIMEOUT ? 1000ULL * microsecondsTimeout : SPINLOOP_NANOSLEEP_TIMEOUT;
    while(true)
    {
        ++loopCounter;
        if ((uint64_t)-1LL != microsecondsTimeout && !(loopCounter % frequency))
        {
            now = NONINSTANTIAL_STATIC_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
            if (microsecondsTimeout < now - enterTime)
            {
                return false;
            }
        }

        if (NON_BLOCKING_STATE_CORRUPTED_MASK & (state = pStateMachine->state))
        {
            if (SPINLOOP_RESCHEDULE_COUNT > loopCounter)
            {
                SCHEDULE_YIELD();
            }
            else
            {
                nanosleep(&ts, NULL);
            }

            continue;
        }

        if (__sync_bool_compare_and_swap(&pStateMachine->state, state, 1 + state))
        {
            ++state;
            break;
        }
    }

    *pState = state;
    return true;
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, EndWriteAndCheckSuccess, bool, NonBlockingStateMachine* pStateMachine, NonBlockingWriteState state)
{
    return __sync_bool_compare_and_swap(&pStateMachine->state, state, 1 + state);
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, RevertWriteAndCheckSuccess, bool, NonBlockingStateMachine* pStateMachine, NonBlockingWriteState state)
{
    return __sync_bool_compare_and_swap(&pStateMachine->state, state, state - 1);
}


typedef bool TYPE_POST_WRITE_DELEGATE (NonBlockingStateMachine* pStateMachine, NonBlockingWriteState state);

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, Read, bool, NonBlockingStateMachine* pStateMachine, TYPE_TRY_ONCE* tryOnce, void* context,  NonBlockingReadState* readState)
{
    bool result = false;
    NonBlockingReadState state = 0;
    do
    {
        state = NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, BeginRead, pStateMachine);
        result = !!tryOnce(state, &pStateMachine->state, context);
    }
    while(!NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, EndReadAndCheckSuccess, pStateMachine, state));
    readState && (*readState = state);
    return result;
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, _Write, bool, NonBlockingStateMachine* pStateMachine, TYPE_TRY_ONCE* tryOnce, void* context, TYPE_POST_WRITE_DELEGATE* succeededDelegate, TYPE_POST_WRITE_DELEGATE* failedDelegate, NonBlockingWriteState* writeState)
{
    bool result = false;
    NonBlockingWriteState state = 0;
    do
    {
        state = NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, BeginWrite, pStateMachine);
        result = !!tryOnce(state, &pStateMachine->state, context);
    }
    while(!(result ? succeededDelegate(pStateMachine, state) : failedDelegate(pStateMachine, state)));
    writeState && (*writeState = state);
    return result;
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, Write, bool, NonBlockingStateMachine* pStateMachine, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, _Write, pStateMachine, tryOnce, context, NonBlocking.EndWriteAndCheckSuccess, NonBlocking.RevertWriteAndCheckSuccess, writeState);
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, WriteAlwaysEnd, bool, NonBlockingStateMachine* pStateMachine, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, _Write, pStateMachine, tryOnce, context, NonBlocking.EndWriteAndCheckSuccess, NonBlocking.EndWriteAndCheckSuccess, writeState);
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, WriteAlwaysRevert, bool, NonBlockingStateMachine* pStateMachine, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, _Write, pStateMachine, tryOnce, context, NonBlocking.RevertWriteAndCheckSuccess, NonBlocking.RevertWriteAndCheckSuccess, writeState);
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, ReadTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingReadState* readState, uint64_t* timeElapsed)
{
    bool result = false;
    NonBlockingReadState state = 0;
    uint64_t enterTime = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    uint64_t now = enterTime;
    uint64_t elapsed = 0;
    bool isAtomic = false;
    uint64_t loopCounter = 0ULL;
    struct timespec ts = {0};
    ts.tv_nsec = 1000ULL * microsecondsTimeout < SPINLOOP_NANOSLEEP_TIMEOUT ? 1000ULL * microsecondsTimeout : SPINLOOP_NANOSLEEP_TIMEOUT;
    while(microsecondsTimeout >= elapsed)
    {
        ++loopCounter;
        now = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
        elapsed = now - enterTime;
        if(!NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, BeginReadTimed, pStateMachine, microsecondsTimeout, &state))
        {
            continue;
        }

        result = !!tryOnce(state, &pStateMachine->state, context);

        if (isAtomic = !!NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, EndReadAndCheckSuccess, pStateMachine, state))
        {
            break;
        }

        if (SPINLOOP_RESCHEDULE_COUNT > loopCounter)
        {
            SCHEDULE_YIELD();
        }
        else
        {
            nanosleep(&ts, NULL);
        }
    }

    result &= isAtomic;
    timeElapsed && (*timeElapsed = elapsed);
    readState && (*readState = state);
    return result;
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, _WriteTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, TYPE_POST_WRITE_DELEGATE* succeededDelegate, TYPE_POST_WRITE_DELEGATE* failedDelegate, NonBlockingWriteState* writeState, uint64_t* timeElapsed)
{
    bool result = false;
    NonBlockingWriteState state = 0;
    uint64_t enterTime = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    uint64_t now = enterTime;
    uint64_t elapsed = 0;
    bool isAtomic = false;
    uint64_t loopCounter = 0ULL;
    struct timespec ts = {0};
    ts.tv_nsec = 1000ULL * microsecondsTimeout < SPINLOOP_NANOSLEEP_TIMEOUT ? 1000ULL * microsecondsTimeout : SPINLOOP_NANOSLEEP_TIMEOUT;
    while(microsecondsTimeout >= elapsed)
    {
        ++loopCounter;
        now = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
        elapsed = now - enterTime;
        if(!NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, BeginWriteTimed, pStateMachine, microsecondsTimeout, &state))
        {
            continue;
        }

        result = !!tryOnce(state, &pStateMachine->state, context);

        if (isAtomic = !!(result ? succeededDelegate(pStateMachine, state) : failedDelegate(pStateMachine, state)))
        {
            break;
        }

        if (SPINLOOP_RESCHEDULE_COUNT > loopCounter)
        {
            SCHEDULE_YIELD();
        }
        else
        {
            nanosleep(&ts, NULL);
        }
    }

    result &= isAtomic;
    timeElapsed && (*timeElapsed = elapsed);
    writeState && (*writeState = state);
    return result;
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, WriteTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState, uint64_t* timeElapsed)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, _WriteTimed, pStateMachine, microsecondsTimeout, tryOnce, context, NonBlocking.EndWriteAndCheckSuccess, NonBlocking.RevertWriteAndCheckSuccess, writeState, timeElapsed);
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, WriteTimedAlwaysEnd, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState, uint64_t* timeElapsed)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, _WriteTimed, pStateMachine, microsecondsTimeout, tryOnce, context, NonBlocking.EndWriteAndCheckSuccess, NonBlocking.EndWriteAndCheckSuccess, writeState, timeElapsed);
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, WriteTimedAlwaysRevert, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState, uint64_t* timeElapsed)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, _WriteTimed, pStateMachine, microsecondsTimeout, tryOnce, context, NonBlocking.RevertWriteAndCheckSuccess, NonBlocking.RevertWriteAndCheckSuccess, writeState, timeElapsed);
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, WaitForNoReader, void, NonBlockingStateMachine* pStateMachine)
{
    uint64_t loopCounter = 0ULL;
    struct timespec ts = {0};
    ts.tv_nsec = SPINLOOP_NANOSLEEP_TIMEOUT;
    while(pStateMachine->readerCounter)
    {
        if (SPINLOOP_RESCHEDULE_COUNT > loopCounter++)
        {
            SCHEDULE_YIELD();
        }
        else
        {
            nanosleep(&ts, NULL);
        }
    }
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, WaitForNoReaderTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, uint64_t* timeElapsed)
{
    bool result = false;
    uint64_t enterTime = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    uint64_t now = enterTime;
    uint64_t elapsed = 0;
    uint64_t loopCounter = 0ULL;
    struct timespec ts = {0};
    ts.tv_nsec = 1000ULL * microsecondsTimeout < SPINLOOP_NANOSLEEP_TIMEOUT ? 1000ULL * microsecondsTimeout : SPINLOOP_NANOSLEEP_TIMEOUT;
    while(microsecondsTimeout >= elapsed)
    {
        ++loopCounter;
        now = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
        elapsed = now - enterTime;
        if(!pStateMachine->readerCounter)
        {
            result = true;
            break;
        }

        if (SPINLOOP_RESCHEDULE_COUNT > loopCounter)
        {
            SCHEDULE_YIELD();
        }
        else
        {
            nanosleep(&ts, NULL);
        }
    }

    timeElapsed && (*timeElapsed = elapsed);
    return result;
}

NAMESPACE_METHOD_IMPLEMENT(NonBlocking, RetryTimed, bool, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, uint64_t* timeElapsed)
{
    NonBlockingStateMachine stateMachine = {0};
    NonBlockingStateMachine* pStateMachine = &stateMachine;
    bool result = false;
    NonBlockingReadState state = 0;
    uint64_t enterTime = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    uint64_t now = enterTime;
    uint64_t elapsed = 0;
    bool isAtomic = false;
    uint64_t loopCounter = 0ULL;
    struct timespec ts = {0};
    ts.tv_nsec = 1000ULL * microsecondsTimeout < SPINLOOP_NANOSLEEP_TIMEOUT ? 1000ULL * microsecondsTimeout : SPINLOOP_NANOSLEEP_TIMEOUT;
    while(microsecondsTimeout >= elapsed)
    {
        ++loopCounter;
        now = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
        elapsed = now - enterTime;
        if(!NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, BeginReadTimed, pStateMachine, microsecondsTimeout, &state))
        {
            continue;
        }

        __sync_add_and_fetch(&pStateMachine->state, 2 * !(result = !!tryOnce(state, &pStateMachine->state, context)));

        if (isAtomic = !!NONINSTANTIAL_NAMESPACE_METHOD_CALL(NonBlocking, EndReadAndCheckSuccess, pStateMachine, state))
        {
            break;
        }

        if (SPINLOOP_RESCHEDULE_COUNT > loopCounter)
        {
            SCHEDULE_YIELD();
        }
        else
        {
            nanosleep(&ts, NULL);
        }
    }

    result &= isAtomic;
    timeElapsed && (*timeElapsed = elapsed);
    return result;
}


NAMESPACE_INITIALIZE(NonBlocking)
{
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, BeginRead)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, BeginReadTimed)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, EndReadAndCheckSuccess)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, BeginWrite)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, BeginWriteTimed)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, EndWriteAndCheckSuccess)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, RevertWriteAndCheckSuccess)

    NAMESPACE_METHOD_INITIALIZE(NonBlocking, Read)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, Write)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, WriteAlwaysEnd)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, WriteAlwaysRevert)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, ReadTimed)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, WriteTimed)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, WriteTimedAlwaysEnd)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, WriteTimedAlwaysRevert)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, WaitForNoReader)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, WaitForNoReaderTimed)
    NAMESPACE_METHOD_INITIALIZE(NonBlocking, RetryTimed)
};
