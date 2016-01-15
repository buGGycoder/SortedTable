#ifndef __ANNIVERSARY_H__
#define __ANNIVERSARY_H__

#include <stdint.h>
#include "ClassSimulator.h"

#include "Error.h"
#include "CalendarConstants.h"
#include "Date.h"


typedef enum
{
    ANNIVERSARY_TYPE_UNKNOWN = 0,
    ANNIVERSARY_TYPE_YEARLY = 0x10000,
    ANNIVERSARY_TYPE_MONTHLY = 0x20000,
    ANNIVERSARY_TYPE_MASK = 0xFFFF0000,
} ANNIVERSARY_TYPE;

typedef enum
{
    ANNIVERSARY_CATEGORY_UNKNOWN = 0,
    ANNIVERSARY_CATEGORY_YEARLY_MONTH_DAY = 1 | ANNIVERSARY_TYPE_YEARLY,
    ANNIVERSARY_CATEGORY_YEARLY_MONTH_DAYOFWEEK_DAYORDER = 2 | ANNIVERSARY_TYPE_YEARLY,
    ANNIVERSARY_CATEGORY_YEARLY_WEEKORDER_DAYOFWEEK = 3 | ANNIVERSARY_TYPE_YEARLY,
    ANNIVERSARY_CATEGORY_YEARLY_DAYOFYEAR = 4 | ANNIVERSARY_TYPE_YEARLY,
    ANNIVERSARY_CATEGORY_MONTHLY_DAY = 5 | ANNIVERSARY_TYPE_MONTHLY,
    ANNIVERSARY_CATEGORY_MONTHLY_DAYOFWEEK_DAYORDER = 6 | ANNIVERSARY_TYPE_MONTHLY,
} ANNIVERSARY_CATEGORY;

typedef enum
{
    DAY_ORDER_UNKNOWN = 0,
    DAY_ORDER_FIRST,
    DAY_ORDER_SECOND,
    DAY_ORDER_THIRD,
    DAY_ORDER_FORTH,
    DAY_ORDER_LAST,
} DAY_ORDER;

MEMBER_METHOD_DECLARE(Anniversary, Type, ANNIVERSARY_TYPE);
MEMBER_METHOD_DECLARE(Anniversary, Category, ANNIVERSARY_CATEGORY);
MEMBER_METHOD_DECLARE(Anniversary, CalendarType, CALENDAR_TYPE);
MEMBER_METHOD_DECLARE(Anniversary, WeekType, WEEK_TYPE);
MEMBER_METHOD_DECLARE(Anniversary, Month, uint8_t);
MEMBER_METHOD_DECLARE(Anniversary, Day, uint8_t);
MEMBER_METHOD_DECLARE(Anniversary, DayOfWeek, DAY_OF_WEEK);
MEMBER_METHOD_DECLARE(Anniversary, DayOrder, DAY_ORDER);
MEMBER_METHOD_DECLARE(Anniversary, WeekOrder, uint8_t);
MEMBER_METHOD_DECLARE(Anniversary, DayOfYear, uint16_t);
MEMBER_METHOD_DECLARE(Anniversary, GetYearlyDate, ERROR_CODE, IN uint16_t year, OUT Date* date);
MEMBER_METHOD_DECLARE(Anniversary, GetMonthlyDate, ERROR_CODE, IN uint16_t year, IN uint8_t month, OUT Date* date);
MEMBER_METHOD_DECLARE(Anniversary, InitializeBy_Month_Day, ERROR_CODE, CALENDAR_TYPE calendarType, uint8_t month, uint8_t day);
MEMBER_METHOD_DECLARE(Anniversary, InitializeBy_Month_DayOfWeek_DayOrder, ERROR_CODE, uint8_t month, DAY_OF_WEEK dayOfWeek, DAY_ORDER dayOrder);
MEMBER_METHOD_DECLARE(Anniversary, InitializeBy_WeekOrder_DayOfWeek, ERROR_CODE, WEEK_TYPE weekType, uint8_t weekOrder, DAY_OF_WEEK dayOfWeek);
MEMBER_METHOD_DECLARE(Anniversary, InitializeBy_DayOfYear, ERROR_CODE, CALENDAR_TYPE calendarType, uint16_t dayOfYear);
MEMBER_METHOD_DECLARE(Anniversary, InitializeBy_Day, ERROR_CODE, CALENDAR_TYPE calendarType, uint8_t day);
MEMBER_METHOD_DECLARE(Anniversary, InitializeBy_DayOfWeek_DayOrder, ERROR_CODE, DAY_OF_WEEK dayOfWeek, uint8_t dayOrder);

CLASS(Anniversary)
{
    MEMBER_METHOD_DEFINE(Anniversary, Type);
    MEMBER_METHOD_DEFINE(Anniversary, Category);
    MEMBER_METHOD_DEFINE(Anniversary, CalendarType);
    MEMBER_METHOD_DEFINE(Anniversary, WeekType);
    MEMBER_METHOD_DEFINE(Anniversary, Month);
    MEMBER_METHOD_DEFINE(Anniversary, Day);
    MEMBER_METHOD_DEFINE(Anniversary, DayOfWeek);
    MEMBER_METHOD_DEFINE(Anniversary, DayOrder);
    MEMBER_METHOD_DEFINE(Anniversary, WeekOrder);
    MEMBER_METHOD_DEFINE(Anniversary, DayOfYear);
    MEMBER_METHOD_DEFINE(Anniversary, GetYearlyDate);
    MEMBER_METHOD_DEFINE(Anniversary, GetMonthlyDate);
    MEMBER_METHOD_DEFINE(Anniversary, InitializeBy_Month_Day);
    MEMBER_METHOD_DEFINE(Anniversary, InitializeBy_Month_DayOfWeek_DayOrder);
    MEMBER_METHOD_DEFINE(Anniversary, InitializeBy_WeekOrder_DayOfWeek);
    MEMBER_METHOD_DEFINE(Anniversary, InitializeBy_DayOfYear);
    MEMBER_METHOD_DEFINE(Anniversary, InitializeBy_Day);
    MEMBER_METHOD_DEFINE(Anniversary, InitializeBy_DayOfWeek_DayOrder);

    ANNIVERSARY_CATEGORY category;
    CALENDAR_TYPE calendarType;
    WEEK_TYPE weekType;
    uint8_t month : 4;
    uint8_t day : 5;
    DAY_OF_WEEK dayOfWeek : 3;
    DAY_ORDER dayOrder : 3;
    uint8_t weekOrder : 6;
    uint16_t dayOfYear : 9;
};

#endif /* __ANNIVERSARY_H__ */
