#include "TimeZone.h"
#include <sys/time.h>
#include "ClosedRange.h"

MEMBER_METHOD_IMPLEMENT(TimeZone, Bias, int16_t)
{
    return thiz->bias;
}

MEMBER_METHOD_IMPLEMENT(TimeZone, DstTransition, DstTransitionTime*)
{
    return thiz->dstTransition;
}

MEMBER_METHOD_IMPLEMENT(TimeZone, StdTransition, DstTransitionTime*)
{
    return thiz->stdTransition;
}

MEMBER_METHOD_IMPLEMENT(TimeZone, GetUtcDateTime, ERROR_CODE, IN DateTime* localTime, OUT DateTime* utcTime)
{
    uint64_t localUseconds = localTime->TotalMicroseconds(localTime);
    if (!thiz->useDst)
    {
        return utcTime->InitializeByTotalMicroseconds(utcTime, localUseconds + MICROSECONDS_PER_SECOND * SECONDS_PER_MINUTE * thiz->bias);
    }

    /* process DST */

    DateTime* dstTransition = I_VAR(DateTime);
    DateTime* stdTransition = I_VAR(DateTime);
    DateTime* earlyTransition = NULL;
    DateTime* lateTransition = NULL;
    int16_t biasBetweenTrans = 0;
    int16_t biasOutOfTrans = 0;
    uint16_t year = localTime->Year(localTime);

    /* calculate the boundaries of dst and std time. mark bias for each time range */
    thiz->dstTransition->GetDateTime(thiz->dstTransition, year, dstTransition);
    thiz->stdTransition->GetDateTime(thiz->stdTransition, year, stdTransition);
    if (dstTransition->TotalMicroseconds(dstTransition) < stdTransition->TotalMicroseconds(stdTransition))
    {
        earlyTransition = dstTransition;
        lateTransition = stdTransition;
        biasOutOfTrans = thiz->stdTransition->Bias(thiz->stdTransition);
        biasBetweenTrans = thiz->dstTransition->Bias(thiz->dstTransition);
    }
    else
    {
        earlyTransition = stdTransition;
        lateTransition = dstTransition;
        biasOutOfTrans = thiz->dstTransition->Bias(thiz->dstTransition);
        biasBetweenTrans = thiz->stdTransition->Bias(thiz->stdTransition);
    }

    uint64_t earlyTransUseconds = earlyTransition->TotalMicroseconds(earlyTransition);
    uint64_t lateTransUseconds = lateTransition->TotalMicroseconds(lateTransition);

    I_DISPOSE(DateTime, dstTransition);
    I_DISPOSE(DateTime, stdTransition);

    /*
     * TODO: check the transition from dst to std since bias (normally one hour) is missing in real worlk clock.
     *       Probably the specified local time is just the time between the missing range, this should be an error input argument.
     */
    int16_t totalBias = thiz->bias + ((localUseconds >= earlyTransUseconds && localUseconds < lateTransUseconds) ? biasBetweenTrans : biasOutOfTrans);
    return utcTime->InitializeByTotalMicroseconds(utcTime, localUseconds + MICROSECONDS_PER_SECOND * SECONDS_PER_MINUTE * totalBias);
}

MEMBER_METHOD_IMPLEMENT(TimeZone, GetLocalDateTime, ERROR_CODE, IN DateTime* utcTime, OUT LocalDateTime* localTime)
{
    uint64_t utcUseconds = utcTime->TotalMicroseconds(utcTime);
    return NONINSTANTIAL_MEMBER_METHOD_CALL(TimeZone, GetLocalDateTimeFromUtcTotalMicroseconds, thiz, utcUseconds, localTime);

}

MEMBER_METHOD_IMPLEMENT(TimeZone, GetLocalDateTimeFromUtcTotalMicroseconds, ERROR_CODE, IN uint64_t utcUseconds, OUT LocalDateTime* localTime)
{
    if (!thiz->useDst)
    {
        return localTime->InitializeByUtcTotalMicroseconds(localTime, thiz->bias, utcUseconds);
    }

    /* process DST */

    ERROR_CODE error = ERROR_UNKNOWN;
    if (ERROR_NO != (error = localTime->InitializeByUtcTotalMicroseconds(localTime, thiz->bias, utcUseconds)))
    {
        I_DISPOSE(LocalDateTime, localTime);
        return error;
    }

    DateTime* dt = localTime->LiteralDateTime(localTime);
    uint16_t year = dt->Year(dt);
    I_RENEW(LocalDateTime, localTime);

    DateTime* dstTransition = I_VAR(DateTime);
    DateTime* stdTransition = I_VAR(DateTime);
    DateTime* earlyTransition = NULL;
    DateTime* lateTransition = NULL;
    int16_t biasAtMomentEarlyTrans = 0;
    int16_t biasAtMomentLateTrans = 0;
    int16_t biasBetweenTrans = 0;
    int16_t biasOutOfTrans = 0;

    /* calculate the boundaries of dst and std time. mark bias for each time range */
    thiz->dstTransition->GetDateTime(thiz->dstTransition, year, dstTransition);
    thiz->stdTransition->GetDateTime(thiz->stdTransition, year, stdTransition);
    if (dstTransition->TotalMicroseconds(dstTransition) < stdTransition->TotalMicroseconds(stdTransition))
    {
        earlyTransition = dstTransition;
        lateTransition = stdTransition;
        biasOutOfTrans = biasAtMomentEarlyTrans = thiz->stdTransition->Bias(thiz->stdTransition);
        biasBetweenTrans = biasAtMomentLateTrans = thiz->dstTransition->Bias(thiz->dstTransition);
    }
    else
    {
        earlyTransition = stdTransition;
        lateTransition = dstTransition;
        biasOutOfTrans = biasAtMomentEarlyTrans = thiz->dstTransition->Bias(thiz->dstTransition);
        biasBetweenTrans = biasAtMomentLateTrans = thiz->stdTransition->Bias(thiz->stdTransition);
    }

    I_DISPOSE(DateTime, dstTransition);
    I_DISPOSE(DateTime, stdTransition);

    /* adjust boundary time to utc */
    uint64_t earlyTransUtcUseconds = earlyTransition->TotalMicroseconds(earlyTransition) + MICROSECONDS_PER_SECOND * SECONDS_PER_MINUTE * (thiz->bias + biasAtMomentEarlyTrans);
    uint64_t lateTransUtcUseconds = lateTransition->TotalMicroseconds(lateTransition) + MICROSECONDS_PER_SECOND * SECONDS_PER_MINUTE * (thiz->bias + biasAtMomentLateTrans);
    int16_t totalBias = thiz->bias + ((utcUseconds >= earlyTransUtcUseconds && utcUseconds < lateTransUtcUseconds) ? biasBetweenTrans : biasOutOfTrans);
    return localTime->InitializeByUtcTotalMicroseconds(localTime, totalBias, utcUseconds);
}

MEMBER_METHOD_IMPLEMENT(TimeZone, Now, ERROR_CODE, OUT LocalDateTime* localTime)
{
    ERROR_CODE error = ERROR_UNKNOWN;
    struct timeval tv = {0};
    if (gettimeofday(&tv, NULL))
    {
        return ERROR_MACHINE_ILL;
    }

    DateTime* utcTime = I_VAR(DateTime);
    if (ERROR_NO != (error = utcTime->InitializeByTotalMicroseconds(utcTime, TIMEVAL_TO_MICROSECONDS(tv))))
    {
        I_DISPOSE(DateTime, utcTime);
        return error;
    }

    return thiz->GetLocalDateTime(thiz, utcTime, localTime);
}

STATIC_METHOD_IMPLEMENT(TimeZone, NowByUtcTotalMicroseconds, uint64_t)
{
    struct timeval tv = {0};
    return gettimeofday(&tv, NULL) ? 0ULL : TIMEVAL_TO_MICROSECONDS(tv);
}

MEMBER_METHOD_IMPLEMENT(TimeZone, InitializeWithoutDst, ERROR_CODE, IN int16_t bias)
{
    thiz->bias = bias;
    thiz->useDst = false;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(TimeZone, InitializeWithDst, ERROR_CODE, IN int16_t bias, IN DstTransitionTime* dstTransitionTime, IN DstTransitionTime* stdTransitionTime)
{
    thiz->bias = bias;
    thiz->useDst = true;
    dstTransitionTime->Duplicate(dstTransitionTime, thiz->dstTransition);
    stdTransitionTime->Duplicate(stdTransitionTime, thiz->stdTransition);
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(TimeZone, TimeZone, TimeZone*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        I_INIT(DstTransitionTime, thiz->dstTransition);
        I_INIT(DstTransitionTime, thiz->stdTransition);
        break;
    case METHOD_DTOR:
        I_DISPOSE(DstTransitionTime, thiz->stdTransition);
        I_DISPOSE(DstTransitionTime, thiz->dstTransition);
        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(TimeZone)
{
    MEMBER_METHOD_INITIALIZE(TimeZone, Bias)
    MEMBER_METHOD_INITIALIZE(TimeZone, DstTransition)
    MEMBER_METHOD_INITIALIZE(TimeZone, StdTransition)
    MEMBER_METHOD_INITIALIZE(TimeZone, GetUtcDateTime)
    MEMBER_METHOD_INITIALIZE(TimeZone, GetLocalDateTime)
    MEMBER_METHOD_INITIALIZE(TimeZone, GetLocalDateTimeFromUtcTotalMicroseconds)
    MEMBER_METHOD_INITIALIZE(TimeZone, Now)
    STATIC_METHOD_INITIALIZE(TimeZone, NowByUtcTotalMicroseconds)
    MEMBER_METHOD_INITIALIZE(TimeZone, InitializeWithoutDst)
    MEMBER_METHOD_INITIALIZE(TimeZone, InitializeWithDst)
};
