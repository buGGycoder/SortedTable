#ifndef __LOCALDATETIME_H__
#define __LOCALDATETIME_H__

#include <stdint.h>
#include "ClassSimulator.h"

#include "DateTime.h"

MEMBER_METHOD_DECLARE(LocalDateTime, Bias, int16_t);
MEMBER_METHOD_DECLARE(LocalDateTime, LiteralDateTime, DateTime*);
MEMBER_METHOD_DECLARE(LocalDateTime, UtcTotalMicroseconds, uint64_t);
MEMBER_METHOD_DECLARE(LocalDateTime, Initialize, ERROR_CODE, int16_t bias, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint32_t microsecond);
MEMBER_METHOD_DECLARE(LocalDateTime, InitializeByUtcTotalMicroseconds, ERROR_CODE, int16_t bias, uint64_t utcTotalMicroseconds);

CLASS(LocalDateTime)
{
    MEMBER_METHOD_DEFINE(LocalDateTime, Bias);
    MEMBER_METHOD_DEFINE(LocalDateTime, LiteralDateTime);
    MEMBER_METHOD_DEFINE(LocalDateTime, UtcTotalMicroseconds);
    MEMBER_METHOD_DEFINE(LocalDateTime, Initialize);
    MEMBER_METHOD_DEFINE(LocalDateTime, InitializeByUtcTotalMicroseconds);

    int16_t bias;
    MEMBER_FIELD_DEFINE(DateTime, dateTime);
    uint64_t utcTotalMicroseconds;
};

#endif /* __LOCALDATETIME_H__ */
