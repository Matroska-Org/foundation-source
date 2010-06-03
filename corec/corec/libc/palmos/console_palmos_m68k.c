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

#include <PalmOS.h>
#include <MemGlue.h>
#include "m68k/peal.h"
#include <stdio.h>
#include <PceNativeCall.h>

#define PALMOS_IX86_DLL(name)           #name
#define PALMOS_IX86_FUNC(name,func)     (NativeFuncType*)(PALMOS_IX86_DLL(name) ".dll\0" #func)

#define SWAP16(a) ((((UInt16)(a) >> 8) & 0x00FF) | (((UInt16)(a) << 8) & 0xFF00))

#define SWAP32(a) ((((UInt32)(a) >> 24) & 0x000000FF) | (((UInt32)(a) >> 8)  & 0x0000FF00) |\
	 	(((UInt32)(a) << 8)  & 0x00FF0000) | (((UInt32)(a) << 24) & 0xFF000000))

typedef struct vfspath
{
	UInt16 volRefNum;
	Char path[256];
} vfspath;

typedef struct launch
{
	MemPtr PealCall;
	PealModule* Module;
	void* LoadModule;
	void* FreeModule;
	void* GetSymbol;

	MemPtr launchParameters;
	UInt16 launchCode;
	UInt16 launchFlags;

} launch;

static Err RomVersionCheck(UInt16 launchFlags)
{
	UInt32 Version;
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &Version);

	if (Version < sysMakeROMVersion(5,0,0,sysROMStageDevelopment,0))
	{
		if ((launchFlags & sysAppLaunchFlagNewGlobals) != 0 &&
			(launchFlags & sysAppLaunchFlagUIApp) != 0)
		{
			FrmCustomAlert(WarningOKAlert, "System version 5.0 or greater is required to run this application!", " ", " ");

			// Palm OS 1.0 requires you to explicitly launch another app
			if (Version < sysMakeROMVersion(1,0,0,sysROMStageRelease,0))
			{
				AppLaunchWithCommand(sysFileCDefaultApp,
						sysAppLaunchCmdNormalLaunch, NULL);
			}
		}

		return sysErrRomIncompatible;
	}
	return errNone;
}

static PealModule* Module = NULL;
static void* PaceMain = NULL;

static void* LookupSymbol86(void* Module,const char* Name)
{
	return NULL;
}

static void* LoadModule(UInt16 ftrId,Boolean mem,Boolean onlyftr,Boolean memsema)
{
	return PealLoadFromResources('armc',1000,Module,PROJECT_FOURCC,ftrId,mem,onlyftr,memsema);
}

static UInt32 PealCall86(void* Module,void* Func,void* Param)
{
	return PceNativeCall((NativeFuncType*)Func,Param);
}

static UInt16 GetHeapId()
{
	MemPtr p=MemPtrNew(8);   
	UInt16 Id=MemPtrHeapID(p);
	MemPtrFree(p);
	return Id;
}

UInt32 PilotMain(UInt16 launchCode, MemPtr launchParameters, UInt16 launchFlags)
{
	UInt32 Value;
	UInt32 Result = errNone;
	UInt32 CPU;
	launch Launch;
	Launch.launchParameters = launchParameters;
	Launch.launchCode = launchCode;
	Launch.launchFlags = launchFlags;

	if ((launchCode == sysAppLaunchCmdNormalLaunch ||
		launchCode == sysAppLaunchCmdOpenDB ||
		launchCode == sysAppLaunchCmdCustomBase) && !RomVersionCheck(launchFlags))
	{
		FtrGet(sysFileCSystem, sysFtrNumProcessorID, &CPU);
		if (CPU == sysFtrNumProcessorx86)
		{
			Module = PealLoadFromResources('armc', 1000, NULL, PROJECT_FOURCC,32,0,0,0); // just for testing

			Launch.FreeModule = PealUnload;
			Launch.LoadModule = LoadModule;
			Launch.GetSymbol = LookupSymbol86;
			Launch.PealCall = PealCall86;
			Launch.Module = Module;
			PceNativeCall(PALMOS_IX86_FUNC(PROJECT_OUTPUT,PaceMain86),&Launch);

			if (Module)
				PealUnload(Module);
		}
		else
		if (sysFtrNumProcessorIsARM(CPU))
		{
			UInt32 Version;
			Boolean MemSema;

			FtrGet(sysFtrCreator, sysFtrNumROMVersion, &Version);
			MemSema = Version < sysMakeROMVersion(6,0,0,sysROMStageDevelopment,0);

			Module = PealLoadFromResources('armc', 1000, NULL, PROJECT_FOURCC,32,0,0,MemSema);
			if (Module)
			{
				PaceMain = PealLookupSymbol(Module, "PaceMain");
				if (PaceMain)
				{
					Launch.FreeModule = PealUnload;
					Launch.LoadModule = LoadModule;
					Launch.GetSymbol = PealLookupSymbol;
					Launch.PealCall = PealCall;
					Launch.Module = Module;

					Result = PealCall(Module,PaceMain,&Launch);
				}
				PealUnload(Module);
				MemHeapCompact(GetHeapId()); 
			}
		}
		else
			FrmCustomAlert(WarningOKAlert, "ARM processor is required to run this application!", " ", " ");

		if (FtrGet(PROJECT_FOURCC,20,&Value)==errNone)
			FtrPtrFree(PROJECT_FOURCC,20);
	}
	else
	if (launchCode == sysAppLaunchCmdNotify && (launchFlags & sysAppLaunchFlagSubCall)!=0)
	{
		FtrGet(sysFileCSystem, sysFtrNumProcessorID, &CPU);
		if (CPU == sysFtrNumProcessorx86)
			Result = PceNativeCall(PALMOS_IX86_FUNC(PROJECT_OUTPUT,PaceMain86),&Launch);
		else
		if (sysFtrNumProcessorIsARM(CPU) && Module && PaceMain)
			Result = PealCall(Module,PaceMain,&Launch);
	}
	return Result;
}
