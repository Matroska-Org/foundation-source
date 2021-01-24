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

#include "matroska2/matroska.h"

static int ShowPos = 0;

#ifdef TARGET_WIN
#include <windows.h>
void DebugMessage(const tchar_t* Msg,...)
{
#if !defined(NDEBUG) || defined(LOGFILE) || defined(LOGTIME)
	va_list Args;
	tchar_t Buffer[1024],*s=Buffer;

	va_start(Args,Msg);
	vstprintf_s(Buffer,TSIZEOF(Buffer), Msg, Args);
	va_end(Args);
	tcscat_s(Buffer,TSIZEOF(Buffer),T("\r\n"));
#endif

#ifdef LOGTIME
    {
        tchar_t timed[1024];
        SysTickToString(timed,TSIZEOF(timed),GetTimeTick(),1,1,0);
        stcatprintf_s(timed,TSIZEOF(timed),T(" %s"),s);
        s = timed;
    }
#endif

#if !defined(NDEBUG)
	OutputDebugString(s);
#endif

#if defined(LOGFILE)
{
    static FILE* f=NULL;
    static char s8[1024];
    size_t i;
    if (!f)
        f=fopen("\\corelog.txt","a+b");
    for (i=0;s[i];++i)
        s8[i]=(char)s[i];
    s8[i]=0;
    fputs(s8,f);
    fflush(f);
}
#endif
}
#endif

static void EndLine(ebml_element *Element)
{
    if (ShowPos && EBML_ElementPosition(Element)!=INVALID_FILEPOS_T)
        fprintf(stdout," at %"PRId64"\r\n",EBML_ElementPosition(Element));
    else
        fprintf(stdout,"\r\n");
}

static ebml_element *OutputElement(ebml_element *Element, const ebml_parser_context *Context, stream *Input, int *Level)
{
    int LevelPrint;
    for (LevelPrint=0;LevelPrint<*Level;++LevelPrint)
        fprintf(stdout,"+ ");
    fprintf(stdout,"%s: ", EBML_ElementGetClassName(Element));
    if (Node_IsPartOf(Element,EBML_MASTER_CLASS))
    {
        int UpperElement = 0;
        ebml_element *SubElement,*NewElement;
        ebml_parser_context SubContext;

        if (EBML_ElementDataSize(Element, 1) == INVALID_FILEPOS_T)
            fprintf(stdout,"(master)");
        else if (!EBML_ElementIsFiniteSize(Element))
            fprintf(stdout,"(master) [unknown size]");
        else
            fprintf(stdout,"(master) [%"PRId64" bytes]",EBML_ElementDataSize(Element, 1));
        EndLine(Element);
        SubContext.UpContext = Context;
        SubContext.Context = EBML_ElementContext(Element);
        SubContext.EndPosition = EBML_ElementPositionEnd(Element);
        SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
		while (SubElement != NULL && UpperElement<=0 && (!EBML_ElementIsFiniteSize(Element) || EBML_ElementPosition(SubElement) <= EBML_ElementPositionEnd(Element)))
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
			else if (UpperElement != 0 && *Level>0)
				*Level -= UpperElement-1;
        }
        return SubElement;
        //EBML_ElementSkipData(Element, Input, Element->Context, NULL, 0);
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
            fprintf(stdout,"'%s'",cString);
        } else
            fprintf(stdout,"<error reading>");
        EndLine(Element);
    }
    else if (Node_IsPartOf(Element,EBML_DATE_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA,0)==ERR_NONE)
        {
            datepack_t Date;
            datetime_t DateTime = EBML_DateTime((ebml_date*)Element);
            GetDatePacked(DateTime,&Date,1);
            fprintf(stdout,"%04"PRIdPTR"-%02"PRIdPTR"-%02"PRIdPTR" %02"PRIdPTR":%02"PRIdPTR":%02"PRIdPTR" UTC",Date.Year,Date.Month,Date.Day,Date.Hour,Date.Minute,Date.Second);
        }
        else
            fprintf(stdout,"<error reading>");
        EndLine(Element);
    }
    else if (Node_IsPartOf(Element,EBML_INTEGER_CLASS) || Node_IsPartOf(Element,EBML_SINTEGER_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA,0)==ERR_NONE)
        {
            if (Node_IsPartOf(Element,EBML_SINTEGER_CLASS))
                fprintf(stdout,"%"PRId64,EBML_IntegerValue((ebml_integer*)Element));
            else
                fprintf(stdout,"%"PRIu64,EBML_IntegerValue((ebml_integer*)Element));
        }
        else
            fprintf(stdout,"<error reading>");
        EndLine(Element);
    }
    else if (Node_IsPartOf(Element,EBML_FLOAT_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_ALL_DATA,0)==ERR_NONE)
            fprintf(stdout,"%f",EBML_FloatValue((ebml_float*)Element));
        else
            fprintf(stdout,"<error reading>");
        EndLine(Element);
    }
    else if (EBML_ElementIsDummy(Element))
    {
        fprintf(stdout,"[%X] [%"PRId64" bytes]",EBML_ElementClassID(Element),EBML_ElementDataSize(Element, 1));
        EBML_ElementSkipData(Element, Input, Context, NULL, 0);
        EndLine(Element);
    }
    else if (Node_IsPartOf(Element,EBML_BINARY_CLASS))
    {
        if (EBML_ElementReadData(Element,Input,NULL,0,SCOPE_PARTIAL_DATA,0)==ERR_NONE)
        {
            const uint8_t *Data = EBML_BinaryGetData((ebml_binary*)Element);
            if (EBML_ElementDataSize(Element, 1) != 0)
            {
                if (Data==NULL)
                    fprintf(stdout,"[data too large] (%"PRId64")",EBML_ElementDataSize(Element, 1));
                else if (EBML_ElementDataSize(Element, 1) == 1)
                    fprintf(stdout,"%02X (%"PRId64")",Data[0],EBML_ElementDataSize(Element, 1));
                else if (EBML_ElementDataSize(Element, 1) == 2)
                    fprintf(stdout,"%02X %02X (%"PRId64")",Data[0],Data[1],EBML_ElementDataSize(Element, 1));
                else if (EBML_ElementDataSize(Element, 1) == 3)
                    fprintf(stdout,"%02X %02X %02X (%"PRId64")",Data[0],Data[1],Data[2],EBML_ElementDataSize(Element, 1));
                else if (EBML_ElementDataSize(Element, 1) == 4)
                    fprintf(stdout,"%02X %02X %02X %02X (%"PRId64")",Data[0],Data[1],Data[2],Data[3],EBML_ElementDataSize(Element, 1));
                else
                    fprintf(stdout,"%02X %02X %02X %02X.. (%"PRId64")",Data[0],Data[1],Data[2],Data[3],EBML_ElementDataSize(Element, 1));
            }
        }
        else
            fprintf(stdout,"<error reading>");
		EBML_ElementSkipData(Element, Input, Context, NULL, 0);
        EndLine(Element);
    }
    else if (Node_IsPartOf(Element,EBML_VOID_CLASS))
    {
        fprintf(stdout,"[%"PRId64" bytes]",EBML_ElementDataSize(Element, 1));
        EBML_ElementSkipData(Element, Input, Context, NULL, 0);
        EndLine(Element);
	}
    else if (Node_IsPartOf(Element,EBML_CRC_CLASS))
    {
        // TODO: handle crc32
        fprintf(stdout,"[%"PRId64" bytes]",EBML_ElementDataSize(Element, 1));
        EBML_ElementSkipData(Element, Input, Context, NULL, 0);
        EndLine(Element);
	}
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
        fprintf(stdout,"%X>",EBML_ElementClassID(Element));
#endif
        EBML_ElementSkipData(Element, Input, Context, NULL, 0);
        EndLine(Element);
    }
    return NULL;
}

static void OutputTree(stream *Input)
{
    ebml_element *Element = EBML_ElementCreate(Input,MATROSKA_getContextStream(),0,NULL);
    if (Element)
    {
		int Level = -1;
        EBML_ElementSetInfiniteSize(Element,1);
        OutputElement(Element, NULL, Input, &Level);
        NodeDelete((node*)Element);
    }
}

int main(int argc, const char *argv[])
{
    parsercontext p;
    stream *Input;
    tchar_t Path[MAXPATHFULL];

    if ((argc!=2 && argc!=3) || (argc==3 && strcmp(argv[1],"--pos")))
    {
        fprintf(stderr, "Usage: mkvtree --pos [matroska_file]\r\n");
		fprintf(stderr, "Options:\r\n");
		fprintf(stderr, "  --pos     output the position of elements\r\n");
        return 1;
    }

    if (argc==3)
        ShowPos = 1;

    // Core-C init phase
    ParserContext_Init(&p,NULL,NULL,NULL);
    // EBML & Matroska Init
    MATROSKA_Init(&p);

    // open the file to parse
    Node_FromStr(&p,Path,TSIZEOF(Path),argv[argc-1]);
    Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    if (Input == NULL)
        fprintf(stderr, "error: mkvtree cannot open file \"%s\"\r\n",argv[1]);
    else
    {
        OutputTree(Input);

        StreamClose(Input);
    }

    // EBML & Matroska ending
    MATROSKA_Done(&p);
    // Core-C ending
    ParserContext_Done(&p);

    return 0;
}
