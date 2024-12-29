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
#include "matroska2/matroska.h"
#include "matroska2/matroska_sem.h"
#include "mkparts_project.h"
#include <corec/helpers/file/streams.h>

static textwriter *StdErr = NULL;
static bool_t Split = 1;
static bool_t Quiet = 0;
static uint8_t Data[8*1024];

// some macros for code readability
#define EL_Pos(elt)         EBML_ElementPosition((const ebml_element*)elt)
#define EL_Int(elt)         EBML_IntegerValue((const ebml_integer*)elt)
#define EL_Type(elt, type)  EBML_ElementIsType((const ebml_element*)elt, type)
#define EL_DataSize(elt)    EBML_ElementDataSize((const ebml_element*)elt, 1)

typedef struct track_info
{
    int Num;
    int Kind;
    ebml_string *CodecID;
    filepos_t DataLength;

} track_info;

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
#else
#include <stdio.h>
void DebugMessage(const tchar_t* Msg,...)
{
    va_list Args;
    tchar_t Buffer[1024];

    va_start(Args,Msg);
    vstprintf_s(Buffer,TSIZEOF(Buffer), Msg, Args);
    va_end(Args);
    tcscat_s(Buffer,TSIZEOF(Buffer),T("\r\n"));

#ifdef UNICODE
    fprintf(stderr, "%ls", Buffer);
#else
    fprintf(stderr, "%s", Buffer);
#endif
}
#endif

static int OutputError(int ErrCode, const tchar_t *ErrString, ...)
{
	tchar_t Buffer[MAXLINE];
	va_list Args;
	va_start(Args,ErrString);
	vstprintf_s(Buffer,TSIZEOF(Buffer), ErrString, Args);
	va_end(Args);
	TextPrintf(StdErr,T("\rERR%03X: %s\r\n"),ErrCode,Buffer);
	return -ErrCode;
}

static int OutputWarning(int ErrCode, const tchar_t *ErrString, ...)
{
    tchar_t Buffer[MAXLINE];
    va_list Args;
    va_start(Args,ErrString);
    vstprintf_s(Buffer,TSIZEOF(Buffer), ErrString, Args);
    va_end(Args);
    TextPrintf(StdErr,T("\rWRN%03X: %s\r\n"),ErrCode,Buffer);
    return -ErrCode;
}

typedef struct {
    filepos_t EbmlHeadPos;
    filepos_t EbmlSize;
    filepos_t SegmentPos;
    filepos_t SegmentSize;
} SegmentStart;

static void CopyTo(struct stream *Input, struct stream *Output, filepos_t StartPos, filepos_t Size)
{
    filepos_t SizeToCopy = Size;
    size_t readSize;
    err_t Err;

    Stream_Seek(Input, StartPos, SEEK_SET);
    while (SizeToCopy)
    {
        Err = Stream_ReadOneOrMore(Input, Data, min(SizeToCopy, (filepos_t)sizeof(Data)), &readSize);
        if (Err==ERR_NONE || Err == ERR_NEED_MORE_DATA)
        {
            if (!Quiet) TextWrite(StdErr,T("."));
            Stream_Write(Output, Data, readSize, NULL);
            SizeToCopy -= readSize;
        }
    }
    if (!Quiet) TextWrite(StdErr,T("\r\n"));
}

int main(int argc, const char *argv[])
{
    int Result = 0;
    int ShowUsage = 0;
    int ShowVersion = 0;
    parsercontext p;
    textwriter _StdErr;
    struct stream *Input = NULL, *Output = NULL;
    tchar_t Path[MAXPATHFULL];
    ebml_element *EbmlHead = NULL;
    ebml_parser_context RContext;
    int i,UpperElement;
    array SegmentStarts;

    // Core-C init phase
    ParserContext_Init(&p,NULL,NULL,NULL);
    Node_SetData(&p.Base.Base.Base,NODECONTEXT_PROJECT_VENDOR,TYPE_STRING,"Matroska");
    Node_SetData(&p.Base.Base.Base,NODECONTEXT_PROJECT_VERSION,TYPE_STRING,PROJECT_VERSION);
    Node_SetData(&p.Base.Base.Base,NODECONTEXT_PROJECT_NAME,TYPE_STRING,PROJECT_NAME);

    // EBML & Matroska Init
    MATROSKA_Init(&p);

    ArrayInit(&SegmentStarts);

    StdErr = &_StdErr;
    memset(StdErr,0,sizeof(_StdErr));
    StdErr->Stream = (struct stream*)NodeSingleton(&p,STDERR_ID);
    assert(StdErr->Stream!=NULL);

	for (i=1;i<argc;++i)
	{
	    Node_FromStr(&p,Path,TSIZEOF(Path),argv[i]);
		if (tcsisame_ascii(Path,T("--split"))) Split = 1;
		else if (tcsisame_ascii(Path,T("--version"))) ShowVersion = 1;
		else if (tcsisame_ascii(Path,T("--quiet"))) Quiet = 1;
        else if (tcsisame_ascii(Path,T("--help"))) {ShowVersion = 1; ShowUsage = 1;}
		else if (i<argc-1) TextPrintf(StdErr,T("Unknown parameter '%s'\r\n"),Path);
	}

    if (argc < 2 || ShowVersion)
    {
        TextWrite(StdErr,PROJECT_NAME T(" v") PROJECT_VERSION T(", Copyright (c) 2011-2020 Matroska Foundation\r\n"));
        if (argc < 2 || ShowUsage)
        {
            Result = OutputError(1,T("Usage: ") PROJECT_NAME T(" [options] <matroska_src>"));
		    TextWrite(StdErr,T("Options:\r\n"));
		    TextWrite(StdErr,T("  --split     split concatenated segments into different files\r\n"));
            TextWrite(StdErr,T("  --quiet     don't ouput progress and file info\r\n"));
            TextWrite(StdErr,T("  --version   show the version of ") PROJECT_NAME T("\r\n"));
            TextWrite(StdErr,T("  --help      show this screen\r\n"));
        }
        goto exit;
    }

    Node_FromStr(&p,Path,TSIZEOF(Path),argv[argc-1]);
    Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    if (!Input)
    {
        TextPrintf(StdErr,T("Could not open file \"%s\" for reading\r\n"),Path);
        Result = -2;
        goto exit;
    }

    if (!Split)
    {
        TextPrintf(StdErr,T("Only the --split option is supported for the moment\r\n"));
        Result = -3;
        goto exit;
    }

    // parse the source file to determine if it's a Matroska file and determine the location of the key parts
    RContext.Context = MATROSKA_getContextStream();
    RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = NULL;
    RContext.Profile = EBML_ANY_PROFILE;

    EbmlHead = EBML_FindNextElement(Input, &RContext, &UpperElement, 0);
    while (EbmlHead!=NULL)
    {
        // parse the list of EBML header+segment start points in memory
        ebml_element *Segment = NULL;

        if (EL_Type(EbmlHead, MATROSKA_getContextSegment()))
        {
            Result |= OutputWarning(3,T("Matroska Segment at %") TPRId64 T(" in %s without an EBML header"),EBML_ElementPosition(EbmlHead), Path);
            // TODO: if there is an existing EBML head before in the file, use it
            //       otherwise create an EBML head with best guess
        }
        else if (!EL_Type(EbmlHead, EBML_getContextHead()))
        {
            Result |= OutputWarning(3,T("Unknown element %x at %") TPRId64 T(" in %s"), EBML_ElementClassID(EbmlHead), EBML_ElementPosition(EbmlHead), Path);
            // TODO: if there is an existing EBML head before in the file, use it
            //       otherwise create an EBML head with best guess
        }

        if (!Quiet) TextWrite(StdErr,T("."));

        do {
            Segment = EBML_FindNextElement(Input, &RContext, &UpperElement, 0);
            if (EL_Type(Segment, EBML_getContextHead()))
            {
                Result |= OutputWarning(3,T("Found a new EBML header at %") TPRId64 T(" instead of a segment after EBML Header at %") TPRId64 T(" in %s"), EBML_ElementPosition(Segment), EBML_ElementPosition(EbmlHead), Path);
                EbmlHead = Segment;
            }
        } while (Segment!=NULL && !EL_Type(Segment, MATROSKA_getContextSegment()));

        if (Segment!=NULL && EL_Type(Segment, MATROSKA_getContextSegment()) && EbmlHead!=NULL && EL_Type(EbmlHead, EBML_getContextHead()))
        {
            SegmentStart currSegment;
            currSegment.EbmlHeadPos = EBML_ElementPosition(EbmlHead);
            currSegment.SegmentPos = EBML_ElementPosition(Segment);

            if (EBML_ElementIsFiniteSize(EbmlHead))
                currSegment.EbmlSize = EBML_ElementFullSize(EbmlHead, 1);
            else
                currSegment.EbmlSize = EBML_ElementPosition(Segment) - EBML_ElementPosition(EbmlHead); // TODO: we should write the proper size in the header if there is enough room
            NodeDelete((node*)EbmlHead);

            EbmlHead = EBML_ElementSkipData(Segment, Input, &RContext, NULL, 1);

            if (EBML_ElementIsFiniteSize(Segment))
                currSegment.SegmentSize = EBML_ElementFullSize(Segment, 1);
            else { // TODO: we should write the proper size in the segment if there is enough room
                if (EbmlHead!=NULL)
                    currSegment.SegmentSize = EBML_ElementPosition(EbmlHead) - EBML_ElementPosition(Segment);
                else
                    currSegment.SegmentSize = Stream_Seek(Input,0,SEEK_CUR);
            }
            NodeDelete((node*)Segment);

            ArrayAppend(&SegmentStarts, &currSegment, sizeof(currSegment), 0);
        }

        if (EbmlHead==NULL)
            EbmlHead = EBML_FindNextElement(Input, &RContext, &UpperElement, 0);
    }

    if (!Quiet) TextWrite(StdErr,T("\r\n"));

    if (ARRAYCOUNT(SegmentStarts, SegmentStart)==0)
        Result = OutputError(3,T("No segment found in %s"), Path);
    else if (ARRAYCOUNT(SegmentStarts, SegmentStart)==1)
        Result = OutputWarning(3,T("Only one segment found in %s, do nothing"), Path);
    else {
        tchar_t OutPath[MAXPATHFULL];
        tchar_t Original[MAXLINE], String[MAXLINE], Ext[MAXDATA];

        // TODO: copy each segment in a separate file
        for (size_t j=0; j<ARRAYCOUNT(SegmentStarts, SegmentStart); ++j)
        {
            SegmentStart *seg = ARRAYBEGIN(SegmentStarts, SegmentStart)+j;
            if (!Quiet) TextPrintf(StdErr,T("write segment %zu from %s %") TPRId64 T(" size %") TPRId64 T(" in %s\r\n"), j, Path, seg->EbmlHeadPos, seg->EbmlSize+seg->SegmentSize, Path);

            SplitPath(Path,Original,TSIZEOF(Original),String,TSIZEOF(String),Ext,TSIZEOF(Ext));
            if (!Original[0])
                OutPath[0] = 0;
            else
            {
                tcscpy_s(OutPath,TSIZEOF(OutPath),Original);
                AddPathDelimiter(OutPath,TSIZEOF(OutPath));
            }
            if (Ext[0])
                stcatprintf_s(OutPath,TSIZEOF(OutPath),T("%s.%zu.%s"),String,j,Ext);
            else
                stcatprintf_s(OutPath,TSIZEOF(OutPath),T("%zu.%s"),j,String);

            Output = StreamOpen(&p,OutPath,SFLAG_WRONLY|SFLAG_CREATE);
            if (!Output)
            {
                Result = OutputError(5,T("Could not open file \"%s\" for writing\r\n"),OutPath);
                goto exit;
            }

            CopyTo(Input, Output, seg->EbmlHeadPos, seg->EbmlSize);
            CopyTo(Input, Output, seg->SegmentPos, seg->SegmentSize);

            StreamClose(Output);
        }
    }

exit:
    ArrayClear(&SegmentStarts);
    if (Input)
        StreamClose(Input);
    if (Output)
        StreamClose(Output);

    // Core-C ending
    ParserContext_Done(&p);

    return Result;
}
