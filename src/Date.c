#include "Date.h"
#include <time.h>

MEMBER_METHOD_IMPLEMENT(Date, Year, uint16_t)
{
    return thiz->year;
}

MEMBER_METHOD_IMPLEMENT(Date, Month, uint8_t)
{
    return thiz->month;
}

MEMBER_METHOD_IMPLEMENT(Date, Day, uint8_t)
{
    return thiz->day;
}

MEMBER_METHOD_IMPLEMENT(Date, DayOfWeek, DAY_OF_WEEK)
{
    return thiz->dayOfWeek;
}

MEMBER_METHOD_IMPLEMENT(Date, DayOfYear, uint16_t)
{
    return thiz->dayOfYear;
}

MEMBER_METHOD_IMPLEMENT(Date, TotalMicroseconds, uint64_t)
{
    return thiz->totalMicroseconds;
}

MEMBER_METHOD_IMPLEMENT(Date, Initialize, ERROR_CODE, uint16_t year, uint8_t month, uint8_t day)
{
    struct tm tm = {0};
    time_t secondsSince1970 = 0;

    thiz->year = year;
    thiz->month = month;
    thiz->day = day;
    tm.tm_year = thiz->year - 1900;
    tm.tm_mon = thiz->month - 1;
    tm.tm_mday = thiz->day;
    secondsSince1970 = timegm(&tm);
    thiz->totalMicroseconds = MICROSECONDS_PER_SECOND * secondsSince1970;
    memset(&tm, 0, sizeof(struct tm));
    gmtime_r(&secondsSince1970, &tm);
    thiz->dayOfWeek = tm.tm_wday;
    thiz->dayOfYear = 1 + tm.tm_yday;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(Date, InitializeByTotalMicroseconds, ERROR_CODE, uint64_t totalMicroseconds)
{
    struct tm tm = {0};
    totalMicroseconds /= MICROSECONDS_PER_SECOND;
    if (!gmtime_r(&totalMicroseconds, &tm))
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    thiz->year = tm.tm_year + 1900;
    thiz->month = tm.tm_mon + 1;
    thiz->dayOfWeek = tm.tm_wday;
    thiz->day = tm.tm_mday;
    thiz->dayOfYear = 1 + tm.tm_yday;
    thiz->totalMicroseconds = totalMicroseconds / SECONDS_PER_MINUTE / MINUTES_PER_HOUR / HOURS_PER_DAY * HOURS_PER_DAY * MINUTES_PER_HOUR * SECONDS_PER_MINUTE * MICROSECONDS_PER_SECOND;
    return ERROR_NO;
}

DEFAULT_MANAGING_METHOD_IMPLEMENT(Date)

CLASS_INITIALIZE(Date)
{
    MEMBER_METHOD_INITIALIZE(Date, Year)
    MEMBER_METHOD_INITIALIZE(Date, Month)
    MEMBER_METHOD_INITIALIZE(Date, DayOfWeek)
    MEMBER_METHOD_INITIALIZE(Date, Day)
    MEMBER_METHOD_INITIALIZE(Date, DayOfYear)
    MEMBER_METHOD_INITIALIZE(Date, TotalMicroseconds)
    MEMBER_METHOD_INITIALIZE(Date, Initialize)
    MEMBER_METHOD_INITIALIZE(Date, InitializeByTotalMicroseconds)
};

