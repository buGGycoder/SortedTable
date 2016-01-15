#include "TableFile.h"
#include "SizedString.h"
#include "KeyValue.h"
#include "File.h"

MEMBER_METHOD_IMPLEMENT(TableFile, Initialize, ERROR_CODE, const char* path, uint64_t size)
{
    if (!size)
    {
        if (!File.Detect(path))
        {
            return ERROR_NOT_FOUND;
        }

        size = File.Size(path);
    }

    if (!thiz->file->Initialize(thiz->file, path, NULL, 0, size, false, true))
    {
        return ERROR_ACCESS_DENIED;
    }

    if (!(thiz->table = SortedTable.Construct(thiz->file->MemoryMap(thiz->file), size, false, false)))
    {
        thiz->file->Unlock(thiz->file);
        return ERROR_CORRUPT_DATA;
    }

    thiz->file->Unlock(thiz->file);
    thiz->path = SizedString.New(path, -1);
    thiz->size = size;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(TableFile, Close, ERROR_CODE)
{
    if (!thiz->file->Close(thiz->file))
    {
        return ERROR_UNKNOWN;
    }

    thiz->table = NULL;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(TableFile, Path, const char*)
{
    return thiz->path;
}

MEMBER_METHOD_IMPLEMENT(TableFile, Size, uint64_t)
{
    return thiz->size;
}

MEMBER_METHOD_IMPLEMENT(TableFile, ExtendSizeTo, ERROR_CODE, uint64_t size)
{
    if (thiz->size >= size)
    {
        return ERROR_ARGUMENT_OUT_OF_RANGE;
    }

    I_RENEW(FileMapping, thiz->file);
    if (!thiz->file->Initialize(thiz->file, thiz->path, NULL, 0, size, false, true))
    {
        return ERROR_ACCESS_DENIED;
    }

    if (!(thiz->table = SortedTable.Construct(thiz->file->MemoryMap(thiz->file), size, false, false)))
    {
        thiz->file->Unlock(thiz->file);
        return ERROR_CORRUPT_DATA;
    }

    thiz->file->Unlock(thiz->file);
    thiz->size = size;
    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(TableFile, Reset, ERROR_CODE)
{
    if (!(thiz->table = SortedTable.Construct(thiz->file->MemoryMap(thiz->file), thiz->size, true, false)))
    {
        return ERROR_CORRUPT_DATA;
    }

    return ERROR_NO;
}

MEMBER_METHOD_IMPLEMENT(TableFile, File, FileMapping*)
{
    return thiz->file;
}

MEMBER_METHOD_IMPLEMENT(TableFile, Table, _SortedTable*)
{
    return thiz->table;
}

MEMBER_METHOD_IMPLEMENT(TableFile, TableFile, TableFile*, INSTANCE_MANAGING method)
{
    switch(method)
    {
    case METHOD_CTOR:
        I_INIT(FileMapping, thiz->file);
        break;
    case METHOD_DTOR:
        I_DISPOSE(FileMapping, thiz->file);
        if (thiz->path)
        {
            SizedString.Delete(thiz->path);
            thiz->path = NULL;
        }

        break;
    case METHOD_ADDREF:
    case METHOD_RELEASE:
    case METHOD_CLONE:
    default:
        return NULL;
    }

    return thiz;
}

CLASS_INITIALIZE(TableFile)
{
    NAMESPACE_METHOD_INITIALIZE(TableFile, Initialize)
    NAMESPACE_METHOD_INITIALIZE(TableFile, Close)
    NAMESPACE_METHOD_INITIALIZE(TableFile, Path)
    NAMESPACE_METHOD_INITIALIZE(TableFile, ExtendSizeTo)
    NAMESPACE_METHOD_INITIALIZE(TableFile, Size)
    NAMESPACE_METHOD_INITIALIZE(TableFile, File)
    NAMESPACE_METHOD_INITIALIZE(TableFile, Table)
};
