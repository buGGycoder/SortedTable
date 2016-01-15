#include "Anniversary.h"
#include "TimeInterval.h"

MEMBER_METHOD_IMPLEMENT(Anniversary, Type, ANNIVERSARY_TYPE)
{
    return ANNIVERSARY_TYPE_MASK & thiz->category;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, Category, ANNIVERSARY_CATEGORY)
{
    return thiz->category;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, CalendarType, CALENDAR_TYPE)
{
    return thiz->calendarType;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, WeekType, WEEK_TYPE)
{
    return thiz->weekType;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, Month, uint8_t)
{
    return thiz->month;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, Day, uint8_t)
{
    return thiz->day;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, DayOfWeek, DAY_OF_WEEK)
{
    return thiz->dayOfWeek;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, DayOrder, DAY_ORDER)
{
    return thiz->dayOrder;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, WeekOrder, uint8_t)
{
    return thiz->weekOrder;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, DayOfYear, uint16_t)
{
    return thiz->dayOfYear;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, _GetDate, ERROR_CODE, IN uint16_t year, IN uint8_t month, OUT Date* date)
{
    ERROR_CODE error = ERROR_NO;

    if (!date)
    {
        return ERROR_ARGUMENT_NULL;
    }

    CALENDAR_TYPE calendarType = thiz->CalendarType(thiz);
    if (CALENDAR_TYPE_CHINESE_LUNAR == calendarType)
    {
        /* it doesn't support yet */
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    Date* dateOperand = I_VAR(Date);
    TimeInterval* timeOperand = I_VAR(TimeInterval);
    switch(thiz->Category(thiz))
    {
    case ANNIVERSARY_CATEGORY_YEARLY_MONTH_DAY:
        month = thiz->Month(thiz);
    case ANNIVERSARY_CATEGORY_MONTHLY_DAY:
        /* Maybe I_RENEW(Date, date); is better */
        error = date->Initialize(date, year, month, thiz->Day(thiz));
        break;

    case ANNIVERSARY_CATEGORY_YEARLY_MONTH_DAYOFWEEK_DAYORDER:
        month = thiz->Month(thiz);
    case ANNIVERSARY_CATEGORY_MONTHLY_DAYOFWEEK_DAYORDER:
    {

        if (ERROR_NO != (error = dateOperand->Initialize(dateOperand, year, month, 1)))
        {
            break;
        }

        uint8_t dayOrder = thiz->DayOrder(thiz);
        if (ERROR_NO != (error = timeOperand->Initialize(timeOperand, (thiz->DayOfWeek(thiz) + DAYS_PER_WEEK - dateOperand->DayOfWeek(dateOperand)) % DAYS_PER_WEEK + DAYS_PER_WEEK * (dayOrder - 1), 0, 0, 0, 0)))
        {
            break;
        }

        uint64_t totalMicroseconds = dateOperand->TotalMicroseconds(dateOperand) + timeOperand->TotalMicroseconds(timeOperand);
        if (DAY_ORDER_LAST == dayOrder)
        {
            if (ERROR_NO != (error = date->InitializeByTotalMicroseconds(date, totalMicroseconds)))
            {
                break;
            }

            if (date->Month(date) == month)
            {
                break;
            }

            totalMicroseconds -= MICROSECONDS_PER_SECOND * SECONDS_PER_MINUTE * MINUTES_PER_HOUR * HOURS_PER_DAY * DAYS_PER_WEEK;
            I_RENEW(Date, date);
        }

        /* Maybe I_RENEW(Date, date); is better */
        error = date->InitializeByTotalMicroseconds(date, totalMicroseconds);
        break;
    }

    case ANNIVERSARY_CATEGORY_YEARLY_WEEKORDER_DAYOFWEEK:
    {
        WEEK_TYPE weekType = thiz->WeekType(thiz);
        /* details refer to http://en.wikipedia.org/wiki/Week */
        DAY_OF_WEEK firstDayOfWeek = 0;
        uint8_t dayOfJanuaryMustInWeek1 = 0;
        switch(weekType)
        {
        case WEEK_TYPE_ISO_8601:
            firstDayOfWeek = MONDAY;
            dayOfJanuaryMustInWeek1 = 4;
            break;
        case WEEK_TYPE_ISLAMIC:
            firstDayOfWeek = SATURDAY;
            dayOfJanuaryMustInWeek1 = 1;
            break;
        case WEEK_TYPE_NORTH_AMERICAN:
            firstDayOfWeek = SUNDAY;
            dayOfJanuaryMustInWeek1 = 1;
            break;
        default:
            return ERROR_INVALID_STATE;
        }

        if (ERROR_NO != (error = dateOperand->Initialize(dateOperand, year, 1, dayOfJanuaryMustInWeek1)))
        {
            break;
        }

        uint64_t totalMicroseconds = dateOperand->TotalMicroseconds(dateOperand);
        if (ERROR_NO != (error = timeOperand->Initialize(timeOperand, (dateOperand->DayOfWeek(dateOperand) + DAYS_PER_WEEK - firstDayOfWeek) % DAYS_PER_WEEK, 0, 0, 0, 0)))
        {
            break;
        }

        totalMicroseconds -= timeOperand->TotalMicroseconds(timeOperand);
        I_RENEW(TimeInterval, timeOperand);
        if (ERROR_NO != (error = timeOperand->Initialize(timeOperand, DAYS_PER_WEEK * (thiz->WeekOrder(thiz) - 1) + (thiz->DayOfWeek(thiz) + DAYS_PER_WEEK - firstDayOfWeek) % DAYS_PER_WEEK, 0, 0, 0, 0)))
        {
            break;
        }

        totalMicroseconds += timeOperand->TotalMicroseconds(timeOperand);
        /* Maybe I_RENEW(Date, date); is better */
        error = date->InitializeByTotalMicroseconds(date, totalMicroseconds);
        break;
    }

    case ANNIVERSARY_CATEGORY_YEARLY_DAYOFYEAR:
    {
        Date* dateOperand = I_VAR(Date);
        TimeInterval* timeOperand = I_VAR(TimeInterval);
        if (ERROR_NO != (error = dateOperand->Initialize(dateOperand, year, 1, 1)))
        {
            break;
        }

        if (ERROR_NO != (error = timeOperand->Initialize(timeOperand, thiz->DayOfYear(thiz) - 1, 0, 0, 0, 0)))
        {
            break;
        }

        /* Maybe I_RENEW(Date, date); is better */
        error = date->InitializeByTotalMicroseconds(date, dateOperand->TotalMicroseconds(dateOperand) + timeOperand->TotalMicroseconds(timeOperand));
        break;
    }

    default:
        error = ERROR_INVALID_STATE;
        break;
    }

    I_DISPOSE(TimeInterval, timeOperand);
    I_DISPOSE(Date, dateOperand);
    return error;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, GetYearlyDate, ERROR_CODE, IN uint16_t year, OUT Date* date)
{
    return NONINSTANTIAL_MEMBER_METHOD_CALL(Anniversary, _GetDate, thiz, year, 0, date);
}

MEMBER_METHOD_IMPLEMENT(Anniversary, GetMonthlyDate, ERROR_CODE, IN uint16_t year, IN uint8_t month, OUT Date* date)
{
    return NONINSTANTIAL_MEMBER_METHOD_CALL(Anniversary, _GetDate, thiz, year, month, date);
}

MEMBER_METHOD_IMPLEMENT(Anniversary, InitializeBy_Month_Day, ERROR_CODE, CALENDAR_TYPE calendarType, uint8_t month, uint8_t day)
{
    if (CALENDAR_TYPE_UNKNOWN == calendarType)
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    thiz->category = ANNIVERSARY_CATEGORY_YEARLY_MONTH_DAY;
    thiz->calendarType = calendarType;
    thiz->month = month;
    thiz->day = day;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, InitializeBy_Month_DayOfWeek_DayOrder, ERROR_CODE, uint8_t month, DAY_OF_WEEK dayOfWeek, DAY_ORDER dayOrder)
{
    if (DAYS_PER_WEEK < dayOfWeek)
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    if (DAY_ORDER_LAST < dayOrder)
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    thiz->category = ANNIVERSARY_CATEGORY_YEARLY_MONTH_DAYOFWEEK_DAYORDER;
    thiz->calendarType = CALENDAR_TYPE_GREGORIAN;
    thiz->month = month;
    thiz->dayOfWeek = dayOfWeek;
    thiz->dayOrder = dayOrder;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, InitializeBy_WeekOrder_DayOfWeek, ERROR_CODE, WEEK_TYPE weekType, uint8_t weekOrder, DAY_OF_WEEK dayOfWeek)
{
    if (WEEK_TYPE_UNKNOWN == weekType)
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    if (DAYS_PER_WEEK < dayOfWeek)
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    thiz->category = ANNIVERSARY_CATEGORY_YEARLY_WEEKORDER_DAYOFWEEK;
    thiz->weekType = weekType;
    thiz->calendarType = CALENDAR_TYPE_GREGORIAN;
    thiz->weekOrder = weekOrder;
    thiz->dayOfWeek = dayOfWeek;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, InitializeBy_DayOfYear, ERROR_CODE, CALENDAR_TYPE calendarType, uint16_t dayOfYear)
{
    if (CALENDAR_TYPE_UNKNOWN == calendarType)
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    thiz->category = ANNIVERSARY_CATEGORY_YEARLY_DAYOFYEAR;
    thiz->calendarType = calendarType;
    thiz->dayOfYear = dayOfYear;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, InitializeBy_Day, ERROR_CODE, CALENDAR_TYPE calendarType, uint8_t day)
{
    if (CALENDAR_TYPE_UNKNOWN == calendarType)
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    thiz->category = ANNIVERSARY_CATEGORY_MONTHLY_DAY;
    thiz->calendarType = calendarType;
    thiz->day = day;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(Anniversary, InitializeBy_DayOfWeek_DayOrder, ERROR_CODE, DAY_OF_WEEK dayOfWeek, uint8_t dayOrder)
{
    if (DAY_ORDER_LAST < dayOrder)
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    thiz->category = ANNIVERSARY_CATEGORY_MONTHLY_DAYOFWEEK_DAYORDER;
    thiz->calendarType = CALENDAR_TYPE_GREGORIAN;
    thiz->dayOfWeek = dayOfWeek;
    thiz->dayOrder = dayOrder;
    return ERROR_NO;
}

DEFAULT_MANAGING_METHOD_IMPLEMENT(Anniversary)

CLASS_INITIALIZE(Anniversary)
{
    MEMBER_METHOD_INITIALIZE(Anniversary, Type)
    MEMBER_METHOD_INITIALIZE(Anniversary, Category)
    MEMBER_METHOD_INITIALIZE(Anniversary, CalendarType)
    MEMBER_METHOD_INITIALIZE(Anniversary, WeekType)
    MEMBER_METHOD_INITIALIZE(Anniversary, Month)
    MEMBER_METHOD_INITIALIZE(Anniversary, Day)
    MEMBER_METHOD_INITIALIZE(Anniversary, DayOfWeek)
    MEMBER_METHOD_INITIALIZE(Anniversary, DayOrder)
    MEMBER_METHOD_INITIALIZE(Anniversary, WeekOrder)
    MEMBER_METHOD_INITIALIZE(Anniversary, DayOfYear)
    MEMBER_METHOD_INITIALIZE(Anniversary, GetYearlyDate)
    MEMBER_METHOD_INITIALIZE(Anniversary, GetMonthlyDate)
    MEMBER_METHOD_INITIALIZE(Anniversary, InitializeBy_Month_Day)
    MEMBER_METHOD_INITIALIZE(Anniversary, InitializeBy_Month_DayOfWeek_DayOrder)
    MEMBER_METHOD_INITIALIZE(Anniversary, InitializeBy_WeekOrder_DayOfWeek)
    MEMBER_METHOD_INITIALIZE(Anniversary, InitializeBy_DayOfYear)
    MEMBER_METHOD_INITIALIZE(Anniversary, InitializeBy_Day)
    MEMBER_METHOD_INITIALIZE(Anniversary, InitializeBy_DayOfWeek_DayOrder)
};
