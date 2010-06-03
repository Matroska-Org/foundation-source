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

#include "str.h"

#if defined(TARGET_SYMBIAN)

#undef T
#undef NULL
#include <e32std.h>

tchar_t* wcsstr(const tchar_t* s,const tchar_t* p)
{
    size_t sn = wcslen(s);
    size_t pn = wcslen(p);
    for (;sn>=pn;++s,--sn)
        if (memcmp(s,p,pn*sizeof(tchar_t))==0)
            return (tchar_t*)s;
    return NULL;
}

tchar_t* wcschr(const tchar_t *p, tchar_t i)
{
	for (;*p;++p)
		if (*p == i)
			return (tchar_t*)p;
	return NULL;
}

tchar_t* wcsrchr(const tchar_t *p, tchar_t i)
{
	int n;
	for (n=wcslen(p);--n>=0;)
		if (p[n] == i)
			return (tchar_t*)(p+n);
	return NULL;
}

int tcsicmp(const tchar_t* a,const tchar_t* b) 
{
    TPtrC _a((TText*)a);
    TPtrC _b((TText*)b);
    return _a.CompareF(_b);
}

int tcsnicmp(const tchar_t* a,const tchar_t* b,size_t n) 
{
    size_t na = tcslen(a);
    size_t nb = tcslen(b);
    TPtrC _a((TText*)a,min(na,n));
    TPtrC _b((TText*)b,min(nb,n));
    return _a.CompareF(_b);
}

int tcscmp(const tchar_t* a,const tchar_t* b) 
{
    TPtrC _a((TText*)a);
    TPtrC _b((TText*)b);
    return _a.Compare(_b);
}

int tcsncmp(const tchar_t* a,const tchar_t* b,size_t n) 
{
    size_t na = tcslen(a);
    size_t nb = tcslen(b);
    TPtrC _a((TText*)a,min(na,n));
    TPtrC _b((TText*)b,min(nb,n));
    return _a.Compare(_b);
}

tchar_t* TcsToUpper(tchar_t* Out,size_t OutLen,const tchar_t* In)
{
    if (OutLen)
    {
        TPtr _Out((TText*)Out,OutLen-1);
        TPtrC _In((TText*)In,tcslen(In));
        _Out.CopyUC(_In);
    	Out[_Out.Length()]=0;
    }
    return Out;
}

#endif
