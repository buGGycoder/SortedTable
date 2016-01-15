#include <ctype.h>
#include <stdarg.h>

#include "StringBuilder.h"

NAMESPACE_METHOD_IMPLEMENT(StringBuilder, ChangeCase, bool, char* stringBegin, int currentLocation, int lengthOfString, int (*changeCase)(int))
{
    if (!stringBegin)
    {
        return false;
    }

    if (!changeCase)
    {
        return false;
    }

    if (0 > currentLocation || 0 > lengthOfString || (lengthOfString && currentLocation >= lengthOfString))
    {
        return false;
    }

    if (lengthOfString)
    {
        int i = currentLocation;
        for (; lengthOfString > i; ++i)
        {
            *(stringBegin + i) = changeCase(*(stringBegin + i));
        }

        return true;
    }

    char* i = stringBegin + currentLocation;
    for (; *i; ++i)
    {
        *i = changeCase(*i);
    }

    return true;
}

NAMESPACE_METHOD_IMPLEMENT(StringBuilder, ToUpper, bool, char* stringBegin, int currentLocation, int lengthOfString)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(StringBuilder, ChangeCase, stringBegin, currentLocation, lengthOfString, toupper);
}

NAMESPACE_METHOD_IMPLEMENT(StringBuilder, ToLower, bool, char* stringBegin, int currentLocation, int lengthOfString)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(StringBuilder, ChangeCase, stringBegin, currentLocation, lengthOfString, tolower);
}

NAMESPACE_METHOD_IMPLEMENT(StringBuilder, AppendAsPath, int, char* pathBegin, int currentLocation, int lengthOfPath, const char* relativePath, int lengthOfRelativePath)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(StringBuilder, Concatenate, pathBegin, currentLocation, lengthOfPath, relativePath, lengthOfRelativePath, true, NULL);
}

NAMESPACE_METHOD_IMPLEMENT(StringBuilder, Append, int, char* stringBegin, int currentLocation, int lengthOfString, const char* subString, int lengthOfSubString)
{
    return NONINSTANTIAL_NAMESPACE_METHOD_CALL(StringBuilder, Concatenate, stringBegin, currentLocation, lengthOfString, subString, lengthOfSubString, false, NULL);
}

NAMESPACE_METHOD_IMPLEMENT(StringBuilder, FormatAsPath, int, char* stringBegin, int currentLocation, int lengthOfString, const char* formatString, ...)
{
    int location = currentLocation;
    va_list args;
    va_start(args, formatString);
    location = NONINSTANTIAL_NAMESPACE_METHOD_CALL(StringBuilder, VFormat, stringBegin, currentLocation, lengthOfString, true, formatString, args);
    va_end(args);
    return location;
}

NAMESPACE_METHOD_IMPLEMENT(StringBuilder, Format, int, char* stringBegin, int currentLocation, int lengthOfString, const char* formatString, ...)
{
    int location = currentLocation;
    va_list args;
    va_start(args, formatString);
    location = NONINSTANTIAL_NAMESPACE_METHOD_CALL(StringBuilder, VFormat, stringBegin, currentLocation, lengthOfString, false, formatString, args);
    va_end(args);
    return location;
}

NAMESPACE_METHOD_IMPLEMENT(StringBuilder, VFormat, int, char* stringBegin, int currentLocation, int lengthOfString, bool formatAsPath, const char* formatString, va_list args)
{
    if (formatAsPath)
    {
        currentLocation = NONINSTANTIAL_NAMESPACE_METHOD_CALL(StringBuilder, AppendAsPath,
                          stringBegin,
                          currentLocation,
                          lengthOfString,
                          "",
                          0);
    }

    if (0 > currentLocation || 0 > lengthOfString)
    {
        return -1;
    }

    if (lengthOfString && lengthOfString <= currentLocation)
    {
        return -1;
    }

    int lengthLeft = lengthOfString - currentLocation;
    int lengthOfFormattedString = vsnprintf(stringBegin + currentLocation, lengthLeft, formatString, args);
    if (lengthLeft - 1 < lengthOfFormattedString)
    {
        return -1;
    }

    return currentLocation + lengthOfFormattedString;
}

NAMESPACE_METHOD_IMPLEMENT(StringBuilder, Concatenate, int, char* stringBegin, int currentLocation, int lengthOfString, const char* subString, int lengthOfSubString, bool appendAsPath, int (*changeCase)(int))
{
    if (!stringBegin)
    {
        return -1;
    }

    if (!subString)
    {
        return -1;
    }

    if (0 > currentLocation || 0 > lengthOfString || 0 > lengthOfSubString)
    {
        return -1;
    }

    if (lengthOfString && lengthOfString <= currentLocation)
    {
        return -1;
    }

    if (appendAsPath && currentLocation)
    {
        if(('/' == stringBegin[currentLocation - 1]) == ('/' == subString[0]))
        {
            if ('/' == subString[0])
            {
                --currentLocation;
            }
            else
            {
                stringBegin[currentLocation++] = '/';
            }
        }
    }

    int lengthToBeCopied = 0;
    if (lengthOfSubString)
    {
        lengthToBeCopied = lengthOfSubString;
    }

    int lengthLeft = 0;
    if (lengthOfString)
    {
        lengthLeft = lengthOfString - 1 - currentLocation;
    }

    if (lengthToBeCopied && lengthLeft && lengthToBeCopied > lengthLeft)
    {
        lengthToBeCopied = lengthLeft;
    }

    if (lengthToBeCopied)
    {
        if (changeCase)
        {
            int i = 0;
            for (; i < lengthToBeCopied; ++i)
            {
                stringBegin[currentLocation++] = changeCase(*subString++);
            }
        }
        else
        {
            int i = 0;
            for (; i < lengthToBeCopied; ++i)
            {
                stringBegin[currentLocation++] = *subString++;
            }
        }
    }
    else
    {
        if (changeCase)
        {
            for (; *subString && lengthLeft--; stringBegin[currentLocation++] = changeCase(*subString++))
                ;
        }
        else
        {
            for (; *subString && lengthLeft--; stringBegin[currentLocation++] = *subString++)
                ;
        }

        if (*subString)
        {
            stringBegin[currentLocation] = 0;
            return -1;
        }
    }

    stringBegin[currentLocation] = 0;

    return currentLocation;
}

NAMESPACE_INITIALIZE(StringBuilder)
{
    NAMESPACE_METHOD_INITIALIZE(StringBuilder, ToUpper)
    NAMESPACE_METHOD_INITIALIZE(StringBuilder, ToLower)
    NAMESPACE_METHOD_INITIALIZE(StringBuilder, AppendAsPath)
    NAMESPACE_METHOD_INITIALIZE(StringBuilder, Append)
    NAMESPACE_METHOD_INITIALIZE(StringBuilder, FormatAsPath)
    NAMESPACE_METHOD_INITIALIZE(StringBuilder, Format)
    NAMESPACE_METHOD_INITIALIZE(StringBuilder, VFormat)
    NAMESPACE_METHOD_INITIALIZE(StringBuilder, Concatenate)
};
