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

#include <charconv.h>

#include "charconvert.h"
#include "corec/str/str.h"

#if defined(TARGET_SYMBIAN)

#include <utf.h>
#include <f32file.h>

void CharConvWS(charconv* CC, wchar_t* Out, size_t OutLen, const char* In)
{
    if (OutLen)
    {
	    TPtrC8 _In((const uint8_t*)In);
	    TPtr16 _Out((uint16_t*)Out,OutLen-1);
    	int State = CCnvCharacterSetConverter::KStateDefault;

        if (CC)
            ((CCnvCharacterSetConverter*)CC)->ConvertToUnicode(_Out,_In,State);
        else
            CnvUtfConverter::ConvertToUnicodeFromUtf8(_Out,_In);

    	Out[_Out.Length()]=0;
    }
}

void CharConvSW(charconv* CC, char* Out, size_t OutLen, const wchar_t* In)
{
    if (OutLen)
    {
	    TPtrC16 _In((const uint16_t*)In);
	    TPtr8 _Out((uint8_t*)Out,OutLen-1);

        if (CC)
            ((CCnvCharacterSetConverter*)CC)->ConvertFromUnicode(_Out,_In);
        else
            CnvUtfConverter::ConvertFromUnicodeToUtf8(_Out,_In);

    	Out[_Out.Length()]=0;
    }
}

void CharConvWW(charconv* UNUSED_PARAM(CC), wchar_t* Out, size_t OutLen, const wchar_t* In)
{
	tcscpy_s(Out,OutLen,In);
}

typedef struct codepage
{
    const tchar_t* Name;
    uint16_t CodePage;

} codepage;

static const codepage CodePage[] = 
{
    {T("Shift_JIS"),    932},
    {T("GB2312"),       936},
    {T("BIG5"),         950},
    {T("ISO-8859-1"),   1252},
    {T("ISO-8859-2"),   1250},
    {T("ISO-8859-3"),   1254},
    {T("ISO-8859-4"),   1257},
    {T("ISO-8859-5"),   1251},
    {T("ISO-8859-6"),   1256},
    {T("ISO-8859-7"),   1253},
    {T("ISO-8859-8"),   1255},
    {NULL},
};

charconv* CharConvOpen(const tchar_t* From, const tchar_t* To)
{
    const tchar_t* Name = From ? From:To;

    if (!Name || tcsicmp(Name,CHARSET_WCHAR)==0 || tcsicmp(Name,CHARSET_UTF8)==0)
        return NULL;

    tchar_t Default[MAX_CHARSET_NAME];
    if (!Name[0])
    {
        CharConvDefault(Default,TSIZEOF(Default));
        Name = Default;
    }

    int CP;
	if (stscanf(Name,T("CP%d"),&CP)==1 ||
        stscanf(Name,T("windows-%d"),&CP)==1)
    {
        const codepage* i;
        for (i=CodePage;i->Name;++i)
        {
	        if (i->CodePage == CP)
            {
                Name = i->Name;
                break;
            }
        }
    }

    TBuf8<MAX_CHARSET_NAME> Name8;
    CnvUtfConverter::ConvertFromUnicodeToUtf8(Name8,TPtrC((TText*)Name));
    
    RFs FsSession;
    FsSession.Connect();

    CCnvCharacterSetConverter* p = CCnvCharacterSetConverter::NewL();
    TUint Id = p->ConvertStandardNameOfCharacterSetToIdentifierL(Name8,FsSession);

	if (!Id || CCnvCharacterSetConverter::EAvailable != p->PrepareToConvertToOrFromL(Id,FsSession))
    {
        delete p;
        p = NULL;
    }
    FsSession.Close();

    return (charconv*)p;
}

void CharConvClose(charconv* p)
{
    if (p)
        delete (CCnvCharacterSetConverter*)p;
}

void CharConvDefault(tchar_t* Out, size_t OutLen)
{
    tcscpy_s(Out,OutLen,T("ISO-8859-1"));
}

#endif
