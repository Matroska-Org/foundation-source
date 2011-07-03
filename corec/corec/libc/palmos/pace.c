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

#include "corec/corec.h"

#if defined(TARGET_PALMOS)

#include "pace.h"
#include "arm/pealstub.h"

#define memNewChunkFlagAllowLarge 0x1000

//tremor uses lot of alloca()
#define STACKSIZE	0x8000
//#define STACKCHECK

#ifdef HAVE_PALMONE_SDK
#define NO_HSEXT_TRAPS
#include <68K/System/HardwareUtils68K.h>
#include <Common/System/HsNavCommon.h>
#include <68K/System/HsExt.h>
#endif

typedef struct launch
{
	UInt32 PealCall; //BE
	UInt32 Module; //BE
	UInt32 LoadModule; //BE
	UInt32 FreeModule; //BE
	UInt32 GetSymbol; //BE

	UInt16 launchParameters[2];
	UInt16 launchCode;
	UInt16 launchFlags;

} launch;

typedef struct gadgethandler
{
	uint8_t Code[68];
	struct gadgethandler* Next;

	FormGadgetTypeInCallback* Gadget; //BE
	uint16_t* Event; //BE
	UInt16 Cmd; //BE
	UInt32 Module; //BE
	UInt32 PealCall; //BE
	UInt32 Wrapper; //BE

	FormType* Form;
	int Index;
	FormGadgetHandlerType* Handler; 

} gadgethandler;

typedef struct eventhandler
{
	uint8_t Code[48];
	struct eventhandler* Next;

	uint16_t* Event; //BE
	UInt32 Module; //BE
	UInt32 PealCall; //BE
	UInt32 Wrapper; //BE

	FormType* Form;
	FormEventHandlerExType *Handler;
    void* This;

} eventhandler;

static gadgethandler* GadgetHandler = NULL;
static eventhandler* EventHandler = NULL;
static launch Peal = {0};

static NOINLINE void FreeHandlers()
{
	gadgethandler* j;
	eventhandler* i;
	while ((i=EventHandler)!=NULL)
	{
		EventHandler = i->Next;
		MemPtrFree(i);
	}
	while ((j=GadgetHandler)!=NULL)
	{
		GadgetHandler = j->Next;
		MemPtrFree(j);
	}
}

static NOINLINE void SwapLaunchParameters(int launchCode,void* launchParameters,bool_t From)
{
	if (launchCode == sysAppLaunchCmdCustomBase)
	{
		vfspath* p = (vfspath*)launchParameters;
		p->volRefNum = SWAP16(p->volRefNum);
	}

	if (launchCode == sysAppLaunchCmdOpenDB)
	{
		UInt16 tmp;
		SysAppLaunchCmdOpenDBType2* p = (SysAppLaunchCmdOpenDBType2*)launchParameters;
		p->cardNo = SWAP16(p->cardNo);
		tmp = p->dbID[0];
		p->dbID[0] = SWAP16(p->dbID[1]);
		p->dbID[1] = SWAP16(tmp);
	}

	if (launchCode == sysAppLaunchCmdNotify)
	{
		SysNotifyParamType* p = (SysNotifyParamType*)launchParameters;
		UInt32 Type = p->notifyType;
		void* Details = p->notifyDetailsP;

		p->notifyType = SWAP32(p->notifyType);
		p->broadcaster = SWAP32(p->broadcaster);
		p->notifyDetailsP = (void*)SWAP32(p->notifyDetailsP);
		p->userDataP = (void*)SWAP32(p->userDataP);
		
		if (From)
		{
			Type = p->notifyType;
			Details = p->notifyDetailsP;
		}

		if (Type == sysNotifySleepRequestEvent)
		{
			SleepEventParamType* i = (SleepEventParamType*)Details;
			i->reason = SWAP16(i->reason);
			i->deferSleep = SWAP16(i->deferSleep);
		}
	}
}

static INLINE uint32_t ReadSwap32(const void* p)
{
	return 
		(((const uint8_t*)p)[0] << 24)|
		(((const uint8_t*)p)[1] << 16)|
		(((const uint8_t*)p)[2] << 8)|
		(((const uint8_t*)p)[3] << 0);
}

static INLINE void WriteSwap32(void* p,uint32_t i)
{
	((uint8_t*)p)[0] = ((uint8_t*)&i)[3];
	((uint8_t*)p)[1] = ((uint8_t*)&i)[2];
	((uint8_t*)p)[2] = ((uint8_t*)&i)[1];
	((uint8_t*)p)[3] = ((uint8_t*)&i)[0];
}

void Event_M68K_To_ARM(const uint16_t* In,EventType* Out)
{
	Out->eType = SWAP16(In[0]);
	Out->penDown = ((uint8_t*)In)[2];
	Out->tapCount = ((uint8_t*)In)[3];
	Out->screenX = SWAP16(In[2]);
	Out->screenY = SWAP16(In[3]);

	switch (Out->eType)
	{
	// 1*16bit
	case frmLoadEvent:
	case menuEvent:
		Out->data.menu.itemID = SWAP16(In[4]);
		break;

	// 3*16bit
	case frmUpdateEvent:
	case keyUpEvent:
	case keyDownEvent:
	case keyHoldEvent:
		Out->data.keyDown.chr = SWAP16(In[4]);
		Out->data.keyDown.keyCode = SWAP16(In[5]);
		Out->data.keyDown.modifiers = SWAP16(In[6]);
		break;

	// 16bit,32bit
	case fldChangedEvent:
    case fldEnterEvent:
    case ctlEnterEvent:
    case ctlExitEvent:
		Out->data.fldChanged.fieldID = SWAP16(In[4]);
		Out->data.fldChanged.pField = (FieldType*)ReadSwap32(In+5);
		break;

	// 16bit,32bit,16bit,32bit,16bit,16bit
	case popSelectEvent:
		Out->data.popSelect.controlID = SWAP16(In[4]);
		Out->data.popSelect.controlP = (ControlType*)ReadSwap32(In+5);
		Out->data.popSelect.listID = SWAP16(In[7]);
		Out->data.popSelect.listP = (ListType*)SWAP32(*(uint32_t*)(In+8));
		Out->data.popSelect.selection = SWAP16(In[10]);
		Out->data.popSelect.priorSelection = SWAP16(In[11]);
		break;

	// 16bit,32bit,16bit
	case lstSelectEvent:
		Out->data.lstSelect.listID = SWAP16(In[4]);
		Out->data.lstSelect.pList = (ListType*)ReadSwap32(In+5);
		Out->data.lstSelect.selection = SWAP16(In[7]);
		break;

    // 16bit,16bit,32bit
    case frmObjectFocusLostEvent:
		Out->data.frmObjectFocusLost.formID = SWAP16(In[4]);
		Out->data.frmObjectFocusLost.objectID = SWAP16(In[5]);
		Out->data.frmObjectFocusLost.dispatchHint = ReadSwap32(In+6);
        break;

	//custom
	case sclRepeatEvent:
		Out->data.sclRepeat.scrollBarID = SWAP16(In[4]);
		Out->data.sclRepeat.pScrollBar = (ScrollBarType*)ReadSwap32(In+5);
		Out->data.sclRepeat.value = SWAP16(In[7]);
		Out->data.sclRepeat.newValue = SWAP16(In[8]);
		Out->data.sclRepeat.time = ReadSwap32(In+9);
		break;

	//custom
	case ctlRepeatEvent:
		Out->data.ctlRepeat.controlID = SWAP16(In[4]);
		Out->data.ctlRepeat.pControl = (ControlType*)ReadSwap32(In+5);
		Out->data.ctlRepeat.time = ReadSwap32(In+7);
		Out->data.ctlRepeat.value = SWAP16(In[9]);
		break;

	//custom
	case ctlSelectEvent: 
		Out->data.ctlSelect.controlID = SWAP16(In[4]);
		Out->data.ctlSelect.pControl = (ControlType*)ReadSwap32(In+5);
		Out->data.ctlSelect.on = ((uint8_t*)In)[14];
		Out->data.ctlSelect.reserved1 = ((uint8_t*)In)[15];
		Out->data.ctlSelect.value = SWAP16(In[8]);
		break;

	// 2*32bit
	case winEnterEvent:
	case winExitEvent:
		Out->data.winExit.enterWindow = (WinHandle)SWAP32(((uint32_t*)In)[2]);
		Out->data.winExit.exitWindow = (WinHandle)SWAP32(((uint32_t*)In)[3]);
		break;

	default:
		memcpy(Out->data.generic.datum,In+4,sizeof(uint16_t)*8);
		break;
	}
}

void Event_ARM_To_M68K(const EventType* In,uint16_t* Out)
{
	Out[0] = SWAP16(In->eType);
	((uint8_t*)Out)[2] = In->penDown;
	((uint8_t*)Out)[3] = In->tapCount;
	Out[2] = SWAP16(In->screenX);
	Out[3] = SWAP16(In->screenY);

	switch (In->eType)
	{
	// 1*16bit
	case frmLoadEvent:
	case menuEvent:
		Out[4] = SWAP16(In->data.menu.itemID);
		break;

	// 3*16bit
	case frmUpdateEvent:
	case keyUpEvent:
	case keyDownEvent:
	case keyHoldEvent:
		Out[4] = SWAP16(In->data.keyDown.chr);
		Out[5] = SWAP16(In->data.keyDown.keyCode);
		Out[6] = SWAP16(In->data.keyDown.modifiers);
		break;

	// 16bit,32bit,16bit
	case lstSelectEvent:
		Out[4] = SWAP16(In->data.lstSelect.listID);
		WriteSwap32(Out+5,(uint32_t)In->data.lstSelect.pList);
		Out[7] = SWAP16(In->data.lstSelect.selection);
		break;

    // 16bit,16bit,32bit
    case frmObjectFocusLostEvent:
		Out[4] = SWAP16(In->data.frmObjectFocusLost.formID);
		Out[5] = SWAP16(In->data.frmObjectFocusLost.objectID);
		WriteSwap32(Out+6,(uint32_t)In->data.frmObjectFocusLost.dispatchHint);
        break;

	// 16bit,32bit
	case fldChangedEvent:
    case fldEnterEvent:
    case ctlEnterEvent:
    case ctlExitEvent:
		Out[4] = SWAP16(In->data.fldChanged.fieldID);
		WriteSwap32(Out+5,(uint32_t)In->data.fldChanged.pField);
		break;

	// 16bit,32bit,16bit,32bit,16bit,16bit
	case popSelectEvent:
		Out[4] = SWAP16(In->data.popSelect.controlID);
		WriteSwap32(Out+5,(uint32_t)In->data.popSelect.controlP);
		Out[7] = SWAP16(In->data.popSelect.listID);
		WriteSwap32(Out+8,(uint32_t)In->data.popSelect.listP);
		Out[10] = SWAP16(In->data.popSelect.selection);
		Out[11] = SWAP16(In->data.popSelect.priorSelection);
		break;

	//custom
	case sclRepeatEvent:
		Out[4] = SWAP16(In->data.sclRepeat.scrollBarID);
		WriteSwap32(Out+5,(uint32_t)In->data.sclRepeat.pScrollBar);
		Out[7] = SWAP16(In->data.sclRepeat.value);
		Out[8] = SWAP16(In->data.sclRepeat.newValue);
		WriteSwap32(Out+9,In->data.sclRepeat.time);
		break;

	//custom
	case ctlRepeatEvent:
		Out[4] = SWAP16(In->data.ctlRepeat.controlID);
		WriteSwap32(Out+5,(uint32_t)In->data.ctlRepeat.pControl);
		WriteSwap32(Out+7,In->data.ctlRepeat.time);
		Out[9] = SWAP16(In->data.ctlRepeat.value);
		break;

	//custom
	case ctlSelectEvent: 
		Out[4] = SWAP16(In->data.ctlSelect.controlID);
		WriteSwap32(Out+5,(uint32_t)In->data.ctlSelect.pControl);
		((uint8_t*)Out)[14] = In->data.ctlSelect.on;
		((uint8_t*)Out)[15] = In->data.ctlSelect.reserved1;
		Out[8] = SWAP16(In->data.ctlSelect.value);
		break;

	// 2*32bit
	case winEnterEvent:
	case winExitEvent:
		((uint32_t*)Out)[2] = SWAP32(In->data.winExit.enterWindow);
		((uint32_t*)Out)[3] = SWAP32(In->data.winExit.exitWindow);
		break;

	default:
		memcpy(Out+4,In->data.generic.datum,sizeof(uint16_t)*8);
		break;
	}
}

static void GadgetTypeInCallback_M68K_To_ARM(const uint16_t* In,FormGadgetTypeInCallback* Out)
{
	Out->id = SWAP16(In[0]);
	*(UInt16*)&Out->attr = SWAP16(In[1]);
	Out->rect.topLeft.x = SWAP16(In[2]);
	Out->rect.topLeft.y = SWAP16(In[3]);
	Out->rect.extent.x = SWAP16(In[4]);
	Out->rect.extent.y = SWAP16(In[5]);
	Out->data = (const void*)SWAP32(*(const uint32_t*)(In+6));
	Out->handler = (FormGadgetHandlerType*)SWAP32(*(const uint32_t*)(In+8));
}

static void GadgetTypeInCallback_ARM_To_M68K(const FormGadgetTypeInCallback* In,uint16_t* Out)
{
	Out[0] = SWAP16(In->id);
	Out[1] = SWAP16(*(UInt16*)&In->attr);
	Out[2] = SWAP16(In->rect.topLeft.x);
	Out[3] = SWAP16(In->rect.topLeft.y);
	Out[4] = SWAP16(In->rect.extent.x);
	Out[5] = SWAP16(In->rect.extent.y);
	*(uint32_t*)(Out+6) = SWAP32(In->data);
	*(uint32_t*)(Out+8) = SWAP32(In->handler);
}

static NOINLINE bool_t CmdLaunch(int Code)
{
	return Code == sysAppLaunchCmdNormalLaunch ||
		Code == sysAppLaunchCmdOpenDB ||
		Code == sysAppLaunchCmdCustomBase;
}

#if defined(IX86)

#define NONE 0

pcecall PceCall = {NULL};
char DLLName[MAXPATH];

#include <windows.h>

NativeFuncType* IX86CallBack(const char* Func,char* Buffer)
{
    size_t n = strlen(DLLName)+1;
    memcpy(Buffer,DLLName,n);
    strcpy(Buffer+n,Func);
    return (NativeFuncType*)Buffer;
}

BOOL WINAPI DllMain(HINSTANCE hDLL,DWORD fdwReason,LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
        GetModuleFileNameA(hDLL,DLLName,sizeof(DLLName));
    return TRUE;
}

DLLEXPORT uint32_t PaceMain86(const void *emulStateP, launch* Launch, Call68KFuncType *call68KFuncP)
{
	MemPtr Parameters = (MemPtr)((SWAP16(Launch->launchParameters[0])<<16)|(SWAP16(Launch->launchParameters[1])));
	int Code = SWAP16(Launch->launchCode);
	uint32_t Result; 
	pcecall Old = PceCall;

	if (CmdLaunch(Code))
		memcpy(&Peal,Launch,sizeof(launch));

	PceCall.Func = call68KFuncP;
	PceCall.State = emulStateP;

	SwapLaunchParameters(Code,Parameters,1);
	Result = PilotMain((UInt16)Code,Parameters,SWAP16(Launch->launchFlags));
	SwapLaunchParameters(Code,Parameters,0);

	if (CmdLaunch(Code))
		FreeHandlers();

	PceCall = Old;
	return Result;
}

void SaveSysRegs(sysregs* p)
{
}

void LoadSysRegs(sysregs* p)
{
}

NOINLINE uint32_t Call68K(uint32_t trapOrFunction, const void *argsOnStackP, uint32_t argsSizeAndwantA0)
{
	pcecall* Call = &PceCall;
	return Call->Func(Call->State,trapOrFunction,argsOnStackP,argsSizeAndwantA0);
}

DLLEXPORT Boolean WrapperEventHandler86(const void *emulStateP, eventhandler* i, Call68KFuncType *call68KFuncP)
{
	Boolean Result;
	EventType EventARM;
	uint16_t* EventM68K = (uint16_t*) SWAP32(i->Event);
	pcecall Old = PceCall;
	PceCall.Func = call68KFuncP;
	PceCall.State = emulStateP;
	Event_M68K_To_ARM(EventM68K,&EventARM);
	Result = i->Handler(&EventARM,i->This);
	if (!Result && EventARM.eType == frmCloseEvent)
		i->Form = NULL;
	PceCall = Old;
	return Result;
}

DLLEXPORT Boolean WrapperGadgetHandler86(const void *emulStateP, gadgethandler* i, Call68KFuncType *call68KFuncP)
{
	EventType EventARM;
	Boolean Result;
	FormGadgetTypeInCallback GadgetARM;
	uint16_t* Gadget = (uint16_t*) SWAP32(i->Gadget);
	UInt16 Cmd = SWAP16(i->Cmd);
	uint16_t* EventM68K = (uint16_t*) SWAP32(i->Event);
	pcecall Old = PceCall;
	PceCall.Func = call68KFuncP;
	PceCall.State = emulStateP;
	GadgetTypeInCallback_M68K_To_ARM(Gadget,&GadgetARM);
	if (Cmd == formGadgetHandleEventCmd)
		Event_M68K_To_ARM(EventM68K,&EventARM);
	Result = i->Handler(&GadgetARM,Cmd,&EventARM);
	GadgetTypeInCallback_ARM_To_M68K(&GadgetARM,Gadget);
	if (Cmd == formGadgetDeleteCmd)
		i->Form = NULL;
	PceCall = Old;
	return Result;
}

#elif defined(ARM)

#define NONE

void SaveSysRegs(sysregs* p)
{
	p->SysReg = PceCall.SysReg;
	asm volatile(
		"mov %0, r10\n\t"
		: "=&r"(p->GOT) : : "cc");
}

void LoadSysRegs(sysregs* p)
{
	asm volatile(
		"mov r9,%0\n\t"
		"mov r10,%1\n\t"
		: : "r"(p->SysReg),"r"(p->GOT): "cc");
}

NOINLINE uint32_t Call68K(uint32_t trapOrFunction, const void *argsOnStackP, uint32_t argsSizeAndwantA0)
{
	pcecall* Call = &PceCall;
	unsigned long Result;
	register void *SysReg asm("r9") = Call->SysReg;
	Result = Call->Func(Call->State,trapOrFunction,argsOnStackP,argsSizeAndwantA0);
	Call->SysReg = SysReg;
	return Result;
}

static NOINLINE void SafeMemCpy(void* d,const void* s,int n)
{
	uint8_t* d8 = (uint8_t*)d;
	const uint8_t* s8= (const uint8_t*)s;
	for (;n>0;--n,++d8,++s8)
		*d8 = *s8;
}

#ifdef STACKCHECK
static void NOINLINE StackCheck(char* p)
{
	int i;
	for (i=0;i<STACKSIZE;++i)
		if (p[i] != 0x55)
			break;
	if (i < 4096)
	{
		char s[64];
		sprintf(s,"Possible stack overflow %d/%d. ",i,STACKSIZE);
		FrmCustomAlert(WarningOKAlert, s, "Please contact developer!", " ");
	}
}
#endif

uint32_t PaceMain(launch* Launch)
{
	MemPtr Parameters = (MemPtr)((SWAP16(Launch->launchParameters[0])<<16)|(SWAP16(Launch->launchParameters[1])));
	int Code = SWAP16(Launch->launchCode);
	void* Old = NULL;
	char* Stack = NULL;
	uint32_t Result; 

	if (CmdLaunch(Code))
	{
		SafeMemCpy(&Peal,Launch,sizeof(launch));
		Stack = MemGluePtrNew(STACKSIZE);
		if (!Stack)
			return errNone;
		memset(Stack,0x55,STACKSIZE);
		Old = SwapSP(Stack+STACKSIZE);
	}

	SwapLaunchParameters(Code,Parameters,1);
	Result = PilotMain(Code,Parameters,SWAP16(Launch->launchFlags));
	SwapLaunchParameters(Code,Parameters,0);

	if (Stack)
	{
		SwapSP(Old);
#ifdef STACKCHECK
		StackCheck(Stack);
#endif
		MemPtrFree(Stack);
		FreeHandlers();
	}

	return Result;
}

static Boolean WrapperEventHandler(eventhandler* i)
{
	Boolean Result;
	EventType EventARM;
	const uint16_t* EventM68K = (const uint16_t*) SWAP32(i->Event);
	Event_M68K_To_ARM(EventM68K,&EventARM);
	Result = i->Handler(&EventARM,i->This);
	if (!Result && EventARM.eType == frmCloseEvent)
		i->Form = NULL;
	return Result;
}

static Boolean WrapperGadgetHandler(gadgethandler* i)
{
	EventType EventARM;
	Boolean Result;
	FormGadgetTypeInCallback GadgetARM;
	uint16_t* Gadget = (uint16_t*) SWAP32(i->Gadget);
	UInt16 Cmd = SWAP16(i->Cmd);
	const uint16_t* EventM68K = (const uint16_t*) SWAP32(i->Event);
	GadgetTypeInCallback_M68K_To_ARM(Gadget,&GadgetARM);
	if (Cmd == formGadgetHandleEventCmd)
		Event_M68K_To_ARM(EventM68K,&EventARM);
	Result = i->Handler(&GadgetARM,Cmd,&EventARM);
	GadgetTypeInCallback_ARM_To_M68K(&GadgetARM,Gadget);
	if (Cmd == formGadgetDeleteCmd)
		i->Form = NULL;
	return Result;
}

#else
#error Not supported platform
#endif

//---------------------------------------------------------------------

static NOINLINE eventhandler* FindEventHandler(FormType *Form)
{
	eventhandler *i,*j;
	for (i=EventHandler,j=NULL;i;j=i,i=i->Next)
		if (i->Form == Form)
		{
			if (j)
			{
				// move ahead for faster search next time
				j->Next = i->Next;
				i->Next = EventHandler;
				EventHandler = i;
			}
			return i;
		}

	return NULL;
}

Boolean FrmCallEventHandlerEx(FormType *Form, EventType* eventP)
{
    eventhandler* p = FindEventHandler(Form);
    if (p)
        return p->Handler(eventP,p->This);
    return 0;
}

Boolean _FrmDispatchEvent(EventType *eventP);
Boolean FrmDispatchEvent(EventType *eventP)
{
	// avoid using m68k wrapper for some common events (which are sent to the active form)

	if (eventP->eType == keyHoldEvent || eventP->eType == keyDownEvent || 
		eventP->eType == keyUpEvent || eventP->eType == nilEvent ||
		eventP->eType == penDownEvent || eventP->eType == penMoveEvent ||
		eventP->eType == penUpEvent)
	{
		FormType *Form = FrmGetActiveForm();
		eventhandler* p= FindEventHandler(Form);
		if (p)
		{
			// call ARM event handler
			if (p->Handler(eventP,p->This))
				return 1;
			// call system event handler
			return FrmHandleEvent(Form,eventP);
		}
	}

	return _FrmDispatchEvent(eventP);
}

static void SetCodePtr(uint8_t* p,void* Ptr)
{
	p[0] = (uint8_t)(((uint32_t)Ptr) >> 24);
	p[1] = (uint8_t)(((uint32_t)Ptr) >> 16);
	p[2] = (uint8_t)(((uint32_t)Ptr) >> 8);
	p[3] = (uint8_t)(((uint32_t)Ptr) >> 0);
}

static void RemoveHandler(FormType *Form)
{
	eventhandler* i;
	for (i=EventHandler;i;i=i->Next)
		if (i->Form == Form)
			i->Form = NULL;
}

void _FrmDeleteForm(FormType *formP);
void FrmDeleteForm( FormType *formP )
{
	_FrmDeleteForm(formP);
	RemoveHandler(formP);
}

void _FrmReturnToForm(UInt16 formId);
void FrmReturnToForm( UInt16 formId )
{
	FormType* Form = FrmGetActiveForm();
	_FrmReturnToForm(formId);
	RemoveHandler(Form);
}

void* m68kCallBack(m68kcallback p,NativeFuncType* Func)
{
	static const uint8_t Code[24] = 
	{ 0x4E,0x56,0x00,0x00,						// link a6,#0
	  0x48,0x6E,0x00,0x08,					    // pea     8(a6)
	  0x2F,0x3C,0x00,0x00,0x00,0x00,			// move.l  #$ARM,-(sp)
	  0x4E,0x4F,								// trap    #$F
	  0xA4,0x5A,								// dc.w    $A45A
	  0x70,0x00,								// moveq   #0,d0
	  0x4E,0x5E,								// unlk    a6
	  0x4E,0x75 };								// rts

	memcpy(p,Code,sizeof(m68kcallback));
	SetCodePtr((uint8_t*)p+10,(void*)(uint32_t)Func);
	return p;
}

void _FrmSetGadgetHandler(FormType *formP, UInt16 objIndex,FormGadgetHandlerType *attrP);
void FrmSetGadgetHandler(FormType *formP, UInt16 objIndex,FormGadgetHandlerType *attrP)
{
	gadgethandler* i;
	gadgethandler* Free = NULL;
	for (i=GadgetHandler;i;i=i->Next)
	{
		if (i->Form == formP && i->Index == objIndex)
			break;
		if (!i->Form)
			Free = i;
	}
	if (!i)
	{
		if (Free)
			i = Free;
		else
		{
			i = MemGluePtrNew(sizeof(gadgethandler));
			i->Next = GadgetHandler;
			GadgetHandler = i;
		}
	}

	if (i)
	{
		static const uint8_t Code[68] = 
		{ 0x4E,0x56,0x00,0x00,						// link a6,#0
		  0x30,0x2E,0x00,0x0C,						// move.w  $C(a6),d0
		  0x22,0x2E,0x00,0x0E,						// move.l  $E(a6),d1
		  0x23,0xEE,0x00,0x08,0x00,0x00,0x00,0x00,  // move.l  8(a6),($Gadget).l
		  0x33,0xC0,0x00,0x00,0x00,0x00,			// move.w  d0,($Cmd).l
		  0x23,0xC1,0x00,0x00,0x00,0x00,			// move.l  d1,($Event).l
		  0x2F,0x3C,0x00,0x00,0x00,0x00,			// move.l  #This,-(sp)
		  0x2F,0x39,0x00,0x00,0x00,0x00,			// move.l  (Wrapper).l,-(sp)
		  0x2F,0x39,0x00,0x00,0x00,0x00,			// move.l  (Module).l,-(sp)
		  0x20,0x79,0x00,0x00,0x00,0x00,			// movea.l (PealCall).l,a0
		  0x4E,0x90,								// jsr     (a0)
		  0x02,0x80,0x00,0x00,0x00,0xFF,			// andi.l  #$FF,d0
		  0x4E,0x5E,								// unlk    a6
		  0x4E,0x75 };								// rts

#if defined(IX86)
        static char Callback[MAXPATH];
		i->Wrapper = SWAP32(IX86CallBack("WrapperGadgetHandler86",Callback));
#else
		i->Wrapper = SWAP32(WrapperGadgetHandler);
#endif
		i->Form = formP;
		i->Index = objIndex;
		i->Handler = attrP;
		i->Module = Peal.Module;
		i->PealCall = Peal.PealCall;

		memcpy(i->Code,Code,sizeof(Code));
		SetCodePtr(i->Code+16,&i->Gadget);
		SetCodePtr(i->Code+22,&i->Cmd);
		SetCodePtr(i->Code+28,&i->Event);
		SetCodePtr(i->Code+34,i);
		SetCodePtr(i->Code+40,&i->Wrapper);
		SetCodePtr(i->Code+46,&i->Module);
		SetCodePtr(i->Code+52,&i->PealCall);

		_FrmSetGadgetHandler(formP,objIndex,(FormGadgetHandlerType*)i->Code);
	}
}

void _FrmSetEventHandler(FormType *formP,FormEventHandlerType *handler);
void FrmSetEventHandlerEx(FormType *formP,FormEventHandlerExType *handler,void* This)
{
	eventhandler* i;
	eventhandler* Free = NULL;
	for (i=EventHandler;i;i=i->Next)
	{
		if (i->Form == formP)
			break;
		if (!i->Form)
			Free = i;
	}
	if (!i)
	{
		if (Free)
			i = Free;
		else
		{
			i = MemGluePtrNew(sizeof(eventhandler));
			i->Next = EventHandler;
			EventHandler = i;
		}
	}

	if (i)
	{
		static const uint8_t Code[48] = 
		{ 0x4E,0x56,0x00,0x00,						// link a6,#0
		  0x23,0xEE,0x00,0x08,0x00,0x00,0x00,0x00,	// move.l  arg_0(a6),(Event).l
		  0x2F,0x3C,0x00,0x00,0x00,0x00,			// move.l  #This,-(sp)
		  0x2F,0x39,0x00,0x00,0x00,0x00,			// move.l  (Wrapper).l,-(sp)
		  0x2F,0x39,0x00,0x00,0x00,0x00,			// move.l  (Module).l,-(sp)
		  0x20,0x79,0x00,0x00,0x00,0x00,			// movea.l (PealCall).l,a0
		  0x4E,0x90,								// jsr     (a0)
		  0x4E,0x5E,								// unlk    a6
		  0x02,0x80,0x00,0x00,0x00,0xFF,			// andi.l  #$FF,d0
		  0x4E,0x75 };								// rts

#if defined(IX86)
        static char Callback[MAXPATH];
		i->Wrapper = SWAP32(IX86CallBack("WrapperEventHandler86",Callback));
#else
		i->Wrapper = SWAP32(WrapperEventHandler);
#endif
        i->This = This;
		i->Form = formP;
		i->Handler = handler;
		i->Module = Peal.Module;
		i->PealCall = Peal.PealCall;
		i->Event = NULL;

		memcpy(i->Code,Code,sizeof(Code));
		SetCodePtr(i->Code+8,&i->Event);
		SetCodePtr(i->Code+14,i);
		SetCodePtr(i->Code+20,&i->Wrapper);
		SetCodePtr(i->Code+26,&i->Module);
		SetCodePtr(i->Code+32,&i->PealCall);

		_FrmSetEventHandler(formP,(FormEventHandlerType*)i->Code);
	}
}

static NOINLINE void ReplaceForm(FormType* Old, FormType* New)
{
    if (New && Old && New != Old)
    {
	    eventhandler* i;
	    for (i=EventHandler;i;i=i->Next)
		    if (i->Form == Old)
                i->Form = New;
    }
}

Err _LstNewList(void **formPP, UInt16 id, 
	Coord x, Coord y, Coord width, Coord height, 
	FontID font, Int16 visibleItems, Int16 triggerId);

Err LstNewList (void **formPP, UInt16 id, 
	Coord x, Coord y, Coord width, Coord height, 
	FontID font, Int16 visibleItems, Int16 triggerId)
{
    Err err;
    FormType* Old = (FormType*)*formPP;
    err = _LstNewList(formPP,id,x,y,width,height,font,visibleItems,triggerId);
    ReplaceForm(Old,*formPP);
    return err;
}

FieldType* _FldNewField(void **formPP, UInt16 id, 
	Coord x, Coord y, Coord width, Coord height, 
	FontID font, UInt32 maxChars, Boolean editable, Boolean underlined, 
	Boolean singleLine, Boolean dynamicSize, JustificationType justification, 
	Boolean autoShift, Boolean hasScrollBar, Boolean numeric);

FieldType* FldNewField(void **formPP, UInt16 id, 
	Coord x, Coord y, Coord width, Coord height, 
	FontID font, UInt32 maxChars, Boolean editable, Boolean underlined, 
	Boolean singleLine, Boolean dynamicSize, JustificationType justification, 
	Boolean autoShift, Boolean hasScrollBar, Boolean numeric)
{
    FieldType* Field;
    FormType* Old = (FormType*)*formPP;
    Field = _FldNewField(formPP,id,x,y,width,height,font,maxChars,editable,underlined,singleLine,dynamicSize,
                       justification,autoShift,hasScrollBar,numeric);
    ReplaceForm(Old,(FormType*)*formPP);
    return Field;
}


Err _FrmRemoveObject(FormType **formPP, UInt16 objIndex);
Err FrmRemoveObject(FormType **formPP, UInt16 objIndex)
{
    Err err;
    FormType* Old = *formPP;
    err = _FrmRemoveObject(formPP,objIndex);
    ReplaceForm(Old,*formPP);
    return err;
}


Err FrmGlueNavGetFocusRingInfo(const FormType* formP, UInt16* objectIDP, 
    Int16* extraInfoP, RectangleType* boundsInsideRingP,
    FrmNavFocusRingStyleEnum* ringStyleP)
{
	UInt32 ver;
	if (FtrGet(sysFileCSystem, sysFtrNumFiveWayNavVersion, &ver)==errNone)
		return FrmNavGetFocusRingInfo(formP,objectIDP,extraInfoP,boundsInsideRingP,ringStyleP);
#ifdef HAVE_PALMONE_SDK
	else if (FtrGet(hsFtrCreator,hsFtrIDNavigationSupported,&ver)==errNone)
		return HsNavGetFocusRingInfo(formP,objectIDP,extraInfoP,boundsInsideRingP,ringStyleP);
#endif
	return uilibErrObjectFocusModeOff;
}

Err FrmGlueNavDrawFocusRing(FormType* formP, UInt16 objectID, Int16 extraInfo,
    RectangleType* boundsInsideRingP,
    FrmNavFocusRingStyleEnum ringStyle, Boolean forceRestore)
{
	UInt32 ver;
	if (FtrGet(sysFileCSystem, sysFtrNumFiveWayNavVersion, &ver)==errNone)
		return FrmNavDrawFocusRing(formP,objectID,extraInfo,boundsInsideRingP,ringStyle,forceRestore);
#ifdef HAVE_PALMONE_SDK
	else if (FtrGet(hsFtrCreator,hsFtrIDNavigationSupported,&ver)==errNone)
		return HsNavDrawFocusRing(formP,objectID,extraInfo,boundsInsideRingP,ringStyle,forceRestore);
#endif
	return uilibErrObjectFocusModeOff;
}

Err FrmGlueNavRemoveFocusRing (FormType* formP)
{
	UInt32 ver;
	if (FtrGet(sysFileCSystem, sysFtrNumFiveWayNavVersion, &ver)==errNone)
		return FrmNavRemoveFocusRing(formP);
#ifdef HAVE_PALMONE_SDK
	else if (FtrGet(hsFtrCreator,hsFtrIDNavigationSupported,&ver)==errNone)
		return HsNavRemoveFocusRing(formP);
#endif
	return uilibErrObjectFocusModeOff;
}

void FrmGlueNavObjectTakeFocus(FormType* formP, UInt16 objID)
{
	UInt32 ver;
	if (FtrGet(sysFileCSystem, sysFtrNumFiveWayNavVersion, &ver)==errNone)
		FrmNavObjectTakeFocus(formP,objID);
#ifdef HAVE_PALMONE_SDK
	else if (FtrGet(hsFtrCreator,hsFtrIDNavigationSupported,&ver)==errNone)
		HsNavObjectTakeFocus(formP,objID);
#endif
}

Boolean FrmGlueNavIsSupported(void)
{
	UInt32 ver;
	if (FtrGet(sysFileCSystem, sysFtrNumFiveWayNavVersion, &ver)==errNone)
		return 1;
#ifdef HAVE_PALMONE_SDK
	if (FtrGet(hsFtrCreator,hsFtrIDNavigationSupported,&ver)==errNone)
		return 1;
#endif
	return 0;
}

SysAppInfoPtr SysGetAppInfo(SysAppInfoPtr *uiAppPP, SysAppInfoPtr* actionCodeAppPP);

NOINLINE MemPtr MemGluePtrNew(UInt32 Size)
{
	MemPtr p = MemPtrNew(Size);
	if (!p)
	{ 
		SysAppInfoPtr appInfoP;
		UInt16 OwnerID = SysGetAppInfo(&appInfoP,&appInfoP)->memOwnerID;
		p = MemChunkNew(0, Size,(UInt16)(OwnerID | memNewChunkFlagNonMovable | memNewChunkFlagAllowLarge));
	}
	return p;
}

void* PealLoadModule(uint16_t FtrId,Boolean Mem,Boolean OnlyFtr,Boolean MemSema)
{
	uint16_t Param[4];
    Param[0] = SWAP16(FtrId);
    Param[1] = (uint8_t)Mem;
    Param[2] = (uint8_t)OnlyFtr;
    Param[3] = (uint8_t)MemSema;
	return (void*)Call68K(SWAP32(Peal.LoadModule),Param,sizeof(Param)|kPceNativeWantA0);
}

void* PealGetSymbol(void* Module,const tchar_t* Name)
{
    uint32_t Param[2]; 
    Param[0] = SWAP32(Module);
    Param[1] = SWAP32(Name);
    return (void*)Call68K(SWAP32(Peal.GetSymbol),Param,sizeof(Param)|kPceNativeWantA0);
}

void PealFreeModule(void* Module)
{
    uint32_t Param[1]; 
    Param[0] = SWAP32(Module);
    Call68K(SWAP32(Peal.FreeModule),Param,sizeof(Param));
}

#endif
