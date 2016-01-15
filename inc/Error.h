#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdint.h>

typedef int64_t ERROR_CODE;

#define MAKE_ERROR(error, permanent, code) ((ERROR_CODE)(((uint64_t)(1 & (uint8_t)(error)) << 63) | ((uint64_t)(1 & (uint8_t)(permanent)) << 62) | (0x3FFFFFFFFFFFFFFFULL & (uint64_t)(code))))

#define IS_SUCCEEDED(e) (0 <= ((ERROR_CODE)(ec)))
#define IS_FAILED(e) (0 > ((ERROR_CODE)(ec)))
#define IS_PERMANENT_FAILURE(e) (!!(1 & ((uint64_t)(e) >> 62)))
#define IS_TRANSIENT_FAILURE(e) (!(1 & ((uint64_t)(e) >> 62)))

#define ERROR_NO                        MAKE_ERROR(0, 0, 0)
#define ERROR_UNKNOWN                   MAKE_ERROR(1, 1, -1)

#define ERROR_CONTINUE                  MAKE_ERROR(0, 0, 1)
#define ERROR_PENDING                   MAKE_ERROR(0, 0, 2)
#define ERROR_PARTIAL_COMPLETE          MAKE_ERROR(0, 0, 3)
#define ERROR_NOT_IMPLEMENTED           MAKE_ERROR(1, 1, 4)
#define ERROR_MACHINE_ILL               MAKE_ERROR(1, 1, 5)
#define ERROR_SHUTTING_DOWN             MAKE_ERROR(1, 1, 6)
#define ERROR_NOT_FOUND                 MAKE_ERROR(1, 1, 7)
#define ERROR_OUT_OF_MEMORY             MAKE_ERROR(1, 1, 8)
#define ERROR_NOT_INITIALIZED           MAKE_ERROR(1, 1, 9)
#define ERROR_CONFIGURATION_CHANGED     MAKE_ERROR(1, 0, 10)
#define ERROR_INVALID_STATE             MAKE_ERROR(1, 1, 11)
#define ERROR_SIZE_TOO_BIG              MAKE_ERROR(1, 1, 12)
#define ERROR_ARGUMENT_NULL             MAKE_ERROR(1, 1, 13)
#define ERROR_ARGUMENT_OUT_OF_RANGE     MAKE_ERROR(1, 1, 14)
#define ERROR_ACCESS_DENIED             MAKE_ERROR(1, 1, 15)
#define ERROR_CORRUPT_DATA              MAKE_ERROR(1, 1, 16)
#define ERROR_MULTIPLE_MATCH            MAKE_ERROR(1, 1, 17)
#define ERROR_NO_QUOTA                  MAKE_ERROR(1, 1, 18)
#define ERROR_RESOUCE_REF               MAKE_ERROR(1, 0, 19)
#define ERROR_BUSY                      MAKE_ERROR(1, 0, 20)
#define ERROR_TIMEOUT                   MAKE_ERROR(1, 0, 21)
#define ERROR_WRONG_VERSION             MAKE_ERROR(1, 1, 22)
#define ERROR_NOT_ENOUGH_MEMORY         MAKE_ERROR(1, 1, 23)

#endif /* __ERROR_H__ */
