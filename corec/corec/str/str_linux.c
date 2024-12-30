/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include "str.h"

#include <locale.h>

int tcscmp(const tchar_t* a,const tchar_t* b)
{
	return strcmp(a, b);
}

int tcsncmp(const tchar_t* a,const tchar_t* b,size_t n)
{
	return strncmp(a, b, n);
}

int tcsicmp(const tchar_t* a,const tchar_t* b)
{
	return strcasecmp(a, b);
}

int tcsnicmp(const tchar_t* a,const tchar_t* b,size_t n)
{
	return strncasecmp(a, b, n);
}

tchar_t* TcsToUpper(tchar_t* Out,size_t OutLen,const tchar_t* In)
{
    if (OutLen)
    {
        tcscpy_s(Out,OutLen,In);
        tcsupr(Out);  //todo: doesn't support multibyte
    }
    return Out;
}
