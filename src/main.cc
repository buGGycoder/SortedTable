#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*#include "../idl/gen/HierarchicalStore.pb-c.h"*/
#define SC_DEBUG


#include "Table.h"
#include "pthread.h"
#include "TimeInterval.h"
#include "TimeZone.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "Table.h"
#include "SizedBuffer.h"
#include "SizedString.h"
#include "HierarchicalStore.h"
#ifdef __cplusplus
}
#endif


/*#include <mysql/mysql.h>*/

volatile bool e = false;

#pragma pack(push)
#pragma pack(1)
PLAIN_OLD_DATA(_TableSyncContext2)
{
    volatile uint64_t activeTableSelector : 1;
    volatile int32_t spinLock;
    MEMBER_FIELD_DEFINE(NonBlockingStateMachine, stateMachine);
};
#pragma pack(pop)


void *thr_fn3(void *arg)
{
    Table* tbl = (Table*) arg;
    tbl->Set(tbl, 5000000LL, "/abc/", sizeof("/abc/") - 1, "/xyz/", sizeof("/xyz/") - 1, 0, true, NULL);
    /*tbl->Set(tbl, 5000000LL, "/abc/def/", sizeof("/abc/def/") - 1, "/xyz/", sizeof("/xyz/") - 1, 0, true, NULL);
    tbl->Set(tbl, 5000000LL, "/abc/def/123", sizeof("/abc/def/123") - 1, "/xyz/", sizeof("/xyz/") - 1, 0, true, NULL);
    tbl->Set(tbl, 5000000LL, "/abc/xyz/", sizeof("/abc/xyz/") - 1, "/xyz/", sizeof("/xyz/") - 1, 0, true, NULL);
    tbl->Set(tbl, 5000000LL, "/abc/xyz/789", sizeof("/abc/xyz/789") - 1, "/xyz/", sizeof("/xyz/") - 1, 0, true, NULL);*/
    uint64_t count = 0;
    _KeyValue** kv = 0;
    tbl->FindPath(tbl, 5000000LL, "/abc/", sizeof("/abc/") - 1, FIND_SCOPE_BASE, 0, 0, false, &count, &kv);
    int64_t i = 0;
    for(; i < count; ++i)
    {
        _KeyValue* data = kv[i];
        printf("%s, %s\n", SizeString.Data(KeyValue.KeyUnsafe(data)), SizeBuffer.Data(KeyValue.ValueUnsafe(data)));
    }


}

void *thr_fn4(void *arg)
{
}

void *thr_fn5(void* arg)
{
}

void *thr_fn1(void *arg)
{
    thr_fn3(arg);
    return NULL;
    /*thr_fn2(arg);
    return NULL;*/

    Table* tbl = (Table*) arg;

    int64_t i = 0;
    _SizedBuffer* buffer = SizedBuffer.New(1024 * 1024 * 10);
    for (; i < 100LL; ++i)
    {
        SizedBuffer.SetSize(buffer, 0);
        int j = 0;
        for (; j < 100; ++j)
        {
            SizedBuffer.AppendFormat(buffer, "%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld", i,i,i,i,i,i,i,i,i,i);
        }

        SizedBuffer.AppendFormat(buffer, "\n");
        tbl->Set(tbl, 5000000LL, buffer->data, 1 + SizedBuffer.Size(buffer), buffer->data, 1 + SizedBuffer.Size(buffer), 0, true, NULL);
        _SizedString* v = NULL;
        ERROR_CODE  err = tbl->Get(tbl, 5000000LL, buffer->data, 1 + SizedBuffer.Size(buffer), &v, NULL, NULL, NULL);
        if (!v)
        {
            /*printf("get error:  %d\n", err);*/
        }
        else
        {
            /*if (strncmp(v, buffer, SizedString.Size(v)))
            {
                printf("%s %s\n", buffer, v);
            }*/

            SizedString.Delete(v);
        }

        if ((TimeZone_.NowByUtcTotalMicroseconds() & 0x1) == (i & 0x1))
        {
            tbl->Delete(tbl, 5000000LL, buffer->data, 1 + SizedBuffer.Size(buffer), 0, true);
        }
    }

    SizedBuffer.Delete(buffer);
}

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

//#include <hiredis/hiredis.h>

#ifdef __cplusplus
}
#endif

#if FALSE

void *thr_fn2(void *arg)
{
    int timeout = 5000;
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = timeout * 1000;
    redisContext* c = redisConnect((char*)"127.0.0.1", 6379);
    if (c->err)
    {
        printf("Failed to connect.\n");
        redisFree(c);
        return NULL;
    }

    int64_t i = 0;
    _SizedBuffer* buffer = SizedBuffer.New(1024 * 1024 * 10);
    _SizedBuffer* cmd = SizedBuffer.New(1024 * 1024 * 10);
    for (; i < 100LL; ++i)
    {
        SizedBuffer.SetSize(buffer, 0);
        int j = 0;
        for (; j < 100; ++j)
        {
            SizedBuffer.AppendFormat(buffer, "%lld%lld%lld%lld%lld%lld%lld%lld%lld%lld", i,i,i,i,i,i,i,i,i,i);
        }

        SizedBuffer.SetSize(cmd, 0);
        SizedBuffer.AppendFormat(cmd, "set %s %s", buffer, buffer);
        redisReply* r = (redisReply*)redisCommand(c,cmd->data);
        if (NULL == r)
        {
            printf("Failed to execute command[%s].\n",cmd);
            redisFree(c);
            return NULL;
        }

        if (!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str,"OK") == 0))
        {
            printf("Failed to execute command[%s].\n",cmd);
            freeReplyObject(r);
            redisFree(c);
            return NULL;
        }

        freeReplyObject(r);

        SizedBuffer.SetSize(cmd, 0);
        SizedBuffer.AppendFormat(cmd, "get %s", buffer);
        r = (redisReply*)redisCommand(c,cmd->data);
        if (r->type != REDIS_REPLY_STRING)
        {
            printf("Failed to execute command[%s].\n",cmd);
            freeReplyObject(r);
            redisFree(c);
            return NULL;
        }

        freeReplyObject(r);
    }

    SizedBuffer.Delete(buffer);
    SizedBuffer.Delete(cmd);
    return NULL;
}



void doTest()
{
    int timeout = 10000;
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = timeout * 1000;
    /*//以带有超时的方式链接Redis服务器，同时获取与Redis连接的上下文对象。
    //该对象将用于其后所有与Redis操作的函数。*/
    redisContext* c = redisConnect((char*)"127.0.0.1", 6379);
    if (c->err)
    {
        redisFree(c);
        return;
    }
    const char* command1 = "set stest1 value9";
    redisReply* r = (redisReply*)redisCommand(c,command1);
    /*//需要注意的是，如果返回的对象是NULL，则表示客户端和服务器之间出现严重错误，必须重新链接。
    //这里只是举例说明，简便起见，后面的命令就不再做这样的判断了。*/
    if (NULL == r)
    {
        redisFree(c);
        return;
    }
    /*//不同的Redis命令返回的数据类型不同，在获取之前需要先判断它的实际类型。
    //至于各种命令的返回值信息，可以参考Redis的官方文档，或者查看该系列博客的前几篇
    //有关Redis各种数据类型的博客。:)
    //字符串类型的set命令的返回值的类型是REDIS_REPLY_STATUS，然后只有当返回信息是"OK"
    //时，才表示该命令执行成功。后面的例子以此类推，就不再过多赘述了。*/
    if (!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str,"OK") == 0))
    {
        printf("Failed to execute command[%s].\n",command1);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    /*//由于后面重复使用该变量，所以需要提前释放，否则内存泄漏。*/
    freeReplyObject(r);
    printf("Succeed to execute command[%s].\n",command1);

    const char* command2 = "strlen stest1";
    r = (redisReply*)redisCommand(c,command2);
    if (r->type != REDIS_REPLY_INTEGER)
    {
        printf("Failed to execute command[%s].\n",command2);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    int length = r->integer;
    freeReplyObject(r);
    printf("The length of 'stest1' is %d.\n",length);
    printf("Succeed to execute command[%s].\n",command2);

    const char* command3 = "get stest1";
    r = (redisReply*)redisCommand(c,command3);
    if (r->type != REDIS_REPLY_STRING)
    {
        printf("Failed to execute command[%s].\n",command3);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    printf("The value of 'stest1' is %s.\n",r->str);
    freeReplyObject(r);
    printf("Succeed to execute command[%s].\n",command3);

    const char* command4 = "get stest2";
    r = (redisReply*)redisCommand(c,command4);
    /*//这里需要先说明一下，由于stest2键并不存在，因此Redis会返回空结果，这里只是为了演示。*/
    if (r->type != REDIS_REPLY_NIL)
    {
        printf("Failed to execute command[%s].\n",command4);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    freeReplyObject(r);
    printf("Succeed to execute command[%s].\n",command4);

    const char* command5 = "mget stest1 stest2";
    r = (redisReply*)redisCommand(c,command5);
    /*//不论stest2存在与否，Redis都会给出结果，只是第二个值为nil。
    //由于有多个值返回，因为返回应答的类型是数组类型。*/
    if (r->type != REDIS_REPLY_ARRAY)
    {
        printf("Failed to execute command[%s].\n",command5);
        freeReplyObject(r);
        redisFree(c);
        /*//r->elements表示子元素的数量，不管请求的key是否存在，该值都等于请求是键的数量。*/
        assert(2 == r->elements);
        return;
    }
    int i;
    for (i = 0; i < r->elements; ++i)
    {
        redisReply* childReply = r->element[i];
        /*//之前已经介绍过，get命令返回的数据类型是string。
        //对于不存在key的返回值，其类型为REDIS_REPLY_NIL。*/
        if (childReply->type == REDIS_REPLY_STRING)
            printf("The value is %s.\n",childReply->str);
    }
    /*//对于每一个子应答，无需使用者单独释放，只需释放最外部的redisReply即可。*/
    freeReplyObject(r);
    printf("Succeed to execute command[%s].\n",command5);

    printf("Begin to test pipeline.\n");
    /*//该命令只是将待发送的命令写入到上下文对象的输出缓冲区中，直到调用后面的
    //redisGetReply命令才会批量将缓冲区中的命令写出到Redis服务器。这样可以
    //有效的减少客户端与服务器之间的同步等候时间，以及网络IO引起的延迟。
    //至于管线的具体性能优势，可以考虑该系列博客中的管线主题。*/
    /* if (REDIS_OK != redisAppendCommand(c,command1)
         || REDIS_OK != redisAppendCommand(c,command2)
         || REDIS_OK != redisAppendCommand(c,command3)
         || REDIS_OK != redisAppendCommand(c,command4)
         || REDIS_OK != redisAppendCommand(c,command5)) {
         redisFree(c);
         return;
     }
    */

    redisAppendCommand(c,command1);
    redisAppendCommand(c,command2);
    redisAppendCommand(c,command3);
    redisAppendCommand(c,command4);
    redisAppendCommand(c,command5);
    redisReply* reply = NULL;
    /*//对pipeline返回结果的处理方式，和前面代码的处理方式完全一直，这里就不再重复给出了。*/
    if (REDIS_OK != redisGetReply(c,(void**)&reply))
    {
        printf("Failed to execute command[%s] with Pipeline.\n",command1);
        freeReplyObject(reply);
        redisFree(c);
    }
    freeReplyObject(reply);
    printf("Succeed to execute command[%s] with Pipeline.\n",command1);

    if (REDIS_OK != redisGetReply(c,(void**)&reply))
    {
        printf("Failed to execute command[%s] with Pipeline.\n",command2);
        freeReplyObject(reply);
        redisFree(c);
    }
    freeReplyObject(reply);
    printf("Succeed to execute command[%s] with Pipeline.\n",command2);

    if (REDIS_OK != redisGetReply(c,(void**)&reply))
    {
        printf("Failed to execute command[%s] with Pipeline.\n",command3);
        freeReplyObject(reply);
        redisFree(c);
    }
    freeReplyObject(reply);
    printf("Succeed to execute command[%s] with Pipeline.\n",command3);

    if (REDIS_OK != redisGetReply(c,(void**)&reply))
    {
        printf("Failed to execute command[%s] with Pipeline.\n",command4);
        freeReplyObject(reply);
        redisFree(c);
    }
    freeReplyObject(reply);
    printf("Succeed to execute command[%s] with Pipeline.\n",command4);

    if (REDIS_OK != redisGetReply(c,(void**)&reply))
    {
        printf("Failed to execute command[%s] with Pipeline.\n",command5);
        freeReplyObject(reply);
        redisFree(c);
    }
    freeReplyObject(reply);
    printf("Succeed to execute command[%s] with Pipeline.\n",command5);
    /*//由于所有通过pipeline提交的命令结果均已为返回，如果此时继续调用redisGetReply，
    //将会导致该函数阻塞并挂起当前线程，直到有新的通过管线提交的命令结果返回。
    //最后不要忘记在退出前释放当前连接的上下文对象。*/
    redisFree(c);
    return;
}

#endif // FALSE

int main()
{
    Table* tbl = I_NEW(Table);
    tbl->Initialize(tbl, "/tmp");

    uint64_t t1 = TimeZone_.NowByUtcTotalMicroseconds();

    int err;
    pthread_t thr[1] = {0};
    int i = 0;
    for (; i < sizeof(thr) / sizeof(thr[0]); ++i)
    {
        err = pthread_create(&thr[i], NULL, thr_fn1, tbl);
        if (err != 0)
        {
            printf("can't create thread: %s\n", strerror(err));
            return -1;
        }
    }

    for (i = 0; i < sizeof(thr) / sizeof(thr[0]); ++i)
    {
        pthread_join(thr[i],NULL);
    }

    uint64_t t2 = TimeZone_.NowByUtcTotalMicroseconds();
    TimeInterval* ti = I_NEW(TimeInterval);
    ti->InitializeByTotalMicroseconds(ti, t2 - t1);
    printf("%s\n", ti->ToString(ti));
    I_DELETE(TimeInterval, ti);
    tbl->Close(tbl);
    I_DELETE(Table, tbl);
    getchar();
    return 0;
}
