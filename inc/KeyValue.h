#ifndef __KEYVALUE_H__
#define __KEYVALUE_H__

#include <stdbool.h>
#include <stdint.h>
#include "ClassSimulator.h"

#include "NonBlocking.h"
#include "SizeBuffer.h"
#include "SizedString.h"
#include "SizeString.h"

CLASS_DECLARE(_KeyValue);

NAMESPACE_METHOD_DECLARE(KeyValue, EvaluatePhysicalSize, uint64_t, uint64_t keySize, uint64_t valueSize);
NAMESPACE_METHOD_DECLARE(KeyValue, Initialize, _KeyValue*, void* buffer, uint64_t bufferSize, const char* key, uint64_t keySize, const char* value, uint64_t valueSize, NonBlockingState state, uint64_t microsecondsTimeStamp, void** end);
NAMESPACE_METHOD_DECLARE(KeyValue, TryFixStateUnsafe, bool, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, StateMachine, NonBlockingStateMachine*, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, TimeStamp, uint64_t*, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, IsDeleted, bool, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, VersionStable, uint64_t, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, VersionUnsafe, uint64_t, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, SetVersionUnsafe, uint64_t, _KeyValue* kv, uint64_t version);
NAMESPACE_METHOD_DECLARE(KeyValue, CheckKeyUnsafe, bool, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, CheckValueUnsafe, bool, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, KeyUnsafe, _SizeString*, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, ValueUnsafe, _SizeBuffer*, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, PhysicalSize, uint64_t, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, SetValueUnsafe, bool, _KeyValue* kv, const char* data, uint64_t dataSize);
NAMESPACE_METHOD_DECLARE(KeyValue, DeleteUnsafe, bool, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, GetValue, bool, _KeyValue* kv, bool* needDeleted, uint64_t* needVersion, uint64_t* needTimeStamp, _SizedString** needKey, _SizedString** needValue);
NAMESPACE_METHOD_DECLARE(KeyValue, SetValue, bool, _KeyValue* kv, const char* data, uint64_t dataSize, uint64_t* versionUpdated);
NAMESPACE_METHOD_DECLARE(KeyValue, Delete, bool, _KeyValue* kv, uint64_t* versionUpdated);
NAMESPACE_METHOD_DECLARE(KeyValue, GetValueTimed, bool, _KeyValue* kv, uint64_t microsecondsTimeout, bool* needDeleted, uint64_t* needVersion, uint64_t* needTimeStamp, _SizedString** needKey, _SizedString** needValue);
NAMESPACE_METHOD_DECLARE(KeyValue, SetValueTimed, bool, _KeyValue* kv, uint64_t microsecondsTimeout, const char* data, uint64_t dataSize, uint64_t* versionUpdated);
NAMESPACE_METHOD_DECLARE(KeyValue, DeleteTimed, bool, _KeyValue* kv, uint64_t microsecondsTimeout, uint64_t* versionUpdated);
NAMESPACE_METHOD_DECLARE(KeyValue, Begin, void*, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, End, void*, _KeyValue* kv);
NAMESPACE_METHOD_DECLARE(KeyValue, CloneTimed, _KeyValue*, _KeyValue* kv, uint64_t microsecondsTimeout);
NAMESPACE_METHOD_DECLARE(KeyValue, CopyUnsafe, uint64_t, _KeyValue* kv, void* buffer, uint64_t bufferSize, void** end);

NAMESPACE(KeyValue)
{
    NAMESPACE_METHOD_DEFINE(KeyValue, EvaluatePhysicalSize);
    NAMESPACE_METHOD_DEFINE(KeyValue, Initialize);
    NAMESPACE_METHOD_DEFINE(KeyValue, TryFixStateUnsafe);
    NAMESPACE_METHOD_DEFINE(KeyValue, StateMachine);
    NAMESPACE_METHOD_DEFINE(KeyValue, TimeStamp);
    NAMESPACE_METHOD_DEFINE(KeyValue, IsDeleted);
    NAMESPACE_METHOD_DEFINE(KeyValue, VersionStable);
    NAMESPACE_METHOD_DEFINE(KeyValue, VersionUnsafe);
    NAMESPACE_METHOD_DEFINE(KeyValue, SetVersionUnsafe);
    NAMESPACE_METHOD_DEFINE(KeyValue, CheckKeyUnsafe);
    NAMESPACE_METHOD_DEFINE(KeyValue, CheckValueUnsafe);
    NAMESPACE_METHOD_DEFINE(KeyValue, KeyUnsafe);
    NAMESPACE_METHOD_DEFINE(KeyValue, ValueUnsafe);
    NAMESPACE_METHOD_DEFINE(KeyValue, PhysicalSize);
    NAMESPACE_METHOD_DEFINE(KeyValue, SetValueUnsafe);
    NAMESPACE_METHOD_DEFINE(KeyValue, DeleteUnsafe);
    NAMESPACE_METHOD_DEFINE(KeyValue, GetValue);
    NAMESPACE_METHOD_DEFINE(KeyValue, SetValue);
    NAMESPACE_METHOD_DEFINE(KeyValue, Delete);
    NAMESPACE_METHOD_DEFINE(KeyValue, GetValueTimed);
    NAMESPACE_METHOD_DEFINE(KeyValue, SetValueTimed);
    NAMESPACE_METHOD_DEFINE(KeyValue, DeleteTimed);
    NAMESPACE_METHOD_DEFINE(KeyValue, Begin);
    NAMESPACE_METHOD_DEFINE(KeyValue, End);
    NAMESPACE_METHOD_DEFINE(KeyValue, CloneTimed);
    NAMESPACE_METHOD_DEFINE(KeyValue, CopyUnsafe);
};

#endif /* __KEYVALUE_H__ */
