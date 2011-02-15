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

#include "spec2data_stdafx.h"
#ifndef CONFIG_EBML_UNICODE
#define CONFIG_EBML_UNICODE
#endif
#include "parser/parser.h"

#define SPEC_ELEMENT_CLASS  FOURCC('S','P','E','C')

typedef struct SpecElement {
    node Base;

    tchar_t Name[MAXPATH]; // verify it matches the ID
    int level;
    int32_t Id;
    bool_t Mandatory;
    bool_t Multiple;
    int Range;
    tchar_t DefaultValue[16];
    char Type;
    int MinVersion, MaxVersion;
    bool_t InWebM;
    tchar_t Description[MAXLINE];
} SpecElement;


static void SkipLevel(parser *p)
{
	// TODO: move this in parser2.c when it's stable
	ParserElementSkipNested(p);
    ParserSkipAfter(p,'>');
}


static void ReadElementText(parser *p, tchar_t *Out, size_t OutLen)
{
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
        else if (ParserIsElementNested(p, Out+len, OutLen-len))
        {
			// TODO: better reading
			SkipLevel(p);
		}
        else
        {
            ParserSkipAfter(p,'>');
            break;
        }
	}
}

static void FillSpecElement(SpecElement *elt, parser *p)
{
    tchar_t Element[MAXDATA], /*String[MAXDATA], */Value[MAXLINE];
    bool_t isSpecData;
	int EltCount = 0;

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

            //if (Value[0]) TextElementAppendData(current,Value);
        }
        else if (ParserIsElementNested(p, Element, TSIZEOF(Element)))
        {
            //textwriter child;
            //TextElementBegin(&child, current, Element);

			if (elt==NULL || tcsisame_ascii(Element,T("th"))) {
				SkipLevel(p);
				elt = NULL; // we don't fill any element as it's not a correct one
			} else {
				isSpecData = tcsisame_ascii(Element,T("td"));

				if (isSpecData)
				{
					ParserElementSkip(p); // we don't care about the td attributes

					if (EltCount==0) {
						ReadElementText(p, Value, TSIZEOF(Value));
						tcscpy_s(elt->Name, TSIZEOF(elt->Name), Value);
					}
					else
					if (EltCount==1) {
						intptr_t level;
						const tchar_t *s = Value;
						ReadElementText(p, Value, TSIZEOF(Value));
						ExprIsInt(&s,&level);
						elt->level = level;
					}
					else
					if (EltCount==2) {
						intptr_t id0,id1,id2,id3;
						const tchar_t *s = Value;
						ReadElementText(p, Value, TSIZEOF(Value));
						if (ExprIsTokenEx(&s,T("[%x][%x][%x][%x]"),&id0,&id1,&id2,&id3))
							elt->Id = FOURCCBE(id0, id1, id2, id3);
						else if (ExprIsTokenEx(&s,T("[%x][%x][%x]"),&id0,&id1,&id2))
							elt->Id = FOURCCBE(id0, id1, id2, 0);
						else if (ExprIsTokenEx(&s,T("[%x][%x]"),&id0,&id1))
							elt->Id = FOURCCBE(id0, id1, 0, 0);
						else if (ExprIsTokenEx(&s,T("[%x]"),&id0))
							elt->Id = FOURCCBE(id0, 0, 0, 0);
					}
					EltCount++;
				}
				else
					SkipLevel(p);
#if 0
				while (ParserIsAttrib(p, String, TSIZEOF(String)))
				{
					if (ParserAttribString(p, Value, TSIZEOF(Value))) {
						if (tcsisame_ascii(String,T("href")) && tcsstr(Value,T("http://www.matroska.org/technical/specs/"))==Value) {
							tchar_t *Val = Value+40;
							if (tcsstr(Val,T("index.html"))==Val)
								Val += 10;
							//TextAttribEx(&child, String, Val, 0, TYPE_STRING);
						}
						else
						{
							if (tcsisame_ascii(String,T("colspan")))
								isSpecData = 0;
							else if (tcsisame_ascii(String,T("class")) && tcsisame_ascii(Value,T("toptitle")))
								isSpecData = 0;
							else
							if (tcsisame_ascii(String,T("id")))
								tcsreplace(Value,TSIZEOF(Value),T(" "),T("_"));
							//TextAttribEx(&child, String, Value, 0, TYPE_STRING);
						}
					}
				}

				if (p->ElementEof)
					p->ElementEof = 0;
				else
					FillSpecElement(elt, p);

				//TextElementEnd(&child);
#endif
			}
        }
        else
        {
            ParserSkipAfter(p,'>');
            break;
        }
    }
}

static void ReadLevel(parser *p, textwriter *current)
{
    tchar_t Element[MAXDATA], String[MAXDATA], Value[MAXLINE];
    bool_t isSpecData;

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
                TextElementAppendData(current,Value);
        }
        else if (ParserIsElementNested(p, Element, TSIZEOF(Element)))
        {
            //textwriter child;
            //TextElementBegin(&child, current, Element);

            isSpecData = tcsisame_ascii(Element,T("tr"));

            if (isSpecData)
            {
                SpecElement *Dumper = (SpecElement*)NodeCreate(p->Context, SPEC_ELEMENT_CLASS);
				ParserElementSkip(p); // we don't care about the tr attributes
                FillSpecElement(Dumper, p);
				// TODO: if element is value serialize it
                NodeDelete((node*)Dumper);
            }
            else
			{
				while (ParserIsAttrib(p, String, TSIZEOF(String)))
				{
					if (ParserAttribString(p, Value, TSIZEOF(Value))) {
						/*if (tcsisame_ascii(String,T("colspan")))
							isSpecData = 0;
						else if (tcsisame_ascii(String,T("class")) && tcsisame_ascii(Value,T("toptitle")))
							isSpecData = 0;
						else*/
						if (tcsisame_ascii(String,T("id")))
							tcsreplace(Value,TSIZEOF(Value),T(" "),T("_"));
						//TextAttribEx(&child, String, Value, 0, TYPE_STRING);
					}
				}

				if (p->ElementEof)
					p->ElementEof = 0;
				else
                    ReadLevel(p, current);
			}

            //TextElementEnd(&child);
        }
        else
        {
            ParserSkipAfter(p,'>');
            break;
        }
    }
}

META_START(SpecElement_Class,SPEC_ELEMENT_CLASS)
META_CLASS(SIZE,sizeof(SpecElement))
META_END(NODE_CLASS)


/**
 * A tool to format the table in the specs
 *  input: spec.xml
 *  output: table_spec.xml
 */
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

        outFile.Stream = Output;
        TextElementXML(&p, &outFile, T("table"));

        ReadLevel(&parseIn, &outFile);

        TextElementEnd(&outFile); // /table
    }

    StreamClose(Input);
    StreamClose(Output);

	StdAfx_Done((nodemodule*)&p);
    ParserContext_Done(&p);
    return 0;
}

