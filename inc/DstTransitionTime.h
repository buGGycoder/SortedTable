#ifndef __DSTTRANSITIONTIME_H__
#define __DSTTRANSITIONTIME_H__

#include <stdint.h>
#include "ClassSimulator.h"

#include "Error.h"

#include "CalendarConstants.h"
#include "Anniversary.h"
#include "TimeInterval.h"
#include "DateTime.h"

MEMBER_METHOD_DECLARE(DstTransitionTime, Bias, int16_t);
MEMBER_METHOD_DECLARE(DstTransitionTime, GetDateTime, ERROR_CODE, IN uint16_t year, OUT DateTime* dateTime);
MEMBER_METHOD_DECLARE(DstTransitionTime, Initialize, ERROR_CODE, int16_t bias, IN uint8_t month, IN DAY_OF_WEEK dayOfWeek, IN uint8_t dayOrder, IN uint8_t hour, IN uint8_t minute);
MEMBER_METHOD_DECLARE(DstTransitionTime, Duplicate, ERROR_CODE, OUT DstTransitionTime* dup);

CLASS(DstTransitionTime)
{
    MEMBER_METHOD_DEFINE(DstTransitionTime, Bias);
    MEMBER_METHOD_DEFINE(DstTransitionTime, GetDateTime);
    MEMBER_METHOD_DEFINE(DstTransitionTime, Initialize);
    MEMBER_METHOD_DEFINE(DstTransitionTime, Duplicate);

    int16_t bias;
    MEMBER_FIELD_DEFINE(Anniversary, date);
    MEMBER_FIELD_DEFINE(TimeInterval, time);
};

#endif /* __DSTTRANSITIONTIME_H__ */
