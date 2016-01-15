#ifndef __CLOSEDRANGE_H__
#define __CLOSEDRANGE_H__

#include <stdbool.h>
#include <stdint.h>

#include "ClassSimulator.h"

typedef uint64_t RANGE_SCALE_T;

MEMBER_METHOD_DECLARE(ClosedRange, Initialize, bool, RANGE_SCALE_T from, RANGE_SCALE_T to);
MEMBER_METHOD_DECLARE(ClosedRange, From, RANGE_SCALE_T);
MEMBER_METHOD_DECLARE(ClosedRange, To, RANGE_SCALE_T);
MEMBER_METHOD_DECLARE(ClosedRange, TryMerge, bool, const ClosedRange* other);
MEMBER_METHOD_DECLARE(ClosedRange, Contains, bool, const ClosedRange* other);

CLASS(ClosedRange)
{
    MEMBER_METHOD_DEFINE(ClosedRange, Initialize);
    MEMBER_METHOD_DEFINE(ClosedRange, From);
    MEMBER_METHOD_DEFINE(ClosedRange, To);
    MEMBER_METHOD_DEFINE(ClosedRange, TryMerge);
    MEMBER_METHOD_DEFINE(ClosedRange, Contains);

    /* private: */
    RANGE_SCALE_T from;
    RANGE_SCALE_T to;
};

#endif /* __CLOSEDRANGE_H__ */
