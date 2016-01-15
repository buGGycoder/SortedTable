/*
 * Written by Tony Chow
 *
 * It leverages the struct and function pointer to simulate the encapulation of the C++ programming language.
 * It supports two levels of encapulations, namespace and class.
 * It doesn't supports embedded namespace. It do supports static methods and member methods.
 *
 * -- 1/17/2015: upgrade to version 2 to support NO_INSTANTIAL_INVOCATION
 *
 * Demostramtion for namespace
 * ===========================
 *
 * in Demo.h
 * ---------
 *
 *     #include <stdio.h>
 *     #include "ClassSimulator.h"
 *
 *     NAMESPACE_METHOD_DECLARE(Demo, some_fun, void*, int a, int b);
 *
 *     NAMESPACE(Demo)
 *     {
 *         NAMESPACE_METHOD_DEFINE(Demo, some_fun);
 *     };
 *
 *
 * in Demo.c
 * ---------
 *
 *     #include "Demo.h"
 *
 *     NAMESPACE_METHOD_IMPLEMENT(Demo, some_fun, void*, int a, int b)
 *     {
 *         printf("some method belongs to namespace Demo gets %d\n", a + b);
 *         return NULL;
 *     }
 *
 *     NAMESPACE_INITIALIZE(Demo)
 *     {
 *         NAMESPACE_METHOD_INITIALIZE(Demo, some_fun)
 *     };
 *
 *
 * in main.c
 * ---------
 *
 *     #include "Demo.h"
 *
 *     int main()
 *     {
 *         Demo.some_fun(2,3);
 *         return 0;
 *     }
 *
 *
 * Demostration for class
 * ======================
 *
 * in Demo.h
 * ---------
 *
 *     #include "ClassSimulator.h"
 *
 *     STATIC_FIELD_DECLARE(Demo, s, int);
 *     STATIC_METHOD_DECLARE(Demo, static_func, void*, int a, int b);
 *     MEMBER_METHOD_DECLARE(Demo, member_func, void*, int a, int b);
 *
 *     CLASS(Demo)
 *     {
 *         int a;
 *         int b;
 *
 *         STATIC_FIELD_DEFINE(Demo, s);
 *         STATIC_METHOD_DEFINE(Demo, static_func);
 *         MEMBER_METHOD_DEFINE(Demo, member_func);
 *
 *     };
 *
 *
 * in Demo.c
 * ---------
 *
 *     #include <stdio.h>
 *     #include "Demo.h"
 *
 *     STATIC_METHOD_IMPLEMENT(Demo, static_func, void*, int a, int b)
 *     {
 *         printf("static method gets %d\n", a + b);
 *         return NULL;
 *     }
 *
 *     MEMBER_METHOD_IMPLEMENT(Demo, member_func, void*, int a, int b)
 *     {
 *         thiz->a = a;
 *         thiz->b = b;
 *         printf("member method gets %d\n", thiz->a + thiz->b);
 *         return NULL;
 *     }
 *
 *     MEMBER_METHOD_IMPLEMENT(Demo, Demo, Demo*, INSTANCE_MANAGING method)
 *     {
 *         switch(method)
 *         {
 *         case METHOD_CTOR:
 *             break;
 *         case METHOD_DTOR:
 *             break;
 *         case METHOD_ADDREF:
 *         case METHOD_RELEASE:
 *         case METHOD_CLONE:
 *         default:
 *             return NULL;
 *         }
 *
 *         return thiz;
 *     }
 *
 *     STATIC_FIELD_IMPLEMENT(Demo, s);
 *
 *     CLASS_INITIALIZE(Demo)
 *     {
 *         STATIC_FIELD_INITIALIZE(Demo, s)
 *         STATIC_METHOD_INITIALIZE(Demo, static_func)
 *         MEMBER_METHOD_INITIALIZE(Demo, member_func)
 *     };
 *
 *
 * in main.c
 * ---------
 *
 *     #include <stdio.h>
 *     #include "Demo.h"
 *
 *     int main()
 *     {
 *         Demo *p = I_VAR(Demo);
 *         Demo *p2 = I_NEW(Demo);
 *         p->member_func(p, 2, 3);
 *         MEMBER_METHOD_CALL(Demo, member_func, p, 2, 3);
 *         p->static_func(2, 3);
 *         STATIC_METHOD_CALL(Demo, static_func, p, 2, 3);
 *         *p->s = 1000;
 *         *p2->s = 2000;
 *         printf("p->s: %d\n", *p->s); // here the value is 2000 since it is static variable.
 *         printf("a: %d, b: %d\n", p->a, p->b); // a: 2, b: 3
 *         I_DELETE(Demo, p2); // only delete p2. p doesn't need to be freed since it is allocated on stack.
 *         return 0;
 *     }
 *
 */

#ifndef __CLASS_SIMULATOR_H__
#define __CLASS_SIMULATOR_H__


#include <memory.h>
#include <stdlib.h>
#include <string.h>

/* Instance Managing Method*/
typedef enum
{
    METHOD_UNKNOWN = 0,
    METHOD_CTOR,
    METHOD_DTOR,
    METHOD_ADDREF,
    METHOD_RELEASE,
    METHOD_CLONE,
} INSTANCE_MANAGING;


#ifdef INLINE
#undef INLINE
#endif

#if __STDC__
#define INLINE
#elif defined(_MSC_VER)
# define INLINE __inline
#else
#define INLINE inline
#endif


/********************
 * macros on naming *
 ********************/

#define MEMBER_METHOD_TYPE_NAME(className,methodName) TYPE_##className##_##methodName
#define MEMBER_METHOD_GLOBAL_NAME(className,methodName) className##_##methodName
#define MEMBER_METHOD_LOCAL_NAME(methodName) methodName

#define STATIC_METHOD_TYPE_NAME(className,methodName) TYPE_##className##_##methodName
#define STATIC_METHOD_GLOBAL_NAME(className,methodName) className##_##methodName
#define STATIC_METHOD_LOCAL_NAME(methodName) methodName

#define INTERFACE_MEMBER_METHOD_TYPE_NAME(className,methodName) TYPE_##className##_##methodName
#define INTERFACE_MEMBER_METHOD_GLOBAL_NAME(className,methodName) className##_##methodName
#define INTERFACE_MEMBER_METHOD_LOCAL_NAME(methodName) methodName

#define INTERFACE_STATIC_METHOD_TYPE_NAME(className,methodName) TYPE_##className##_##methodName
#define INTERFACE_STATIC_METHOD_GLOBAL_NAME(className,methodName) className##_##methodName
#define INTERFACE_STATIC_METHOD_LOCAL_NAME(methodName) methodName

#define NAMESPACE_METHOD_TYPE_NAME(namespaceName,methodName) TYPE_##namespaceName##_##methodName
#define NAMESPACE_METHOD_GLOBAL_NAME(namespaceName,methodName) namespaceName##_##methodName
#define NAMESPACE_METHOD_LOCAL_NAME(methodName) methodName

#define STATIC_FIELD_TYPE_NAME(className,fieldName) TYPE_##className##_##fieldName
#define STATIC_FIELD_GLOBAL_NAME(className,fieldName) className##_##fieldName
#define STATIC_FIELD_LOCAL_NAME(fieldName) fieldName

#define CLASS_DEFAULT_INSTANCE_NAME(className) className##_


/*******************************
 * macros on declaring methods *
 *******************************/

#define TEMPLATE_MEMBER_METHOD_DECLARE(templateClassName,templateMethodName,className,methodName) \
    extern MEMBER_METHOD_TYPE_NAME(templateClassName,templateMethodName) MEMBER_METHOD_GLOBAL_NAME(className,methodName)

#define TEMPLATE_STATIC_METHOD_DECLARE(templateClassName,templateMethodName,className,methodName) \
    extern STATIC_METHOD_TYPE_NAME(templateClassName,templateMethodName) STATIC_METHOD_GLOBAL_NAME(className,methodName)

#define TEMPLATE_INTERFACE_MEMBER_METHOD_DECLARE(templateClassName,templateMethodName,className,methodName) \
    extern INTERFACE_MEMBER_METHOD_TYPE_NAME(templateClassName,templateMethodName) INTERFACE_MEMBER_METHOD_GLOBAL_NAME(className,methodName)

#define TEMPLATE_INTERFACE_STATIC_METHOD_DECLARE(templateClassName,templateMethodName,className,methodName) \
    extern INTERFACE_STATIC_METHOD_TYPE_NAME(templateClassName,templateMethodName) INTERFACE_STATIC_METHOD_GLOBAL_NAME(className,methodName)

#define TEMPLATE_NAMESPACE_METHOD_DECLARE(templateNamespaceName,templateMethodName,namespaceName,methodName) \
    extern NAMESPACE_METHOD_TYPE_NAME(templateNamespaceName,templateMethodName) NAMESPACE_METHOD_GLOBAL_NAME(namespaceName,methodName)


/******************************
 * macros on invoking methods *
 ******************************/

#define NONINSTANTIAL_MEMBER_METHOD_CALL(className,methodName,instance,...) MEMBER_METHOD_GLOBAL_NAME(className,methodName)((instance),##__VA_ARGS__)

#define NONINSTANTIAL_STATIC_METHOD_CALL(className,methodName,...) STATIC_METHOD_GLOBAL_NAME(className,methodName)(__VA_ARGS__)

#define NONINSTANTIAL_NAMESPACE_METHOD_CALL(namespaceName,methodName,...) NAMESPACE_METHOD_GLOBAL_NAME(namespaceName,methodName)(__VA_ARGS__)


/****************************
 * macros on member methods *
 ****************************/

#define MEMBER_METHOD_DECLARE(className,methodName,returnType,...) \
    typedef struct className className; \
    typedef returnType MEMBER_METHOD_TYPE_NAME(className,methodName)(struct className* thiz,##__VA_ARGS__); \
    TEMPLATE_MEMBER_METHOD_DECLARE(className,methodName,className,methodName)

#ifdef NO_INSTANTIAL_INVOCATION
#define MEMBER_METHOD_DEFINE(className,methodName)
#else
#define MEMBER_METHOD_DEFINE(className,methodName) \
    MEMBER_METHOD_TYPE_NAME(className,methodName)*MEMBER_METHOD_LOCAL_NAME(methodName)
#endif

#define MEMBER_METHOD_IMPLEMENT(className,methodName,returnType,...) \
    struct className; \
    INLINE returnType MEMBER_METHOD_GLOBAL_NAME(className,methodName)(struct className* thiz,##__VA_ARGS__)

#if defined(NO_INSTANTIAL_INVOCATION)
#define MEMBER_METHOD_INITIALIZE(className,methodName)
#elif defined(_WIN32) && defined(__cplusplus)
#define MEMBER_METHOD_INITIALIZE(className,methodName) CLASS_DEFAULT_INSTANCE_NAME(className).MEMBER_METHOD_LOCAL_NAME(methodName) = MEMBER_METHOD_GLOBAL_NAME(className,methodName),
#else
#define MEMBER_METHOD_INITIALIZE(className,methodName) .MEMBER_METHOD_LOCAL_NAME(methodName) = MEMBER_METHOD_GLOBAL_NAME(className,methodName),
#endif

#ifndef NO_INSTANTIAL_INVOCATION
#define MEMBER_METHOD_OVERRIDE(className,methodName,instance) (instance)->MEMBER_METHOD_LOCAL_NAME(methodName) = MEMBER_METHOD_GLOBAL_NAME(className,methodName)
#endif

#ifdef NO_INSTANTIAL_INVOCATION
#define MEMBER_METHOD_CALL(className,methodName,instance,...) MEMBER_METHOD_GLOBAL_NAME(className,methodName)((instance),##__VA_ARGS__)
#else
#define MEMBER_METHOD_CALL(className,methodName,instance,...) (instance)->MEMBER_METHOD_LOCAL_NAME(methodName)((instance),##__VA_ARGS__)
#endif


/****************************
 * macros on static methods *
 ****************************/

#define STATIC_METHOD_DECLARE(className,methodName,returnType,...) \
    typedef struct className className; \
    typedef returnType STATIC_METHOD_TYPE_NAME(className,methodName)(__VA_ARGS__); \
    TEMPLATE_STATIC_METHOD_DECLARE(className,methodName,className,methodName)

#ifdef NO_INSTANTIAL_INVOCATION
#define STATIC_METHOD_DEFINE(className,methodName)
#else
#define STATIC_METHOD_DEFINE(className,methodName) \
    STATIC_METHOD_TYPE_NAME(className,methodName)*STATIC_METHOD_LOCAL_NAME(methodName)
#endif

#define STATIC_METHOD_IMPLEMENT(className,methodName,returnType,...) \
    struct className; \
    INLINE returnType STATIC_METHOD_GLOBAL_NAME(className,methodName)(__VA_ARGS__)

#if defined(NO_INSTANTIAL_INVOCATION)
#define STATIC_METHOD_INITIALIZE(className,methodName)
#elif defined(_WIN32) && defined(__cplusplus)
#define STATIC_METHOD_INITIALIZE(className,methodName) CLASS_DEFAULT_INSTANCE_NAME(className).STATIC_METHOD_LOCAL_NAME(methodName) = STATIC_METHOD_GLOBAL_NAME(className,methodName),
#else
#define STATIC_METHOD_INITIALIZE(className,methodName) .STATIC_METHOD_LOCAL_NAME(methodName) = STATIC_METHOD_GLOBAL_NAME(className,methodName),
#endif

#ifndef NO_INSTANTIAL_INVOCATION
#define STATIC_METHOD_OVERRIDE(className,methodName,instance) (instance)->STATIC_METHOD_LOCAL_NAME(methodName) = STATIC_METHOD_GLOBAL_NAME(className,methodName)
#endif

#ifdef NO_INSTANTIAL_INVOCATION
#define STATIC_METHOD_CALL(className,methodName,instance,...) STATIC_METHOD_GLOBAL_NAME(className,methodName)(__VA_ARGS__)
#else
#define STATIC_METHOD_CALL(className,methodName,instance,...) (instance)->STATIC_METHOD_LOCAL_NAME(methodName)(__VA_ARGS__)
#endif


/**************************************
 * macros on interface member methods *
 **************************************/

#define INTERFACE_MEMBER_METHOD_DECLARE(className,methodName,returnType,...) \
    typedef struct className className; \
    typedef returnType INTERFACE_MEMBER_METHOD_TYPE_NAME(className,methodName)(struct className* thiz,##__VA_ARGS__); \
    TEMPLATE_INTERFACE_MEMBER_METHOD_DECLARE(className,methodName,className,methodName)

#define INTERFACE_MEMBER_METHOD_DEFINE(className,methodName) \
    INTERFACE_MEMBER_METHOD_TYPE_NAME(className,methodName)*INTERFACE_MEMBER_METHOD_LOCAL_NAME(methodName)

#define INTERFACE_MEMBER_METHOD_IMPLEMENT(className,methodName,returnType,...) \
    struct className; \
    INLINE returnType INTERFACE_MEMBER_METHOD_GLOBAL_NAME(className,methodName)(struct className* thiz,##__VA_ARGS__)

#if defined(_WIN32) && defined(__cplusplus)
#define INTERFACE_MEMBER_METHOD_INITIALIZE(className,methodName) CLASS_DEFAULT_INSTANCE_NAME(className).INTERFACE_MEMBER_METHOD_LOCAL_NAME(methodName) = INTERFACE_MEMBER_METHOD_GLOBAL_NAME(className,methodName),
#else
#define INTERFACE_MEMBER_METHOD_INITIALIZE(className,methodName) .INTERFACE_MEMBER_METHOD_LOCAL_NAME(methodName) = INTERFACE_MEMBER_METHOD_GLOBAL_NAME(className,methodName),
#endif

#define INTERFACE_MEMBER_METHOD_OVERRIDE(className,methodName,instance) (instance)->INTERFACE_MEMBER_METHOD_LOCAL_NAME(methodName) = INTERFACE_MEMBER_METHOD_GLOBAL_NAME(className,methodName)

#define INTERFACE_MEMBER_METHOD_CALL(className,methodName,instance,...) (instance)->INTERFACE_MEMBER_METHOD_LOCAL_NAME(methodName)((instance),##__VA_ARGS__)

/**************************************
 * macros on interface static methods *
 **************************************/

#define INTERFACE_STATIC_METHOD_DECLARE(className,methodName,returnType,...) \
    typedef struct className className; \
    typedef returnType INTERFACE_STATIC_METHOD_TYPE_NAME(className,methodName)(__VA_ARGS__); \
    TEMPLATE_INTERFACE_STATIC_METHOD_DECLARE(className,methodName,className,methodName)

#define INTERFACE_STATIC_METHOD_DEFINE(className,methodName) \
    INTERFACE_STATIC_METHOD_TYPE_NAME(className,methodName)*INTERFACE_STATIC_METHOD_LOCAL_NAME(methodName)

#define INTERFACE_STATIC_METHOD_IMPLEMENT(className,methodName,returnType,...) \
    struct className; \
    INLINE returnType INTERFACE_STATIC_METHOD_GLOBAL_NAME(className,methodName)(__VA_ARGS__)

#if defined(_WIN32) && defined(__cplusplus)
#define INTERFACE_STATIC_METHOD_INITIALIZE(className,methodName) CLASS_DEFAULT_INSTANCE_NAME(className).INTERFACE_STATIC_METHOD_LOCAL_NAME(methodName) = INTERFACE_STATIC_METHOD_GLOBAL_NAME(className,methodName),
#else
#define INTERFACE_STATIC_METHOD_INITIALIZE(className,methodName) .INTERFACE_STATIC_METHOD_LOCAL_NAME(methodName) = INTERFACE_STATIC_METHOD_GLOBAL_NAME(className,methodName),
#endif

#define INTERFACE_STATIC_METHOD_OVERRIDE(className,methodName,instance) (instance)->INTERFACE_STATIC_METHOD_LOCAL_NAME(methodName) = INTERFACE_STATIC_METHOD_GLOBAL_NAME(className,methodName)

#define INTERFACE_STATIC_METHOD_CALL(className,methodName,instance,...) (instance)->INTERFACE_STATIC_METHOD_LOCAL_NAME(methodName)(__VA_ARGS__)


/*******************************
 * macros on namespace methods *
 *******************************/

#define NAMESPACE_METHOD_DECLARE(namespaceName,methodName,returnType,...) \
    typedef returnType NAMESPACE_METHOD_TYPE_NAME(namespaceName,methodName)(__VA_ARGS__); \
    TEMPLATE_NAMESPACE_METHOD_DECLARE(namespaceName,methodName,namespaceName,methodName)

#define NAMESPACE_METHOD_DEFINE(namespaceName,methodName) \
    NAMESPACE_METHOD_TYPE_NAME(namespaceName,methodName)*NAMESPACE_METHOD_LOCAL_NAME(methodName)

#define NAMESPACE_METHOD_IMPLEMENT(namespaceName,methodName,returnType,...) \
    INLINE returnType NAMESPACE_METHOD_GLOBAL_NAME(namespaceName,methodName)(__VA_ARGS__)

#if defined(_WIN32) && defined(__cplusplus)
#define NAMESPACE_METHOD_INITIALIZE(namespaceName,methodName) namespaceName##.NAMESPACE_METHOD_LOCAL_NAME(methodName) = NAMESPACE_METHOD_GLOBAL_NAME(namespaceName,methodName),
#else
#define NAMESPACE_METHOD_INITIALIZE(namespaceName,methodName) .NAMESPACE_METHOD_LOCAL_NAME(methodName) = NAMESPACE_METHOD_GLOBAL_NAME(namespaceName,methodName),
#endif

#define NAMESPACE_METHOD_CALL(namespaceName,methodName,...) NAMESPACE_METHOD_GLOBAL_NAME(namespaceName,methodName)(__VA_ARGS__)


/**************************
 * macros on static fields *
 **************************/

#define STATIC_FIELD_DECLARE(className,fieldName,fieldType) \
    typedef fieldType STATIC_FIELD_TYPE_NAME(className,fieldName); \
    extern fieldType STATIC_FIELD_GLOBAL_NAME(className,fieldName)

#define STATIC_FIELD_DEFINE(className,fieldName) \
    STATIC_FIELD_TYPE_NAME(className,fieldName)*STATIC_FIELD_LOCAL_NAME(fieldName)

#define STATIC_FIELD_IMPLEMENT(className,fieldName) \
    STATIC_FIELD_TYPE_NAME(className,fieldName) STATIC_FIELD_GLOBAL_NAME(className,fieldName)

#if defined(_WIN32) && defined(__cplusplus)
#define STATIC_FIELD_INITIALIZE(className,memberName) CLASS_DEFAULT_INSTANCE_NAME(className).STATIC_FIELD_LOCAL_NAME(memberName) = &(STATIC_FIELD_GLOBAL_NAME(className,memberName)),
#else
#define STATIC_FIELD_INITIALIZE(className,memberName) .STATIC_FIELD_LOCAL_NAME(memberName) = &(STATIC_FIELD_GLOBAL_NAME(className,memberName)),
#endif

#define STATIC_FIELD_OVERRIDE(className,memberName,instance) (instance)->STATIC_FIELD_LOCAL_NAME(memberName) = &(STATIC_FIELD_GLOBAL_NAME(className,memberName))

/******************************************
 * macros on strongly typed member fields *
 ******************************************/

#define MEMBER_FIELD_DEFINE(fieldClassName,fieldName) \
    fieldClassName fieldName[1]


/************************
 * macros on declaration *
 ************************/

#define CLASS_DECLARE(className) \
    struct className; \
    typedef struct className className

#define CLASS_DEFINE(className) \
    struct className

#define CLASS(className) \
    CLASS_DECLARE(className); \
    extern className CLASS_DEFAULT_INSTANCE_NAME(className); \
    MEMBER_METHOD_DECLARE(className,className,className*,INSTANCE_MANAGING method); \
    CLASS_DEFINE(className)

#define PLAIN_OLD_DATA(className) \
    CLASS_DECLARE(className); \
    CLASS_DEFINE(className)

#define NAMESPACE(namespaceName) \
    struct namespaceName; \
    extern struct namespaceName namespaceName; \
    struct namespaceName

#define DEFAULT_MANAGING_METHOD_IMPLEMENT(className) INLINE MEMBER_METHOD_IMPLEMENT(className, className, className*, INSTANCE_MANAGING method){return(METHOD_CTOR == method || METHOD_DTOR == method)?thiz:NULL;}

#define CLASS_INITIALIZE(className) struct className CLASS_DEFAULT_INSTANCE_NAME(className) =
#define NAMESPACE_INITIALIZE(namespaceName) struct namespaceName namespaceName =


/***********************
 * macros on lifecycle *
 ***********************/

#define I_CONSTRUCT(className,pointer) MEMBER_METHOD_GLOBAL_NAME(className, className)(pointer,METHOD_CTOR)
#define I_DISPOSE(className,pointer) MEMBER_METHOD_GLOBAL_NAME(className, className)(pointer,METHOD_DTOR)
#define I_ADDREF(className,pointer) MEMBER_METHOD_GLOBAL_NAME(className, className)(pointer,METHOD_ADDREF)
#define I_RELEASE(className,pointer) MEMBER_METHOD_GLOBAL_NAME(className, className)(pointer,METHOD_RELEASE)
#define I_CLONE(className,pointer) MEMBER_METHOD_GLOBAL_NAME(className, className)(pointer,METHOD_CLONE)

#define I_MEMMOVE memmove
#define I_MALLOC malloc
#define I_FREE free
#ifdef _WIN32
#include <malloc.h>
#define I_ALLOCA _alloca
#else
#include <alloca.h>
#define I_ALLOCA alloca
#endif /* _WIN32 */

#define I_VAR(className) I_CONSTRUCT(className,(struct className*)I_MEMMOVE(I_ALLOCA(sizeof(struct className)),&CLASS_DEFAULT_INSTANCE_NAME(className),sizeof(struct className)))
#define I_VARSIZED(className,specificSize) I_CONSTRUCT(className,(struct className*)I_MEMMOVE(I_ALLOCA(specificSize),&CLASS_DEFAULT_INSTANCE_NAME(className),sizeof(struct className)))
#define I_NEW(className) I_CONSTRUCT(className,(struct className*)I_MEMMOVE(I_MALLOC(sizeof(struct className)),&CLASS_DEFAULT_INSTANCE_NAME(className),sizeof(struct className)))
#define I_NEWSIZED(className,specificSize) I_CONSTRUCT(className,(struct className*)I_MEMMOVE(I_MALLOC(specificSize),&CLASS_DEFAULT_INSTANCE_NAME(className),sizeof(struct className)))
#define I_INIT(className,pointer) I_CONSTRUCT(className,(struct className*)I_MEMMOVE((pointer),&CLASS_DEFAULT_INSTANCE_NAME(className),sizeof(struct className)))
#define I_INITSIZED(className,pointer,specificSize) I_CONSTRUCT(className,(struct className*)I_MEMMOVE((pointer),&CLASS_DEFAULT_INSTANCE_NAME(className),sizeof(struct className)))
#define I_RENEW(className,pointer) I_CONSTRUCT(className,(struct className*)I_MEMMOVE(I_DISPOSE(className,(pointer)),&CLASS_DEFAULT_INSTANCE_NAME(className),sizeof(struct className)))
#define I_RENEWSIZED(className,pointer,specificSize) I_CONSTRUCT(className,(struct className*)I_MEMMOVE(I_DISPOSE(className,(pointer)),&CLASS_DEFAULT_INSTANCE_NAME(className),sizeof(struct className)))
#define I_DELETE(className,pointer) I_FREE(I_DISPOSE(className,(pointer)))

/*************
 * assertion *
 *************/

#define STATIC_ASSERT(expression,...) \
    typedef char STATIC_ASSERT[!!sizeof(struct _{char ASSERTION_FAILED__##__VA_ARGS__:(expression)?1:-1;})]

#ifdef RETAIL
#define ASSERT(expression,...) (expression)
#else
#define ASSERT(expression,...) \
    ((expression) || (*(uint8_t*)0 = (printf(__VA_ARGS__) / (((void*)printf - (void*)0) ^ ((void*)printf - (void*)0)))))
#endif /* DEBUG */

/********************
 * in/out parameter *
 ********************/

#define IN
#define OUT
#define INOUT

/**************************
 * macros on integer size *
 **************************/

typedef enum
{
    INTEGER_SCALE_0 = 0,
    INTEGER_SCALE_1 = 1,
    INTEGER_SCALE_2 = 2,
    INTEGER_SCALE_3 = 3,
} INTEGER_SCALE;

#define GET_INTEGER_SCALE(integer) \
            ((uint64_t)((char*)(integer) - (char*)0) <= 0xFFULL \
            ? INTEGER_SCALE_0 \
            : ((uint64_t)((char*)(integer) - (char*)0) <= 0xFFFFULL \
            ? INTEGER_SCALE_1 \
            : ((uint64_t)((char*)(integer) - (char*)0) <= 0xFFFFFFFFULL \
            ? INTEGER_SCALE_2 \
            : INTEGER_SCALE_3)))

#define GET_INTEGER_SIZE_FROM_SCALE(scale) (((char*)1 - (char*)0) << (scale))

/**********************************
 * macros on pointer computations *
 **********************************/

#define POINTER_OFFSET(p,b) ((void*)&((char*)(p))[(char*)(b) - (char*)0])
#define POINTER_DISTANCE(p1,p2) ((char*)(p1) - (char*)(p2))
#define POINTER_U8(p) ((uint8_t*)(p))
#define POINTER_U16(p) ((uint16_t*)(p))
#define POINTER_U32(p) ((uint32_t*)(p))
#define POINTER_U64(p) ((uint64_t*)(p))
#define POINTER_I8(p) ((int8_t*)(p))
#define POINTER_I16(p) ((int16_t*)(p))
#define POINTER_I32(p) ((int32_t*)(p))
#define POINTER_I64(p) ((int64_t*)(p))
#define LVALUE_OF_POINTER_U8(p) (*POINTER_U8(p))
#define LVALUE_OF_POINTER_U16(p) (*POINTER_U16(p))
#define LVALUE_OF_POINTER_U32(p) (*POINTER_U32(p))
#define LVALUE_OF_POINTER_U64(p) (*POINTER_U64(p))
#define LVALUE_OF_POINTER_I8(p) (*POINTER_I8(p))
#define LVALUE_OF_POINTER_I16(p) (*POINTER_I16(p))
#define LVALUE_OF_POINTER_I32(p) (*POINTER_I32(p))
#define LVALUE_OF_POINTER_I64(p) (*POINTER_I64(p))
#define RVALUE_OF_POINTER_U8(p) (0[POINTER_U8(p)])
#define RVALUE_OF_POINTER_U16(p) (0[POINTER_U16(p)])
#define RVALUE_OF_POINTER_U32(p) (0[POINTER_U32(p)])
#define RVALUE_OF_POINTER_U64(p) (0[POINTER_U64(p)])
#define RVALUE_OF_POINTER_I8(p) (0[POINTER_I8(p)])
#define RVALUE_OF_POINTER_I16(p) (0[POINTER_I16(p)])
#define RVALUE_OF_POINTER_I32(p) (0[POINTER_I32(p)])
#define RVALUE_OF_POINTER_I64(p) (0[POINTER_I64(p)])
#define VALUE_OF_POINTER_U8 LVALUE_OF_POINTER_U8
#define VALUE_OF_POINTER_U16 LVALUE_OF_POINTER_U16
#define VALUE_OF_POINTER_U32 LVALUE_OF_POINTER_U32
#define VALUE_OF_POINTER_U64 LVALUE_OF_POINTER_U64
#define VALUE_OF_POINTER_I8 LVALUE_OF_POINTER_I8
#define VALUE_OF_POINTER_I16 LVALUE_OF_POINTER_I16
#define VALUE_OF_POINTER_I32 LVALUE_OF_POINTER_I32
#define VALUE_OF_POINTER_I64 LVALUE_OF_POINTER_I64

#define POINTER_ROUND_UP(pointer,boundaryType) ((void*)(((char*)(pointer) - (char*)0 + sizeof(boundaryType) - 1) & ~(sizeof(boundaryType) - 1)))
#define POINTER_ROUND_DOWN(pointer,boundaryType) ((void*)(((char*)(pointer) - (char*)0 - sizeof(boundaryType) + 1) & ~(sizeof(boundaryType) - 1)))

#define POINTER_ROUND_UP_TO_SCALE(pointer,scale) ((void*)(((char*)(pointer) - (char*)0 + GET_INTEGER_SIZE_FROM_SCALE(scale) - 1) & ~(GET_INTEGER_SIZE_FROM_SCALE(scale) - 1)))
#define POINTER_ROUND_DOWN_TO_SCALE(pointer,scale) ((void*)(((char*)(pointer) - (char*)0 - GET_INTEGER_SIZE_FROM_SCALE(scale) + 1) & ~(GET_INTEGER_SIZE_FROM_SCALE(scale) - 1)))

#define OFFSET_OF(className,memberName) ((char*)(((className*)0)->memberName) - (char*)0)
#define SIZE_OF(className,memberName) (sizeof(((className*)0)->memberName))

/*******************************
 * macros on directional steps *
 *******************************/

typedef enum
{
    DIRECTIONAL_ONE_STEP_BACKWARD = -1,
    DIRECTIONAL_ONE_STEP_FORWARD = +1,
} DIRECTIONAL_ONE_STEP;


#endif /*__CLASS_SIMULATOR_H__ */
