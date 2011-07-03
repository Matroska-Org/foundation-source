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

#ifndef __CHARCONVERT_H
#define __CHARCONVERT_H

#include "corec/corec.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CHARCONVERT_EXPORTS)
#define CHARCONVERT_DLL DLLEXPORT
#elif defined(CHARCONVERT_IMPORTS)
#define CHARCONVERT_DLL DLLIMPORT
#else
#define CHARCONVERT_DLL
#endif

typedef struct charconv charconv;

#define MAX_CHARSET_NAME    16

#define CHARSET_DEFAULT	T("")
#if defined(TARGET_WIN)
#define CHARSET_WCHAR	T("UCS-2")
#elif SIZEOF_WCHAR==4
#define CHARSET_WCHAR	T("UTF-32")
#elif SIZEOF_WCHAR==2
#define CHARSET_WCHAR	T("UTF-16")
#else
#error unsupported wchar_t size!
#endif
#define CHARSET_UTF8	T("UTF-8")
#define CHARSET_UTF16	T("UTF-16")

CHARCONVERT_DLL void CharConvDefault(tchar_t* Out, size_t OutLen);
CHARCONVERT_DLL charconv* CharConvOpen(const tchar_t* From, const tchar_t* To);
CHARCONVERT_DLL void CharConvClose(charconv*);
CHARCONVERT_DLL void CharConvSS(charconv*, char* Out,    size_t OutLen, const char* In);
CHARCONVERT_DLL void CharConvWS(charconv*, wchar_t* Out, size_t OutLen, const char* In);
CHARCONVERT_DLL void CharConvSW(charconv*, char* Out,    size_t OutLen, const wchar_t* In);
CHARCONVERT_DLL void CharConvWW(charconv*, wchar_t* Out, size_t OutLen, const wchar_t* In);

#if SIZEOF_WCHAR==2
#define CharConvUS CharConvWS
#define CharConvSU CharConvSW
#define CharConvUW CharConvWW
#define CharConvWU CharConvWW
#else
CHARCONVERT_DLL void CharConvUS(charconv*, utf16_t* Out, size_t OutLen, const char* In);
CHARCONVERT_DLL void CharConvSU(charconv*, char* Out,    size_t OutLen, const utf16_t* In);
CHARCONVERT_DLL void CharConvUW(charconv*, utf16_t* Out, size_t OutLen, const wchar_t* In);
CHARCONVERT_DLL void CharConvWU(charconv*, wchar_t* Out, size_t OutLen, const utf16_t* In);
#endif

#ifdef UNICODE
#define CharConvTS CharConvWS
#define CharConvTW CharConvWW
#define CharConvST CharConvSW
#define CharConvWT CharConvWW
#define CharConvTU CharConvWU
#define CharConvUT CharConvUW
#else
#define CharConvTS CharConvSS
#define CharConvTW CharConvSW
#define CharConvST CharConvSS
#define CharConvWT CharConvWS
#define CharConvTU CharConvSU
#define CharConvUT CharConvUS
#endif

#ifdef __cplusplus
}
#endif

#endif
