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
 * A tool to generate the libmatroska2 semantic files
 *  input: specdata.xml
 *  output: matroska_sem.c / matroska_sem.h
 */

#include "data2lib2_stdafx.h"
#include "spec_element.h"

typedef struct table_extras
{
    bool_t StartedGlobal;
    bool_t PassedEBML;
    bool_t IsLast;
    bool_t InTags;
    int CurrLevel;

} table_extras;

static void AddElementSemantic(textwriter *CFile, const SpecElement *elt, bool_t InRecursive)
{
    TextPrintf(CFile, T("    {%d, %d, &MATROSKA_Context%s, "), InRecursive?0:(elt->Mandatory?1:0), elt->Multiple?0:1, elt->Name);
    if (elt->InWebM && elt->MinVersion==1 && !elt->MaxVersion && elt->InDivX)
        TextWrite(CFile, T("0"));
    else {
        bool_t hasData = 0;
        if (!elt->MinVersion || elt->MinVersion>1) {
            if (hasData)
                TextWrite(CFile, T("|"));
            hasData = 1;
            TextWrite(CFile, T("PROFILE_MATROSKA_V1"));
        }
        if (!elt->MinVersion || elt->MinVersion>2 || (elt->MaxVersion && elt->MaxVersion<2)) {
            if (hasData)
                TextWrite(CFile, T("|"));
            hasData = 1;
            TextWrite(CFile, T("PROFILE_MATROSKA_V2"));
        }
        if (!elt->MinVersion || elt->MinVersion>3 || (elt->MaxVersion && elt->MaxVersion<3)) {
            if (hasData)
                TextWrite(CFile, T("|"));
            hasData = 1;
            TextWrite(CFile, T("PROFILE_MATROSKA_V3"));
        }
        if (!elt->MinVersion || elt->MinVersion>4 || (elt->MaxVersion && elt->MaxVersion<4)) {
            if (hasData)
                TextWrite(CFile, T("|"));
            hasData = 1;
            TextWrite(CFile, T("PROFILE_MATROSKA_V4"));
        }
        if (!elt->InDivX) {
            if (hasData)
                TextWrite(CFile, T("|"));
            hasData = 1;
            TextWrite(CFile, T("PROFILE_DIVX"));
        }
        if (!elt->InWebM) {
            if (hasData)
                TextWrite(CFile, T("|"));
            hasData = 1;
            TextWrite(CFile, T("PROFILE_WEBM"));
        }
    }
    if (InRecursive)
        TextWrite(CFile, T("}, // recursive\n"));
    else
        TextWrite(CFile, T("},\n"));
}

static bool_t IsValidElement(const SpecElement *elt)
{
    return elt->InWebM || elt->MinVersion || elt->InDivX;
}

static void OutputElementDefinition(const SpecElement **pElt, const SpecElement **EltEnd, textwriter *CFile, table_extras *Extras)
{
    const SpecElement *elt = *pElt;

    if (pElt==EltEnd)
        return;

    if (elt->Type==EBML_MASTER)
    {
        const SpecElement **sub;
        for (sub = pElt+1; sub!=EltEnd; ++sub)
        {
            if ((*sub)->Level<= elt->Level && (*sub)->Level>=0)
                break;
            if ((*sub)->Level== elt->Level+1)
                OutputElementDefinition(sub, EltEnd, CFile, Extras);
        }
    }

    //if (!IsValidElement(elt)) TextWrite(CFile, T("// not supported "));

    if (elt->Type != EBML_unknown && elt->Name[0])
    {
        if (elt->Level==-1 && !Extras->StartedGlobal)
        {
            Extras->StartedGlobal = 1;
        }

        if (elt->Level>=0 && Extras->StartedGlobal)
        {
            Extras->StartedGlobal = 0;
        }

        if (elt->Level==0 || (elt->Level==1 && Extras->PassedEBML))
        {
            Extras->InTags = 0;
        }
        Extras->CurrLevel = elt->Level;

        if (Extras->PassedEBML)
        {
            const tchar_t *s;
            intptr_t value;

            if (elt->Type==EBML_MASTER)
            {
                const SpecElement **sub;

                // write the semantic
                TextPrintf(CFile, T("\nconst ebml_semantic EBML_Semantic%s[] = {\n"), elt->Name);
                if (elt->Recursive)
                    AddElementSemantic(CFile, elt, 1);
                for (sub = pElt+1; sub!=EltEnd; ++sub) 
                {
                    if ((*sub)->Level<= elt->Level)
                        break;
                    if ((*sub)->Level== elt->Level+1)
                    {
                        //if (!IsValidElement(*sub)) TextWrite(CFile, T("// "));
                        AddElementSemantic(CFile, *sub, 0);
                    }
                }
                TextWrite(CFile, T("    {0, 0, NULL ,0} // end of the table\n};\n"));
            }

            TextPrintf(CFile, T("const ebml_context MATROSKA_Context%s = {0x%X, "), elt->Name, elt->Id);
            if (elt->Id==0x4DBB)
                TextWrite(CFile, T("MATROSKA_SEEKPOINT_CLASS, "));
            else if (elt->Id==0xAE)
                TextWrite(CFile, T("MATROSKA_TRACKENTRY_CLASS, "));
            else if (elt->Id==0x73A4 || elt->Id==0x3CB923 || elt->Id==0x3EB923)
                TextWrite(CFile, T("MATROSKA_SEGMENTUID_CLASS, "));
            else if (elt->Id==0xA3 || elt->Id==0xA1)
                TextWrite(CFile, T("MATROSKA_BLOCK_CLASS, "));
            else if (elt->Id==0xA0)
                TextWrite(CFile, T("MATROSKA_BLOCKGROUP_CLASS, "));
            else if (elt->Id==0x1F43B675)
                TextWrite(CFile, T("MATROSKA_CLUSTER_CLASS, "));
            else if (elt->Id==0xBB)
                TextWrite(CFile, T("MATROSKA_CUEPOINT_CLASS, "));
            else if (elt->Id==0x465C)
                TextWrite(CFile, T("MATROSKA_BIGBINARY_CLASS, "));
            else if (elt->Id==0x61A7)
                TextWrite(CFile, T("MATROSKA_ATTACHMENT_CLASS, "));
            else switch (elt->Type)
            {
            case EBML_MASTER:
                TextWrite(CFile, T("EBML_MASTER_CLASS, "));
                break;
            case EBML_INTEGER:
                TextWrite(CFile, T("EBML_SINTEGER_CLASS, "));
                break;
            case EBML_UNSIGNED_INTEGER:
                if (tcsisame_ascii(elt->Range,T("0-1")))
                    TextWrite(CFile, T("EBML_BOOLEAN_CLASS, "));
                else
                    TextWrite(CFile, T("EBML_INTEGER_CLASS, "));
                break;
            case EBML_DATE:
                TextWrite(CFile, T("EBML_DATE_CLASS, "));
                break;
            case EBML_FLOAT:
                TextWrite(CFile, T("EBML_FLOAT_CLASS, "));
                break;
            case EBML_STRING:
                TextWrite(CFile, T("EBML_STRING_CLASS, "));
                break;
            case EBML_UNICODE_STRING:
                TextWrite(CFile, T("EBML_UNISTRING_CLASS, "));
                break;
            case EBML_BINARY:
                TextWrite(CFile, T("EBML_BINARY_CLASS, "));
                break;
            }
            switch (elt->Type)
            {
            case EBML_INTEGER:
            case EBML_UNSIGNED_INTEGER:
            case EBML_DATE:
                s = elt->DefaultValue;
                if (!elt->DefaultValue[0])
                    TextWrite(CFile, T("0, 0, ")); // no default value
                else if (ExprIsInt(&s,&value))
                    TextPrintf(CFile, T("1, (intptr_t)%d, "), value);
                else
                    TextPrintf(CFile, T("0, 0, "), s); // not supported
                break;
            case EBML_FLOAT:
                s = elt->DefaultValue;
                if (!elt->DefaultValue[0])
                    TextWrite(CFile, T("0, 0, ")); // no default value
                else if (ExprIsInt(&s,&value))
                    TextPrintf(CFile, T("1, (intptr_t)%s, "), elt->DefaultValue);
                else
                    TextPrintf(CFile, T("0, 0, "), s); // not supported
                break;
            case EBML_STRING:
            case EBML_UNICODE_STRING:
                if (!elt->DefaultValue[0])
                    TextWrite(CFile, T("0, 0, ")); // no default value
                else
                    TextPrintf(CFile, T("1, (intptr_t)\"%s\", "), elt->DefaultValue);
                break;
            default:
                TextPrintf(CFile, T("0, 0, "), elt->DefaultValue);
                break;
            }

            TextPrintf(CFile, T("\"%s\", "), elt->Name);
            if (elt->Type!=EBML_MASTER)
                TextWrite(CFile, T("NULL, "));
            else
                TextPrintf(CFile, T("EBML_Semantic%s, "), elt->Name);
            TextWrite(CFile, T("EBML_SemanticGlobals, "));
            TextWrite(CFile, T("NULL};\n"));
        }
    }
}

static void OutputElementDeclaration(const SpecElement **pElt, const SpecElement **EltEnd, textwriter *CFile, table_extras *Extras)
{
    const SpecElement *elt = *pElt;

    if (pElt==EltEnd)
        return;

    if (elt->Type==EBML_MASTER)
    {
        const SpecElement **sub;
        for (sub = pElt+1; sub!=EltEnd; ++sub)
        {
            if ((*sub)->Level<= elt->Level && (*sub)->Level>=0)
                break;
            if ((*sub)->Level== elt->Level+1)
                OutputElementDeclaration(sub, EltEnd, CFile, Extras);
        }
    }

    if (elt->Type != EBML_unknown && elt->Name[0])
    {
        if (elt->Level==-1 && !Extras->StartedGlobal)
        {
            Extras->StartedGlobal = 1;
        }

        if (elt->Level>=0 && Extras->StartedGlobal)
        {
            Extras->StartedGlobal = 0;
        }

        if (elt->Level==0 || (elt->Level==1 && Extras->PassedEBML))
        {
            Extras->InTags = 0;
        }
        Extras->CurrLevel = elt->Level;

        if (Extras->PassedEBML)
        {
            TextPrintf(CFile, T("extern const ebml_context MATROSKA_Context%s;\n"), elt->Name);
            if (elt->Type==EBML_MASTER)
                TextWrite(CFile, T("\n"));
        }
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


static void OutputCHeader(textwriter *CFile, bool_t WithInclude)
{
    TextWrite(CFile, T("/*\n"));
    TextWrite(CFile, T(" * DO NOT EDIT, GENERATED WITH DATA2LIB2\n"));
    TextWrite(CFile, T(" *\n"));
    TextPrintf(CFile, T(" * $Id$\n"));
    TextWrite(CFile, T(" * Copyright (c) 2008-2011, Matroska (non-profit organisation)\n"));
    TextWrite(CFile, T(" * All rights reserved.\n"));
    TextWrite(CFile, T(" *\n"));
    TextWrite(CFile, T(" * Redistribution and use in source and binary forms, with or without\n"));
    TextWrite(CFile, T(" * modification, are permitted provided that the following conditions are met:\n"));
    TextWrite(CFile, T(" *     * Redistributions of source code must retain the above copyright\n"));
    TextWrite(CFile, T(" *       notice, this list of conditions and the following disclaimer.\n"));
    TextWrite(CFile, T(" *     * Redistributions in binary form must reproduce the above copyright\n"));
    TextWrite(CFile, T(" *       notice, this list of conditions and the following disclaimer in the\n"));
    TextWrite(CFile, T(" *       documentation and/or other materials provided with the distribution.\n"));
    TextWrite(CFile, T(" *     * Neither the name of the Matroska assocation nor the\n"));
    TextWrite(CFile, T(" *       names of its contributors may be used to endorse or promote products\n"));
    TextWrite(CFile, T(" *       derived from this software without specific prior written permission.\n"));
    TextWrite(CFile, T(" *\n"));
    TextWrite(CFile, T(" * THIS SOFTWARE IS PROVIDED BY the Matroska association ``AS IS'' AND ANY\n"));
    TextWrite(CFile, T(" * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n"));
    TextWrite(CFile, T(" * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"));
    TextWrite(CFile, T(" * DISCLAIMED. IN NO EVENT SHALL The Matroska Foundation BE LIABLE FOR ANY\n"));
    TextWrite(CFile, T(" * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n"));
    TextWrite(CFile, T(" * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n"));
    TextWrite(CFile, T(" * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n"));
    TextWrite(CFile, T(" * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"));
    TextWrite(CFile, T(" * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n"));
    TextWrite(CFile, T(" * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"));
    TextWrite(CFile, T(" */\n"));
    TextWrite(CFile, T("#include \"matroska/matroska.h\"\n"));
    if (WithInclude) TextWrite(CFile, T("#include \"matroska/matroska_sem.h\"\n"));
    TextWrite(CFile, T("#include \"matroska/matroska_internal.h\"\n"));
    TextWrite(CFile, T("\n"));
}


int main(void)
{
    parsercontext p;
    parser parseIn;
    stream *Input = NULL,*OutputC = NULL,*OutputH = NULL;
    array Elements;
    SpecElement **element;
    //tchar_t Element[MAXLINE], String[MAXLINE], Value[MAXLINE];

    ParserContext_Init(&p,NULL,NULL,NULL);
    StdAfx_Init((nodemodule*)&p);

    Input = StreamOpen(&p,T("specdata.xml"),SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    OutputC = StreamOpen(&p,T("matroska_sem.c"),SFLAG_WRONLY|SFLAG_CREATE);
    OutputH = StreamOpen(&p,T("matroska_sem.h"),SFLAG_WRONLY|SFLAG_CREATE);

    memset(&parseIn, 0, sizeof(parseIn));
    ArrayInit(&Elements);

    if (ParserStreamXML(&parseIn, Input, &p, T("table"), 0)==ERR_NONE)
    {
        textwriter CFile;
        table_extras Extras;

        ReadLevel(&parseIn, &Elements);

        memset(&Extras,0,sizeof(Extras));
        memset(&CFile,0,sizeof(CFile));
        Extras.CurrLevel = -1;

        CFile.Stream = OutputC;
        OutputCHeader(&CFile, 1);

        for (element=ARRAYBEGIN(Elements,SpecElement*); element!=ARRAYEND(Elements,SpecElement*);++element) {
            if ((element+1) == ARRAYEND(Elements,SpecElement*))
                Extras.IsLast = 1;
            if ((*element)->Id == 0x18538067)
            {
                Extras.PassedEBML = 1;
                OutputElementDefinition(element, ARRAYEND(Elements,SpecElement*), &CFile, &Extras);
                break;
            }
        }

        memset(&Extras,0,sizeof(Extras));
        Extras.CurrLevel = -1;

        CFile.Stream = OutputH;
        OutputCHeader(&CFile, 0);

        TextWrite(&CFile, T("#ifndef MATROSKA_SEMANTIC_H\n"));
        TextWrite(&CFile, T("#define MATROSKA_SEMANTIC_H\n\n"));

        for (element=ARRAYBEGIN(Elements,SpecElement*); element!=ARRAYEND(Elements,SpecElement*);++element) {
            if ((element+1) == ARRAYEND(Elements,SpecElement*))
                Extras.IsLast = 1;
            if ((*element)->Id == 0x18538067)
            {
                Extras.PassedEBML = 1;
                OutputElementDeclaration(element, ARRAYEND(Elements,SpecElement*), &CFile, &Extras);
                break;
            }
        }

        TextWrite(&CFile, T("#endif // MATROSKA_SEMANTIC_H\n"));
    }

    for (element=ARRAYBEGIN(Elements,SpecElement*); element!=ARRAYEND(Elements,SpecElement*);++element)
        NodeDelete((node*)*element);
    ArrayClear(&Elements);

    StreamClose(Input);
    StreamClose(OutputC);
    StreamClose(OutputH);

	StdAfx_Done((nodemodule*)&p);
    ParserContext_Done(&p);
    return 0;
}

