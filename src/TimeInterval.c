#include "TimeInterval.h"
#include "CalendarConstants.h"
#include "SizedBuffer.h"

MEMBER_METHOD_IMPLEMENT(TimeInterval, Day, uint32_t)
{
    return thiz->day;
}

MEMBER_METHOD_IMPLEMENT(TimeInterval, Hour, uint8_t)
{
    return thiz->hour;
}

MEMBER_METHOD_IMPLEMENT(TimeInterval, Minute, uint8_t)
{
    return thiz->minute;
}

MEMBER_METHOD_IMPLEMENT(TimeInterval, Second, uint8_t)
{
    return thiz->second;
}

MEMBER_METHOD_IMPLEMENT(TimeInterval, Microsecond, uint32_t)
{
    return thiz->microsecond;
}

MEMBER_METHOD_IMPLEMENT(TimeInterval, TotalMicroseconds, uint64_t)
{
    return thiz->totalMicroseconds;
}

MEMBER_METHOD_IMPLEMENT(TimeInterval, Initialize, ERROR_CODE, uint32_t day, uint8_t hour, uint8_t minute, uint8_t second, uint32_t microsecond)
{
    if (HOURS_PER_DAY <= hour || MINUTES_PER_HOUR <= minute || SECONDS_PER_MINUTE <= second || MICROSECONDS_PER_SECOND <= microsecond)
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    thiz->totalMicroseconds = thiz->day = day;
    thiz->totalMicroseconds *= HOURS_PER_DAY;
    thiz->totalMicroseconds += thiz->hour = hour;
    thiz->totalMicroseconds *= MINUTES_PER_HOUR;
    thiz->totalMicroseconds += thiz->minute = minute;
    thiz->totalMicroseconds *= SECONDS_PER_MINUTE;
    thiz->totalMicroseconds += thiz->second = second;
    thiz->totalMicroseconds *= MICROSECONDS_PER_SECOND;
    thiz->totalMicroseconds += thiz->microsecond = microsecond;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(TimeInterval, InitializeByTotalMicroseconds, ERROR_CODE, uint64_t totalMicroseconds)
{
    thiz->totalMicroseconds = totalMicroseconds;
    thiz->microsecond = totalMicroseconds % MICROSECONDS_PER_SECOND;
    totalMicroseconds /= MICROSECONDS_PER_SECOND;
    thiz->second = totalMicroseconds % SECONDS_PER_MINUTE;
    totalMicroseconds /= SECONDS_PER_MINUTE;
    thiz->minute = totalMicroseconds % MINUTES_PER_HOUR;
    totalMicroseconds /= MINUTES_PER_HOUR;
    thiz->hour = totalMicroseconds % HOURS_PER_DAY;
    totalMicroseconds /= HOURS_PER_DAY;
    thiz->day = totalMicroseconds;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(TimeInterval, ToString, _SizedString*)
{
    char* buffer[1024];
    _SizedBuffer* sb = SizedBuffer.Initialize(buffer, sizeof(buffer), NULL);
    SizedBuffer.AppendFormat(sb, "%02u %02u:%02u:%02u.%06u",
                             thiz->Day(thiz),
                             thiz->Hour(thiz),
                             thiz->Minute(thiz),
                             thiz->Second(thiz),
                             thiz->Microsecond(thiz));
    return SizedString.New(SizedBuffer.Data(sb), SizedString.Size(sb));
}

DEFAULT_MANAGING_METHOD_IMPLEMENT(TimeInterval)

CLASS_INITIALIZE(TimeInterval)
{
    MEMBER_METHOD_INITIALIZE(TimeInterval, Day)
    MEMBER_METHOD_INITIALIZE(TimeInterval, Hour)
    MEMBER_METHOD_INITIALIZE(TimeInterval, Minute)
    MEMBER_METHOD_INITIALIZE(TimeInterval, Second)
    MEMBER_METHOD_INITIALIZE(TimeInterval, Microsecond)
    MEMBER_METHOD_INITIALIZE(TimeInterval, TotalMicroseconds)
    MEMBER_METHOD_INITIALIZE(TimeInterval, Initialize)
    MEMBER_METHOD_INITIALIZE(TimeInterval, InitializeByTotalMicroseconds)
    MEMBER_METHOD_INITIALIZE(TimeInterval, ToString)
};
