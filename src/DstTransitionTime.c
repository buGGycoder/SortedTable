#include "DstTransitionTime.h"

MEMBER_METHOD_IMPLEMENT(DstTransitionTime, Bias, int16_t)
{
    return thiz->bias;
}

MEMBER_METHOD_IMPLEMENT(DstTransitionTime, GetDateTime, ERROR_CODE, IN uint16_t year, OUT DateTime* dateTime)
{
    ERROR_CODE error = ERROR_UNKNOWN;
    Date* date = I_VAR(Date);
    thiz->date->GetYearlyDate(thiz->date, year, date);
    error = dateTime->InitializeByTotalMicroseconds(dateTime, date->TotalMicroseconds(date) + thiz->time->TotalMicroseconds(thiz->time));
    I_DISPOSE(Date, date);
    return error;
}

MEMBER_METHOD_IMPLEMENT(DstTransitionTime, Initialize, ERROR_CODE, int16_t bias, IN uint8_t month, IN DAY_OF_WEEK dayOfWeek, IN uint8_t dayOrder, IN uint8_t hour, IN uint8_t minute)
{
    ERROR_CODE error = ERROR_UNKNOWN;
    if (ERROR_NO != (error = thiz->date->InitializeBy_Month_DayOfWeek_DayOrder(thiz->date, month, dayOfWeek, dayOrder)))
    {
        return error;
    }

    if (ERROR_NO != (error = thiz->time->Initialize(thiz->time, 0, hour, minute, 0, 0)))
    {
        return error;
    }

    thiz->bias = bias;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(DstTransitionTime, Duplicate, ERROR_CODE, OUT DstTransitionTime* dup)
{
    *dup = *thiz;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(DstTransitionTime, DstTransitionTime, DstTransitionTime*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        I_INIT(Anniversary, thiz->date);
        I_INIT(TimeInterval, thiz->time);
        break;
    case METHOD_DTOR:
        I_DISPOSE(TimeInterval, thiz->time);
        I_DISPOSE(Anniversary, thiz->date);
        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(DstTransitionTime)
{
    MEMBER_METHOD_INITIALIZE(DstTransitionTime, Bias)
    MEMBER_METHOD_INITIALIZE(DstTransitionTime, GetDateTime)
    MEMBER_METHOD_INITIALIZE(DstTransitionTime, Initialize)
    MEMBER_METHOD_INITIALIZE(DstTransitionTime, Duplicate)
};
