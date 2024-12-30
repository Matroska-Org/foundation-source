/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include "str.h"

#if defined(TARGET_WIN)

#ifdef _MSC_VER
#define wcsicmp _wcsicmp
#define wcsnicmp _wcsnicmp
#endif

#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int tcsicmp(const tchar_t* a,const tchar_t* b)
{
    int i = CompareString(LOCALE_USER_DEFAULT,NORM_IGNORECASE,a,-1,b,-1);
    if (i)
        return i-CSTR_EQUAL;

    // fallback
#ifdef UNICODE
	return wcsicmp(a,b);
#else
	return stricmp(a,b);
#endif
}

int tcsnicmp(const tchar_t* a,const tchar_t* b,size_t n)
{
    int i = CompareString(LOCALE_USER_DEFAULT,NORM_IGNORECASE,a,min(tcslen(a),n),b,min(tcslen(b),n));
    if (i)
        return i-CSTR_EQUAL;

    // fallback
#ifdef UNICODE
	return wcsnicmp(a,b,n);
#else
	return strnicmp(a,b,n);
#endif
}

int tcscmp(const tchar_t* a,const tchar_t* b)
{
    int i = CompareString(LOCALE_USER_DEFAULT,0,a,-1,b,-1);
    if (i)
        return i-CSTR_EQUAL;

    // fallback
#ifdef UNICODE
	return wcscmp(a,b);
#else
	return strcmp(a,b);
#endif
}

int tcsncmp(const tchar_t* a,const tchar_t* b,size_t n)
{
    int i = CompareString(LOCALE_USER_DEFAULT,0,a,min(tcslen(a),n),b,min(tcslen(b),n));
    if (i)
        return i-CSTR_EQUAL;

    // fallback
#ifdef UNICODE
	return wcsncmp(a,b,n);
#else
	return strncmp(a,b,n);
#endif
}

tchar_t* TcsToUpper(tchar_t* Out,size_t OutLen,const tchar_t* In)
{
    if (LCMapString(LOCALE_USER_DEFAULT,LCMAP_UPPERCASE,In,-1,Out,OutLen))
        return Out;

    // fallback
    if (OutLen)
    {
        for (;*In && OutLen>1;++In,++Out,--OutLen)
            *Out = (tchar_t)toupper(*In);
        *Out = 0;
    }
    return Out;
}

#endif
