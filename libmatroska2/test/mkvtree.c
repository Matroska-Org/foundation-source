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
 *     * Neither the name of the Matroska Foundation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY The Matroska Foundation ``AS IS'' AND ANY
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

#include "matroska/matroska.h"
#include "mkvtree_stdafx.h"

static ebml_element *OutputElement(ebml_element *Element, const ebml_parser_context *Context, stream *Input, int *Level)
{
    int LevelPrint;
    for (LevelPrint=0;LevelPrint<*Level;++LevelPrint)
        fprintf(stdout,"+ ");
    fprintf(stdout,"%s: ",Element->Context->ElementName);
    if (Node_IsPartOf(Element,EBML_MASTER_CLASS))
    {
        int UpperElement = 0;
        ebml_element *SubElement,*NewElement;
        ebml_parser_context SubContext;

        if (Element->Size == INVALID_FILEPOS_T)
            fprintf(stdout,"(master)\r\n");
        else
            fprintf(stdout,"(master) [%d bytes]\r\n",(int)Element->Size);
        SubContext.UpContext = Context;
        SubContext.Context = Element->Context;
        SubContext.EndPosition = EBML_ElementPositionEnd(Element);
        SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
        while (SubElement != NULL && UpperElement<=0 && (SubElement->ElementPosition < (Element->Size + Element->SizePosition + Element->SizeLength) || *Level==-1))
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
            if (!SubElement)
            {
                if (UpperElement < 0)
                    *Level += UpperElement;
                else if (UpperElement != 0 && *Level>0)
                    *Level -= UpperElement-1;
            }
        }
        return SubElement;
        //EBML_ElementSkipData(Element, Input, Element->Context, NULL, 0);
    }
    else if (Node_IsPartOf(Element,EBML_STRING_CLASS) || Node_IsPartOf(Element,EBML_UNISTRING_CLASS))
    {
        //tchar_t UnicodeString[MAXDATA];
        //EBML_StringRead((ebml_string*)Element,Input,UnicodeString,TSIZEOF(UnicodeString));
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA)==ERR_NONE)
            fprintf(stdout,"%s\r\n",((ebml_string*)Element)->Buffer);
        else
            fprintf(stdout,"<error reading>\r\n");
    }
    else if (Node_IsPartOf(Element,EBML_DATE_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA)==ERR_NONE)
        {
            datepack_t Date;
            datetime_t DateTime = EBML_DateTime((ebml_date*)Element);
            GetDatePacked(DateTime,&Date,1);
            fprintf(stdout,"%04d-%02d-%02d %02d:%02d:%02d UTC\r\n",Date.Year,Date.Month,Date.Day,Date.Hour,Date.Minute,Date.Second);
        }
        else
            fprintf(stdout,"<error reading>\r\n");
    }
    else if (Node_IsPartOf(Element,EBML_INTEGER_CLASS) || Node_IsPartOf(Element,EBML_SINTEGER_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA)==ERR_NONE)
        {
            if (Node_IsPartOf(Element,EBML_SINTEGER_CLASS))
                fprintf(stdout,"%ld\r\n",(int)((ebml_integer*)Element)->Value);
            else
                fprintf(stdout,"%lu\r\n",(int)((ebml_integer*)Element)->Value);
        }
        else
            fprintf(stdout,"<error reading>\r\n");
    }
    else if (Node_IsPartOf(Element,EBML_FLOAT_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA)==ERR_NONE)
            fprintf(stdout,"%f\r\n",((ebml_float*)Element)->Value);
        else
            fprintf(stdout,"<error reading>\r\n");
    }
    else if (EBML_ElementIsDummy(Element))
    {
#ifdef IS_BIG_ENDIAN
        fprintf(stdout,"%x\r\n",Element->Context->Id);
#else
        fourcc_t Id = Element->Context->Id;
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
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA)==ERR_NONE)
        {
            uint8_t *Data = ARRAYBEGIN(((ebml_binary*)Element)->Data,uint8_t);
            if (Element->Size != 0)
            {
                if (Element->Size == 1)
                    fprintf(stdout,"%02X (%d)\r\n",Data[0],Element->Size);
                else if (Element->Size == 2)
                    fprintf(stdout,"%02X %02X (%d)\r\n",Data[0],Data[1],Element->Size);
                else if (Element->Size == 3)
                    fprintf(stdout,"%02X %02X %02X (%d)\r\n",Data[0],Data[1],Data[2],Element->Size);
                else if (Element->Size == 4)
                    fprintf(stdout,"%02X %02X %02X %02X (%d)\r\n",Data[0],Data[1],Data[2],Data[3],Element->Size);
                else
                    fprintf(stdout,"%02X %02X %02X %02X.. (%d)\r\n",Data[0],Data[1],Data[2],Data[3],Element->Size);
            }
        }
        else
            fprintf(stdout,"<error reading>\r\n");
    }
    // TODO: handle crc32
    else
    {
#ifdef IS_BIG_ENDIAN
        fprintf(stdout,"<unsupported element %x>\r\n",Element->Context->Id);
#else
        fourcc_t Id = Element->Context->Id;
        fprintf(stdout,"<unsupported element ");
        while (Id & 0xFF)
        {
            fprintf(stdout,"[%x]",Id & 0xFF);
            Id >>= 8;
        }
        fprintf(stdout,">\r\n",Element->Context->Id);
#endif
        EBML_ElementSkipData(Element, Input, Context, NULL, 0);
    }
    return NULL;
}

static void OutputTree(stream *Input)
{
    ebml_element *Element = EBML_ElementCreate(Input,&MATROSKA_ContextStream,0,NULL);
    if (Element)
    {
        int Level = -1;
        OutputElement(Element, NULL, Input, &Level);
        NodeDelete((node*)Element);
    }
}

int main(int argc, const char *argv[])
{
#if defined(CONFIG_EBML_UNICODE)
    parsercontext p;
#else
    nodecontext p;
#endif
    stream *Input;
    tchar_t Path[MAXPATHFULL];

    if (argc != 2)
    {
        fprintf(stderr, "Usage: mkvtree [matroska_file]\r\n");
        return 1;
    }

    // Core-C init phase
#if defined(CONFIG_EBML_UNICODE)
    ParserContext_Init(&p,NULL,NULL,NULL);
#else
    NodeContext_Init(&p,NULL,NULL,NULL);
#endif
	StdAfx_Init((nodemodule*)&p);
    // EBML & Matroska Init
    MATROSKA_Init((nodecontext*)&p);

    // open the file to parse
    Node_FromStr(&p,Path,TSIZEOF(Path),argv[1]);
    Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    if (Input == NULL)
        fprintf(stderr, "error: mkvtree cannot open file \"%s\"\r\n",argv[1]);
    else
    {
        OutputTree(Input);

        StreamClose(Input);
    }

    // EBML & Matroska ending
    MATROSKA_Done((nodecontext*)&p);
    // Core-C ending
	StdAfx_Done((nodemodule*)&p);
#if defined(CONFIG_EBML_UNICODE)
    ParserContext_Done(&p);
#else
    NodeContext_Done(&p);
#endif

    return 0;
}
