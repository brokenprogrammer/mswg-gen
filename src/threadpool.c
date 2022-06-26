#include "threadpool.h"

bool
_IsEmpty(threadpool* Threadpool)
{
    return ((Threadpool->WorkWriteIndex - Threadpool->WorkReadIndex) == 0);
}

bool
_HasWorkInQueue(threadpool* Threadpool)
{
    return ((Threadpool->WorkWriteIndex - Threadpool->WorkReadIndex) != 0);
}

typedef struct
{
    threadpool_work_function Function;
    void *Argument;
    bool Success;
} threadpool_work_copy;

// NOTE(Oskar): Helper function to pull the next work item from the queue.
threadpool_work_copy
_ThreadpoolGetWork(threadpool *Threadpool)
{
    threadpool_work_copy Work = { 0 };
    Work.Success = false;

    if (Threadpool == NULL)
    {
        return Work;
    }

    if ((Threadpool->WorkWriteIndex - Threadpool->WorkReadIndex) == 0)
    {
        return Work;
    }

    uint32_t OriginalNextEntryToRead = Threadpool->WorkReadIndex;
    uint32_t NewNextEntryToRead = (OriginalNextEntryToRead + 1) % Threadpool->Size;
    if(OriginalNextEntryToRead != Threadpool->WorkWriteIndex)
    {
        threadpool_work *WorkPtr = Threadpool->Work + Threadpool->WorkReadIndex;
        Work.Function = WorkPtr->Function;
        Work.Argument = WorkPtr->Argument;
        Work.Success = true;

        Threadpool->WorkReadIndex = NewNextEntryToRead;
    }

    return Work;
}

// NOTE(Oskar): Thread worker function.
void *
_ThreadpoolWorker(void *Argument)
{
    threadpool *Threadpool = Argument;
    threadpool_work_copy Work;

    while (1) 
    {
        // NOTE(Oskar): Lock to prevent anything else to use threadpool members.
        pthread_mutex_lock(&Threadpool->WorkMutex);

        // NOTE(Oskar): Check if there is work available. If not we wait in conditional
        // cond_wait unlocks the mutex and upon signal reaquires the lock.
        while (!Threadpool->Stop && 
               _IsEmpty(Threadpool))
        {
            pthread_cond_wait(&Threadpool->WorkCondition, &Threadpool->WorkMutex);
        }

        // NOTE(Oskar): Thread was signalled to stop. Not unlocking mutex untill
        // end of function since we want to manipulate variables.
        if (Threadpool->Stop)
        {
            break;
        }

        // NOTE(Oskar): Pull work from queue and increment working count.
        // WorkingCount tells the pool that there are threads working.
        // Unlock mutex so other threads can work as well.
        Work = _ThreadpoolGetWork(Threadpool);
        Threadpool->WorkingCount++;
        pthread_mutex_unlock(&Threadpool->WorkMutex);

        // NOTE(Oskar): If there was work then we process it.
        if (Work.Success)
        {
            Work.Function(Work.Argument);
        }

        // NOTE(Oskar): Finally lock mutex again and decrement working count.
        // If no threads are working and no items in queue then we signal the wait 
        // function to wake up. 
        pthread_mutex_lock(&Threadpool->WorkMutex);
        Threadpool->WorkingCount--;
        if (!Threadpool->Stop && 
            Threadpool->WorkingCount == 0 && 
            _IsEmpty(Threadpool))
        {
            pthread_cond_signal(&Threadpool->WorkingCondition);
        }
        pthread_mutex_unlock(&Threadpool->WorkMutex);
    }

    // NOTE(Oskar): If thread was told to stop we decrement thread count and 
    // signal the waiting function that a thread has exited.
    Threadpool->ThreadCount--;
    pthread_cond_signal(&Threadpool->WorkingCondition);
    pthread_mutex_unlock(&Threadpool->WorkMutex);

    return NULL;
}

threadpool *
ThreadpoolCreate(size_t NumberOfThreads, size_t WorkSize)
{
    threadpool *Threadpool;
    pthread_t  Thread;

    // NOTE(Oskar): Default to 2 threads if no number was specified.
    if (NumberOfThreads == 0)
    {
        NumberOfThreads = 2;
    }


    Threadpool              = calloc(1, sizeof(threadpool));
    Threadpool->Work        = calloc(WorkSize, sizeof(threadpool_work));
    Threadpool->ThreadCount = NumberOfThreads;
    Threadpool->Size = WorkSize;

    pthread_mutex_init(&Threadpool->WorkMutex, NULL);
    pthread_cond_init(&Threadpool->WorkCondition, NULL);
    pthread_cond_init(&Threadpool->WorkingCondition, NULL);

    Threadpool->WorkReadIndex = 0;
    Threadpool->WorkWriteIndex = 0;

    // NOTE(Oskar): Create number of threads. Threadpool worker is specified as 
    // working function and threads are detached so that they will be cleaned up on exit.
    for (size_t Index = 0; Index < NumberOfThreads; ++Index) 
    {
        pthread_create(&Thread, NULL, _ThreadpoolWorker, Threadpool);
        pthread_detach(Thread);
    }

    return Threadpool;
}

void 
ThreadpoolDelete(threadpool *Threadpool)
{
    if (Threadpool == NULL)
    {
        return;
    }

    pthread_mutex_lock(&Threadpool->WorkMutex);

    // NOTE(Oskar): Tell threads they need to stop.
    Threadpool->Stop = true;

    pthread_cond_broadcast(&Threadpool->WorkCondition);
    pthread_mutex_unlock(&Threadpool->WorkMutex);

    // NOTE(Oskar): Wait for threads already in the proces of working to finish.
    ThreadpoolWait(Threadpool);

    pthread_mutex_destroy(&Threadpool->WorkMutex);
    pthread_cond_destroy(&Threadpool->WorkCondition);
    pthread_cond_destroy(&Threadpool->WorkingCondition);

    free(Threadpool->Work);
    free(Threadpool);
}

bool 
ThreadpoolAddWork(threadpool *Threadpool, threadpool_work_function Function, void *Argument)
{
    threadpool_work *Work;

    if (Threadpool == NULL)
    {
        return false;
    }

    pthread_mutex_lock(&Threadpool->WorkMutex);
    
    uint32_t NewNextEntryToWrite = (Threadpool->WorkWriteIndex + 1) % Threadpool->Size;
    // assert(NewNextEntryToWrite != Threadpool->WorkReadIndex);
    // if (NewNextEntryToWrite == Threadpool->WorkReadIndex)
    if (NewNextEntryToWrite == Threadpool->WorkReadIndex)
    {
        pthread_mutex_unlock(&Threadpool->WorkMutex);
        return false;
    }

    Work = Threadpool->Work + Threadpool->WorkWriteIndex;
    Work->Argument = Argument;
    Work->Function = Function;

    Threadpool->WorkWriteIndex = NewNextEntryToWrite;

    pthread_cond_broadcast(&Threadpool->WorkCondition);
    pthread_mutex_unlock(&Threadpool->WorkMutex);

    return true;
}

// NOTE(Oskar): Blocking function that will only return when there is no work
// left to be done. 
void 
ThreadpoolWait(threadpool *Threadpool)
{
    if (Threadpool == NULL)
    {
        return;
    }

    pthread_mutex_lock(&Threadpool->WorkMutex);
    while (1) 
    {
        if ((!Threadpool->Stop && Threadpool->WorkingCount != 0) || 
            (Threadpool->Stop && Threadpool->ThreadCount != 0) ||
            _HasWorkInQueue(Threadpool)) 
        {
            pthread_cond_wait(&Threadpool->WorkingCondition, &Threadpool->WorkMutex);
        } 
        else 
        {
            break;
        }
    }
    pthread_mutex_unlock(&Threadpool->WorkMutex);
}