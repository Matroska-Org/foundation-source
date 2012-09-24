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

#include "spec2data_stdafx.h"
#include "spec_element.h"

static void SkipLevel(parser *p)
{
	// TODO: move this in parser2.c when it's stable
	ParserElementSkipNested(p);
    ParserSkipAfter(p,'>');
}


static void FillSpecElement(SpecElement *elt, parser *p)
{
    tchar_t Element[MAXDATA], /*String[MAXDATA], */Value[MAXLINE];
	int EltCount = 0;
    bool_t hasV1 = 0, hasV2 = 0;

	for (;;) {
        if (ParserElementContent(p,Value,TSIZEOF(Value)) && Value[0])
        {
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("\n"),T(" "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("  "),T(" "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("  "),T(" "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("  "),T(" "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("  "),T(" "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("<br/>"),T("<br>")); // Drupal doesn't like <br/>
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("& "),T("&amp; "));
            if (Value[0]) tcsreplace(Value,TSIZEOF(Value),T("\""),T("&quot;"));
        }
        else if (ParserIsElementNested(p, Element, TSIZEOF(Element)))
        {
			if (elt==NULL || tcsisame_ascii(Element,T("th"))) {
				SkipLevel(p);
				elt = NULL; // we don't fill any element as it's not a correct one
			} else {
				if (tcsisame_ascii(Element,T("td")))
				{
					intptr_t id0,id1,id2,id3;
					const tchar_t *s = Value;

                    ParserElementSkip(p); // we don't care about the td attributes
					ReadElementText(p, Value, TSIZEOF(Value));

                    switch (EltCount++)
                    {
                    case 0:
						tcscpy_s(elt->Name, TSIZEOF(elt->Name), Value);
                        break;
                    case 1:
                        if (tcsisame_ascii(Value,T("global")) || tcsisame_ascii(Value,T("g"))) {
                            elt->Level = -1;
                        }
                        else
                        {
						    ExprIsInt(&s,&id0);
						    elt->Level = id0;
                            if (s[0]=='+')
                                elt->Recursive = 1;
                        }
                        break;
                    case 2:
						if (ExprIsTokenEx(&s,T("[%x][%x][%x][%x]"),&id0,&id1,&id2,&id3))
							elt->Id = FOURCCBE(id0, id1, id2, id3);
						else if (ExprIsTokenEx(&s,T("[%x][%x][%x]"),&id0,&id1,&id2))
							elt->Id = FOURCCBE(0, id0, id1, id2);
						else if (ExprIsTokenEx(&s,T("[%x][%x]"),&id0,&id1))
							elt->Id = FOURCCBE(0, 0, id0, id1);
						else if (ExprIsTokenEx(&s,T("[%x]"),&id0))
							elt->Id = FOURCCBE(0, 0, 0, id0);
                        else
                            elt->Id = 0;
					    break;
                    case 3:
                        elt->Mandatory = tcschr(Value,'*')!=NULL;
                        break;
                    case 4:
                        elt->Multiple = tcschr(Value,'*')!=NULL;
                        break;
                    case 5:
						tcscpy_s(elt->Range, TSIZEOF(elt->Range), Value);
                        break;
                    case 6:
						tcscpy_s(elt->DefaultValue, TSIZEOF(elt->DefaultValue), Value);
                        break;
                    case 7:
                        // handle <abbr>
                        s = tcschr(Value,'>');
                        if (s==NULL)
                            s = Value;
                        else
                            ++s;
                        switch (s[0])
                        {
                        case 'm': elt->Type = EBML_MASTER; break;
                        case 'u': elt->Type = EBML_UNSIGNED_INTEGER; break;
                        case 'i': elt->Type = EBML_INTEGER; break;
                        case 'd': elt->Type = EBML_DATE; break;
                        case 's': elt->Type = EBML_STRING; break;
                        case 'b': elt->Type = EBML_BINARY; break;
                        case '8': elt->Type = EBML_UNICODE_STRING; break;
                        case 'f': elt->Type = EBML_FLOAT; break;
                        default: elt->Type = EBML_unknown; break;
                        }
                        break;
                    case 8:
						hasV1 = tcschr(Value,'*')!=NULL;
                        break;
                    case 9:
						hasV2 = tcschr(Value,'*')!=NULL;
                        break;
                    case 10:
                        elt->InWebM = tcschr(Value,'*')!=NULL;
                        break;
                    case 11:
                        tcscpy_s(elt->Description, TSIZEOF(elt->Description), Value);
                        break;
                    }
				}
				else
					SkipLevel(p);
			}
        }
        else
        {
            ParserSkipAfter(p,'>');
            break;
        }
    }
    if (elt!=NULL)
    {
        elt->MinVersion = hasV1 ? 1 : (hasV2 ? 2 : 0);
        elt->MaxVersion = hasV2 ? 2 : (hasV1 ? 1 : 0);
    }
}

static const tchar_t *GetTypeString(ebml_type Type)
{
    switch (Type)
    {
    case EBML_MASTER: return T("master");
    case EBML_INTEGER: return T("integer");
    case EBML_UNSIGNED_INTEGER: return T("uinteger");
    case EBML_DATE: return T("date");
    case EBML_FLOAT: return T("float");
    case EBML_STRING: return T("string");
    case EBML_BINARY: return T("binary");
    case EBML_UNICODE_STRING: return T("utf-8");
    default: return NULL;
    }
}

static void OutputElement(SpecElement *elt, textwriter *parent)
{
    if (elt->Type != EBML_unknown && elt->Name[0])
    {
        tchar_t IdString[16];
        textwriter child;
        TextElementBegin(&child, parent, T("element"));

        TextAttribEx(&child, T("name"), elt->Name, 0, TYPE_STRING);
        TextAttribEx(&child, T("level"), &elt->Level, sizeof(elt->Level), TYPE_INT);
        if (elt->Recursive)
            TextAttribEx(&child, T("recursive"), &elt->Recursive, sizeof(elt->Recursive), TYPE_BOOLEAN);

        stprintf_s(IdString,TSIZEOF(IdString), T("0x%X"),elt->Id);
        TextAttribEx(&child, T("id"), IdString, 0, TYPE_STRING);

        TextAttribEx(&child, T("type"), GetTypeString(elt->Type), 0, TYPE_STRING);

        if (elt->Mandatory)
            TextAttribEx(&child, T("mandatory"), &elt->Mandatory, sizeof(elt->Mandatory), TYPE_BOOLEAN);
        if (elt->Multiple)
            TextAttribEx(&child, T("multiple"), &elt->Multiple, sizeof(elt->Multiple), TYPE_BOOLEAN);

        if (elt->MinVersion)
            TextAttribEx(&child, T("minver"), &elt->MinVersion, sizeof(elt->MinVersion), TYPE_INT);
        if (elt->MaxVersion)
            TextAttribEx(&child, T("maxver"), &elt->MaxVersion, sizeof(elt->MaxVersion), TYPE_INT);

        TextAttribEx(&child, T("webm"), &elt->InWebM, sizeof(elt->InWebM), TYPE_BOOLEAN);

        if (elt->DefaultValue[0] && elt->DefaultValue[0]!='-')
            TextAttribEx(&child, T("default"), elt->DefaultValue, 0, TYPE_STRING);

        if (elt->Range[0] && elt->Range[0]!='-')
            TextAttribEx(&child, T("range"), elt->Range, 0, TYPE_STRING);

        tcsreplace(elt->Description, TSIZEOF(elt->Description), T("& "), T("&amp; "));
        TextElementAppendData(&child, elt->Description);

        TextElementEnd(&child);
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
            isSpecData = tcsisame_ascii(Element,T("tr"));

            if (isSpecData)
            {
                SpecElement *Dumper = (SpecElement*)NodeCreate(p->Context, SPEC_ELEMENT_CLASS);
				ParserElementSkip(p); // we don't care about the tr attributes
                FillSpecElement(Dumper, p);
				OutputElement(Dumper, current);
                NodeDelete((node*)Dumper);
            }
            else
			{
				while (ParserIsAttrib(p, String, TSIZEOF(String)))
				{
					if (ParserAttribString(p, Value, TSIZEOF(Value))) {
						if (tcsisame_ascii(String,T("id")))
							tcsreplace(Value,TSIZEOF(Value),T(" "),T("_"));
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
    Output = StreamOpen(&p,T("specdata.xml"),SFLAG_WRONLY|SFLAG_CREATE);

    memset(&parseIn, 0, sizeof(parseIn));

    if (ParserStreamXML(&parseIn, Input, &p, T("table"), 0)==ERR_NONE)
    {
        textwriter outFile;

        memset(&outFile,0,sizeof(outFile));
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
