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

#ifdef LIBC_EXPORTS
#define ASSERT_DLL DLLEXPORT
#else
#define ASSERT_DLL
#endif

#define WarningOKAlert 10031

extern uint16_t FrmCustomAlert(uint16_t alertId, const char *s1, const char *s2, const char *s3);

ASSERT_DLL void _Assert(const char* Exp, const char* File, int Line)
{
	char Msg[MAXPATH];
    size_t i;
	int n=1000000;

	Msg[0] = '\n';

    for (i=1;i<MAXPATH-1 && File[i];++i)
        Msg[i] = File[i];

	if (i<MAXPATH-1)
		Msg[i++] = ':';

	while (n>1 && Line<n)
		n/=10;

    for (;n && i<MAXPATH-1;++i)
	{
	    Msg[i] = (char)('0'+(Line/n));
		Line %= n;
		n /= 10;
	}

    Msg[i] = 0;

	FrmCustomAlert(WarningOKAlert,Exp,Msg," ");
}

#endif
