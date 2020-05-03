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

#include "charconvert.h"
#include "corec/str/str.h"

#if defined(TARGET_WIN)

#ifndef STRICT
#define STRICT
#endif
#include <windows.h>

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif

#ifndef CP_UTF7
#define CP_UTF7 65000
#endif

void CharConvSS(charconv* CC, char* Out, size_t OutLen, const char* In)
{
	if (OutLen>0)
	{
		WCHAR Temp[1024];
		UINT OutCode = LOWORD(CC);
		UINT InCode = HIWORD(CC);

		if (InCode == OutCode ||
			!MultiByteToWideChar(InCode,0,In,-1,Temp,512) ||
			!WideCharToMultiByte(OutCode,0,Temp,-1,Out,(int)OutLen,0,0))
		{
			size_t n = min(strlen(In),OutLen-1);
			memcpy(Out,In,n*sizeof(char));
			Out[n] = 0;
		}
	}
}

void CharConvWS(charconv* CC, wchar_t* Out, size_t OutLen, const char* In)
{
	UINT InCode = HIWORD(CC);
	if (!MultiByteToWideChar(InCode,0,In,-1,Out,(int)OutLen))
	{
		for (;OutLen>1 && *In;++In,--OutLen,++Out)
			*Out = (wchar_t)*In;
		*Out = 0;
	}
}

void CharConvSW(charconv* CC, char* Out, size_t OutLen, const wchar_t* In)
{
	UINT OutCode = LOWORD(CC);
	if (!WideCharToMultiByte(OutCode,0,In,-1,Out,(int)OutLen,0,0))
	{
		for (;OutLen>1 && *In;++In,--OutLen,++Out)
			*Out = (char)(*In>255?'*':*In);
		*Out = 0;	
	}
}

void CharConvWW(charconv* UNUSED_PARAM(CC), wchar_t* Out, size_t OutLen, const wchar_t* In)
{
#ifdef UNICODE
	tcscpy_s(Out,OutLen,In);
#else
	if (OutLen>0)
	{
		size_t n = min(wcslen(In),OutLen-1);
		memcpy(Out,In,n*sizeof(wchar_t));
		Out[n] = 0;
	}
#endif
}

typedef struct codepage
{
    const tchar_t* Name;
    uint16_t CodePage;
    uint16_t CodePage2;

} codepage;

static const codepage CodePage[] = 
{
    {T("UTF-7"),        CP_UTF7,0},
    {T("UTF-8"),        CP_UTF8,0},
    {T("Shift_JIS"),    932,0},
    {T("GB2312"),       936,0},
    {T("BIG5"),         950,0},
    {T("ISO-8859-1"),   28591,1252},
    {T("ISO-8859-2"),   28592,1250},
    {T("ISO-8859-3"),   28593,1254},
    {T("ISO-8859-4"),   28594,1257},
    {T("ISO-8859-5"),   28595,1251},
    {T("ISO-8859-6"),   28596,1256},
    {T("ISO-8859-7"),   28597,1253},
    {T("ISO-8859-8"),   28598,1255},
    {NULL,0,0}
};

static NOINLINE UINT GetCodePage(const tchar_t* Name, bool_t To)
{
    if (Name && Name[0])
    {
        const codepage* i;

    	int CP;
	    if (stscanf(Name,T("CP%d"),&CP)==1 ||
            stscanf(Name,T("windows-%d"),&CP)==1)
		    return CP;

        for (i=CodePage;i->Name;++i)
	        if (tcsisame_ascii(Name,i->Name))
            {
                if (IsValidCodePage(i->CodePage))
    		        return i->CodePage;
                if (i->CodePage2 && IsValidCodePage(i->CodePage2))
    		        return i->CodePage2;
                break;
            }
    }
    if (To)
        return GetOEMCP();
    else
        return CP_ACP;
}

charconv* CharConvOpen(const tchar_t* From, const tchar_t* To)
{
	return (charconv*)(intptr_t)MAKELONG(GetCodePage(To,1),GetCodePage(From,0));
}

void CharConvClose(charconv* UNUSED_PARAM(p))
{
}

void CharConvDefault(tchar_t* UNUSED_PARAM(Out), size_t UNUSED_PARAM(OutLen))
{
    *Out = 0;
}

#endif
