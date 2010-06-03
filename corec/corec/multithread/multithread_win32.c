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

#if defined(TARGET_WIN)

//#define LOCK_TIMEOUT

#ifndef STRICT
#define STRICT
#endif
#include <windows.h>

#if defined(CONFIG_MULTITHREAD)

#if defined(TARGET_WINCE)
static BOOL (WINAPI* FuncCeSetThreadQuantum)(HANDLE, DWORD) = NULL;
static NOINLINE bool_t FindCeSetThreadQuantum()
{
    // slow, but multithread safe
	HMODULE CoreDLL = GetModuleHandleW(L"coredll.dll");
	if (CoreDLL)
		*(FARPROC*)(void*)&FuncCeSetThreadQuantum = GetProcAddressW(CoreDLL,L"CeSetThreadQuantum");
    return FuncCeSetThreadQuantum != NULL;
}
#undef CreateEvent
#define CreateEvent CreateEventW
#endif

void* LockCreate()
{
#ifndef LOCK_TIMEOUT
	void* p = malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection((LPCRITICAL_SECTION)p);
	return p;
#else
	return CreateMutex(NULL,FALSE,NULL);
#endif
}

void LockDelete(void* p)
{
	if (p)
	{
#ifndef LOCK_TIMEOUT
		DeleteCriticalSection((LPCRITICAL_SECTION)p);
		free(p);
#else
		CloseHandle(p);
#endif
	}
}

void LockEnter(void* p)
{
	if (p)
#ifndef LOCK_TIMEOUT
		EnterCriticalSection((LPCRITICAL_SECTION)p);
#else
		while (WaitForSingleObject(p,3000)==WAIT_TIMEOUT)
			DebugBreak();
#endif
}

void LockLeave(void* p)
{
	if (p)
#ifndef LOCK_TIMEOUT
		LeaveCriticalSection((LPCRITICAL_SECTION)p);
#else
		ReleaseMutex(p);
#endif
}

int ThreadGetPriority(void* Thread)
{
	int v = GetThreadPriority(Thread);
#if defined(TARGET_WINCE)
	v = v - THREAD_PRIORITY_NORMAL;
#else
	v = THREAD_PRIORITY_NORMAL - v;
#endif
    if (v>1)
        return MULTITHREAD_PRIORITY_IDLE;
    if (v==1)
        return MULTITHREAD_PRIORITY_LOW;
    if (v==0)
        return MULTITHREAD_PRIORITY_NORMAL;
    if (v==-1)
        return MULTITHREAD_PRIORITY_USERINPUT;
    return MULTITHREAD_PRIORITY_HIGH;
}

void ThreadSetPriority(void* Thread,int Priority)
{
    int v;
    if (Priority<=MULTITHREAD_PRIORITY_IDLE)
        v=2;
    else if (Priority<=MULTITHREAD_PRIORITY_LOW)
        v=1;
    else if (Priority<=MULTITHREAD_PRIORITY_IO) // normal and I/O are same under windows to keep old behaviour
        v=0;
    else if (Priority<=MULTITHREAD_PRIORITY_USERINPUT)
        v=-1;
    else
        v=-2;

#if defined(TARGET_WINCE)
	v = THREAD_PRIORITY_NORMAL + v;
	if (v > THREAD_PRIORITY_IDLE)
		v = THREAD_PRIORITY_IDLE;
	if (v < THREAD_PRIORITY_TIME_CRITICAL)
		v = THREAD_PRIORITY_TIME_CRITICAL;
	SetThreadPriority(Thread,v);
#else
	v = THREAD_PRIORITY_NORMAL - v;
	if (v < THREAD_PRIORITY_IDLE)
		v = THREAD_PRIORITY_IDLE;
	if (v > THREAD_PRIORITY_TIME_CRITICAL)
		v = THREAD_PRIORITY_TIME_CRITICAL;
	SetThreadPriority(Thread,v);
#endif
}

uintptr_t ThreadId() { return GetCurrentThreadId(); }
void* ThreadSelf() { return GetCurrentThread(); };

void ThreadExit(uintptr_t ExitCode)
{
	ExitThread((DWORD)ExitCode);
}

bool_t ThreadDetach(void* Handle)
{
	return Handle && CloseHandle(Handle) != 0;
}

bool_t ThreadJoin(void* Handle,uintptr_t* ExitCode)
{
	bool_t Result = 0;

	if (Handle)
	{
		DWORD v = WaitForSingleObject(Handle,5000);

		if (v == WAIT_OBJECT_0)
			Result = 1;
		else
		if (v == WAIT_TIMEOUT)
			TerminateThread(Handle,0);

		if (ExitCode)
		{
			DWORD dwExitCode = 0;
			GetExitCodeThread(Handle, &dwExitCode);
			*ExitCode = dwExitCode;
		}

		CloseHandle(Handle);
	}

	return Result;
}

void* ThreadCreate(int(THREADCALL *Start)(void*),void* Parameter)
{
	DWORD Id;
	HANDLE Handle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Start,Parameter,0,&Id);

#if defined(TARGET_WINCE)
	if (Handle && (FuncCeSetThreadQuantum || FindCeSetThreadQuantum()))
		FuncCeSetThreadQuantum(Handle,25);
#endif
	return Handle;
}

#if !defined(TARGET_WINCE)
static DWORD (WINAPI* FuncSetThreadIdealProcessor)(HANDLE, DWORD) = NULL;
static BOOL (WINAPI* FuncGetProcessAffinityMask)(HANDLE, uintptr_t*, uintptr_t*) = NULL;
static bool_t FindSetThreadIdealProcessor()
{
    // slow, but multithread safe
	HMODULE DLL = GetModuleHandle(T("kernel32.dll"));
	if (DLL)
		*(FARPROC*)(void*)&FuncSetThreadIdealProcessor = GetProcAddress(DLL,"SetThreadIdealProcessor");
    return FuncSetThreadIdealProcessor != NULL;
}
static bool_t FindGetProcessAffinityMask()
{
    // slow, but multithread safe
	HMODULE DLL = GetModuleHandle(T("kernel32.dll"));
	if (DLL)
		*(FARPROC*)(void*)&FuncGetProcessAffinityMask = GetProcAddress(DLL,"GetProcessAffinityMask");
    return FuncGetProcessAffinityMask != NULL;
}
#endif

void ThreadCPU(void* Handle,int CPU)
{
#if !defined(TARGET_WINCE)
	if (CPU<0)
		CPU = MAXIMUM_PROCESSORS;
	if (FuncSetThreadIdealProcessor || FindSetThreadIdealProcessor())
    	FuncSetThreadIdealProcessor(Handle,CPU);
#endif
}

uint32_t ThreadCPUMask()
{
	uintptr_t a=0;
#if !defined(TARGET_WINCE)
	uintptr_t b;
	if ((!FuncGetProcessAffinityMask && !FindGetProcessAffinityMask()) || 
		!FuncGetProcessAffinityMask(GetCurrentProcess(),&a,&b))
		a = 0;
#endif
	return (uint32_t)a;
}

void* SemaphoreCreate(int Init)
{
	return CreateSemaphore(NULL,Init,32000,NULL);
}

void SemaphoreClose(void* Handle)
{
	CloseHandle(Handle);
}

bool_t SemaphoreWait(void* Handle,int Time)
{ 
	return WaitForSingleObject(Handle,Time) == WAIT_OBJECT_0; 
}

void SemaphoreRelease(void* Handle,int n) 
{ 
	ReleaseSemaphore(Handle,n,NULL); 
}

typedef struct condition_t 
{
	LONG waiters;
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

    InterlockedIncrement(&cond->waiters);
	LockLeave(Lock);
	ret = SemaphoreWait(cond->semaphore, Tick);
	if (!ret) 
        InterlockedDecrement(&cond->waiters);
	LockEnter(Lock);
	return ret;
}

void ConditionBroadcast(void* Handle)
{
	condition_t *cond = (condition_t *)Handle;

    while (InterlockedDecrement(&cond->waiters)>=0)
		SemaphoreRelease(cond->semaphore, 1);
    InterlockedIncrement(&cond->waiters);
}

void ConditionSignal(void* Handle)
{
	condition_t *cond = (condition_t *)Handle;

    if (InterlockedDecrement(&cond->waiters)>=0)
		SemaphoreRelease(cond->semaphore, 1);
    else
        InterlockedIncrement(&cond->waiters);
}

#endif //CONFIG_MULTITHREAD

void ThreadSleepTicks(systick_t Ticks) { Sleep(Ticks); }

// same as ThreadSleepTicks() as 1 systick_t = 1 ms
void ThreadSleepMs(int msTime) { Sleep(msTime); }

#endif
