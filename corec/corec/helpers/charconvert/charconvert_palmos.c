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

#if defined(TARGET_PALMOS)

#include <PalmOS.h>

static NOINLINE size_t wcslen_(const wchar_t* s)
{
    size_t n=0;
    for (;*s;++s)
        ++n;
    return n;
}

void CharConvSS(charconv* CC, char* Out, size_t OutLen, const char* In)
{
    if (OutLen)
    {
        if (CC)
        {
            UInt16 InBytes = (UInt16)strlen(In);
            UInt16 OutBytes = (UInt16)--OutLen; 
            TxtConvertEncoding(1,NULL,In,&InBytes,(uint8_t)(uintptr_t)CC,Out,&OutBytes,(uint8_t)((((uintptr_t)CC)>>16)),"*",1);
            Out[min(OutLen,OutBytes)]=0;
        }
        else
            tcscpy_s(Out,OutLen,In);
    }
}

void CharConvWS(charconv* CC, wchar_t* Out, size_t OutLen, const char* In)
{
    if (OutLen)
    {
        UInt16 InBytes = (UInt16)strlen(In);
        UInt16 OutBytes = (UInt16)(--OutLen*sizeof(wchar_t)); 
        TxtConvertEncoding(1,NULL,In,&InBytes,(uint8_t)(uintptr_t)CC,(char*)Out,&OutBytes,(uint8_t)((((uintptr_t)CC)>>16)),(char*)(L"*"),sizeof(wchar_t));
        Out[min(OutLen,OutBytes/sizeof(wchar_t))]=0;
    }
}

void CharConvSW(charconv* CC, char* Out, size_t OutLen, const wchar_t* In)
{
    if (OutLen)
    {
        UInt16 InBytes = (UInt16)(wcslen_(In)*sizeof(wchar_t));
        UInt16 OutBytes = (UInt16)--OutLen; 
        TxtConvertEncoding(1,NULL,(char*)In,&InBytes,(uint8_t)(uintptr_t)CC,Out,&OutBytes,(uint8_t)((((uintptr_t)CC)>>16)),"*",sizeof(char));
        Out[min(OutLen,OutBytes)]=0;
    }
}

void CharConvWW(charconv* CC, wchar_t* Out, size_t OutLen, const wchar_t* In)
{
	if (OutLen>0)
	{
		size_t n = min(wcslen_(In),OutLen-1);
		memcpy(Out,In,n*sizeof(wchar_t));
		Out[n] = 0;
	}
}

static NOINLINE UInt32 GetCharSet(const tchar_t* Name)
{
    UInt32 Type;

    if (Name && Name[0])
    {
        if (tcsicmp(Name,CHARSET_WCHAR)==0)
            return charEncodingUTF16LE;

        if (tcsicmp(Name,CHARSET_UTF8)==0)
            return charEncodingUTF8;

        // only unicode<->current charset supported on PalmOS
        //if ((Type = TxtNameToEncoding(Name)) != charEncodingUnknown)
        //    return Type;
    }

    if (FtrGet(sysFtrCreator, sysFtrNumEncoding, &Type) != errNone)
        Type = charEncodingPalmLatin; 

    return Type;
}

charconv* CharConvOpen(const tchar_t* From, const tchar_t* To)
{
    UInt32 FromType = GetCharSet(From);
    UInt32 ToType = GetCharSet(To);
    if (FromType == ToType)
        return NULL;
	return (charconv*)(FromType | (ToType << 16));
}

void CharConvClose(charconv* p)
{
}

void CharConvDefault(tchar_t* Out, size_t OutLen)
{
    const tchar_t* Name;
    UInt32 Type;
    if (FtrGet(sysFtrCreator, sysFtrNumEncoding, &Type) != errNone)
        Type = charEncodingPalmLatin;

    Name = TxtEncodingName((CharEncodingType)Type);
    if (Name)
        tcscpy_s(Out,OutLen,Name);
    else
        *Out = 0;
}

#endif
