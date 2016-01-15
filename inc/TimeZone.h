#ifndef __TIMEZONE_H__
#define __TIMEZONE_H__

#include <stdint.h>
#include <stdbool.h>
#include "ClassSimulator.h"

#include "Error.h"

#include "DstTransitionTime.h"
#include "LocalDateTime.h"

MEMBER_METHOD_DECLARE(TimeZone, Bias, int16_t);
MEMBER_METHOD_DECLARE(TimeZone, DstTransition, DstTransitionTime*);
MEMBER_METHOD_DECLARE(TimeZone, StdTransition, DstTransitionTime*);
MEMBER_METHOD_DECLARE(TimeZone, GetUtcDateTime, ERROR_CODE, IN DateTime* localTime, OUT DateTime* utcTime);
MEMBER_METHOD_DECLARE(TimeZone, GetLocalDateTime, ERROR_CODE, IN DateTime* utcTime, OUT LocalDateTime* localTime);
MEMBER_METHOD_DECLARE(TimeZone, GetLocalDateTimeFromUtcTotalMicroseconds, ERROR_CODE, IN uint64_t utcUseconds, OUT LocalDateTime* localTime);
MEMBER_METHOD_DECLARE(TimeZone, Now, ERROR_CODE, OUT LocalDateTime* localTime);
STATIC_METHOD_DECLARE(TimeZone, NowByUtcTotalMicroseconds, uint64_t);
MEMBER_METHOD_DECLARE(TimeZone, InitializeWithoutDst, ERROR_CODE, IN int16_t bias);
MEMBER_METHOD_DECLARE(TimeZone, InitializeWithDst, ERROR_CODE, IN int16_t bias, IN DstTransitionTime* dstTransitionTime, IN DstTransitionTime* stdTransitionTime);

CLASS(TimeZone)
{
    MEMBER_METHOD_DEFINE(TimeZone, Bias);
    MEMBER_METHOD_DEFINE(TimeZone, DstTransition);
    MEMBER_METHOD_DEFINE(TimeZone, StdTransition);
    MEMBER_METHOD_DEFINE(TimeZone, GetUtcDateTime);
    MEMBER_METHOD_DEFINE(TimeZone, GetLocalDateTime);
    MEMBER_METHOD_DEFINE(TimeZone, GetLocalDateTimeFromUtcTotalMicroseconds);
    MEMBER_METHOD_DEFINE(TimeZone, Now);
    STATIC_METHOD_DEFINE(TimeZone, NowByUtcTotalMicroseconds);
    MEMBER_METHOD_DEFINE(TimeZone, InitializeWithoutDst);
    MEMBER_METHOD_DEFINE(TimeZone, InitializeWithDst);

    int16_t bias;
    bool useDst;
    MEMBER_FIELD_DEFINE(DstTransitionTime, dstTransition);
    MEMBER_FIELD_DEFINE(DstTransitionTime, stdTransition);
};

#endif /* __TIMEZONE_H__ */
