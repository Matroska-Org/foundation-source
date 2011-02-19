/*
 * $Id: mkclean.c 676 2011-02-12 14:51:07Z robux4 $
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

