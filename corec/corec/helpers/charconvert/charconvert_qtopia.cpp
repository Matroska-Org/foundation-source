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

#include <QTextCodec>
#include <string>

#include "charconvert.h"
#include "corec/str/str.h"

#if defined(TARGET_QTOPIA)

#include <cstdio>

typedef struct charconv
{
  QTextCodec *From;
  QTextCodec *To;
} charconv;

void CharConvSS(charconv* CC, char* Out, size_t OutLen, const char* In)
{
    if (OutLen>0)
    {
        size_t _InLen;
        if (!CC)
        {
            size_t n = min(strlen(In),OutLen-1);
            memcpy(Out,In,n*sizeof(char));
            Out[n] = 0;
        }
        else if (!(_InLen = strlen(In)))
            Out[0] = 0;
        else
        {
            QString From = CC->From->toUnicode(In);
            QByteArray To = CC->To->fromUnicode(From);
            tcscpy_s(Out,OutLen,To.constData());
        }
    }
}

void CharConvWS(charconv* CC, wchar_t* Out, size_t OutLen, const char* In)
{
    if (OutLen>0)
    {
        size_t _InLen;
        if (!CC)
        {
            for (;OutLen>1 && *In;++In,--OutLen,++Out)
                *Out = (wchar_t)*In;
            *Out = 0;
        }
        else if (!(_InLen = strlen(In)))
            Out[0] = 0;
        else
        {
            QString From = CC->From->toUnicode(In);
#ifndef QTOPIA_PHONE
            std::wstring To = From.toStdWString();
            const wchar_t *_In = To.c_str();
            for (;OutLen>1 && *_In;++_In,--OutLen,++Out)
                *Out = (wchar_t)*_In;
            *Out = 0;
#endif
        }
    }
}

void CharConvSW(charconv* CC, char* Out, size_t OutLen, const wchar_t* In)
{
    if (OutLen>0)
    {
        size_t _InLen;
        if (!CC)
        {
            for (;OutLen>1 && *In;++In,--OutLen,++Out)
                *Out = (char)*In;
            *Out = 0;
        }
        else if (!(_InLen = wcslen(In)))
            Out[0] = 0;
        else
        {
#ifndef QTOPIA_PHONE
            QString From = QString::fromStdWString(In);
            QByteArray To = CC->To->fromUnicode(From);
            tcscpy_s(Out,OutLen,To.constData());
#endif
        }
    }
}

void CharConvUS(charconv* CC, utf16_t* Out, size_t OutLen, const char* In)
{
    if (OutLen>0)
    {
        size_t _InLen;
        if (!CC)
        {
            for (;OutLen>1 && *In;++In,--OutLen,++Out)
                *Out = (utf16_t)*In;
            *Out = 0;
        }
        else if (!(_InLen = strlen(In)))
            Out[0] = 0;
        else
        {
            QString From = CC->From->toUnicode(In);
#ifndef QTOPIA_PHONE
            const ushort *_In = From.utf16();
            for (;OutLen>1 && *_In;++_In,--OutLen,++Out)
                *Out = (utf16_t)*_In;
            *Out = 0;
#endif
        }
    }
}

void CharConvSU(charconv* CC, char* Out, size_t OutLen, const utf16_t* In)
{
    if (OutLen>0)
    {
        size_t _InLen;
        if (!CC)
        {
            for (;OutLen>1 && *In;++In,--OutLen,++Out)
                *Out = (char)*In;
            *Out = 0;
        }
        else if (!(_InLen = utf16len(In)))
            Out[0] = 0;
        else
        {
#ifndef QTOPIA_PHONE
            QString From = QString::fromUtf16((const ushort*)In);
            QByteArray To = CC->To->fromUnicode(From);
            tcscpy_s(Out,OutLen,To.constData());
#endif
        }
    }
}

void CharConvWW(charconv* CC, wchar_t* Out, size_t OutLen, const wchar_t* In)
{
    if (OutLen>0)
    {
        size_t _InLen;
        if (!CC)
        {
            for (;OutLen>1 && *In;++In,--OutLen,++Out)
                *Out = *In;
            *Out = 0;
        }
        else if (!(_InLen = wcslen(In)))
            Out[0] = 0;
#if 0
        ICONV_CONST char* _In = (ICONV_CONST char*)In;
        size_t _InLen = (wcslen(In)+1)*sizeof(wchar_t);
        char* _Out = (char*)Out;
        size_t _OutLen = OutLen*sizeof(wchar_t);

        if (!CC || !_InLen || iconv((iconv_t)CC, &_In, &_InLen, &_Out, &_OutLen) == -1 || 
                iconv((iconv_t)CC, NULL, NULL, &_Out, &_OutLen) == -1)
        {
            size_t n = min(wcslen(In),OutLen-1);
            memcpy(Out,In,n*sizeof(wchar_t));
            Out[n] = 0;
                if (CC && _InLen) iconv((iconv_t)CC, NULL, NULL, NULL, NULL); // reset state
        }
#endif
    }
}

static bool_t CharConvWinCP(const tchar_t *Conv, tchar_t *Fill, size_t FillLen)
{
    if (tcsicmp(Conv,T("CP1252"))==0)
    {
        tcscpy_s(Fill,FillLen,T("latin1"));
        return 1;
    }
    if (Conv && Conv[0] == 'C' && Conv[1] == 'P' && (Conv[2] <= '9' && Conv[2] >= '0'))
    {
        tcscpy_s(Fill,FillLen,T("Windows-"));
        tcscat_s(Fill,FillLen,Conv+2);
        return 1;
    }
    return 0;
}

charconv* CharConvOpen(const tchar_t* From, const tchar_t* To)
{
    charconv *p,CC = {NULL,NULL};
    tchar_t AltCP[20];

    if (!From || !From[0])
        From = T("UTF-8");
    else if (CharConvWinCP(From,AltCP,TSIZEOF(AltCP)))
        From = AltCP;
    if (!CC.From)
        CC.From = QTextCodec::codecForName(From);

    if (!To || !To[0])
        To = T("UTF-8");
    else if (CharConvWinCP(To,AltCP,TSIZEOF(AltCP)))
        To = AltCP;
    if (!CC.To)
        CC.To = QTextCodec::codecForName(To);

    if (!CC.To || !CC.From || CC.To == CC.From)
        return NULL;

    p = (charconv*)malloc(sizeof(charconv));
    memcpy(p,&CC,sizeof(charconv));
    return p;
}

void CharConvClose(charconv* p)
{
    if (p)
        free(p);
}

void CharConvDefault(tchar_t* Out, size_t OutLen)
{
    if (Out && OutLen)
    {
        QTextCodec *Locale = QTextCodec::codecForLocale();
        if (Locale)
            tcscpy_s(Out,OutLen,Locale->name().constData());
    }
}

#endif
