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

#ifndef __STR_H
#define __STR_H

#include "corec/corec.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STR_EXPORTS)
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
STR_DLL int stscanf_s(const tchar_t* In, size_t *InLen, const tchar_t* Mask, ...);
STR_DLL void stprintf_s(tchar_t* Out,size_t OutLen,const tchar_t* Mask, ...);
STR_DLL void stcatprintf_s(tchar_t* Out,size_t OutLen,const tchar_t* Mask, ...);
STR_DLL void vstprintf_s(tchar_t* Out,size_t OutLen,const tchar_t* Mask,va_list Arg);

STR_DLL tchar_t* tcsreplace(tchar_t* Out,size_t OutLen, const tchar_t *Src, const tchar_t *Dst);
STR_DLL tchar_t* tcsreplacechar(tchar_t *ts, tchar_t From, tchar_t To);

static INLINE tchar_t* tcsclr_s(tchar_t *ts, size_t len)   
{
    if (ts && len)
        *ts = 0;
    return ts;
}

STR_DLL size_t utf16len(const utf16_t *);

#ifndef stricmp
#if defined(TARGET_WIN)
#define stricmp(x,y) _stricmp(x,y)
#elif defined(TARGET_PALMOS)
#define stricmp(x,y) tcsicmp(x,y)
#else
#define stricmp(x,y) strcasecmp(x,y)
#endif
#endif

#ifdef TARGET_SYMBIAN
STR_DLL int wcsncmp(const tchar_t *,const tchar_t *,size_t);
STR_DLL tchar_t* wcschr(const tchar_t *, tchar_t);
STR_DLL tchar_t* wcsrchr(const tchar_t *, tchar_t);
STR_DLL tchar_t* wcsstr(const tchar_t*,const tchar_t*);
#endif

STR_DLL size_t tcsbytes(const tchar_t*);

#ifdef __cplusplus
}
#endif

#endif

