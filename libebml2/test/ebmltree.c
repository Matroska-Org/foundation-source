/*
 * $Id$
 * Copyright (c) 2008-2010, Matroska Foundation
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

#include <stdio.h>

#include "ebml2/ebml.h"

static ebml_element *OutputElement(ebml_element *Element, const ebml_parser_context *Context, stream *Input, int *Level)
{
    int LevelPrint;
    for (LevelPrint=0;LevelPrint<*Level;++LevelPrint)
        fprintf(stdout,"+ ");
    fprintf(stdout,"%s: ",EBML_ElementGetClassName(Element));
    if (Node_IsPartOf(Element,EBML_MASTER_CLASS))
    {
        int UpperElement = 0;
        ebml_element *SubElement,*NewElement;
        ebml_parser_context SubContext;

        fprintf(stdout,"(master) [%d bytes]\r\n",(int)EBML_ElementDataSize(Element,1));
        SubContext.UpContext = Context;
        SubContext.Context = EBML_ElementContext(Element);
        SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
        while (SubElement != NULL && UpperElement<=0 && (EBML_ElementPosition(SubElement) < (EBML_ElementDataSize(Element,1) + EBML_ElementPositionData(Element)) || *Level==-1))
        {
            // a sub element == not higher level and contained inside the current element
            (*Level)++;
            NewElement = OutputElement(SubElement, &SubContext, Input, Level);
            NodeDelete((node*)SubElement);
            if (NewElement)
                SubElement = NewElement;
            else
                SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
            (*Level)--;
            if (UpperElement < 0)
                *Level += UpperElement;
        }
        if (UpperElement < 0) // global element
            *Level += UpperElement+1;
        else
            *Level -= UpperElement-1;
        return SubElement;
        //EBML_ElementSkipData(Element, Input, EBML_ElementContext(Element), NULL, 0);
    }
    else if (Node_IsPartOf(Element,EBML_STRING_CLASS) || Node_IsPartOf(Element,EBML_UNISTRING_CLASS))
    {
        //tchar_t UnicodeString[MAXDATA];
        //EBML_StringRead((ebml_string*)Element,Input,UnicodeString,TSIZEOF(UnicodeString));
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA,0)==ERR_NONE)
        {
            tchar_t String[MAXDATA];
            char cString[MAXDATA];
            EBML_StringGet((ebml_string*)Element, String, TSIZEOF(String));
            Node_ToUTF8(Element, cString, sizeof(cString), String);
            fprintf(stdout,"%s\r\n",cString);
        }
        else
            fprintf(stdout,"<error reading>\r\n");
    }
    else if (Node_IsPartOf(Element,EBML_DATE_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA,0)==ERR_NONE)
        {
            datepack_t Date;
            datetime_t DateTime = EBML_DateTime((ebml_date*)Element);
            GetDatePacked(DateTime,&Date,0);
            fprintf(stdout,"%04"PRIdPTR"-%02"PRIdPTR"-%02"PRIdPTR" %02"PRIdPTR":%02"PRIdPTR":%02"PRIdPTR"\r\n",Date.Year,Date.Month,Date.Day,Date.Hour,Date.Minute,Date.Second);
        }
        else
            fprintf(stdout,"<error reading>\r\n");
    }
    else if (Node_IsPartOf(Element,EBML_INTEGER_CLASS) || Node_IsPartOf(Element,EBML_SINTEGER_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA,0)==ERR_NONE)
        {
            if (Node_IsPartOf(Element,EBML_SINTEGER_CLASS))
                fprintf(stdout,"%"PRId64"\r\n",EBML_IntegerValue((ebml_integer*)Element));
            else
                fprintf(stdout,"%"PRIu64"\r\n",(uint64_t)EBML_IntegerValue((ebml_integer*)Element));
        }
        else
            fprintf(stdout,"<error reading>\r\n");
    }
    else if (Node_IsPartOf(Element,EBML_FLOAT_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA,0)==ERR_NONE)
            fprintf(stdout,"%f\r\n", EBML_FloatValue((ebml_float*)Element));
        else
            fprintf(stdout,"<error reading>\r\n");
    }
    else if (EBML_ElementIsDummy(Element))
    {
#ifdef IS_BIG_ENDIAN
        fprintf(stdout,"%x\r\n",EBML_ElementClassID(Element));
#else
        fourcc_t Id = EBML_ElementClassID(Element);
        while (Id & 0xFF)
        {
            fprintf(stdout,"[%x]",Id & 0xFF);
            Id >>= 8;
        }
        fprintf(stdout,"\r\n");
#endif
        EBML_ElementSkipData(Element, Input, Context, NULL, 0);
    }
    else if (Node_IsPartOf(Element,EBML_BINARY_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA,0)==ERR_NONE)
        {
            const uint8_t *Data = EBML_BinaryGetData((ebml_binary*)Element);
            fprintf(stdout,"%02X %02X %02X %02X.. (%"PRId64")\r\n",Data[0],Data[1],Data[2],Data[3],EBML_ElementDataSize(Element,1));
        }
        else
            fprintf(stdout,"<error reading>\r\n");
    }
    // TODO: handle crc32
    else
    {
#ifdef IS_BIG_ENDIAN
        fprintf(stdout,"<unsupported element %x>\r\n",EBML_ElementClassID(Element));
#else
        fourcc_t Id = EBML_ElementClassID(Element);
        fprintf(stdout,"<unsupported element ");
        while (Id & 0xFF)
        {
            fprintf(stdout,"[%x]",Id & 0xFF);
            Id >>= 8;
        }
        fprintf(stdout,">\r\n");
#endif
        EBML_ElementSkipData(Element, Input, Context, NULL, 0);
    }
    return NULL;
}

static void OutputTree(stream *Input)
{
    ebml_element *Element = EBML_ElementCreate(Input,EBML_getContextHead(),0, EBML_ANY_PROFILE,NULL);
    if (Element)
    {
        int Level = -1;
        OutputElement(Element, NULL, Input, &Level);
        NodeDelete((node*)Element);
    }
}

int main(int argc, const char *argv[])
{
    parsercontext p;
    stream *Input;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: ebmltree [ebml_file]\r\n");
        return 1;
    }

    // Core-C init phase
    ParserContext_Init(&p,NULL,NULL,NULL);
    // EBML Init
    EBML_Init(&p);

    // open the file to parse
    tchar_t fileName[MAXPATH];
    Node_FromStr(&p, fileName, TSIZEOF(fileName), argv[1]);
    Input = StreamOpen(&p,fileName,SFLAG_RDONLY);
    if (Input == NULL)
        fprintf(stderr, "error: ebmltree cannot open file \"%s\"\r\n",argv[1]);
    else
    {
        OutputTree(Input);

        StreamClose(Input);
    }

    // EBML ending
    EBML_Done(&p);
    // Core-C ending
    ParserContext_Done(&p);

    return 0;
}
