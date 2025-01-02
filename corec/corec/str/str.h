/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __STR_H
#define __STR_H

#include <corec/corec.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(corec_EXPORTS)
#define STR_DLL DLLEXPORT
#elif defined(STR_IMPORTS)
#define STR_DLL DLLIMPORT
#else
#define STR_DLL
#endif

STR_DLL bool_t IsSpace(int);
STR_DLL bool_t IsAlpha(int);
STR_DLL bool_t IsDigit(int);
STR_DLL int Hex(int);
STR_DLL int StrListIndex(const tchar_t* s, const tchar_t* List);

STR_DLL bool_t tcsisame_ascii(const tchar_t* a,const tchar_t* b);
STR_DLL int tcsicmp_ascii(const tchar_t* a,const tchar_t* b);
STR_DLL int tcsnicmp_ascii(const tchar_t* a,const tchar_t* b,size_t n);

STR_DLL tchar_t* TcsToUpper(tchar_t* Out,size_t OutLen,const tchar_t* In);
STR_DLL int tcsicmp(const tchar_t* a,const tchar_t* b);
STR_DLL int tcsnicmp(const tchar_t* a,const tchar_t* b,size_t n);
STR_DLL int tcscmp(const tchar_t* a,const tchar_t* b);
STR_DLL int tcsncmp(const tchar_t* a,const tchar_t* b,size_t n);

STR_DLL tchar_t* tcsupr(tchar_t* a); // only for ascii!
STR_DLL tchar_t* tcslwr(tchar_t* a); // only for ascii!
STR_DLL tchar_t* tcscpy_s(tchar_t* Out,size_t OutLen,const tchar_t* In);
STR_DLL tchar_t* tcsncpy_s(tchar_t* Out,size_t OutLen,const tchar_t* In,size_t n);
STR_DLL tchar_t* tcscat_s(tchar_t* Out,size_t OutLen,const tchar_t* In);
STR_DLL tchar_t* tcsncat_s(tchar_t* Out,size_t OutLen,const tchar_t* In,size_t n);
STR_DLL int stscanf(const tchar_t* In, const tchar_t* Mask, ...);
STR_DLL void stprintf_s(tchar_t* Out,size_t OutLen,const tchar_t* Mask, ...);
STR_DLL void stcatprintf_s(tchar_t* Out,size_t OutLen,const tchar_t* Mask, ...);
STR_DLL void vstprintf_s(tchar_t* Out,size_t OutLen,const tchar_t* Mask,va_list Arg);

STR_DLL size_t FourCCToString(tchar_t* Out, size_t OutLen, fourcc_t FourCC);

STR_DLL size_t utf16len(const utf16_t *);

STR_DLL size_t tcsbytes(const tchar_t*);

#ifdef __cplusplus
}
#endif

#endif
