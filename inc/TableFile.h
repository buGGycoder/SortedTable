#ifndef __TABLEFILE_H__
#define __TABLEFILE_H__

#include <stdint.h>
#include "ClassSimulator.h"
#include "Error.h"
#include "SortedTable.h"
#include "FileMapping.h"

MEMBER_METHOD_DECLARE(TableFile, Initialize, ERROR_CODE, const char* path, uint64_t size);
MEMBER_METHOD_DECLARE(TableFile, Close, ERROR_CODE);
MEMBER_METHOD_DECLARE(TableFile, Path, const char*);
MEMBER_METHOD_DECLARE(TableFile, Size, uint64_t);
MEMBER_METHOD_DECLARE(TableFile, ExtendSizeTo, ERROR_CODE, uint64_t size);
MEMBER_METHOD_DECLARE(TableFile, File, FileMapping*);
MEMBER_METHOD_DECLARE(TableFile, Table, _SortedTable*);

CLASS(TableFile)
{
    NAMESPACE_METHOD_DEFINE(TableFile, Initialize);
    NAMESPACE_METHOD_DEFINE(TableFile, Close);
    NAMESPACE_METHOD_DEFINE(TableFile, Path);
    NAMESPACE_METHOD_DEFINE(TableFile, Size);
    NAMESPACE_METHOD_DEFINE(TableFile, ExtendSizeTo);
    NAMESPACE_METHOD_DEFINE(TableFile, File);
    NAMESPACE_METHOD_DEFINE(TableFile, Table);

    _SizedString* path;
    uint64_t size;
    MEMBER_FIELD_DEFINE(FileMapping, file);
    _SortedTable* table;
};

#endif /* __TABLEFILE_H__ */
