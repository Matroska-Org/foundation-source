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

#if defined(TARGET_LINUX)

#include <iconv.h>
#include <locale.h>
#include <wchar.h>
#include <errno.h>

#ifndef ICONV_CONST
#if defined(__GLIBC__)
#define ICONV_CONST
#else
#define ICONV_CONST const
#endif
#endif

static tchar_t *Current = NULL;

void CharConvSS(charconv* CC, char* Out, size_t OutLen, const char* In)
{
    if (OutLen>0)
    {
        ICONV_CONST char* _In = (ICONV_CONST char*)In;
        size_t _InLen = strlen(In)+1;
        char* _Out = Out;
        size_t _OutLen = OutLen;

        if (!CC || !_InLen || iconv((iconv_t)CC, &_In, &_InLen, &_Out, &_OutLen) == -1 || 
             iconv((iconv_t)CC, NULL, NULL, &_Out, &_OutLen) == -1)
        {
            size_t n = min(strlen(In),OutLen-1);
            memcpy(Out,In,n*sizeof(char));
            Out[n] = 0;
            if (CC && _InLen)
            {
//              DebugMessage("iconv failed: %d for '%s'",errno,In);
              iconv((iconv_t)CC, NULL, NULL, NULL, NULL); // reset state
            }
        }
        else
            *_Out=0;
    }
}

void CharConvWS(charconv* CC, wchar_t* Out, size_t OutLen, const char* In)
{
    if (OutLen>0)
    {
        ICONV_CONST char* _In = (ICONV_CONST char*)In;
        size_t _InLen = strlen(In)+1;
        char* _Out = (char*)Out;
        size_t _OutLen = OutLen*sizeof(wchar_t);

        if (!CC || !_InLen || iconv((iconv_t)CC, &_In, &_InLen, &_Out, &_OutLen) == -1 || 
                iconv((iconv_t)CC, NULL, NULL, &_Out, &_OutLen) == -1)
        {
            for (;OutLen>1 && *In;++In,--OutLen,++Out)
                *Out = (wchar_t)*In;
            *Out = 0;
            if (CC && _InLen) iconv((iconv_t)CC, NULL, NULL, NULL, NULL); // reset state
        }
        else
            *(wchar_t*)_Out=0;
    }
}

void CharConvSW(charconv* CC, char* Out, size_t OutLen, const wchar_t* In)
{
    if (OutLen>0)
    {
        ICONV_CONST char* _In = (ICONV_CONST char*)In;
        size_t _InLen = wcslen(In)+1;
        char* _Out = Out;
        size_t _OutLen = OutLen;

        if (!CC || !_InLen || iconv((iconv_t)CC, &_In, &_InLen, &_Out, &_OutLen) == -1 || 
                    iconv((iconv_t)CC, NULL, NULL, &_Out, &_OutLen) == -1)
        {
            for (;OutLen>1 && *In;++In,--OutLen,++Out)
                *Out = (char)(*In>255?'*':*In);
            *Out = 0;	
            if (CC && _InLen) iconv((iconv_t)CC, NULL, NULL, NULL, NULL); // reset state
        }
        else
            *_Out=0;
    }
}

void CharConvUS(charconv* CC, utf16_t* Out, size_t OutLen, const char* In)
{
    if (OutLen>0)
    {
        ICONV_CONST char* _In = (ICONV_CONST char*)In;
        size_t _InLen = strlen(In)+1;
        char* _Out = (char*)Out;
        size_t _OutLen = OutLen*sizeof(utf16_t);

        if (!CC || !_InLen || iconv((iconv_t)CC, &_In, &_InLen, &_Out, &_OutLen) == -1 || 
                iconv((iconv_t)CC, NULL, NULL, &_Out, &_OutLen) == -1)
        {
            for (;OutLen>1 && *In;++In,--OutLen,++Out)
                *Out = (utf16_t)*In;
            *Out = 0;
            if (CC && _InLen) iconv((iconv_t)CC, NULL, NULL, NULL, NULL); // reset state
        }
        else
            *(utf16_t*)_Out=0;
    }
}

void CharConvSU(charconv* CC, char* Out, size_t OutLen, const utf16_t* In)
{
    if (OutLen>0)
    {
        ICONV_CONST char* _In = (ICONV_CONST char*)In;
        size_t _InLen = utf16len(In)+1;
        char* _Out = Out;
        size_t _OutLen = OutLen;

        if (!CC || !_InLen || iconv((iconv_t)CC, &_In, &_InLen, &_Out, &_OutLen) == -1 || 
                    iconv((iconv_t)CC, NULL, NULL, &_Out, &_OutLen) == -1)
        {
            for (;OutLen>1 && *In;++In,--OutLen,++Out)
                *Out = (char)(*In>255?'*':*In);
            *Out = 0;	
            if (CC && _InLen) iconv((iconv_t)CC, NULL, NULL, NULL, NULL); // reset state
        }
        else
            *_Out=0;
    }
}

void CharConvWW(charconv* CC, wchar_t* Out, size_t OutLen, const wchar_t* In)
{
    if (OutLen>0)
    {
        ICONV_CONST char* _In = (ICONV_CONST char*)In;
        size_t _InLen = (wcslen(In)+1)*sizeof(wchar_t);
        char* _Out = (char*)Out;
        size_t _OutLen = OutLen*sizeof(wchar_t);

        if (!CC || !_InLen || iconv((iconv_t)CC, &_In, &_InLen, &_Out, &_OutLen) == -1 || 
                iconv((iconv_t)CC, NULL, NULL, &_Out, &_OutLen) == -1)
        {
            size_t n = min(wcslen(In),OutLen-1);
            memcpy(Out,In,n*sizeof(wchar_t));
            Out[n] = 0;
                if (CC && _InLen) iconv((iconv_t)CC, NULL, NULL, NULL, NULL); // reset state
        }
        else
            *(wchar_t*)_Out=0;
   }
}

static NOINLINE void GetDefault()
{
    if (!Current)
    {
        setlocale(LC_ALL,""); // set default for all
        Current = setlocale(LC_CTYPE,"");
        if (Current)
            Current = strrchr(Current,'.');
        if (Current)
           ++Current;
        else
           Current = "";
    }
}

charconv* CharConvOpen(const tchar_t* From, const tchar_t* To)
{
    iconv_t CC;

    GetDefault();

    if (!From || !From[0])
        From = Current;

    if (!To || !To[0])
        To = Current;

    if (tcsicmp(To,From)==0)
        return NULL;

    CC = iconv_open(To,From);
    if (CC == (iconv_t)-1)
        return NULL;

    return (charconv*)CC;
}

void CharConvClose(charconv* p)
{
    if (p)
        iconv_close((iconv_t)p);
}

void CharConvDefault(tchar_t* Out, size_t OutLen)
{
    GetDefault();

    tcscpy_s(Out,OutLen,Current);
}

#endif
