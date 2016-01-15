#ifndef __DATETIME_H__
#define __DATETIME_H__

#include <stdint.h>
#include "ClassSimulator.h"

#include "Date.h"
#include "TimeInterval.h"
#include "SizedString.h"

MEMBER_METHOD_DECLARE(DateTime, Year, uint16_t);
MEMBER_METHOD_DECLARE(DateTime, Month, uint8_t);
MEMBER_METHOD_DECLARE(DateTime, Day, uint8_t);
MEMBER_METHOD_DECLARE(DateTime, DayOfWeek, DAY_OF_WEEK);
MEMBER_METHOD_DECLARE(DateTime, DayOfYear, uint16_t);
MEMBER_METHOD_DECLARE(DateTime, Hour, uint8_t);
MEMBER_METHOD_DECLARE(DateTime, Minute, uint8_t);
MEMBER_METHOD_DECLARE(DateTime, Second, uint8_t);
MEMBER_METHOD_DECLARE(DateTime, Microsecond, uint32_t);
MEMBER_METHOD_DECLARE(DateTime, TotalMicroseconds, uint64_t);
MEMBER_METHOD_DECLARE(DateTime, Initialize, ERROR_CODE, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint32_t microsecond);
MEMBER_METHOD_DECLARE(DateTime, InitializeByTotalMicroseconds, ERROR_CODE, uint64_t totalMicroseconds);
MEMBER_METHOD_DECLARE(DateTime, ToString, _SizedString*);

CLASS(DateTime)
{
    MEMBER_METHOD_DEFINE(DateTime, Year);
    MEMBER_METHOD_DEFINE(DateTime, Month);
    MEMBER_METHOD_DEFINE(DateTime, Day);
    MEMBER_METHOD_DEFINE(DateTime, DayOfWeek);
    MEMBER_METHOD_DEFINE(DateTime, DayOfYear);
    MEMBER_METHOD_DEFINE(DateTime, Hour);
    MEMBER_METHOD_DEFINE(DateTime, Minute);
    MEMBER_METHOD_DEFINE(DateTime, Second);
    MEMBER_METHOD_DEFINE(DateTime, Microsecond);
    MEMBER_METHOD_DEFINE(DateTime, TotalMicroseconds);
    MEMBER_METHOD_DEFINE(DateTime, Initialize);
    MEMBER_METHOD_DEFINE(DateTime, InitializeByTotalMicroseconds);
    MEMBER_METHOD_DEFINE(DateTime, ToString);

    MEMBER_FIELD_DEFINE(Date, date);
    MEMBER_FIELD_DEFINE(TimeInterval, time);
    uint64_t totalMicroseconds;
};

#endif /* __DATETIME_H__ */
