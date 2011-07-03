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

#ifndef __MULTITHREAD_H
#define __MULTITHREAD_H

#include "corec/corec.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MULTITHREAD_EXPORTS)
#define MULTITHREAD_DLL DLLEXPORT
#elif defined(MULTITHREAD_IMPORTS)
#define MULTITHREAD_DLL DLLIMPORT
#else
#define MULTITHREAD_DLL
#endif

#define MULTITHREAD_PRIORITY_HIGH       100
#define MULTITHREAD_PRIORITY_USERINPUT  50
#define MULTITHREAD_PRIORITY_IO         25
#define MULTITHREAD_PRIORITY_NORMAL     0
#define MULTITHREAD_PRIORITY_LOW        -50
#define MULTITHREAD_PRIORITY_IDLE       -100

#ifdef CONFIG_MULTITHREAD
MULTITHREAD_DLL	void* LockCreate();
MULTITHREAD_DLL void LockDelete(void*);
MULTITHREAD_DLL	void LockEnter(void*);
MULTITHREAD_DLL void LockLeave(void*);
#else
#define LockCreate() ((void*)1)
#define LockDelete(p)
#define LockEnter(p)
#define LockLeave(p)
#endif

MULTITHREAD_DLL void ThreadSleepMs(int msTime);
MULTITHREAD_DLL void ThreadSleepTicks(systick_t Ticks);

#ifdef TARGET_WIN
#define THREADCALL STDCALL
#else
#define THREADCALL
#endif

#ifdef CONFIG_MULTITHREAD

typedef int (THREADCALL *threadfunc)(void*);

MULTITHREAD_DLL void* ThreadCreate(threadfunc Start,void* Parameter);
MULTITHREAD_DLL void ThreadCPU(void*,int CPU);
MULTITHREAD_DLL uint32_t ThreadCPUMask();
MULTITHREAD_DLL int ThreadGetPriority(void*);
MULTITHREAD_DLL void ThreadSetPriority(void*, int);
MULTITHREAD_DLL uintptr_t ThreadId();
MULTITHREAD_DLL void* ThreadSelf();
MULTITHREAD_DLL void ThreadExit(uintptr_t ExitCode);
MULTITHREAD_DLL bool_t ThreadDetach(void* Handle);
MULTITHREAD_DLL bool_t ThreadJoin(void* Handle,uintptr_t* ExitCode);

MULTITHREAD_DLL void* SemaphoreCreate(int Init);
MULTITHREAD_DLL void SemaphoreClose(void* Handle);
MULTITHREAD_DLL bool_t SemaphoreWait(void* Handle,int Tick); // Tick=-1 : INFINITE
MULTITHREAD_DLL void SemaphoreRelease(void* Handle,int n);

MULTITHREAD_DLL void* ConditionCreate();
MULTITHREAD_DLL void ConditionClose(void* Handle);
MULTITHREAD_DLL bool_t ConditionWait(void* Handle,int Tick,void* Lock);
MULTITHREAD_DLL void ConditionBroadcast(void* Handle);
MULTITHREAD_DLL void ConditionSignal(void* Handle);

MULTITHREAD_DLL void *ThreadStorageCreate();
MULTITHREAD_DLL void ThreadStorageSet(void *Storage, const void *Value);
MULTITHREAD_DLL void *ThreadStorageGet(void *Storage);
MULTITHREAD_DLL void ThreadStorageDelete(void *Storage);

#else
static INLINE int ThreadGetPriority(void* UNUSED_PARAM(p)) { return MULTITHREAD_PRIORITY_NORMAL; }
static INLINE void ThreadSetPriority(void* UNUSED_PARAM(p), int UNUSED_PARAM(n)) {}
#endif

#ifdef __cplusplus
}
#endif

#endif
