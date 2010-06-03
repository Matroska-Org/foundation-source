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

#if defined(TARGET_SYMBIAN)

//#error todo: remove RThread::Rendezvous usage
//#error todo: SemaphoreWait with timeout for pre Symbian9

#undef T
#undef NULL
#include <e32std.h>
#include <e32hal.h>
#include <e32math.h>

#ifdef CONFIG_MULTITHREAD

#define	STACKSIZE 25000

#if defined(SYMBIAN90)

void* LockCreate()
{
	RMutex p;
	p.CreateLocal();
	return (void*)p.Handle();
}

void LockDelete(void* Handle)
{
	RMutex p;
	p.SetHandle((int)Handle);
	p.Close();
}

void LockEnter(void* Handle)
{
	RMutex p;
	p.SetHandle((int)Handle);
	p.Wait();
}

void LockLeave(void* Handle)
{
	RMutex p;
	p.SetHandle((int)Handle);
	p.Signal();
}

#else

typedef struct lock_t
{
    int Handle;
    volatile TUint Thread;
    volatile size_t Counter;

} lock_t;

void* LockCreate()
{
	lock_t *lock = (lock_t *)malloc(sizeof(lock_t));
    if (lock)
    {
	    RMutex p;
	    p.CreateLocal();
        lock->Handle = p.Handle();
        lock->Counter = 0;
        lock->Thread = 0;
    }
	return lock;
}

void LockDelete(void* Handle)
{
	lock_t *lock = (lock_t *)Handle;
    if (lock)
    {
	    RMutex p;
	    p.SetHandle(lock->Handle);
	    p.Close();
        free(lock);
    }
}

void LockEnter(void* Handle)
{
	lock_t *lock = (lock_t *)Handle;
    if (lock)
    {
        RThread current;
        TUint currentId = current.Id();
        if (lock->Thread == currentId)
            ++lock->Counter;
        else
        {
	        RMutex p;
	        p.SetHandle(lock->Handle);
	        p.Wait();
            assert(lock->Counter==0);
            lock->Counter = 1;
            lock->Thread = currentId;
        }
    }
}

void LockLeave(void* Handle)
{
	lock_t *lock = (lock_t *)Handle;
    if (lock && --lock->Counter == 0)
    {
        lock->Thread = 0;
    	RMutex p;
	    p.SetHandle(lock->Handle);
	    p.Signal();
    }
}

#endif

uintptr_t ThreadId() 
{ 
    RThread current;
    return current.Id();
}

class thread
{
public:
	RThread Thread;
	RSemaphore Semaphore;	
	void* Tls;
	int(*Start)(void*);
	void* Parameter;
};

int ThreadGetPriority(void* Handle) 
{ 
	RThread Thread;
	Thread.SetHandle((TInt)Handle);	
    TThreadPriority v = Thread.Priority();
    if (v<=EPriorityMuchLess)
        return MULTITHREAD_PRIORITY_IDLE;
    if (v<=EPriorityLess)
        return MULTITHREAD_PRIORITY_LOW;
    if (v<=EPriorityNormal)
        return MULTITHREAD_PRIORITY_NORMAL;
    if (v<=EPriorityMore)
        return MULTITHREAD_PRIORITY_IO;
    if (v<=EPriorityMuchMore)
        return MULTITHREAD_PRIORITY_USERINPUT;
    return MULTITHREAD_PRIORITY_HIGH;
}    

void ThreadSetPriority(void* Handle,int Priority) 
{ 
	RThread Thread;
	Thread.SetHandle((TInt)Handle);	
    TThreadPriority v;
    if (Priority<=MULTITHREAD_PRIORITY_IDLE)
        v = EPriorityMuchLess;
    else if (Priority<=MULTITHREAD_PRIORITY_LOW)
        v = EPriorityLess;
    else if (Priority<=MULTITHREAD_PRIORITY_NORMAL)
        v = EPriorityNormal;
    else if (Priority<=MULTITHREAD_PRIORITY_IO)
        v = EPriorityMore;
    else if (Priority<=MULTITHREAD_PRIORITY_USERINPUT)
        v = EPriorityMuchMore;
    else
        v = EPriorityRealTime;
	Thread.SetPriority(v);
}

static int ThreadFunc(TAny* data)
{
	thread* Param = (thread*)data;
	
#if !defined(SYMBIAN90)
	Dll::SetTls(Param->Tls);
#endif
	
	// Run actual thread function
	Param->Start(Param->Parameter);
	
	// Signal that we've stopped
	Param->Semaphore.Signal();
	Param->Semaphore.Close();
	delete Param;
	
	return 0;
}

void* ThreadCreate(int(*Start)(void*),void* Parameter) 
{ 
	thread* Thread = new thread;
	Thread->Semaphore.CreateLocal(0);

#if !defined(SYMBIAN90)
	Thread->Tls = Dll::Tls();
#endif

	Thread->Start = Start;
	Thread->Parameter = Parameter;
	
    TInt Err;
    TBuf<32> Name;
    do
    {
        Name.Format(_L("__%08x_%08x"),(uintptr_t)Start,Math::Random());
	    Err = Thread->Thread.Create(Name,ThreadFunc,STACKSIZE,NULL,Thread);

    } while (Err == KErrAlreadyExists);

    if (Err != KErrNone)
	{
		Thread->Semaphore.Close();
		delete Thread;
		return NULL;
	}

	Thread->Thread.Resume();
	return (void *)Thread->Thread.Handle(); 
}

bool_t ThreadJoin(void* Handle,uintptr_t *ExitCode)
{
	RThread Thread;
	Thread.SetHandle((TInt)Handle);	
    //!!! todo 
	// This is only supported in Symbian 9.2, but it's the only clean way
	// Thread.Rendezvous(0);
	if (ExitCode)
		*ExitCode = Thread.ExitReason();
	Thread.Close();
	return 1;
}

void* ThreadSelf()
{
	RThread Thread;
	return (void *)Thread.Handle(); 
}

bool_t ThreadDetach(void* Handle)
{
	RThread Thread;
	Thread.SetHandle((TInt)Handle);	
	Thread.Close();
	return 1;
}

void ThreadExit(uintptr_t ExitCode)
{
    User::Exit((TInt)ExitCode);
}

void* SemaphoreCreate(int Init)
{
	RSemaphore p;
	p.CreateLocal(Init);
	return (void *)p.Handle();
}

void SemaphoreClose(void* Handle)
{
	RSemaphore p;
	p.SetHandle((int)Handle);
	p.Close();	
}

bool_t SemaphoreWait(void* Handle,int Tick)
{
	RSemaphore p;	
	p.SetHandle((int)Handle);
    if (Tick>=0)
    {
#if defined(SYMBIAN90)
        return p.Wait(Tick*1000) == KErrNone;
#else
        //!!! todo 
#endif
    }
    p.Wait();
    return 1;
}

void SemaphoreRelease(void* Handle,int n)
{
	RSemaphore p;
	p.SetHandle((int)Handle);
	p.Signal(n);			
}

#if defined(SYMBIAN90)

void* ConditionCreate()
{
	RCondVar p;
	p.CreateLocal();
	return (void *)p.Handle();
}

void ConditionClose(void* Handle)
{
	RCondVar p;
	p.SetHandle((int)Handle);
	p.Close();	
}

bool_t ConditionWait(void* Handle,int Tick,void* Lock)
{
	RCondVar p;
	RMutex m;	
	p.SetHandle((int)Handle);
	m.SetHandle((int)Lock);
	// Increase the tick by one to mimic other platforms
	// which is -1 = Wait forever, 0 = Return
	// For Symbian, 0 = Wait forever
	Tick++;		
	return p.TimedWait(m, Tick*1000) == KErrNone;
}

void ConditionBroadcast(void* Handle)
{
	RCondVar p;
	p.SetHandle((int)Handle);
	p.Broadcast();	
}

void ConditionSignal(void* Handle)
{
	RCondVar p;
	p.SetHandle((int)Handle);
	p.Signal();
}

#else

typedef struct condition_t 
{
	TInt waiters;
	void *semaphore;

} condition_t;

void* ConditionCreate()
{
	condition_t *cond = (condition_t *)malloc(sizeof(condition_t));
    if (cond)
    {
	    cond->waiters = 0;
	    cond->semaphore = SemaphoreCreate(0);
    }
	return cond;
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

    User::LockedInc(cond->waiters);
	LockLeave(Lock);
	ret = SemaphoreWait(cond->semaphore, Tick);
	if (!ret) 
        User::LockedDec(cond->waiters);
	LockEnter(Lock);
	return ret;
}

void ConditionBroadcast(void* Handle)
{
	condition_t *cond = (condition_t *)Handle;

    while (User::LockedDec(cond->waiters)>0)
		SemaphoreRelease(cond->semaphore, 1);
    User::LockedInc(cond->waiters);
}

void ConditionSignal(void* Handle)
{
	condition_t *cond = (condition_t *)Handle;

    if (User::LockedDec(cond->waiters)>0)
		SemaphoreRelease(cond->semaphore, 1);
    else
        User::LockedInc(cond->waiters);
}
#endif

#endif

void ThreadSleepTicks(systick_t Ticks)
{
    if (Ticks)
    {
        TTimeIntervalMicroSeconds32 n;
	    UserHal::TickPeriod(n);
        Ticks *= n.Int();
    }
	User::After(Ticks);
}

void ThreadSleepMs(int msTime)
{ 
	User::After(msTime*1000);
}

#endif
