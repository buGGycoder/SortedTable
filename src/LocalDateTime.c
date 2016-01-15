#include "LocalDateTime.h"

MEMBER_METHOD_IMPLEMENT(LocalDateTime, Bias, int16_t)
{
    return thiz->bias;
}

MEMBER_METHOD_IMPLEMENT(LocalDateTime, LiteralDateTime, DateTime*)
{
    return thiz->dateTime;
}

MEMBER_METHOD_IMPLEMENT(LocalDateTime, UtcTotalMicroseconds, uint64_t)
{
    return thiz->utcTotalMicroseconds;
}

MEMBER_METHOD_IMPLEMENT(LocalDateTime, Initialize, ERROR_CODE, int16_t bias, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint32_t microsecond)
{
    ERROR_CODE error = ERROR_UNKNOWN;
    if (ERROR_NO != (error = thiz->dateTime->Initialize(thiz->dateTime, year, month, day, hour, minute, second, microsecond)))
    {
        return error;
    }

    thiz->bias = bias;
    thiz->utcTotalMicroseconds = thiz->dateTime->TotalMicroseconds(thiz->dateTime) + MICROSECONDS_PER_SECOND * SECONDS_PER_MINUTE * bias;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(LocalDateTime, InitializeByUtcTotalMicroseconds, ERROR_CODE, int16_t bias, uint64_t utcTotalMicroseconds)
{
    ERROR_CODE error = ERROR_UNKNOWN;
    if (ERROR_NO != (error = thiz->dateTime->InitializeByTotalMicroseconds(thiz->dateTime, utcTotalMicroseconds - MICROSECONDS_PER_SECOND * SECONDS_PER_MINUTE * bias)))
    {
        return error;
    }

    thiz->bias = bias;
    thiz->utcTotalMicroseconds = utcTotalMicroseconds;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(LocalDateTime, LocalDateTime, LocalDateTime*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        I_INIT(DateTime, thiz->dateTime);
        break;
    case METHOD_DTOR:
        I_DISPOSE(DateTime, thiz->dateTime);
        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(LocalDateTime)
{
    MEMBER_METHOD_INITIALIZE(LocalDateTime, Bias)
    MEMBER_METHOD_INITIALIZE(LocalDateTime, LiteralDateTime)
    MEMBER_METHOD_INITIALIZE(LocalDateTime, UtcTotalMicroseconds)
    MEMBER_METHOD_INITIALIZE(LocalDateTime, Initialize)
    MEMBER_METHOD_INITIALIZE(LocalDateTime, InitializeByUtcTotalMicroseconds)
};
