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

#if defined(TARGET_PALMOS) && defined(IX86)

#include "pace.h"

Err SysGetEntryAddresses(UInt32 refNum, UInt32 entryNumStart, UInt32 entryNumEnd, void **addressP)
{
	return 1; //fail
}

Err SysFindModule(UInt32 dbType, UInt32 dbCreator, UInt16 rsrcID, UInt32 flags, UInt32 *refNumP)
{
	return 1;
}

Err SysLoadModule(UInt32 dbType, UInt32 dbCreator, UInt16 rsrcID, UInt32 flags, UInt32 *refNumP)
{
	return 1;
}

Err SysUnloadModule(UInt32 refNum)
{
	return 1;
}

#include <windows.h>

void** HALDispatch()
{
	static void** Table = NULL;
	if (!Table)
	{
		HMODULE Module = LoadLibrary(T("dal.dll"));
		void (__cdecl *GetDispath)(void***) = (void (__cdecl*)(void***)) GetProcAddress(Module,"__ExportDispatchTable");
		GetDispath(&Table);
		//FreeLibrary(Module);
	}
	return Table;
}

void SonyCleanDCache(void* p, UInt32 n) {}
void SonyInvalidateDCache(void* p, UInt32 n) {}
void HALDelay(UInt32 n) 
{
	((void (__cdecl*)(UInt32))HALDispatch()[0x0A])(n);
}
void HALDisplayWake() 
{
	((void (__cdecl*)())HALDispatch()[0x10])();
}
void HALDisplayOff_TREO650() 
{
	((void (__cdecl*)())HALDispatch()[0xD6])();
}

#endif
