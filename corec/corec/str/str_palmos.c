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

#if defined(TARGET_PALMOS)

#include <PalmOS.h>

int tcsicmp(const tchar_t* a,const tchar_t* b) 
{
    UInt16 na = (UInt16)tcslen(a);
    UInt16 nb = (UInt16)tcslen(b);
    return TxtCaselessCompare(a,na,&na,b,nb,&nb);
}

int tcsnicmp(const tchar_t* a,const tchar_t* b,size_t n) 
{
    UInt16 na = (UInt16)(min(tcslen(a),n));
    UInt16 nb = (UInt16)(min(tcslen(b),n));
    return TxtCaselessCompare(a,na,&na,b,nb,&nb);
}

int tcscmp(const tchar_t* a,const tchar_t* b) 
{
    UInt16 na = (UInt16)tcslen(a);
    UInt16 nb = (UInt16)tcslen(b);
    return TxtCompare(a,na,&na,b,nb,&nb);
}

int tcsncmp(const tchar_t* a,const tchar_t* b,size_t n) 
{
    UInt16 na = (UInt16)(min(tcslen(a),n));
    UInt16 nb = (UInt16)(min(tcslen(b),n));
    return TxtCompare(a,na,&na,b,nb,&nb);
}

tchar_t* TcsToUpper(tchar_t* Out,size_t OutLen,const tchar_t* In)
{
    if (OutLen)
    {
		UInt16 n = (UInt16)(OutLen-1);
		if (TxtTransliterate(In,(UInt16)tcslen(In),Out,&n,translitOpUpperCase) == errNone)
		{
			Out[min(n,OutLen-1)]=0;
			return Out;
		}

	    // fallback
        for (;*In && OutLen>1;++In,++Out,--OutLen)
            *Out = (tchar_t)toupper(*In);
        *Out = 0;
    }
    return Out;
}
    
#endif
