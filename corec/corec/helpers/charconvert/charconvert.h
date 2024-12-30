/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * Copyright (C) CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __CHARCONVERT_H
#define __CHARCONVERT_H

#include <corec/corec.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(corec_EXPORTS)
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
