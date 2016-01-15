#include "DateTime.h"
#include "SizedString.h"
#include "SizedBuffer.h"

MEMBER_METHOD_IMPLEMENT(DateTime, Year, uint16_t)
{
    return thiz->date->Year(thiz->date);
}

MEMBER_METHOD_IMPLEMENT(DateTime, Month, uint8_t)
{
    return thiz->date->Month(thiz->date);
}

MEMBER_METHOD_IMPLEMENT(DateTime, Day, uint8_t)
{
    return thiz->date->Day(thiz->date);
}

MEMBER_METHOD_IMPLEMENT(DateTime, DayOfWeek, DAY_OF_WEEK)
{
    return thiz->date->DayOfWeek(thiz->date);
}

MEMBER_METHOD_IMPLEMENT(DateTime, DayOfYear, uint16_t)
{
    return thiz->date->DayOfYear(thiz->date);
}

MEMBER_METHOD_IMPLEMENT(DateTime, Hour, uint8_t)
{
    return thiz->time->Hour(thiz->time);
}

MEMBER_METHOD_IMPLEMENT(DateTime, Minute, uint8_t)
{
    return thiz->time->Minute(thiz->time);
}

MEMBER_METHOD_IMPLEMENT(DateTime, Second, uint8_t)
{
    return thiz->time->Second(thiz->time);
}

MEMBER_METHOD_IMPLEMENT(DateTime, Microsecond, uint32_t)
{
    return thiz->time->Microsecond(thiz->time);
}

MEMBER_METHOD_IMPLEMENT(DateTime, TotalMicroseconds, uint64_t)
{
    return thiz->totalMicroseconds;
}

MEMBER_METHOD_IMPLEMENT(DateTime, Initialize, ERROR_CODE, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint32_t microsecond)
{
    ERROR_CODE error = ERROR_UNKNOWN;
    error = thiz->date->Initialize(thiz->date, year, month, day);
    if (ERROR_NO != error)
    {
        return error;
    }

    error = thiz->time->Initialize(thiz->time, 0, hour, minute, second, microsecond);
    if (ERROR_NO != error)
    {
        return error;
    }

    thiz->totalMicroseconds = thiz->date->TotalMicroseconds(thiz->date) + thiz->time->TotalMicroseconds(thiz->time);
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(DateTime, InitializeByTotalMicroseconds, ERROR_CODE, uint64_t totalMicroseconds)
{
    ERROR_CODE error = ERROR_UNKNOWN;
    thiz->totalMicroseconds = totalMicroseconds;
    error = thiz->date->InitializeByTotalMicroseconds(thiz->date, totalMicroseconds);
    if (ERROR_NO != error)
    {
        return error;
    }

    totalMicroseconds -= MEMBER_METHOD_CALL(Date, TotalMicroseconds, thiz->date);
    error = thiz->time->InitializeByTotalMicroseconds(thiz->time, totalMicroseconds);
    if (ERROR_NO != error)
    {
        return error;
    }

    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(DateTime, ToString, _SizedString*)
{
    char* buffer[1024];
    _SizedBuffer* sb = SizedBuffer.Initialize(buffer, sizeof(buffer), NULL);
    SizedBuffer.AppendFormat(sb, "%04u/%02u/%02u %02u:%02u:%02u.%06u DoW#%01u DoY#%u",
                             thiz->Year(thiz),
                             thiz->Month(thiz),
                             thiz->Day(thiz),
                             thiz->Hour(thiz),
                             thiz->Minute(thiz),
                             thiz->Second(thiz),
                             thiz->Microsecond(thiz),
                             thiz->DayOfWeek(thiz),
                             thiz->DayOfYear(thiz));
    return SizedString.New(SizedBuffer.Data(sb), SizedString.Size(sb));
}

MEMBER_METHOD_IMPLEMENT(DateTime, DateTime, DateTime*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        I_INIT(Date, thiz->date);
        I_INIT(TimeInterval, thiz->time);
        break;
    case METHOD_DTOR:
        I_DISPOSE(Date, thiz->date);
        I_DISPOSE(TimeInterval, thiz->time);
        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(DateTime)
{
    MEMBER_METHOD_INITIALIZE(DateTime, Year)
    MEMBER_METHOD_INITIALIZE(DateTime, Month)
    MEMBER_METHOD_INITIALIZE(DateTime, DayOfWeek)
    MEMBER_METHOD_INITIALIZE(DateTime, Day)
    MEMBER_METHOD_INITIALIZE(DateTime, DayOfYear)
    MEMBER_METHOD_INITIALIZE(DateTime, Hour)
    MEMBER_METHOD_INITIALIZE(DateTime, Minute)
    MEMBER_METHOD_INITIALIZE(DateTime, Second)
    MEMBER_METHOD_INITIALIZE(DateTime, Microsecond)
    MEMBER_METHOD_INITIALIZE(DateTime, TotalMicroseconds)
    MEMBER_METHOD_INITIALIZE(DateTime, Initialize)
    MEMBER_METHOD_INITIALIZE(DateTime, InitializeByTotalMicroseconds)
    MEMBER_METHOD_INITIALIZE(DateTime, ToString)
};

