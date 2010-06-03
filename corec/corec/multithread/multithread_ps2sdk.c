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

#if defined(TARGET_PS2SDK)

#include <kernel.h>
#include <ps2timer.h>

#if defined(CONFIG_MULTITHREAD)

extern void *_gp;

typedef struct lock_t
{
    s32 SemaId;
    s32 ThreadId;
    s32 Counter;

} lock_t;

void* LockCreate()
{
    lock_t* p = NULL;
    s32 SemaId;
    ee_sema_t Sema;
    Sema.init_count = 1;
    Sema.max_count = 1;
    Sema.option = 0;
    Sema.attr = 0;
    SemaId = CreateSema(&Sema);

    if (SemaId>=0)
    {
        p = malloc(sizeof(lock_t));
        if (p)
        {
            p->SemaId = SemaId;
            p->ThreadId = -1;
            p->Counter = 0;
        }
    }
    return p;
}

void LockDelete(void* Handle)
{
    if (Handle)
    {
        lock_t* p = Handle;
        DeleteSema(p->SemaId);
        free(p);
    }
}

void LockEnter(void* Handle)
{
    if (Handle)
    {
        lock_t* p = Handle;
        s32 ThreadId = GetThreadId();
        if (p->ThreadId == ThreadId)
            ++p->Counter;
        else
        {
            WaitSema(p->SemaId);
            p->ThreadId  =ThreadId;
            p->Counter = 1;
        }
    }
}

void LockLeave(void* Handle)
{
    if (Handle)
    {
        lock_t* p = Handle;
        assert(p->ThreadId == GetThreadId() && p->Counter>=0);
        if (--p->Counter==0)
        {
            p->ThreadId = -1;
            SignalSema(p->SemaId);
        }
    }
}

uintptr_t ThreadId() 
{ 
    return GetThreadId();
}

typedef struct thread_t thread_t;

static thread_t* Threads = NULL;

struct thread_t
{
    thread_t *Next;
    s32 ThreadId;
    int (*Func)(void*);
    void* Cookie;
    uintptr_t ExitCode;
    uint8_t Stack[32768];
};

int ThreadGetPriority(void* Handle) 
{
	return MULTITHREAD_PRIORITY_NORMAL;
}

void ThreadSetPriority(void* Handle,int Priority)
{
    if (Handle)
    {
        thread_t* p = Handle;
        ChangeThreadPriority(p->ThreadId,64 + (MULTITHREAD_PRIORITY_USERINPUT - Priority)/5); // ULTITHREAD_PRIORITY_USERINPUT=64 is the reference
    }
}

static void ThreadFunc(void* Handle)
{
    thread_t* p = Handle;
    p->ExitCode = p->Func(p->Cookie);
    p->Func = NULL;
    ExitThread();
}

void* ThreadCreate(int(*Func)(void*),void* Cookie) 
{ 
    ee_thread_t Thread;
    thread_t* p = NULL;
    thread_t** i;

    DI();
    for (i=&Threads;*i;i=&(*i)->Next)
        if ((*i)->ThreadId==-1)
        {
            p=*i;
            *i=p->Next;
            break;
        }
    EI();

    if (!p)
    {
        p = malloc(sizeof(thread_t));
        if (!p)
            return NULL;
    }

    memset(&Thread,0,sizeof(Thread));
	Thread.func = ThreadFunc;
	Thread.stack = (void*)ALIGN16((uintptr_t)p->Stack);
    Thread.stack_size = sizeof(p->Stack)-16;
	Thread.gp_reg = (void *)&_gp;
	Thread.initial_priority = 64 + (MULTITHREAD_PRIORITY_USERINPUT - MULTITHREAD_PRIORITY_NORMAL)/5;

	p->ThreadId = CreateThread(&Thread);
	if (p->ThreadId < 0) 
    {
        free(p);
        return NULL;
    }

    p->Cookie = Cookie;
    p->Func = Func;

    DI();
    p->Next = Threads;
    Threads = p;
    EI();

    StartThread(p->ThreadId,p);
	return p;
}

bool_t ThreadJoin(void* Handle,uintptr_t *ExitCode)
{
    bool_t Result=0;
    if (Handle)
    {
        thread_t* p = Handle;
        size_t n;
        for (n=0;n<5000;++n)
        {
            if (!p->Func)
            {
                if (ExitCode)
                    *ExitCode = p->ExitCode;
                Result=1;
                break;
            }
            ThreadSleepTicks(1);
        }
        TerminateThread(p->ThreadId);
        DeleteThread(p->ThreadId);
        p->ThreadId=-1;
    }
	return 0;
}

void* ThreadSelf()
{
    s32 ThreadId = GetThreadId();
    thread_t* p;
    for (p=Threads;p;p=p->Next)
        if (p->ThreadId==ThreadId)
            return p;
	return NULL;
}

bool_t ThreadDetach(void* Handle)
{
    return 0;
}

void ThreadExit(uintptr_t ExitCode)
{
    thread_t* p = ThreadSelf();
    if (p)
    {
        p->ExitCode = ExitCode;
        p->Func = NULL;
        ExitThread();
    }
    ExitDeleteThread();
}

void* SemaphoreCreate(int Init)
{
    s32 SemaId;
    ee_sema_t Sema;
    Sema.init_count = Init;
    Sema.max_count = INT_MAX;
    Sema.option = 0;
    Sema.attr = 0;
    SemaId = CreateSema(&Sema);
    if (SemaId<0)
        return NULL;
    return (void*)SemaId;
}

void SemaphoreClose(void* Handle)
{
    if (Handle)
        DeleteSema((s32)Handle);
}

bool_t SemaphoreWait(void* Handle,int Tick)
{
    //TODO: support timeout
    WaitSema((s32)Handle);
	return 1;	
}

void SemaphoreRelease(void* Handle,int n)
{
    for (;n>0;--n)
        SignalSema((s32)Handle);
}

int LockedInc(int* v)
{
    int i;
    DI();
    i = ++(*v);
    EI();
    return i;
}

int LockedDec(int* v)
{
    int i;
    DI();
    i = --(*v);
    EI();
    return i;
}

typedef struct condition_t 
{
	int waiters;
	void *semaphore;

} condition_t;

void* ConditionCreate()
{
	condition_t *cond = (condition_t *)malloc(sizeof(condition_t));
	cond->waiters = 0;
	cond->semaphore = SemaphoreCreate(0);
	return (void *)cond;
}

void ConditionClose(void* Handle)
{
	condition_t *cond = (condition_t *)Handle;
	SemaphoreClose(cond->semaphore);
	free(cond);
}

bool_t ConditionWait(void* Handle, int Tick, void *Lock)
{
	condition_t *cond = (condition_t *)Handle;
	bool_t ret;

    LockedInc(&cond->waiters);
	LockLeave(Lock);
	ret = SemaphoreWait(cond->semaphore, Tick);
	if (!ret) 
        LockedDec(&cond->waiters);
	LockEnter(Lock);
	return ret;
}

void ConditionBroadcast(void* Handle)
{
	condition_t *cond = (condition_t *)Handle;

    while (LockedDec(&cond->waiters)>=0)
		SemaphoreRelease(cond->semaphore, 1);
    LockedInc(&cond->waiters);
}

void ConditionSignal(void* Handle)
{
	condition_t *cond = (condition_t *)Handle;

    if (LockedDec(&cond->waiters)>=0)
		SemaphoreRelease(cond->semaphore, 1);
    else
        LockedInc(&cond->waiters);
}

#endif

void ThreadSleepTicks(systick_t Ticks)
{
    if (Ticks)
    {
        s32 ThreadId = GetThreadId();
        CancelWakeupThread(ThreadId);
        if (WakeupThreadLater(ThreadId,Ticks))
        {
            SleepThread();
            return;
        }
    }
    RotateThreadReadyQueue(64);
}

// same as ThreadSleepTicks() as 1 systick_t = 1 ms
void ThreadSleepMs(int msTime)
{
    if (msTime)
    {
        s32 ThreadId = GetThreadId();
        CancelWakeupThread(ThreadId);
        if (WakeupThreadLater(ThreadId,msTime))
        {
            SleepThread();
            return;
        }
    }
    RotateThreadReadyQueue(64);
}

#endif
