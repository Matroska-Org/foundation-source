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

#ifndef NDEBUG

#ifndef STRICT
#define STRICT
#endif
#include <windows.h>

ASSERT_DLL void _Assert(const char* Exp,const char* File,int Line)
{
	WCHAR WExp[MAXPATH];
	WCHAR WFile[MAXPATH];
    size_t i;
	int n=1000000;

    for (i=0;i<MAXPATH-1 && File[i];++i)
        WFile[i] = (WCHAR)File[i];

	if (i<MAXPATH-1)
		WFile[i++] = ':';

	while (n>1 && Line<n)
		n/=10;

    for (;n && i<MAXPATH-1;++i)
	{
	    WFile[i] = (WCHAR)('0'+(Line/n));
		Line %= n;
		n /= 10;
	}

    WFile[i] = 0;

    for (i=0;i<MAXPATH-1 && Exp[i];++i)
        WExp[i] = (WCHAR)Exp[i];
    WExp[i] = 0;

#ifndef NDEBUG
	DebugBreak();
#endif

	MessageBoxW(NULL,WExp,WFile,MB_OK);
}

#endif
