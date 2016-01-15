#ifndef __NONBLOCKING_H__
#define __NONBLOCKING_H__

#include <stdbool.h>
#include <stdint.h>
#include "ClassSimulator.h"

typedef uint64_t NonBlockingState;
typedef NonBlockingState NonBlockingReadState;
typedef NonBlockingState NonBlockingWriteState;

#define NON_BLOCKING_STATE_CORRUPTED_MASK (1ULL)
#define NON_BLOCKING_STATE_NEXT_STABLE(state) ((NON_BLOCKING_STATE_CORRUPTED_MASK | (uint64_t)(state)) + 1)
#define NON_BLOCKING_STATE_NEXT_STABLE_X2(state) ((NON_BLOCKING_STATE_CORRUPTED_MASK | (uint64_t)(state)) + 3)
#define NON_BLOCKING_STATE_LAST_STABLE(state) ((NON_BLOCKING_STATE_CORRUPTED_MASK | (uint64_t)(state)) - 1)
#define NON_BLOCKING_STATE_LAST_STABLE_X2(state) ((NON_BLOCKING_STATE_CORRUPTED_MASK | (uint64_t)(state)) - 3)

#pragma pack(push)
#pragma pack(1)
PLAIN_OLD_DATA(NonBlockingStateMachine)
{
    volatile NonBlockingState state;
    volatile uint64_t readerCounter;
};
#pragma pack(pop)

typedef bool TYPE_TRY_ONCE(NonBlockingState enterState, NonBlockingState* currentState, void* context);

NAMESPACE_METHOD_DECLARE(NonBlocking, BeginRead, NonBlockingReadState, NonBlockingStateMachine* pStateMachine);
NAMESPACE_METHOD_DECLARE(NonBlocking, BeginReadTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, OUT NonBlockingReadState* state);
NAMESPACE_METHOD_DECLARE(NonBlocking, EndReadAndCheckSuccess, bool, NonBlockingStateMachine* pStateMachine, NonBlockingReadState state);
NAMESPACE_METHOD_DECLARE(NonBlocking, BeginWrite, NonBlockingWriteState, NonBlockingStateMachine* pStateMachine);
NAMESPACE_METHOD_DECLARE(NonBlocking, BeginWriteTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, OUT NonBlockingWriteState* state);
NAMESPACE_METHOD_DECLARE(NonBlocking, EndWriteAndCheckSuccess, bool, NonBlockingStateMachine* pStateMachine, NonBlockingWriteState state);
NAMESPACE_METHOD_DECLARE(NonBlocking, RevertWriteAndCheckSuccess, bool, NonBlockingStateMachine* pStateMachine, NonBlockingWriteState state);

NAMESPACE_METHOD_DECLARE(NonBlocking, Read, bool, NonBlockingStateMachine* pStateMachine, TYPE_TRY_ONCE* tryOnce, void* context,  NonBlockingReadState* readState);
NAMESPACE_METHOD_DECLARE(NonBlocking, Write, bool, NonBlockingStateMachine* pStateMachine, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState);
NAMESPACE_METHOD_DECLARE(NonBlocking, WriteAlwaysEnd, bool, NonBlockingStateMachine* pStateMachine, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState);
NAMESPACE_METHOD_DECLARE(NonBlocking, WriteAlwaysRevert, bool, NonBlockingStateMachine* pStateMachine, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState);
NAMESPACE_METHOD_DECLARE(NonBlocking, ReadTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingReadState* readState, uint64_t* timeElapsed);
NAMESPACE_METHOD_DECLARE(NonBlocking, WriteTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState, uint64_t* timeElapsed);
NAMESPACE_METHOD_DECLARE(NonBlocking, WriteTimedAlwaysEnd, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState, uint64_t* timeElapsed);
NAMESPACE_METHOD_DECLARE(NonBlocking, WriteTimedAlwaysRevert, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, NonBlockingWriteState* writeState, uint64_t* timeElapsed);
NAMESPACE_METHOD_DECLARE(NonBlocking, WaitForNoReader, void, NonBlockingStateMachine* pStateMachine);
NAMESPACE_METHOD_DECLARE(NonBlocking, WaitForNoReaderTimed, bool, NonBlockingStateMachine* pStateMachine, uint64_t microsecondsTimeout, uint64_t* timeElapsed);
NAMESPACE_METHOD_DECLARE(NonBlocking, RetryTimed, bool, uint64_t microsecondsTimeout, TYPE_TRY_ONCE* tryOnce, void* context, uint64_t* timeElapsed);


NAMESPACE(NonBlocking)
{
    NAMESPACE_METHOD_DEFINE(NonBlocking, BeginRead);
    NAMESPACE_METHOD_DEFINE(NonBlocking, BeginReadTimed);
    NAMESPACE_METHOD_DEFINE(NonBlocking, EndReadAndCheckSuccess);
    NAMESPACE_METHOD_DEFINE(NonBlocking, BeginWrite);
    NAMESPACE_METHOD_DEFINE(NonBlocking, BeginWriteTimed);
    NAMESPACE_METHOD_DEFINE(NonBlocking, EndWriteAndCheckSuccess);
    NAMESPACE_METHOD_DEFINE(NonBlocking, RevertWriteAndCheckSuccess);

    NAMESPACE_METHOD_DEFINE(NonBlocking, Read);
    NAMESPACE_METHOD_DEFINE(NonBlocking, Write);
    NAMESPACE_METHOD_DEFINE(NonBlocking, WriteAlwaysEnd);
    NAMESPACE_METHOD_DEFINE(NonBlocking, WriteAlwaysRevert);
    NAMESPACE_METHOD_DEFINE(NonBlocking, ReadTimed);
    NAMESPACE_METHOD_DEFINE(NonBlocking, WriteTimed);
    NAMESPACE_METHOD_DEFINE(NonBlocking, WriteTimedAlwaysEnd);
    NAMESPACE_METHOD_DEFINE(NonBlocking, WriteTimedAlwaysRevert);
    NAMESPACE_METHOD_DEFINE(NonBlocking, WaitForNoReader);
    NAMESPACE_METHOD_DEFINE(NonBlocking, WaitForNoReaderTimed);
    NAMESPACE_METHOD_DEFINE(NonBlocking, RetryTimed);
};

#endif /* __NONBLOCKING_H__ */
