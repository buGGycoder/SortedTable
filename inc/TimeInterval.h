#ifndef __TIMEINTERVAL_H__
#define __TIMEINTERVAL_H__

#include <stdint.h>
#include "ClassSimulator.h"
#include "Error.h"
#include "SizedString.h"

MEMBER_METHOD_DECLARE(TimeInterval, Day, uint32_t);
MEMBER_METHOD_DECLARE(TimeInterval, Hour, uint8_t);
MEMBER_METHOD_DECLARE(TimeInterval, Minute, uint8_t);
MEMBER_METHOD_DECLARE(TimeInterval, Second, uint8_t);
MEMBER_METHOD_DECLARE(TimeInterval, Microsecond, uint32_t);
MEMBER_METHOD_DECLARE(TimeInterval, TotalMicroseconds, uint64_t);
MEMBER_METHOD_DECLARE(TimeInterval, Initialize, ERROR_CODE, uint32_t day, uint8_t hour, uint8_t minute, uint8_t second, uint32_t microsecond);
MEMBER_METHOD_DECLARE(TimeInterval, InitializeByTotalMicroseconds, ERROR_CODE, uint64_t totalMicroseconds);
MEMBER_METHOD_DECLARE(TimeInterval, ToString, _SizedString*);

CLASS(TimeInterval)
{
    MEMBER_METHOD_DEFINE(TimeInterval, Day);
    MEMBER_METHOD_DEFINE(TimeInterval, Hour);
    MEMBER_METHOD_DEFINE(TimeInterval, Minute);
    MEMBER_METHOD_DEFINE(TimeInterval, Second);
    MEMBER_METHOD_DEFINE(TimeInterval, Microsecond);
    MEMBER_METHOD_DEFINE(TimeInterval, TotalMicroseconds);
    MEMBER_METHOD_DEFINE(TimeInterval, Initialize);
    MEMBER_METHOD_DEFINE(TimeInterval, InitializeByTotalMicroseconds);
    MEMBER_METHOD_DEFINE(TimeInterval, ToString);

    uint32_t day;
    uint8_t hour : 5;
    uint8_t minute : 6;
    uint8_t second : 6;
    uint32_t microsecond;
    uint64_t totalMicroseconds;
};

#endif /* __TIMEINTERVAL_H__ */
