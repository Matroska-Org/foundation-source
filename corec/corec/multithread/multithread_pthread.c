/*****************************************************************************
 * 
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of CoreCodec, Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY CoreCodec, Inc. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CoreCodec, Inc. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "multithread.h"

#if defined(TARGET_LINUX) || defined(TARGET_OSX) || defined(TARGET_ANDROID)

#include <unistd.h>

#if defined(TARGET_OSX)
#include <sys/sysctl.h>
#include <stdio.h>
#if defined(TARGET_IPHONE)
#include <sys/timeb.h>
#endif
#define POLL_NO_WARN
#include <poll.h>
#endif

#if defined(CONFIG_MULTITHREAD)

#ifdef __GLIBC__
#define __USE_GNU
#endif

#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sched.h>

void* LockCreate()
{
	pthread_mutex_t *pmt = malloc(sizeof(pthread_mutex_t));
    if (pmt)
    {
#if defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
	pthread_mutex_t lock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
        *pmt = lock;
        return pmt;
#else
#ifndef PTHREAD_MUTEX_RECURSIVE_NP
#define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE
#endif
	    pthread_mutexattr_t attr;
	    pthread_mutexattr_init(&attr);
	    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    	if (pthread_mutex_init(pmt, &attr) == 0)
		    return pmt;
    	free(pmt);
#endif
    }
    return NULL;
}

void LockDelete(void* p)
{
	if (p)
    {
		pthread_mutex_destroy(p);
        free(p);
    }
}

void LockEnter(void* p)
{
	if (p)
		pthread_mutex_lock(p);
}

void LockLeave(void* p)
{
	if (p)
		pthread_mutex_unlock(p);
}

uintptr_t ThreadId()
{
	return (uintptr_t)pthread_self();
}

void* ThreadSelf()
{
	return (void*)pthread_self();
}

bool_t ThreadDetach(void* p)
{
	bool_t Result=0;
	if (p)
		Result = pthread_detach(*(pthread_t*)p)==0; //TODO: memory leak. pthread_t is never freed
	return Result;
}

void ThreadExit(uintptr_t ExitCode)
{
    pthread_exit((void*)ExitCode);
}

bool_t ThreadJoin(void* p,uintptr_t* ExitCode)
{
	bool_t Result=0;
    if (p)
    {
		Result = pthread_join(*(pthread_t*)p,(void**)ExitCode)==0;
        free(p);
    }
	return Result;
}

void* ThreadCreate(threadfunc Start,void* Parameter)
{
	pthread_t *p = malloc(sizeof(pthread_t));
	if (pthread_create(p, NULL, (void*(*)(void*)) Start, Parameter) != 0)
	{
		free(p);
		return NULL;
	}
	return p;
}

void *ThreadStorageCreate()
{
    pthread_key_t Key;
    int Res = pthread_key_create(&Key,NULL);
    if (Res)
        return NULL;
    return (void*)Key;
}

void ThreadStorageSet(void *Storage, const void *Value)
{
    pthread_setspecific((pthread_key_t)Storage,Value);
}

void *ThreadStorageGet(void *Storage)
{
    return pthread_getspecific((pthread_key_t)Storage);
}

void ThreadStorageDelete(void *Storage)
{
    pthread_key_delete((pthread_key_t)Storage);
}

int ThreadGetPriority(void* Thread)
{
    return MULTITHREAD_PRIORITY_NORMAL;
}

void ThreadSetPriority(void* Thread,int Priority)
{
}

void ThreadCPU(void* Handle,int CPU)
{
}

uint32_t ThreadCPUMask()
{
    int n;
#ifdef TARGET_OSX
    size_t Size = sizeof(n);
    int hw_ncpu[2] = { CTL_HW, HW_NCPU };
    if (sysctl(hw_ncpu, 2, &n, &Size, NULL, 0)<0)
        n=0;
#else
    n=sysconf(_SC_NPROCESSORS_CONF);
    if (n<0) 
        n=0;
#endif
    return (1<<n)-1;
}

#ifndef TARGET_IPHONE
void* SemaphoreCreate(int Init)
{
#ifdef TARGET_OSX
    sem_t *p;
    char* name = tmpnam(NULL);
    if (!name || (p = sem_open(name,O_CREAT,0666,(unsigned int)0)) == (sem_t*)SEM_FAILED)
        p=NULL;
    else
    {
        sem_unlink(name);
        SemaphoreRelease(p,Init);
    }
#else
	sem_t *p = malloc(sizeof(sem_t));
    if (p && sem_init(p,0,Init)<0)
    {
        free(p);
        p = NULL;
    }
#endif
    return p;
}

void SemaphoreClose(void* Handle)
{
#ifdef TARGET_OSX
    if (Handle)
        sem_close((sem_t*)Handle);
#else
    if (Handle)
    {
        sem_destroy((sem_t*)Handle);
        free(Handle);
    }
#endif
}

bool_t SemaphoreWait(void* Handle,int Time) // -1 = INFINITE
{ 
	if (Time == 0)
		return sem_trywait((sem_t*)Handle) == 0;
  return sem_wait((sem_t*)Handle) == 0;
} 

void SemaphoreRelease(void* Handle,int n) 
{ 
    for (;n>0;--n)
        if (sem_post((sem_t*)Handle) != 0)
            break;
}

#else

typedef struct iphone_semaphore_hack_t
{
    pthread_cond_t Condition;
    pthread_mutex_t ConditionMutex;
    int SemaphoreCount;
} iphone_semaphore_hack_t;

    
void* SemaphoreCreate(int Init)
{
    iphone_semaphore_hack_t* p = malloc(sizeof(iphone_semaphore_hack_t));
    if (p)
    {
        memset(p,0,sizeof(iphone_semaphore_hack_t));
        p->SemaphoreCount = Init;

        if (pthread_cond_init(&p->Condition, NULL) == 0)
        {
            if (pthread_mutex_init(&p->ConditionMutex, NULL) == 0)
            {
                return p;
            }
            pthread_cond_destroy(&p->Condition);
        }
        free(p);
    }
    return NULL;
}

void SemaphoreClose(void* Handle)
{
    iphone_semaphore_hack_t* p = (iphone_semaphore_hack_t*)Handle;
    if (p)
    {
        pthread_mutex_destroy(&p->ConditionMutex);
        pthread_cond_destroy(&p->Condition);
        free(p);
    }
}

bool_t SemaphoreWait(void* Handle,int Time) // -1 = INFINITE
{ 
    iphone_semaphore_hack_t* p = (iphone_semaphore_hack_t*)Handle;
    int success;

    pthread_mutex_lock(&p->ConditionMutex);
    
    if (Time != 0)
    {
        while (p->SemaphoreCount <= 0)
            if (pthread_cond_wait(&p->Condition,&p->ConditionMutex) != 0)
                break;
    }

    success = (p->SemaphoreCount > 0);
    if (success)
        p->SemaphoreCount--;
    pthread_mutex_unlock(&p->ConditionMutex);
    return success;
}

void SemaphoreRelease(void* Handle,int n) 
{ 
    iphone_semaphore_hack_t* p = (iphone_semaphore_hack_t*)Handle;
    for (;n>0;--n)
    {
        pthread_mutex_lock(&p->ConditionMutex);
        p->SemaphoreCount++;
        pthread_mutex_unlock(&p->ConditionMutex);
        pthread_cond_signal(&p->Condition);
    }
}

#endif

void* ConditionCreate()
{
	pthread_cond_t *cond = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
	if (pthread_cond_init(cond, NULL) != 0) {
		free(cond);
		cond = NULL;
	}
	return (void *)cond;
}

void ConditionClose(void* Handle)
{
	pthread_cond_t *cond = (pthread_cond_t *)Handle;
	pthread_cond_destroy(cond);
	free(cond);
}

bool_t ConditionWait(void* Handle, int Tick, void *Lock)
{
	pthread_cond_t *cond = (pthread_cond_t *)Handle;
	pthread_mutex_t *mutex = (pthread_mutex_t *)Lock;
	struct timespec t = {0};
	struct timeval tp = {0};
	int seconds;

	if (Tick == -1) // INFINTE
		return pthread_cond_wait(cond, mutex) == 0;

	gettimeofday(&tp, NULL);
	t.tv_sec  = tp.tv_sec;
	t.tv_nsec = tp.tv_usec * 1000;
	seconds = Tick / 1000;
	t.tv_sec += seconds;
	t.tv_nsec += (Tick - (seconds * 1000)) * 1000;
	return pthread_cond_timedwait(cond, mutex, &t);
}

void ConditionBroadcast(void* Handle)
{
	pthread_cond_t *cond = (pthread_cond_t *)Handle;

	pthread_cond_broadcast(cond);
}

void ConditionSignal(void* Handle)
{
	pthread_cond_t *cond = (pthread_cond_t *)Handle;
	
	pthread_cond_signal(cond);
}

#endif //CONFIG_MULTITHREAD

void ThreadSleepTicks(systick_t Ticks)
{
#if defined(TARGET_OSX) && !defined(TARGET_IPHONE)
    // for some reason usleep and nanosleep sometimes blocks forever under OSX
    struct pollfd foo;
    poll(&foo,0,Ticks);
#else
	usleep(Ticks*1000); 

	//struct timespec tv;
    //int msec = Time % 1000;
	//tv.tv_sec = Time-msec;
	//tv.tv_nsec = msec*1000000;
    //nanosleep(&tv, NULL);
#endif
}

// same as ThreadSleepTicks() as 1 systick_t = 1 ms
void ThreadSleepMs(int msTime)
{
#if defined(TARGET_OSX) && !defined(TARGET_IPHONE)
    // for some reason usleep and nanosleep sometimes blocks forever under OSX
    struct pollfd foo;
    poll(&foo,0,msTime);
#else
	usleep(msTime*1000); 

	//struct timespec tv;
    //int msec = Time % 1000;
	//tv.tv_sec = Time-msec;
	//tv.tv_nsec = msec*1000000;
    //nanosleep(&tv, NULL);
#endif
}

#endif
