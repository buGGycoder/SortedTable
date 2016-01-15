#include "KeyValue.h"
#include "TimeZone.h"
#include "Crc32.h"

#define KEY_VALUE_FLAG_DELETED (1ULL)

#pragma pack(push)
#pragma pack(1)
PLAIN_OLD_DATA(_KeyValueHeader)
{
    MEMBER_FIELD_DEFINE(NonBlockingStateMachine, stateMachine);
    uint64_t timeStamp;
    uint32_t keyCrc32;
    uint32_t valueCrc32;
    uint64_t deleted : 1;
};

PLAIN_OLD_DATA(_KeyValue)
{
    MEMBER_FIELD_DEFINE(_KeyValueHeader, header);
    MEMBER_FIELD_DEFINE(_SizeString, key);
};
#pragma pack(pop)

NAMESPACE_METHOD_IMPLEMENT(KeyValue, EvaluatePhysicalSize, uint64_t, uint64_t keySize, uint64_t valueSize)
{
    return SizeString.EvaluatePhysicalSize(keySize) + SizeBuffer.EvaluatePhysicalSize(valueSize) + sizeof(_KeyValueHeader);
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, Initialize, _KeyValue*, void* buffer, uint64_t bufferSize, const char* key, uint64_t keySize, const char* value, uint64_t valueSize, NonBlockingState state, uint64_t timeStamp, void** end)
{
    if (sizeof(_KeyValueHeader) >= bufferSize)
    {
        return NULL;
    }

    _KeyValue* kv = (_KeyValue*)buffer;
    memset(kv->header, 0, sizeof(kv->header));
    kv->header->stateMachine->state = NON_BLOCKING_STATE_CORRUPTED_MASK;
    kv->header->timeStamp = timeStamp ? timeStamp : NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    buffer = POINTER_OFFSET(buffer, sizeof(_KeyValueHeader));
    bufferSize -= sizeof(_KeyValueHeader);

    uint64_t physicalSize = 0;
    _SizeString* k = NULL;
    if (!(k = SizeString.Initialize(buffer, bufferSize, key, keySize, &physicalSize)))
    {
        kv->header->stateMachine->state &= ~NON_BLOCKING_STATE_CORRUPTED_MASK;
        return NULL;
    }

    buffer = POINTER_OFFSET(buffer, physicalSize);
    bufferSize -= physicalSize;
    _SizeBuffer* v = NULL;
    if (!(v = SizeBuffer.InitializeByString(buffer, bufferSize, value, valueSize, NULL)))
    {
        kv->header->stateMachine->state &= ~NON_BLOCKING_STATE_CORRUPTED_MASK;
        return NULL;
    }

    keySize = SizeString.Size(k);
    kv->header->keyCrc32 = Crc32.Finish(Crc32.Accumulate(0, key, keySize), keySize);
    valueSize = SizeBuffer.Size(v);
    kv->header->valueCrc32 = Crc32.Finish(Crc32.Accumulate(0, value, valueSize), valueSize);

    kv->header->stateMachine->state = state;
    end && (*end = SizeBuffer.End(v));
    return kv;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, TryFixStateUnsafe, bool, _KeyValue* kv)
{
    if (!(NON_BLOCKING_STATE_CORRUPTED_MASK & kv->header->stateMachine->state))
    {
        return true;
    }

    if (NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, CheckKeyUnsafe, kv)
            && NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, CheckValueUnsafe, kv))
    {
        ++kv->header->stateMachine->state;
        return true;
    }

    return false;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, StateMachine, NonBlockingStateMachine*, _KeyValue* kv)
{
    return kv->header->stateMachine;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, TimeStamp, uint64_t*, _KeyValue* kv)
{
    return &kv->header->timeStamp;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, IsDeleted, bool, _KeyValue* kv)
{
    return kv->header->deleted;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, VersionStable, uint64_t, _KeyValue* kv)
{
    NonBlockingReadState state = NonBlocking.BeginRead(kv);
    NonBlocking.EndReadAndCheckSuccess(kv, state);
    return state;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, VersionUnsafe, uint64_t, _KeyValue* kv)
{
    return kv->header->stateMachine->state;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, SetVersionUnsafe, uint64_t, _KeyValue* kv, uint64_t version)
{
    return __sync_lock_test_and_set(&kv->header->stateMachine->state, version);
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, CheckKeyUnsafe, bool, _KeyValue* kv)
{
    _SizeString* key = (_SizeString*)POINTER_OFFSET(kv, sizeof(_KeyValueHeader));
    uint64_t keySize = SizeString.Size(key);
    uint32_t keyCrc32 = Crc32.Finish(Crc32.Accumulate(0, SizeString.Data(key), keySize), keySize);
    return keyCrc32 == kv->header->keyCrc32;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, CheckValueUnsafe, bool, _KeyValue* kv)
{
    _SizeBuffer* value = (_SizeBuffer*)SizeString.End((_SizeString*)POINTER_OFFSET(kv, sizeof(_KeyValueHeader)));
    uint64_t valueSize = SizeBuffer.Size(value);
    uint32_t valueCrc32 = Crc32.Finish(Crc32.Accumulate(0, SizeBuffer.Data(value), valueSize), valueSize);
    return valueCrc32 == kv->header->valueCrc32;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, KeyUnsafe, _SizeString*, _KeyValue* kv)
{
    return (_SizeString*)POINTER_OFFSET(kv, sizeof(_KeyValueHeader));
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, ValueUnsafe, _SizeBuffer*, _KeyValue* kv)
{
    return (_SizeBuffer*)SizeString.End((_SizeString*)POINTER_OFFSET(kv, sizeof(_KeyValueHeader)));
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, PhysicalSize, uint64_t, _KeyValue* kv)
{
    _SizeString* key = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, KeyUnsafe, kv);
    _SizeBuffer* value = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, ValueUnsafe, kv);
    return POINTER_DISTANCE(SizeString.End(key), SizeString.Begin(key))
            + POINTER_DISTANCE(SizeBuffer.End(value), SizeBuffer.Begin(value))
            + sizeof(_KeyValueHeader);
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, SetValueUnsafe, bool, _KeyValue* kv, const char* data, uint64_t dataSize)
{
    if (!data)
    {
        return false;
    }

    if (!dataSize)
    {
        dataSize = strlen(data);
    }

    _SizeBuffer* sizeBuffer = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, ValueUnsafe, kv);
    if (SizeBuffer.Capacity(sizeBuffer) < dataSize)
    {
        return false;
    }

    I_MEMMOVE(SizeBuffer.Data(sizeBuffer), data, dataSize);
    SizeBuffer.SetSize(sizeBuffer, dataSize);
    kv->header->valueCrc32 = Crc32.Finish(Crc32.Accumulate(0, data, dataSize), dataSize);
    return true;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, DeleteUnsafe, bool, _KeyValue* kv)
{
    bool deleted = kv->header->deleted;
    kv->header->deleted = true;
    return !deleted;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, GetValue, bool, _KeyValue* kv, bool* needDeleted, uint64_t* needVersion, uint64_t* needTimeStamp, _SizedString** needKey, _SizedString** needValue)
{
    bool result = true;
    bool hasDeleted = false;
    _SizedString* k = NULL;
    _SizedString* v = NULL;
    _SizeString* key  = NULL;
    _SizeBuffer* value = NULL;
    uint64_t timeStamp = 0;
    NonBlockingReadState state = 0;
    do
    {
        if (v)
        {
            SizedString.Delete(v);
            v = NULL;
        }

        state = NonBlocking.BeginRead(kv->header->stateMachine);
        timeStamp = kv->header->timeStamp;
        if (needKey && !k)
        {
            key = KeyValue.KeyUnsafe(kv);
            k = SizedString.New(SizeString.Data(key), SizeString.Size(key));
            result &= !!k;
        }

        if (hasDeleted = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, IsDeleted, kv))
        {
            continue;
        }

        if (needKey)
        {
            value = KeyValue.ValueUnsafe(kv);
            v = SizedString.New(SizeBuffer.Data(value), SizeBuffer.Size(value));
            result &= !!v;
        }
    }
    while(!NonBlocking.EndReadAndCheckSuccess(kv->header->stateMachine, state));
    needDeleted && (*needDeleted = hasDeleted);
    needVersion && (*needVersion = state);
    needTimeStamp && (*needTimeStamp = timeStamp);
    needKey && (*needKey = k);
    needValue && (*needValue = v);
    return result;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, SetValue, bool, _KeyValue* kv, const char* data, uint64_t dataSize, uint64_t* versionUpdated)
{
    bool result = false;
    bool deleted = false;
    NonBlockingWriteState state = 0;
    uint64_t enterTime = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    do
    {
        state = NonBlocking.BeginWrite(kv->header->stateMachine);
        if (deleted = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, IsDeleted, kv))
        {
            continue;
        }

        if (result = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, SetValueUnsafe, kv, data, dataSize))
        {
            kv->header->timeStamp = enterTime;
        }
    }
    while(!((deleted || !result) ? NonBlocking.RevertWriteAndCheckSuccess(kv->header->stateMachine, state) : NonBlocking.EndWriteAndCheckSuccess(kv->header->stateMachine, state)));
    versionUpdated && (*versionUpdated = state + ((deleted || !result) ? -1 : 1));
    return result;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, Delete, bool, _KeyValue* kv, uint64_t* versionUpdated)
{
    bool hasDeleted = false;
    bool deleteSucceeded = false;
    NonBlockingWriteState state = 0;
    uint64_t enterTime = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    do
    {
        state = NonBlocking.BeginWrite(kv->header->stateMachine);
        if (hasDeleted = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, IsDeleted, kv))
        {
            continue;
        }

        if (deleteSucceeded = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, DeleteUnsafe, kv))
        {
            kv->header->timeStamp = enterTime;
        }
    }
    while(!(deleteSucceeded ? NonBlocking.EndWriteAndCheckSuccess(kv->header->stateMachine, state) : NonBlocking.RevertWriteAndCheckSuccess(kv->header->stateMachine, state)));
    versionUpdated && (*versionUpdated = state + (deleteSucceeded ? 1 : -1));
    return deleteSucceeded;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, GetValueTimed, bool, _KeyValue* kv, uint64_t microsecondsTimeout, bool* needDeleted, uint64_t* needVersion, uint64_t* needTimeStamp, _SizedString** needKey, _SizedString** needValue)
{
    if(!(needDeleted || needVersion || needTimeStamp || needKey || needValue))
    {
        return false;
    }

    bool result = false;
    bool hasDeleted = false;
    _SizedString* k = NULL;
    _SizedString* v = NULL;
    _SizeString* key  = NULL;
    _SizeBuffer* value = NULL;
    uint64_t timeStamp = 0;
    NonBlockingReadState state = 0;
    uint64_t enterTime = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    uint64_t now = enterTime;
    while(microsecondsTimeout >= now - enterTime)
    {
        now = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
        if(!NonBlocking.BeginReadTimed(kv->header->stateMachine, microsecondsTimeout, &state))
        {
            continue;
        }

        timeStamp = kv->header->timeStamp;
        if (needKey && !k)
        {
            key = KeyValue.KeyUnsafe(kv);
            k = SizedString.New(SizeString.Data(key), SizeString.Size(key));
        }

        if (!(hasDeleted = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, IsDeleted, kv)))
        {
            if (needValue)
            {
                value = KeyValue.ValueUnsafe(kv);
                v = SizedString.New(SizeBuffer.Data(value), SizeBuffer.Size(value));
            }
        }

        if ((result = NonBlocking.EndReadAndCheckSuccess(kv->header->stateMachine, state)))
        {
            break;
        }

        if (v)
        {
            SizedString.Delete(v);
            v = NULL;
        }
    }

    needDeleted && (*needDeleted = hasDeleted);
    if (result)
    {
        needVersion && (*needVersion = state);
        needTimeStamp && (*needTimeStamp = timeStamp);
        needKey && (*needKey = k);
        needValue && (*needValue = v);
    }
    else
    {
        if (k)
        {
            SizedString.Delete(k);
            k = NULL;
        }

        if (v)
        {
            SizedString.Delete(v);
            v = NULL;
        }
    }

    return result;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, SetValueTimed, bool, _KeyValue* kv, uint64_t microsecondsTimeout, const char* data, uint64_t dataSize, uint64_t* versionUpdated)
{
    bool result = false;
    bool deleted = false;
    NonBlockingWriteState state = 0;
    uint64_t enterTime = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    uint64_t now = enterTime;
    while(microsecondsTimeout >= now - enterTime)
    {
        now = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
        if(!NonBlocking.BeginWriteTimed(kv->header->stateMachine, microsecondsTimeout, &state))
        {
            continue;
        }

        if (!(deleted = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, IsDeleted, kv)))
        {
            if (result = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, SetValueUnsafe, kv, data, dataSize))
            {
                kv->header->timeStamp = enterTime;
            }
        }

        if ((deleted || !result) ? NonBlocking.RevertWriteAndCheckSuccess(kv->header->stateMachine, state) : NonBlocking.EndWriteAndCheckSuccess(kv->header->stateMachine, state))
        {
            break;
        }

        result = false;
    }

    versionUpdated && (*versionUpdated = state + ((deleted || !result) ? -1 : 1));
    return result;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, DeleteTimed, bool, _KeyValue* kv, uint64_t microsecondsTimeout, uint64_t* versionUpdated)
{
    bool hasDeleted = false;
    bool deleteSucceeded = false;
    NonBlockingWriteState state = 0;
    uint64_t enterTime = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    uint64_t now = enterTime;
    while(microsecondsTimeout >= now - enterTime)
    {
        now = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
        if(!NonBlocking.BeginWriteTimed(kv->header->stateMachine, microsecondsTimeout, &state))
        {
            continue;
        }

        if (!(hasDeleted = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, IsDeleted, kv)))
        {
            if (deleteSucceeded = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, DeleteUnsafe, kv))
            {
                kv->header->timeStamp = enterTime;
            }
        }

        if (deleteSucceeded ? NonBlocking.EndWriteAndCheckSuccess(kv->header->stateMachine, state) : NonBlocking.RevertWriteAndCheckSuccess(kv->header->stateMachine, state))
        {
            break;
        }
    }

    versionUpdated && (*versionUpdated = state + (deleteSucceeded ? 1 : -1));
    return deleteSucceeded;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, Begin, void*, _KeyValue* kv)
{
    return kv;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, End, void*, _KeyValue* kv)
{
    return SizeBuffer.End(NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, ValueUnsafe, kv));
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, CloneTimed, _KeyValue*, _KeyValue* kv, uint64_t microsecondsTimeout)
{
    void* buffer = NULL;
    bool result = false;
    NonBlockingReadState state = 0;
    uint64_t enterTime = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
    uint64_t now = enterTime;
    while(microsecondsTimeout >= now - enterTime)
    {
        now = NONINSTANTIAL_NAMESPACE_METHOD_CALL(TimeZone, NowByUtcTotalMicroseconds);
        if(!NonBlocking.BeginReadTimed(kv->header->stateMachine, microsecondsTimeout, &state))
        {
            continue;
        }

        uint64_t keySize = SizeString.Size(NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, KeyUnsafe, kv));
        _SizeBuffer* value = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, ValueUnsafe, kv);
        uint64_t valueSize = SizeBuffer.Size(value);
        uint64_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, EvaluatePhysicalSize, keySize, valueSize);
        uint64_t bufferSize = size;
        void* buffer = I_MALLOC(bufferSize);
        if (buffer)
        {
            void* tgt = buffer;
            void* src = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, Begin, kv);
            I_MEMMOVE(tgt, src, sizeof(_KeyValueHeader));

            size -= sizeof(_KeyValueHeader);
            tgt = POINTER_OFFSET(tgt, sizeof(_KeyValueHeader));
            src = POINTER_OFFSET(src, sizeof(_KeyValueHeader));

            void* end = NULL;
            _SizeString* k = SizeString.CopyTo((_SizeString*)src, tgt, size, &end);
            ASSERT(k, "DEV BUG!!! Cannot copy key for KeyValue.Clone");

            size -= POINTER_DISTANCE(end, tgt);
            tgt = end;
            src = value;

            _SizeBuffer* v = SizeBuffer.InitializeByString(tgt, size, SizeBuffer.Data(src), SizeBuffer.Size(src), NULL);
            ASSERT(v, "DEV BUG!!! Cannot copy value for KeyValue.Clone");
        }
        else
        {
            /* not enough memory */
            NonBlocking.EndReadAndCheckSuccess(kv->header->stateMachine, state);
            break;
        }

        if (NonBlocking.EndReadAndCheckSuccess(kv->header->stateMachine, state))
        {
            break;
        }
        else
        {
            I_FREE(buffer);
            buffer = NULL;
        }
    }

    return (_KeyValue*)buffer;
}

NAMESPACE_METHOD_IMPLEMENT(KeyValue, CopyUnsafe, uint64_t, _KeyValue* kv, void* buffer, uint64_t bufferSize, void** end)
{
    uint64_t keySize = SizeString.Size(NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, KeyUnsafe, kv));
    _SizeBuffer* value = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, ValueUnsafe, kv);
    uint64_t valueSize = SizeBuffer.Size(value);
    uint64_t size = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, EvaluatePhysicalSize, keySize, valueSize);
    if (size > bufferSize)
    {
        return 0;
    }

    bufferSize = size;
    void* tgt = buffer;
    void* src = NONINSTANTIAL_NAMESPACE_METHOD_CALL(KeyValue, Begin, kv);
    I_MEMMOVE(tgt, src, sizeof(_KeyValueHeader));

    size -= sizeof(_KeyValueHeader);
    tgt = POINTER_OFFSET(tgt, sizeof(_KeyValueHeader));
    src = POINTER_OFFSET(src, sizeof(_KeyValueHeader));

    void* kEnd = NULL;
    _SizeString* k = SizeString.CopyTo((_SizeString*)src, tgt, size, &kEnd);
    ASSERT(k, "DEV BUG!!! Cannot copy key for KeyValue.Clone");

    size -= POINTER_DISTANCE(kEnd, tgt);
    tgt = kEnd;
    src = value;

    _SizeBuffer* v = SizeBuffer.InitializeByString(tgt, size, SizeBuffer.Data(src), SizeBuffer.Size(src), NULL);
    ASSERT(v, "DEV BUG!!! Cannot copy value for KeyValue.Clone");

    end && (*end = POINTER_OFFSET(buffer, bufferSize));
    return size;
}

NAMESPACE_INITIALIZE(KeyValue)
{
    NAMESPACE_METHOD_INITIALIZE(KeyValue, EvaluatePhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, Initialize)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, TryFixStateUnsafe)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, StateMachine)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, TimeStamp)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, IsDeleted)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, VersionStable)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, VersionUnsafe)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, SetVersionUnsafe)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, CheckKeyUnsafe)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, CheckValueUnsafe)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, KeyUnsafe)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, ValueUnsafe)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, PhysicalSize)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, SetValueUnsafe)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, DeleteUnsafe)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, GetValue)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, SetValue)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, Delete)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, GetValueTimed)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, SetValueTimed)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, DeleteTimed)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, Begin)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, End)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, CloneTimed)
    NAMESPACE_METHOD_INITIALIZE(KeyValue, CopyUnsafe)
};
