#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

typedef void (*threadpool_work_function)(void *arg);

typedef struct
{
    threadpool_work_function Function;
    void *Argument;
} threadpool_work;

typedef struct
{
    threadpool_work *Work;
    size_t Size;

    uint32_t volatile WorkReadIndex;
    uint32_t volatile WorkWriteIndex;

    pthread_mutex_t  WorkMutex;
    pthread_cond_t   WorkCondition;
    pthread_cond_t   WorkingCondition;

    size_t           WorkingCount;
    size_t           ThreadCount;

    bool             Stop;
} threadpool;

threadpool *ThreadpoolCreate(size_t NumberOfThreads, size_t WorkSize);
void ThreadpoolDelete(threadpool *Threadpool);

bool ThreadpoolAddWork(threadpool *Threadpool, threadpool_work_function Function, void *Argument);
void ThreadpoolWait(threadpool *Threadpool);

#endif // THREADPOOL_H