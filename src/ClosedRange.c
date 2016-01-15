#include "ClosedRange.h"

MEMBER_METHOD_IMPLEMENT(ClosedRange, Initialize, bool, RANGE_SCALE_T from, RANGE_SCALE_T to)
{
    if (from <= to)
    {
        thiz->from = from;
        thiz->to = to;
    }
    else
    {
        thiz->from = to;
        thiz->to = from;
    }

    return true;
}

MEMBER_METHOD_IMPLEMENT(ClosedRange, From, RANGE_SCALE_T)
{
    return thiz->from;
}

MEMBER_METHOD_IMPLEMENT(ClosedRange, To, RANGE_SCALE_T)
{
    return thiz->to;
}

MEMBER_METHOD_IMPLEMENT(ClosedRange, TryMerge, bool, const ClosedRange* other)
{
    if ((thiz->from < other->from && thiz->to + 1 < other->from)
            || (other->from < thiz->from && other->to + 1 < thiz->from))
    {
        return false;
    }

    thiz->from > other->from && (thiz->from = other->from);
    thiz->to < other->to && (thiz->to = other->to);
    return true;
}

MEMBER_METHOD_IMPLEMENT(ClosedRange, Contains, bool, const ClosedRange* other)
{
    return thiz->from <= other->from && thiz->to >= other->to;
}

DEFAULT_MANAGING_METHOD_IMPLEMENT(ClosedRange);

CLASS_INITIALIZE(ClosedRange)
{
    MEMBER_METHOD_INITIALIZE(ClosedRange, Initialize)
    MEMBER_METHOD_INITIALIZE(ClosedRange, From)
    MEMBER_METHOD_INITIALIZE(ClosedRange, To)
    MEMBER_METHOD_INITIALIZE(ClosedRange, TryMerge)
    MEMBER_METHOD_INITIALIZE(ClosedRange, Contains)
};
