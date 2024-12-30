/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include "str.h"

#if defined(TARGET_OSX)

#include <locale.h>

int tcscmp(const tchar_t* a,const tchar_t* b)
{
#ifdef UNICODE
	return wcscmp(a,b);
#else
	return strcmp(a, b);
#endif
}

int tcsncmp(const tchar_t* a,const tchar_t* b,size_t n)
{
#ifdef UNICODE
	return wcsncmp(a,b,n);
#else
	return strncmp(a, b, n);
#endif
}

int tcsicmp(const tchar_t* a,const tchar_t* b)
{
#ifdef UNICODE
    fprintf(stderr, "Not supported yet: %s\n", __FUNCTION__);
	return 0;
#else
	return strcasecmp(a, b);
#endif
}

int tcsnicmp(const tchar_t* a,const tchar_t* b,size_t n)
{
#ifdef UNICODE
    fprintf(stderr, "Not supported yet: %s\n", __FUNCTION__);
	return 0;
#else
	return strncasecmp(a, b, n);
#endif
}

tchar_t* TcsToUpper(tchar_t* Out,size_t OutLen,const tchar_t* In)
{
    if (OutLen)
    {
        tcscpy_s(Out,OutLen,In);
        tcsupr(Out);  //TODO: doesn't support multibyte
    }
    return Out;
}

#endif
