/*
 * $Id$
 * Copyright (c) 2011, Matroska (non-profit organisation)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Matroska assocation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY the Matroska association ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL The Matroska Foundation BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "spec_element.h"

void ReadElementText(parser *p, tchar_t *Out, size_t OutLen)
{
    tchar_t Element[MAXDATA], String[MAXDATA], Value[MAXLINE];
	size_t len = 0;
	Out[0] = 0;
	for (;;) {
		if (ParserElementContent(p, Out+len, OutLen-len) && Out[len])
		{
			if (Out[0]) tcsreplace(Out+len, OutLen-len, T("\n"),T(" "));
			if (Out[0]) tcsreplace(Out+len, OutLen-len, T("  "),T(" "));
			if (Out[0]) tcsreplace(Out+len, OutLen-len, T("  "),T(" "));
			if (Out[0]) tcsreplace(Out+len, OutLen-len, T("  "),T(" "));
			if (Out[0]) tcsreplace(Out+len, OutLen-len, T("  "),T(" "));
			if (Out[0]) tcsreplace(Out+len, OutLen-len, T("  "),T(" "));
			if (Out[0]) tcsreplace(Out+len, OutLen-len, T("&quot;"),T("\""));
			ParserHTMLChars(p, Out+len, OutLen-len);
			len += tcslen(Out+len);
		}
        else if (ParserIsElementNested(p, Element, TSIZEOF(Element)))
        {
            if (len)
                tcscat_s(Out+len, OutLen-len,T(" "));
            stprintf_s(Out+len, OutLen-len, T("<%s"), Element);

            while (ParserIsAttrib(p, String, TSIZEOF(String)))
                if (ParserAttribString(p, Value, TSIZEOF(Value)))
                    stcatprintf_s(Out+len, OutLen-len, T(" %s=\"%s\""), String, Value);

            if (p->ElementEof)
            {
                stcatprintf_s(Out+len, OutLen-len, T("/>"));
                p->ElementEof = 0;
            }
            else
            {
                stcatprintf_s(Out+len, OutLen-len, T(">"));
                len += tcslen(Out+len);

                ReadElementText(p, Out+len, OutLen-len);
                len += tcslen(Out+len);

                stprintf_s(Out+len, OutLen-len, T("</%s>"), Element);
            }
            len += tcslen(Out+len);
		}
        else
        {
            ParserSkipAfter(p,'>');
            break;
        }
	}
}

void ReadSpecElement(SpecElement *elt, parser *p)
{
    tchar_t String[MAXDATA], Value[MAXLINE];

    elt->InDivX = -1;
    elt->InWebM = -1;
	while (ParserIsAttrib(p, String, TSIZEOF(String)))
	{
		if (ParserAttribString(p, Value, TSIZEOF(Value))) {
            const tchar_t *s = Value;
        	intptr_t intval;

			if (tcsisame_ascii(String,T("name")))
                tcscpy_s(elt->Name, TSIZEOF(elt->Name), Value);
			else if (tcsisame_ascii(String,T("cppname")))
                tcscpy_s(elt->CppName, TSIZEOF(elt->CppName), Value);
            else if (tcsisame_ascii(String,T("level"))) {
                ExprIsInt(&s,&intval);
                elt->Level = intval;
            }
            else if (tcsisame_ascii(String,T("recursive"))) {
                ExprIsInt(&s,&intval);
                elt->Recursive = intval!=0;
            }
            else if (tcsisame_ascii(String,T("id"))) {
                ExprIsHex(&s,&intval);
                elt->Id = intval;
            }
            else if (tcsisame_ascii(String,T("type"))) {
                if (tcsisame_ascii(Value,T("master")))
                    elt->Type = EBML_MASTER;
                else if (tcsisame_ascii(Value,T("integer")))
                    elt->Type = EBML_INTEGER;
                else if (tcsisame_ascii(Value,T("uinteger")))
                    elt->Type = EBML_UNSIGNED_INTEGER;
                else if (tcsisame_ascii(Value,T("date")))
                    elt->Type = EBML_DATE;
                else if (tcsisame_ascii(Value,T("float")))
                    elt->Type = EBML_FLOAT;
                else if (tcsisame_ascii(Value,T("string")))
                    elt->Type = EBML_STRING;
                else if (tcsisame_ascii(Value,T("utf-8")))
                    elt->Type = EBML_UNICODE_STRING;
                else if (tcsisame_ascii(Value,T("binary")))
                    elt->Type = EBML_BINARY;
                else
                    elt->Type = EBML_unknown;
            }
            else if (tcsisame_ascii(String,T("mandatory"))) {
                ExprIsInt(&s,&intval);
                elt->Mandatory = intval!=0;
            }
            else if (tcsisame_ascii(String,T("multiple"))) {
                ExprIsInt(&s,&intval);
                elt->Multiple = intval!=0;
            }
            else if (tcsisame_ascii(String,T("default")))
                tcscpy_s(elt->DefaultValue, TSIZEOF(elt->DefaultValue), Value);
            else if (tcsisame_ascii(String,T("range")))
                tcscpy_s(elt->Range, TSIZEOF(elt->Range), Value);
            else if (tcsisame_ascii(String,T("minver"))) {
                ExprIsInt(&s,&intval);
                elt->MinVersion = intval;
            }
            else if (tcsisame_ascii(String,T("maxver"))) {
                ExprIsInt(&s,&intval);
                elt->MaxVersion = intval;
            }
            else if (tcsisame_ascii(String,T("bytesize"))) {
                ExprIsInt(&s,&intval);
                elt->ByteSize = intval;
            }
            else if (tcsisame_ascii(String,T("webm"))) {
                ExprIsInt(&s,&intval);
                elt->InWebM = intval!=0;
            }
            else if (tcsisame_ascii(String,T("divx"))) {
                ExprIsInt(&s,&intval);
                elt->InDivX = intval!=0;
            }
		}
	}
    if (elt->InDivX==-1)
    {
        if (elt->MinVersion==1)
            elt->InDivX = 1;
        else
            elt->InDivX=0;
    }
    if (elt->InWebM==-1)
    {
        if (elt->MinVersion==1)
            elt->InWebM = 1;
        else
            elt->InWebM=0;
    }

    ReadElementText(p, elt->Description, TSIZEOF(elt->Description));
}

