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
 *  input: specdata.xml
 *  output: spec.xml
 */

#include "data2spec_stdafx.h"
#include "spec_element.h"

#define WITH_V4 0

typedef struct table_extras
{
    bool_t StartedGlobal;
    bool_t PassedEBML;
    bool_t IsLast;
    bool_t InTags;
    int CurrLevel;

} table_extras;

static void AddColumnHeader(textwriter *TBody)
{
    textwriter Tr, Th, Abbr;

    TextElementBegin(&Tr, TBody, T("tr"));
    TextAttribEx(&Tr, T("class"), T("toptitle"), 0, TYPE_STRING);

    TextElementBegin(&Th, &Tr, T("th"));
    TextAttribEx(&Th, T("style"), T("white-space: nowrap"), 0, TYPE_STRING);
    TextElementEndData(&Th, T("Element Name"));

    TextElementBegin(&Th, &Tr, T("th"));
    Th.InsideContent = 1;
    TextAttribEx(&Th, T("title"), T("Level"), 0, TYPE_STRING);
    TextElementBegin(&Abbr, &Th, T("abbr"));
    Abbr.InsideContent = 1;
    TextAttribEx(&Abbr, T("title"), T("Level"), 0, TYPE_STRING);
    TextElementAppendData(&Abbr, T("L"));
    TextElementEnd(&Abbr);
    Th.InsideContent = 0;
    Th.Deep = 0;
    TextElementEnd(&Th);

    TextElementBegin(&Th, &Tr, T("th"));
    TextAttribEx(&Th, T("style"), T("white-space: nowrap"), 0, TYPE_STRING);
    TextElementEndData(&Th, T("EBML ID"));

    TextElementBegin(&Th, &Tr, T("th"));
    Th.InsideContent = 1;
    TextAttribEx(&Th, T("title"), T("Mandatory"), 0, TYPE_STRING);
    TextElementBegin(&Abbr, &Th, T("abbr"));
    Abbr.InsideContent = 1;
    TextAttribEx(&Abbr, T("title"), T("Mandatory"), 0, TYPE_STRING);
    TextElementAppendData(&Abbr, T("Ma"));
    TextElementEnd(&Abbr);
    Th.InsideContent = 0;
    Th.Deep = 0;
    TextElementEnd(&Th);

    TextElementBegin(&Th, &Tr, T("th"));
    Th.InsideContent = 1;
    TextAttribEx(&Th, T("title"), T("Multiple"), 0, TYPE_STRING);
    TextElementBegin(&Abbr, &Th, T("abbr"));
    Abbr.InsideContent = 1;
    TextAttribEx(&Abbr, T("title"), T("Multiple"), 0, TYPE_STRING);
    TextElementAppendData(&Abbr, T("Mu"));
    TextElementEnd(&Abbr);
    Th.InsideContent = 0;
    Th.Deep = 0;
    TextElementEnd(&Th);

    TextElementBegin(&Th, &Tr, T("th"));
    Th.InsideContent = 1;
    TextAttribEx(&Th, T("title"), T("Range"), 0, TYPE_STRING);
    TextElementBegin(&Abbr, &Th, T("abbr"));
    Abbr.InsideContent = 1;
    TextAttribEx(&Abbr, T("title"), T("Range"), 0, TYPE_STRING);
    TextElementAppendData(&Abbr, T("Rng"));
    TextElementEnd(&Abbr);
    Th.InsideContent = 0;
    Th.Deep = 0;
    TextElementEnd(&Th);

    TextElementBegin(&Th, &Tr, T("th"));
    TextElementEndData(&Th, T("Default"));

    TextElementBegin(&Th, &Tr, T("th"));
    Th.InsideContent = 1;
    TextAttribEx(&Th, T("title"), T("Element Type"), 0, TYPE_STRING);
    TextElementBegin(&Abbr, &Th, T("abbr"));
    Abbr.InsideContent = 1;
    TextAttribEx(&Abbr, T("title"), T("Element Type"), 0, TYPE_STRING);
    TextElementAppendData(&Abbr, T("T"));
    TextElementEnd(&Abbr);
    Th.InsideContent = 0;
    Th.Deep = 0;
    TextElementEnd(&Th);

    TextElementBegin(&Th, &Tr, T("th"));
    Th.InsideContent = 1;
    TextAttribEx(&Th, T("title"), T("Version 1"), 0, TYPE_STRING);
    TextElementBegin(&Abbr, &Th, T("abbr"));
    Abbr.InsideContent = 1;
    TextAttribEx(&Abbr, T("title"), T("Version 1"), 0, TYPE_STRING);
    TextElementAppendData(&Abbr, T("1"));
    TextElementEnd(&Abbr);
    Th.InsideContent = 0;
    Th.Deep = 0;
    TextElementEnd(&Th);

    TextElementBegin(&Th, &Tr, T("th"));
    Th.InsideContent = 1;
    TextAttribEx(&Th, T("title"), T("Version 2"), 0, TYPE_STRING);
    TextElementBegin(&Abbr, &Th, T("abbr"));
    Abbr.InsideContent = 1;
    TextAttribEx(&Abbr, T("title"), T("Version 2"), 0, TYPE_STRING);
    TextElementAppendData(&Abbr, T("2"));
    TextElementEnd(&Abbr);
    Th.InsideContent = 0;
    Th.Deep = 0;
    TextElementEnd(&Th);

    TextElementBegin(&Th, &Tr, T("th"));
    Th.InsideContent = 1;
    TextAttribEx(&Th, T("title"), T("Version 3"), 0, TYPE_STRING);
    TextElementBegin(&Abbr, &Th, T("abbr"));
    Abbr.InsideContent = 1;
    TextAttribEx(&Abbr, T("title"), T("Version 3"), 0, TYPE_STRING);
    TextElementAppendData(&Abbr, T("3"));
    TextElementEnd(&Abbr);
    Th.InsideContent = 0;
    Th.Deep = 0;
    TextElementEnd(&Th);

#if defined(WITH_V4)
    TextElementBegin(&Th, &Tr, T("th"));
    Th.InsideContent = 1;
    TextAttribEx(&Th, T("title"), T("Version 4"), 0, TYPE_STRING);
    TextElementBegin(&Abbr, &Th, T("abbr"));
    Abbr.InsideContent = 1;
    TextAttribEx(&Abbr, T("title"), T("Version 4"), 0, TYPE_STRING);
    TextElementAppendData(&Abbr, T("4"));
    TextElementEnd(&Abbr);
    Th.InsideContent = 0;
    Th.Deep = 0;
    TextElementEnd(&Th);
#endif

    TextElementBegin(&Th, &Tr, T("th"));
    Th.InsideContent = 1;
    TextAttribEx(&Th, T("title"), T("WebM"), 0, TYPE_STRING);
    TextElementBegin(&Abbr, &Th, T("abbr"));
    Abbr.InsideContent = 1;
    TextAttribEx(&Abbr, T("title"), T("WebM"), 0, TYPE_STRING);
    TextElementAppendData(&Abbr, T("W"));
    TextElementEnd(&Abbr);
    Th.InsideContent = 0;
    Th.Deep = 0;
    TextElementEnd(&Th);

    TextElementBegin(&Th, &Tr, T("th"));
    TextElementEndData(&Th, T("Description"));

    TextElementEnd(&Tr);
}

static void CleanHTML(tchar_t *Out, size_t OutLen)
{
    if (Out[0]) tcsreplace(Out,OutLen,T("\n"),T(" "));
    if (Out[0]) tcsreplace(Out,OutLen,T("  "),T(" "));
    if (Out[0]) tcsreplace(Out,OutLen,T("  "),T(" "));
    if (Out[0]) tcsreplace(Out,OutLen,T("  "),T(" "));
    if (Out[0]) tcsreplace(Out,OutLen,T("  "),T(" "));
    if (Out[0]) tcsreplace(Out,OutLen,T("<br/>"),T("<br>")); // Drupal doesn't like <br/>
    if (Out[0]) tcsreplace(Out,OutLen,T("& "),T("&amp; "));
    if (Out[0]) tcsreplace(Out,OutLen,T("\""),T("&quot;"));
    if (Out[0]) tcsreplace(Out,OutLen,T(">"),T("&gt;"));
    if (Out[0]) tcsreplace(Out,OutLen,T("<"),T("&lt;"));
}

static void OutputElement(SpecElement *elt, textwriter *TBody, table_extras *Extras)
{
    if (elt->Type != EBML_unknown && elt->Name[0])
    {
        tchar_t IdString[32];
        textwriter Tr, Td, Abbr;

        if (elt->Level==0/*!Extras->HadColumnHeader*/)
        {
            //Extras->HadColumnHeader = 1;
            AddColumnHeader(TBody);
        }

        if (elt->Level==-1 && !Extras->StartedGlobal)
        {
            Extras->StartedGlobal = 1;
            TextElementBegin(&Tr, TBody, T("tr"));
            TextElementBegin(&Td, &Tr, T("th"));
#if defined(WITH_V4)
            TextAttribEx(&Td, T("colspan"), T("14"), 0, TYPE_STRING);
#else
            TextAttribEx(&Td, T("colspan"), T("13"), 0, TYPE_STRING);
#endif
            TextElementAppendData(&Td, T("Global elements (used everywhere in the format)"));
            TextElementEnd(&Td);
            TextElementEnd(&Tr);
        }

        if (elt->Level>=0 && Extras->StartedGlobal)
        {
            Extras->StartedGlobal = 0;
        }

        if (elt->Level==0 || (elt->Level==1 && Extras->PassedEBML))
        {
            Extras->InTags = 0;
            TextElementBegin(&Tr, TBody, T("tr"));
            TextElementBegin(&Td, &Tr, T("th"));
#if defined(WITH_V4)
            TextAttribEx(&Td, T("colspan"), T("14"), 0, TYPE_STRING);
#else
            TextAttribEx(&Td, T("colspan"), T("13"), 0, TYPE_STRING);
#endif
            if (elt->Id == 0x1A45DFA3) {
                TextAttribEx(&Td, T("id"), T("LevelEBML"), 0, TYPE_STRING);
                TextElementAppendData(&Td, T("EBML Header"));
            }
            else if (elt->Id == 0x18538067) {
                TextAttribEx(&Td, T("id"), T("LevelSegment"), 0, TYPE_STRING);
                TextElementAppendData(&Td, T("Segment"));
            }
            else if (elt->Id == 0x114D9B74) {
                TextAttribEx(&Td, T("id"), T("MetaSeekInformation"), 0, TYPE_STRING);
                TextElementAppendData(&Td, T("Meta Seek Information"));
            }
            else if (elt->Id == 0x1549A966) {
                TextAttribEx(&Td, T("id"), T("SegmentInformation"), 0, TYPE_STRING);
                TextElementAppendData(&Td, T("Segment Information"));
            }
            else if (elt->Id == 0x1F43B675) {
                TextAttribEx(&Td, T("id"), T("LevelCluster"), 0, TYPE_STRING);
                TextElementAppendData(&Td, T("Cluster"));
            }
            else if (elt->Id == 0x1654AE6B) {
                TextAttribEx(&Td, T("id"), T("LevelTrack"), 0, TYPE_STRING);
                TextElementAppendData(&Td, T("Track"));
            }
            else if (elt->Id == 0x1C53BB6B) {
                TextAttribEx(&Td, T("id"), T("CueingData"), 0, TYPE_STRING);
                TextElementAppendData(&Td, T("Cueing Data"));
            }
            else if (elt->Id == 0x1941A469) {
                TextAttribEx(&Td, T("id"), T("Attachment"), 0, TYPE_STRING);
                TextElementAppendData(&Td, T("Attachment"));
            }
            else if (elt->Id == 0x1254C367) {
                Extras->InTags = 1;
                TextAttribEx(&Td, T("id"), T("Tagging"), 0, TYPE_STRING);
                TextElementAppendData(&Td, T("Tagging"));
            }
            TextElementEnd(&Td);
            TextElementEnd(&Tr);
        }
        Extras->CurrLevel = elt->Level;

        if (elt->Id == 0x18538067)
            Extras->PassedEBML = 1;

        TextElementBegin(&Tr, TBody, T("tr"));
#if 0
        if (Extras->InTags && tcsstr(elt->Name,T("UID"))) {
            stprintf_s(IdString,TSIZEOF(IdString), T("T%s"),elt->Name);
            TextAttribEx(&Tr, T("id"), IdString, 0, TYPE_STRING);
        }
        else
#endif
            TextAttribEx(&Tr, T("id"), elt->Name, 0, TYPE_STRING);
        if (elt->MaxVersion || (!elt->MinVersion && !elt->InWebM))
            stprintf_s(IdString,TSIZEOF(IdString),T("version2"));
        else if (elt->Level < 0)
            stprintf_s(IdString,TSIZEOF(IdString),T("level1"));
        else
            stprintf_s(IdString,TSIZEOF(IdString),T("level%d"), elt->Level);
        TextAttribEx(&Tr, T("class"), IdString, 0, TYPE_STRING);

        TextElementBegin(&Td, &Tr, T("td"));
        TextElementEndData(&Td, elt->Name);

        TextElementBegin(&Td, &Tr, T("td"));
        if (elt->Level < 0)
            stprintf_s(IdString,TSIZEOF(IdString),T("g"));
        else
            stprintf_s(IdString,TSIZEOF(IdString),T("%d%s"), elt->Level, elt->Recursive?T("+"):T(""));
        TextElementEndData(&Td, IdString);

        TextElementBegin(&Td, &Tr, T("td"));
        if (elt->Level==0)
            TextAttribEx(&Td, T("style"), T("white-space: nowrap"), 0, TYPE_STRING);
        if (elt->Id > 0x00FFFFFF)
            stprintf_s(IdString,TSIZEOF(IdString),T("[%02X][%02X][%02X][%02X]"),((elt->Id)>>24)&0xFF,((elt->Id)>>16)&0xFF,((elt->Id)>>8)&0xFF,elt->Id&0xFF);
        else if (elt->Id > 0x0000FFFF)
            stprintf_s(IdString,TSIZEOF(IdString),T("[%02X][%02X][%02X]"),((elt->Id)>>16)&0xFF,((elt->Id)>>8)&0xFF,elt->Id&0xFF);
        else if (elt->Id > 0x000000FF)
            stprintf_s(IdString,TSIZEOF(IdString),T("[%02X][%02X]"),((elt->Id)>>8)&0xFF,elt->Id&0xFF);
        else
            stprintf_s(IdString,TSIZEOF(IdString),T("[%02X]"),elt->Id&0xFF);
        TextElementEndData(&Td, IdString);

        TextElementBegin(&Td, &Tr, T("td"));
        if (elt->Mandatory)
            TextElementEndData(&Td, T("*"));
        else {
            TextAttribEx(&Tr, T("class"), T("unset"), 0, TYPE_STRING);
            TextElementEndData(&Td, T("-"));
        }

        TextElementBegin(&Td, &Tr, T("td"));
        if (elt->Multiple)
            TextElementEndData(&Td, T("*"));
        else {
            TextAttribEx(&Tr, T("class"), T("unset"), 0, TYPE_STRING);
            TextElementEndData(&Td, T("-"));
        }

        TextElementBegin(&Td, &Tr, T("td"));
        if (elt->Range[0]) {
            CleanHTML(elt->Range, TSIZEOF(elt->Range));
            TextElementEndData(&Td, elt->Range);
        } else {
            TextAttribEx(&Tr, T("class"), T("unset"), 0, TYPE_STRING);
            TextElementEndData(&Td, T("-"));
        }

        TextElementBegin(&Td, &Tr, T("td"));
        if (elt->DefaultValue[0]) {
            CleanHTML(elt->DefaultValue, TSIZEOF(elt->DefaultValue));
            TextElementEndData(&Td, elt->DefaultValue);
        } else {
            TextAttribEx(&Tr, T("class"), T("unset"), 0, TYPE_STRING);
            TextElementEndData(&Td, T("-"));
        }

        TextElementBegin(&Td, &Tr, T("td"));
        Td.InsideContent = 1;
        TextElementBegin(&Abbr, &Td, T("abbr"));
        Abbr.InsideContent = 1;
        switch (elt->Type)
        {
        case EBML_MASTER:
            TextAttribEx(&Abbr, T("title"), T("Master Elements"), 0, TYPE_STRING);
            TextElementAppendData(&Abbr, T("m"));
            break;
        case EBML_INTEGER:
            TextAttribEx(&Abbr, T("title"), T("Signed Integer"), 0, TYPE_STRING);
            TextElementAppendData(&Abbr, T("i"));
            break;
        case EBML_UNSIGNED_INTEGER:
            TextAttribEx(&Abbr, T("title"), T("Unsigned Integer"), 0, TYPE_STRING);
            TextElementAppendData(&Abbr, T("u"));
            break;
        case EBML_DATE:
            TextAttribEx(&Abbr, T("title"), T("Date"), 0, TYPE_STRING);
            TextElementAppendData(&Abbr, T("d"));
            break;
        case EBML_FLOAT:
            TextAttribEx(&Abbr, T("title"), T("Float"), 0, TYPE_STRING);
            TextElementAppendData(&Abbr, T("f"));
            break;
        case EBML_STRING:
            TextAttribEx(&Abbr, T("title"), T("String"), 0, TYPE_STRING);
            TextElementAppendData(&Abbr, T("s"));
            break;
        case EBML_UNICODE_STRING:
            TextAttribEx(&Abbr, T("title"), T("UTF-8"), 0, TYPE_STRING);
            TextElementAppendData(&Abbr, T("8"));
            break;
        case EBML_BINARY:
            TextAttribEx(&Abbr, T("title"), T("Binary"), 0, TYPE_STRING);
            TextElementAppendData(&Abbr, T("b"));
            break;
        }
        TextElementEnd(&Abbr);
        Td.InsideContent = 0;
        Td.Deep = 0;
        TextElementEnd(&Td);

        // v1
        TextElementBegin(&Td, &Tr, T("td"));
        if (!elt->MinVersion && !elt->MaxVersion && !elt->InWebM) {
            TextElementEnd(&Td);
        }
        else
        if (!elt->MinVersion || elt->MinVersion > 1) {
            TextAttribEx(&Tr, T("class"), T("flagnot"), 0, TYPE_STRING);
            TextElementEnd(&Td);
        }
        else
            TextElementEndData(&Td, T("*"));

        // v2
        TextElementBegin(&Td, &Tr, T("td"));
        if (!elt->MinVersion && !elt->MaxVersion && !elt->InWebM) {
            TextElementEnd(&Td);
        }
        else
        if (!elt->MinVersion || elt->MinVersion > 2 || (elt->MaxVersion && elt->MaxVersion < 2)) {
            TextAttribEx(&Tr, T("class"), T("flagnot"), 0, TYPE_STRING);
            TextElementEnd(&Td);
        }
        else
            TextElementEndData(&Td, T("*"));

        // v3
        TextElementBegin(&Td, &Tr, T("td"));
        if (!elt->MinVersion && !elt->MaxVersion && !elt->InWebM) {
            TextElementEnd(&Td);
        }
        else
        if (!elt->MinVersion || elt->MinVersion > 3 || (elt->MaxVersion && elt->MaxVersion < 3)) {
            TextAttribEx(&Tr, T("class"), T("flagnot"), 0, TYPE_STRING);
            TextElementEnd(&Td);
        }
        else
            TextElementEndData(&Td, T("*"));

#if defined(WITH_V4)
        // v4
        TextElementBegin(&Td, &Tr, T("td"));
        if (!elt->MinVersion && !elt->MaxVersion && !elt->InWebM) {
            TextElementEnd(&Td);
        }
        else
        if (!elt->MinVersion || elt->MinVersion > 4 || (elt->MaxVersion && elt->MaxVersion < 4)) {
            TextAttribEx(&Tr, T("class"), T("flagnot"), 0, TYPE_STRING);
            TextElementEnd(&Td);
        }
        else
            TextElementEndData(&Td, T("*"));
#endif

        // webm
        TextElementBegin(&Td, &Tr, T("td"));
        if (!elt->MinVersion && !elt->MaxVersion && !elt->InWebM) {
            TextElementEnd(&Td);
        }
        else
        if (!elt->InWebM) {
            TextAttribEx(&Tr, T("class"), T("flagnot"), 0, TYPE_STRING);
            TextElementEnd(&Td);
        }
        else
            TextElementEndData(&Td, T("*"));

        TextElementBegin(&Td, &Tr, T("td"));
        if (elt->Description[0]) tcsreplace(elt->Description,TSIZEOF(elt->Description),T("& "),T("&amp; "));
        if (elt->Description[0]) tcsreplace(elt->Description,TSIZEOF(elt->Description),T("<br/>"),T("<br>\n")); // Drupal doesn't like <br/>
        if (elt->Description[0]) tcsreplace(elt->Description,TSIZEOF(elt->Description),T("http://www.matroska.org/technical/specs/"),T(""));
        TextElementEndData(&Td, elt->Description);

        TextElementEnd(&Tr);

        if (Extras->IsLast)
            AddColumnHeader(TBody);
    }
}

static void ReadLevel(parser *p, array *Elements)
{
    tchar_t Element[MAXDATA], String[MAXDATA], Value[MAXLINE];

    for (;;) {
        if (ParserElementContent(p,Value,TSIZEOF(Value)) && Value[0])
        {
        }
        else if (ParserIsElementNested(p, Element, TSIZEOF(Element)))
        {
            if (tcsisame_ascii(Element,T("element")))
            {
                SpecElement *Dumper = (SpecElement*)NodeCreate(p->Context, SPEC_ELEMENT_CLASS);
                ReadSpecElement(Dumper, p);
                ArrayAppend(Elements, &Dumper, sizeof(Dumper), 128);
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
                    ReadLevel(p, Elements);
			}
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


int main(void)
{
    parsercontext p;
    parser parseIn;
    stream *Input = NULL,*Output = NULL;
    array Elements;
    SpecElement **element;
    //tchar_t Element[MAXLINE], String[MAXLINE], Value[MAXLINE];

    ParserContext_Init(&p,NULL,NULL,NULL);
    StdAfx_Init((nodemodule*)&p);

    Input = StreamOpen(&p,T("specdata.xml"),SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    Output = StreamOpen(&p,T("spec.xml"),SFLAG_WRONLY|SFLAG_CREATE);

    memset(&parseIn, 0, sizeof(parseIn));
    ArrayInit(&Elements);

    if (ParserStreamXML(&parseIn, Input, &p, T("table"), 0)==ERR_NONE)
    {
        textwriter Table, TBody;
        table_extras Extras;

        memset(&Extras,0,sizeof(Extras));
        memset(&Table,0,sizeof(Table));
        memset(&TBody,0,sizeof(TBody));
        Extras.CurrLevel = -1;

        ReadLevel(&parseIn, &Elements);

        Table.Stream = Output;
        TextElementXML(&p, &Table, T("table"));
        TextElementBegin(&TBody, &Table, T("tbody"));

        TBody.SafeFormat = 1;
        for (element=ARRAYBEGIN(Elements,SpecElement*); element!=ARRAYEND(Elements,SpecElement*);++element) {
            if ((element+1) == ARRAYEND(Elements,SpecElement*))
                Extras.IsLast = 1;
            OutputElement(*element, &TBody, &Extras);
        }

        TextElementEnd(&TBody);
        TextElementEnd(&Table);
    }

    for (element=ARRAYBEGIN(Elements,SpecElement*); element!=ARRAYEND(Elements,SpecElement*);++element)
        NodeDelete((node*)*element);
    ArrayClear(&Elements);

    StreamClose(Input);
    StreamClose(Output);

	StdAfx_Done((nodemodule*)&p);
    ParserContext_Done(&p);
    return 0;
}
