#ifndef __STRING_BUILDER_H__
#define __STRING_BUILDER_H__

#include <stdarg.h>
#include <stdbool.h>

#include "ClassSimulator.h"

NAMESPACE_METHOD_DECLARE(StringBuilder, ToUpper, bool, char* stringBegin, int currentLocation, int lengthOfString);
NAMESPACE_METHOD_DECLARE(StringBuilder, ToLower, bool, char* stringBegin, int currentLocation, int lengthOfString);
NAMESPACE_METHOD_DECLARE(StringBuilder, AppendAsPath, int, char* pathBegin, int currentLocation, int lengthOfPath, const char* relativePath, int lengthOfRelativePath);
NAMESPACE_METHOD_DECLARE(StringBuilder, Append, int, char* stringBegin, int currentLocation, int lengthOfString, const char* subString, int lengthOfSubString);
NAMESPACE_METHOD_DECLARE(StringBuilder, FormatAsPath, int, char* stringBegin, int currentLocation, int lengthOfString, const char* formatString, ...);
NAMESPACE_METHOD_DECLARE(StringBuilder, Format, int, char* stringBegin, int currentLocation, int lengthOfString, const char* formatString, ...);
NAMESPACE_METHOD_DECLARE(StringBuilder, VFormat, int, char* stringBegin, int currentLocation, int lengthOfString, bool formatAsPath, const char* formatString, va_list args);
NAMESPACE_METHOD_DECLARE(StringBuilder, Concatenate, int, char* stringBegin, int currentLocation, int lengthOfString, const char* subString, int lengthOfSubString, bool appendAsPath, int (*changeCase)(int));

NAMESPACE(StringBuilder)
{
    NAMESPACE_METHOD_DEFINE(StringBuilder, ToUpper);
    NAMESPACE_METHOD_DEFINE(StringBuilder, ToLower);
    NAMESPACE_METHOD_DEFINE(StringBuilder, AppendAsPath);
    NAMESPACE_METHOD_DEFINE(StringBuilder, Append);
    NAMESPACE_METHOD_DEFINE(StringBuilder, FormatAsPath);
    NAMESPACE_METHOD_DEFINE(StringBuilder, Format);
    NAMESPACE_METHOD_DEFINE(StringBuilder, VFormat);
    NAMESPACE_METHOD_DEFINE(StringBuilder, Concatenate);
};


#endif /* __STRING_BUILDER_H__ */
