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

#ifndef __PACE_H
#define __PACE_H

#if defined(TARGET_PALMOS)

#include <PalmOS.h>
#include <CoreTraps.h>
#include <UIResources.h>
#include <Rect.h>
#include <SystemMgr.h>
#include <Form.h>
#include <Extensions/ExpansionMgr/ExpansionMgr.h>
#include <Extensions/ExpansionMgr/VFSMgr.h>
#include <PalmOSGlue/MemGlue.h>
#include <PceNativeCall.h>
#include <PalmOSGlue/SysGlue.h>
#include <PalmOSGlue/FrmGlue.h>
#include <DLServer.h>
#include <DateTime.h>

#if PALMOS_SDK_VERSION < 0x0541
#error Please update to Palm OS 5 SDK R4
#endif

typedef struct sysregs
{
	void* GOT;
	void* SysReg;

} sysregs;

void SaveSysRegs(sysregs* p);
void LoadSysRegs(sysregs* p);

extern Int32 GetOEMSleepMode();
extern Err SetOEMSleepMode(Int32 Mode);

extern void* PealLoadModule(uint16_t FtrId,Boolean Mem,Boolean OnlyFtr,Boolean MemSema);
extern void PealFreeModule(void*);
extern void* PealGetSymbol(void*,const tchar_t* Name);

extern Err SysGetEntryAddresses(UInt32 refNum, UInt32 entryNumStart,
		UInt32 entryNumEnd, void **addressP);
extern Err SysFindModule(UInt32 dbType, UInt32 dbCreator, UInt16 rsrcID,
		UInt32 flags, UInt32 *refNumP);
extern Err SysLoadModule(UInt32 dbType, UInt32 dbCreator, UInt16 rsrcID,
		UInt32 flags, UInt32 *refNumP);
extern Err SysUnloadModule(UInt32 refNum);
extern void HALDelay(UInt32 microSec);
extern void HALDisplayWake();
extern void HALDisplayOff_TREO650();

extern void SonyCleanDCache(void*, UInt32);
extern void SonyInvalidateDCache(void*, UInt32);

extern int PalmCall(void* Func,...); //0..4
extern int PalmCall2(void* Func,...); //5..8

#if defined(_MSC_VER)
#define PALMCALL0(Func) Func()
#define PALMCALL1(Func,a) Func(a)
#define PALMCALL2(Func,a,b) Func(a,b)
#define PALMCALL3(Func,a,b,c) Func(a,b,c)
#define PALMCALL4(Func,a,b,c,d) Func(a,b,c,d)
#define PALMCALL5(Func,a,b,c,d,e) Func(a,b,c,d,e)
#define PALMCALL6(Func,a,b,c,d,e,f) Func(a,b,c,d,e,f)
#define PALMCALL7(Func,a,b,c,d,e,f,g) Func(a,b,c,d,e,f,g)
#define PALMCALL8(Func,a,b,c,d,e,f,g,h) Func(a,b,c,d,e,f,g,h)
#else
#define PALMCALL0(Func) PalmCall(Func)
#define PALMCALL1(Func,a) PalmCall(Func,a)
#define PALMCALL2(Func,a,b) PalmCall(Func,a,b)
#define PALMCALL3(Func,a,b,c) PalmCall(Func,a,b,c)
#define PALMCALL4(Func,a,b,c,d) PalmCall(Func,a,b,c,d)
#define PALMCALL5(Func,a,b,c,d,e) PalmCall2(Func,a,b,c,d,e)
#define PALMCALL6(Func,a,b,c,d,e,f) PalmCall2(Func,a,b,c,d,e,f)
#define PALMCALL7(Func,a,b,c,d,e,f,g) PalmCall2(Func,a,b,c,d,e,f,g)
#define PALMCALL8(Func,a,b,c,d,e,f,g,h) PalmCall2(Func,a,b,c,d,e,f,g,h)
#endif

typedef uint16_t m68kcallback[12];
extern void* m68kCallBack(m68kcallback,NativeFuncType*);

typedef Boolean FormEventHandlerExType(EventType *eventP, void* This);

extern void FrmSetEventHandlerEx(FormType *formP,FormEventHandlerExType *handler,void* This);

extern Boolean FrmCallEventHandlerEx(FormType *Form, EventType* eventP);

typedef struct SysAppLaunchCmdOpenDBType2
{
	UInt16		cardNo;
	UInt16		dbID[2];

} SysAppLaunchCmdOpenDBType2;

typedef struct vfspath
{
	UInt16 volRefNum;
	Char path[256];
} vfspath;

#if defined(IX86)
NativeFuncType* IX86CallBack(const char* Func,char* Buffer);
#endif

#endif
#endif
