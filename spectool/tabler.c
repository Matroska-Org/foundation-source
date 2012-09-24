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

/**
 * A tool to format the table in the specs
 *  input: spec.xml
 *  output: table_spec.xml
 */

#include "tabler_stdafx.h"
#ifndef CONFIG_EBML_UNICODE
#define CONFIG_EBML_UNICODE
#endif
#include "parser/parser.h"

static void DumpLevel(parser *p, textwriter *parent)
{
    tchar_t Element[MAXDATA], String[MAXDATA], Value[MAXLINE];

    for (;;) {
        if (ParserElementContent(p,Value,TSIZEOF(Value)) && Value[0])
        {
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("\n"),T(" "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("  "),T(" "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("  "),T(" "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("  "),T(" "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("  "),T(" "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("<br/>"),T("<br>")); // Drupal doesn't like <br/>
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("\""),T("&quot;"));

            if (Value[0])
                TextElementAppendData(parent,Value);
        }
        else if (ParserIsElementNested(p, Element, TSIZEOF(Element)))
        {
            textwriter child;
            TextElementBegin(&child, parent, Element);

            while (ParserIsAttrib(p, String, TSIZEOF(String)))
            {
                if (ParserAttribString(p, Value, TSIZEOF(Value))) {
                    if (tcsisame_ascii(String,T("href")) && tcsstr(Value,T("http://www.matroska.org/technical/specs/"))==Value) {
                        tchar_t *Val = Value+40;
                        if (tcsstr(Val,T("index.html"))==Val)
                            Val += 10;
                        TextAttribEx(&child, String, Val, 0, TYPE_STRING);
                    }
                    else
                    {
                        if (tcsisame_ascii(String,T("id")))
                            tcsreplace(Value,TSIZEOF(Value),T(" "),T("_"));
                        TextAttribEx(&child, String, Value, 0, TYPE_STRING);
                    }
                }
            }

            if (p->ElementEof)
                p->ElementEof = 0;
            else
                DumpLevel(p, &child);

            TextElementEnd(&child);
        }
        else
        {
            ParserSkipAfter(p,'>');
            break;
        }
    }
}

int main(void)
{
    parsercontext p;
    parser parseIn;
    stream *Input = NULL,*Output = NULL;
    //tchar_t Element[MAXLINE], String[MAXLINE], Value[MAXLINE];

    ParserContext_Init(&p,NULL,NULL,NULL);
    StdAfx_Init((nodemodule*)&p);

    Input = StreamOpen(&p,T("spec.xml"),SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    Output = StreamOpen(&p,T("table_spec.xml"),SFLAG_WRONLY|SFLAG_CREATE);

    memset(&parseIn, 0, sizeof(parseIn));

    if (ParserStreamXML(&parseIn, Input, &p, T("table"), 0)==ERR_NONE)
    {
        textwriter outFile;

        memset(&outFile,0,sizeof(outFile));
        outFile.Stream = Output;
        TextElementXML(&p, &outFile, T("table"));

		outFile.SafeFormat = 1; // Drupal doesn't like <td/>

        DumpLevel(&parseIn, &outFile);

        TextElementEnd(&outFile); // /table
    }

    StreamClose(Input);
    StreamClose(Output);

	StdAfx_Done((nodemodule*)&p);
    ParserContext_Done(&p);
    return 0;
}
