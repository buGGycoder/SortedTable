#ifndef __DATE_H__
#define __DATE_H__

#include <stdint.h>
#include "ClassSimulator.h"

#include "Error.h"
#include "CalendarConstants.h"


MEMBER_METHOD_DECLARE(Date, Year, uint16_t);
MEMBER_METHOD_DECLARE(Date, Month, uint8_t);
MEMBER_METHOD_DECLARE(Date, Day, uint8_t);
MEMBER_METHOD_DECLARE(Date, DayOfWeek, DAY_OF_WEEK);
MEMBER_METHOD_DECLARE(Date, DayOfYear, uint16_t);
MEMBER_METHOD_DECLARE(Date, TotalMicroseconds, uint64_t);
MEMBER_METHOD_DECLARE(Date, Initialize, ERROR_CODE, uint16_t year, uint8_t month, uint8_t day);
MEMBER_METHOD_DECLARE(Date, InitializeByTotalMicroseconds, ERROR_CODE, uint64_t totalMicorseconds);

CLASS(Date)
{
    MEMBER_METHOD_DEFINE(Date, Year);
    MEMBER_METHOD_DEFINE(Date, Month);
    MEMBER_METHOD_DEFINE(Date, Day);
    MEMBER_METHOD_DEFINE(Date, DayOfWeek);
    MEMBER_METHOD_DEFINE(Date, DayOfYear);
    MEMBER_METHOD_DEFINE(Date, TotalMicroseconds);
    MEMBER_METHOD_DEFINE(Date, Initialize);
    MEMBER_METHOD_DEFINE(Date, InitializeByTotalMicroseconds);

    uint16_t year;
    uint8_t month : 4;
    uint8_t day : 5;
    uint8_t dayOfWeek : 3;
    uint16_t dayOfYear : 9;
    uint64_t totalMicroseconds;
};

#endif /* __DATE_H__ */
