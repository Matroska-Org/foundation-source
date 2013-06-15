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
 * A tool to generate the libmatroska semantic files
 *  input: specdata.xml
 *  output: KaxSemantic.cpp / KaxSemantic.h
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
    int CurrProfile;

} table_extras;

static const tchar_t *GetCppName(const SpecElement *elt)
{
    if (elt->CppName[0])
        return elt->CppName;
    return elt->Name;
}

static const tchar_t *GetClassName(const SpecElement *elt)
{
    if (elt->Id==0x61A7)
        return T("Attached");
    if (elt->Id==0x4660)
        return T("MimeType");
    if (elt->Id==0x447A)
        return T("TagLangue");
    if (elt->Id!=0x114D9B74 && elt->Id!=0x4DBB && elt->Id!=0xFA)
    {
        if (elt->CppName[0])
            return elt->CppName;
    }
    return elt->Name;
}

static bool_t IsValidElement(const SpecElement *elt)
{
    return elt->InWebM || elt->MinVersion || elt->InDivX;
}

static void AddElementSemantic(textwriter *CFile, const SpecElement *elt, bool_t InRecursive)
{
    TextPrintf(CFile, T("DEFINE_SEMANTIC_ITEM(%s, %s, Kax%s)"), InRecursive?T("false"):(elt->Mandatory?T("true"):T("false")), elt->Multiple?T("false"):T("true"), GetClassName(elt));
    if (!IsValidElement(elt))
        TextWrite(CFile, T(" // not supported\n"));
    else if (!elt->MinVersion && elt->InDivX)
        TextWrite(CFile, T(" // DivX specific\n"));
    else if (InRecursive)
        TextWrite(CFile, T(" // recursive\n"));
    else
        TextWrite(CFile, T("\n"));
}

static const tchar_t *GetClassType(const SpecElement *elt, bool_t Define)
{
    if (elt->Id==0x3CB923 || elt->Id==0x3EB923 || elt->Id==0xA3 || elt->Id==0xA1 || elt->Id==0xA2)
        return T("BINARY_CONS");
    else if (elt->Id==0x1F43B675 || elt->Id==0xA0 || elt->Id==0xAE)
        return T("MASTER_CONS");
    else if (Define && (elt->Id==0x61A7 || elt->Id==0x1941A469))
        return T("MASTER_CONS");
    else if (elt->Id==0xFB)
        return T("SINTEGER_CONS");
    else switch (elt->Type)
    {
    case EBML_MASTER:
        return T("MASTER");
        break;
    case EBML_INTEGER:
        return T("SINTEGER");
        break;
    case EBML_UNSIGNED_INTEGER:
        return T("UINTEGER");
        break;
    case EBML_DATE:
        return T("DATE    ");
        break;
    case EBML_FLOAT:
        return T("FLOAT");
        break;
    case EBML_STRING:
        return T("STRING");
        break;
    case EBML_UNICODE_STRING:
        return T("UNISTRING");
        break;
    case EBML_BINARY:
        return T("BINARY ");
        break;
    }
    return NULL;
}

static void OutputElementDefinition(const SpecElement **pElt, const SpecElement *parent, const SpecElement **EltEnd, textwriter *CFile, table_extras *Extras)
{
    const SpecElement *elt = *pElt;

    if (pElt==EltEnd)
        return;

    //if (!IsValidElement(elt)) TextWrite(CFile, T("// not supported "));

    if (elt->Type != EBML_unknown && elt->Name[0])
    {
        const SpecElement **sub;

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

            if (!IsValidElement(elt))
            {
                if (Extras->CurrProfile != 2)
                {
                    Extras->CurrProfile = 2;
                    TextPrintf(CFile, T("#if MATROSKA_VERSION >= %d\n"), Extras->CurrProfile);
                }
            }
            else if (!elt->MinVersion || Extras->CurrProfile < elt->MinVersion)
            {
                if (Extras->CurrProfile != 2)
                {
                    Extras->CurrProfile = 2;
                    TextPrintf(CFile, T("#if MATROSKA_VERSION >= %d\n"), Extras->CurrProfile);
                }
            }

            if (elt->Type==EBML_MASTER)
            {
                // write the semantic
                int minProfile = Extras->CurrProfile;
                TextPrintf(CFile, T("\nDEFINE_START_SEMANTIC(Kax%s)\n"), GetClassName(elt));
                if (elt->Recursive)
                    AddElementSemantic(CFile, elt, 1);
                for (sub = pElt+1; sub!=EltEnd; ++sub) 
                {
                    if ((*sub)->Level<= elt->Level)
                        break;
                    if ((*sub)->Level== elt->Level+1)
                    {
                        if (!IsValidElement(*sub))
                        {
                            if (minProfile != 2)
                            {
                                minProfile = 2;
                                TextPrintf(CFile, T("#if MATROSKA_VERSION >= %d\n"), minProfile);
                            }
                        }
                        else if (!(*sub)->MinVersion || minProfile < (*sub)->MinVersion)
                        {
                            if (minProfile != 2)
                            {
                                minProfile = 2;
                                TextPrintf(CFile, T("#if MATROSKA_VERSION >= %d\n"), minProfile);
                            }
                        }
                        else if (minProfile > (*sub)->MinVersion)
                        {
                            minProfile = (*sub)->MinVersion;
                            TextPrintf(CFile, T("#endif // MATROSKA_VERSION\n"));
                        }

                        //if (!IsValidElement(*sub)) TextWrite(CFile, T("// "));
                        AddElementSemantic(CFile, *sub, 0);
                    }
                }
                    
                if (minProfile > 1 && Extras->CurrProfile < 2)
                {
                    minProfile = 1;
                    TextPrintf(CFile, T("#endif // MATROSKA_VERSION\n"));
                }
                TextPrintf(CFile, T("DEFINE_END_SEMANTIC(Kax%s)\n\n"), GetClassName(elt));
            }

            TextWrite(CFile, T("DEFINE_MKX_"));

            TextWrite(CFile, GetClassType(elt, 1));

            switch (elt->Type)
            {
            case EBML_INTEGER:
            case EBML_UNSIGNED_INTEGER:
            case EBML_DATE:
                s = elt->DefaultValue;
                if (!elt->DefaultValue[0])
                    TextPrintf(CFile, T("(Kax%s, 0x%X, %d, Kax%s, \"%s\");\n"), GetClassName(elt), elt->Id, elt->Id<0x100 ? 1 : (elt->Id<0x10000 ? 2 : (elt->Id<0x1000000 ? 3 : 4)), GetClassName(parent), GetCppName(elt));
                else if (ExprIsInt(&s,&value))
                    TextPrintf(CFile, T("_DEF(Kax%s, 0x%X, %d, Kax%s, \"%s\", %d);\n"), GetClassName(elt), elt->Id, elt->Id<0x100 ? 1 : (elt->Id<0x10000 ? 2 : (elt->Id<0x1000000 ? 3 : 4)), GetClassName(parent), GetCppName(elt), value);
                else
                    TextPrintf(CFile, T("(Kax%s, 0x%X, %d, Kax%s, \"%s\");\n"), GetClassName(elt), elt->Id, elt->Id<0x100 ? 1 : (elt->Id<0x10000 ? 2 : (elt->Id<0x1000000 ? 3 : 4)), GetClassName(parent), GetCppName(elt));
                break;
            case EBML_FLOAT:
                s = elt->DefaultValue;
                if (!elt->DefaultValue[0])
                    TextPrintf(CFile, T("(Kax%s, 0x%X, %d, Kax%s, \"%s\");\n"), GetClassName(elt), elt->Id, elt->Id<0x100 ? 1 : (elt->Id<0x10000 ? 2 : (elt->Id<0x1000000 ? 3 : 4)), GetClassName(parent), GetCppName(elt));
                else if (ExprIsInt(&s,&value))
                    TextPrintf(CFile, T("_DEF(Kax%s, 0x%X, %d, Kax%s, \"%s\", %d);\n"), GetClassName(elt), elt->Id, elt->Id<0x100 ? 1 : (elt->Id<0x10000 ? 2 : (elt->Id<0x1000000 ? 3 : 4)), GetClassName(parent), GetCppName(elt), value);
                else
                    TextPrintf(CFile, T("(Kax%s, 0x%X, %d, Kax%s, \"%s\");\n"), GetClassName(elt), elt->Id, elt->Id<0x100 ? 1 : (elt->Id<0x10000 ? 2 : (elt->Id<0x1000000 ? 3 : 4)), GetClassName(parent), GetCppName(elt));
                break;
            case EBML_STRING:
            case EBML_UNICODE_STRING:
                if (!elt->DefaultValue[0])
                    TextPrintf(CFile, T("(Kax%s, 0x%X, %d, Kax%s, \"%s\");\n"), GetClassName(elt), elt->Id, elt->Id<0x100 ? 1 : (elt->Id<0x10000 ? 2 : (elt->Id<0x1000000 ? 3 : 4)), GetClassName(parent), GetCppName(elt));
                else
                    TextPrintf(CFile, T("_DEF(Kax%s, 0x%X, %d, Kax%s, \"%s\", \"%s\");\n"), GetClassName(elt), elt->Id, elt->Id<0x100 ? 1 : (elt->Id<0x10000 ? 2 : (elt->Id<0x1000000 ? 3 : 4)), GetClassName(parent), GetCppName(elt), elt->DefaultValue);
                break;
            default:
                if (parent==NULL)
                    TextPrintf(CFile, T("_ORPHAN(Kax%s, 0x%X, %d, \"%s\");\n"), GetClassName(elt), elt->Id, elt->Id<0x100 ? 1 : (elt->Id<0x10000 ? 2 : (elt->Id<0x1000000 ? 3 : 4)), GetCppName(elt));
                else
                    TextPrintf(CFile, T("(Kax%s, 0x%X, %d, Kax%s, \"%s\");\n"), GetClassName(elt), elt->Id, elt->Id<0x100 ? 1 : (elt->Id<0x10000 ? 2 : (elt->Id<0x1000000 ? 3 : 4)), GetClassName(parent), GetCppName(elt));
                break;
            }
        }

        sub = pElt+1;
        if (sub!=EltEnd && (*sub)->MinVersion && Extras->CurrProfile > (*sub)->MinVersion)
        {
            Extras->CurrProfile = (*sub)->MinVersion;
            TextPrintf(CFile, T("#endif\n"));
        }
    }

    if (elt->Type==EBML_MASTER)
    {
        const SpecElement **sub;
        for (sub = pElt+1; sub!=EltEnd; ++sub)
        {
            if ((*sub)->Level<= elt->Level && (*sub)->Level>=0)
                break;
            if ((*sub)->Level== elt->Level+1)
                OutputElementDefinition(sub, elt, EltEnd, CFile, Extras);
        }
    }
}

static void OutputElementDeclaration(const SpecElement **pElt, const SpecElement **EltEnd, textwriter *CFile, table_extras *Extras)
{
    const SpecElement *elt = *pElt;
    const SpecElement **sub;

    if (pElt==EltEnd)
        return;

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

        if (Extras->PassedEBML && elt->Id!=0xAE && elt->Id!=0xA0 && elt->Id!=0xA1 && elt->Id!=0xA2 && elt->Id!=0xA3 && elt->Id!=0xFB
            && elt->Id!=0x1C53BB6B && elt->Id!=0x1F43B675 && elt->Id!=0x18538067 && elt->Id!=0xBB && elt->Id!=0xB7 && elt->Id!=0xDB
            && elt->Id!=0x3CB923 && elt->Id!=0x3EB923 && elt->Id!=0x114D9B74 && elt->Id!=0x4DBB)
        {
            if (!IsValidElement(elt))
            {
                if (Extras->CurrProfile != 2)
                {
                    Extras->CurrProfile = 2;
                    TextPrintf(CFile, T("#if MATROSKA_VERSION >= %d\n"), Extras->CurrProfile);
                }
            }
            else if (!elt->MinVersion || Extras->CurrProfile < elt->MinVersion)
            {
                if (Extras->CurrProfile != 2)
                {
                    Extras->CurrProfile = 2;
                    TextPrintf(CFile, T("#if MATROSKA_VERSION >= %d\n"), Extras->CurrProfile);
                }
            }

            TextWrite(CFile, T("DECLARE_MKX_"));
            TextWrite(CFile, GetClassType(elt, 0));
            TextPrintf(CFile, T("(Kax%s)\n"), GetClassName(elt));

            switch (elt->Id)
            {
            case 0x53AB:
                TextWrite(CFile, T("public:\n"));
                TextWrite(CFile, T("\tvirtual bool ValidateSize() const {return IsFiniteSize() && GetSize() <= 4;}\n"));
                break;
            case 0x73A4:
                TextWrite(CFile, T("#if defined(HAVE_EBML2) || defined(HAS_EBML2)\n"));
                TextWrite(CFile, T("public:\n"));
                TextWrite(CFile, T("\tKaxSegmentUID(EBML_DEF_CONS EBML_DEF_SEP EBML_EXTRA_PARAM);\n"));
                TextWrite(CFile, T("#endif\n"));
                break;
            }

            if (elt->ByteSize)
            {
                TextWrite(CFile, T("public:\n"));
                TextPrintf(CFile, T("\tvirtual bool ValidateSize() const {return IsFiniteSize() && GetSize() == %d;}\n"), elt->ByteSize);
            }
            if (!IsValidElement(elt) || elt->MaxVersion!=0)
            {
                TextWrite(CFile, T("public:\n"));
                TextWrite(CFile, T("\tfilepos_t RenderData(IOCallback & output, bool bForceRender, bool bSaveDefault);\n"));
            }

            TextWrite(CFile, T("};\n"));

            sub = pElt+1;
            if (sub!=EltEnd && (*sub)->MinVersion && Extras->CurrProfile > (*sub)->MinVersion)
            {
                Extras->CurrProfile = (*sub)->MinVersion;
                TextPrintf(CFile, T("#endif\n\n"));
            }
            else if (elt->Type==EBML_MASTER)
                TextWrite(CFile, T("\n\n"));
            else
                TextWrite(CFile, T("\n"));
        }
    }

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
    TextWrite(CFile, T("/**********************************************************************\n"));
    TextWrite(CFile, T("**  DO NOT EDIT, GENERATED WITH DATA2LIB\n"));
    TextWrite(CFile, T("** \n"));
    TextWrite(CFile, T("**  libmatroska : parse Matroska files, see http://www.matroska.org/\n"));
    TextWrite(CFile, T("** \n"));
    TextWrite(CFile, T("**  Copyright (c) 2002-2010, Matroska (non-profit organisation)\n"));
    TextWrite(CFile, T("**  All rights reserved.\n"));
    TextWrite(CFile, T("** \n"));
    TextWrite(CFile, T("** This file is part of libmatroska.\n"));
    TextWrite(CFile, T("**\n"));
    TextWrite(CFile, T("** This library is free software; you can redistribute it and/or\n"));
    TextWrite(CFile, T("** modify it under the terms of the GNU Lesser General Public\n"));
    TextWrite(CFile, T("** License as published by the Free Software Foundation; either\n"));
    TextWrite(CFile, T("** version 2.1 of the License, or (at your option) any later version.\n"));
    TextWrite(CFile, T("** \n"));
    TextWrite(CFile, T("** This library is distributed in the hope that it will be useful,\n"));
    TextWrite(CFile, T("** but WITHOUT ANY WARRANTY; without even the implied warranty of\n"));
    TextWrite(CFile, T("** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"));
    TextWrite(CFile, T("** Lesser General Public License for more details.\n"));
    TextWrite(CFile, T("** \n"));
    TextWrite(CFile, T("** You should have received a copy of the GNU Lesser General Public\n"));
    TextWrite(CFile, T("** License along with this library; if not, write to the Free Software\n"));
    TextWrite(CFile, T("** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA\n"));
    TextWrite(CFile, T("**\n"));
    TextWrite(CFile, T("** See http://www.gnu.org/licenses/lgpl-2.1.html for LGPL licensing information.**\n"));
    TextWrite(CFile, T("** Contact license@matroska.org if any conditions of this licensing are\n"));
    TextWrite(CFile, T("** not clear to you.\n"));
    TextWrite(CFile, T("**\n"));
    TextWrite(CFile, T("**********************************************************************/\n\n"));

    if (WithInclude)
    {
        TextWrite(CFile, T("#include \"matroska/KaxContexts.h\"\n"));
        TextWrite(CFile, T("#include \"matroska/KaxSemantic.h\"\n"));
        TextWrite(CFile, T("#include \"matroska/KaxSegment.h\"\n"));
        TextWrite(CFile, T("#include \"matroska/KaxSeekHead.h\"\n"));
        TextWrite(CFile, T("#include \"matroska/KaxCluster.h\"\n"));
        TextWrite(CFile, T("#include \"matroska/KaxTracks.h\"\n"));
        TextWrite(CFile, T("#include \"matroska/KaxCues.h\"\n"));
        TextWrite(CFile, T("#include \"matroska/KaxInfoData.h\"\n"));
        TextWrite(CFile, T("#include \"matroska/KaxBlockData.h\"\n"));
        TextWrite(CFile, T("#include \"matroska/KaxCuesData.h\"\n"));
    }
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
    OutputC = StreamOpen(&p,T("KaxSemantic.cpp"),SFLAG_WRONLY|SFLAG_CREATE);
    OutputH = StreamOpen(&p,T("KaxSemantic.h"),SFLAG_WRONLY|SFLAG_CREATE);

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
        Extras.CurrProfile = 1;

        CFile.Stream = OutputC;
        OutputCHeader(&CFile, 1);

        TextWrite(&CFile, T("START_LIBMATROSKA_NAMESPACE\n"));
        for (element=ARRAYBEGIN(Elements,SpecElement*); element!=ARRAYEND(Elements,SpecElement*);++element) {
            if ((element+1) == ARRAYEND(Elements,SpecElement*))
                Extras.IsLast = 1;
            if ((*element)->Id == 0x18538067)
            {
                Extras.PassedEBML = 1;
                OutputElementDefinition(element, NULL, ARRAYEND(Elements,SpecElement*), &CFile, &Extras);
                break;
            }
        }

        memset(&Extras,0,sizeof(Extras));
        Extras.CurrLevel = -1;
        Extras.CurrProfile = 1;
        for (element=ARRAYBEGIN(Elements,SpecElement*); element!=ARRAYEND(Elements,SpecElement*);++element) {
            if ((element+1) == ARRAYEND(Elements,SpecElement*))
                Extras.IsLast = 1;
            if ((*element)->Id == 0x18538067) // Segment
                Extras.PassedEBML = 1;
            if (Extras.PassedEBML)
            {
                if (!IsValidElement(*element) || (*element)->MaxVersion!=0)
                {
                    TextPrintf(&CFile, T("\nfilepos_t Kax%s::RenderData(IOCallback & /* output */, bool /* bForceRender */, bool /* bSaveDefault */) {\n"), GetClassName(*element));
                    TextWrite(&CFile, T("\tassert(false); // no you are not allowed to use this element !\n"));
                    TextWrite(&CFile, T("\treturn 0;\n"));
                    TextWrite(&CFile, T("}\n"));
                }
            }
        }

        TextWrite(&CFile, T("\nEND_LIBMATROSKA_NAMESPACE\n"));

        memset(&Extras,0,sizeof(Extras));
        Extras.CurrLevel = -1;

#if 1
        CFile.Stream = OutputH;
        OutputCHeader(&CFile, 0);

        TextWrite(&CFile, T("#ifndef LIBMATROSKA_SEMANTIC_H\n"));
        TextWrite(&CFile, T("#define LIBMATROSKA_SEMANTIC_H\n\n"));

        TextWrite(&CFile, T("#include \"matroska/KaxTypes.h\"\n"));
        TextWrite(&CFile, T("#include \"ebml/EbmlUInteger.h\"\n"));
        TextWrite(&CFile, T("#include \"ebml/EbmlSInteger.h\"\n"));
        TextWrite(&CFile, T("#include \"ebml/EbmlDate.h\"\n"));
        TextWrite(&CFile, T("#include \"ebml/EbmlFloat.h\"\n"));
        TextWrite(&CFile, T("#include \"ebml/EbmlString.h\"\n"));
        TextWrite(&CFile, T("#include \"ebml/EbmlUnicodeString.h\"\n"));
        TextWrite(&CFile, T("#include \"ebml/EbmlBinary.h\"\n"));
        TextWrite(&CFile, T("#include \"ebml/EbmlMaster.h\"\n"));
        TextWrite(&CFile, T("#include \"matroska/KaxDefines.h\"\n"));

        TextWrite(&CFile, T("\nusing namespace LIBEBML_NAMESPACE;\n"));

        TextWrite(&CFile, T("\nSTART_LIBMATROSKA_NAMESPACE\n\n"));

        for (element=ARRAYBEGIN(Elements,SpecElement*); element!=ARRAYEND(Elements,SpecElement*);++element) {
            if ((element+1) == ARRAYEND(Elements,SpecElement*))
                Extras.IsLast = 1;
            if ((*element)->Id == 0x18538067)
            {
                Extras.PassedEBML = 1;
                Extras.CurrProfile = 1;
                OutputElementDeclaration(element, ARRAYEND(Elements,SpecElement*), &CFile, &Extras);
                break;
            }
        }

        TextWrite(&CFile, T("END_LIBMATROSKA_NAMESPACE\n"));
        TextWrite(&CFile, T("\n#endif // LIBMATROSKA_SEMANTIC_H\n"));
#endif
    }

    for (element=ARRAYBEGIN(Elements,SpecElement*); element!=ARRAYEND(Elements,SpecElement*);++element)
        NodeDelete((node*)*element);
    ArrayClear(&Elements);

    StreamClose(Input);
    StreamClose(OutputC);
    //StreamClose(OutputH);

	StdAfx_Done((nodemodule*)&p);
    ParserContext_Done(&p);
    return 0;
}

