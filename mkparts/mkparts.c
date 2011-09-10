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
#include "mkparts_stdafx.h"
#include "mkparts_project.h"
#ifndef CONFIG_EBML_UNICODE
#define CONFIG_EBML_UNICODE
#endif
#include "matroska/matroska.h"
#include "matroska/matroska_sem.h"

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
#if defined(TARGET_WINCE)
    {
        tchar_t DocPath[MAXPATH];
        char LogPath[MAXPATH];
        charconv *ToStr = CharConvOpen(NULL,CHARSET_DEFAULT);
        GetDocumentPath(NULL,DocPath,TSIZEOF(DocPath),FTYPE_LOG); // more visible via ActiveSync
        if (!DocPath[0])
            tcscpy_s(DocPath,TSIZEOF(DocPath),T("\\My Documents"));
        if (!PathIsFolder(NULL,DocPath))
            FolderCreate(NULL,DocPath);
        tcscat_s(DocPath,TSIZEOF(DocPath),T("\\corelog.txt"));
        CharConvST(ToStr,LogPath,sizeof(LogPath),DocPath);
        CharConvClose(ToStr);
        f=fopen(LogPath,"a+b");
        if (!f)
            f=fopen("\\corelog.txt","a+b");
    }
#else
        f=fopen("\\corelog.txt","a+b");
#endif
    for (i=0;s[i];++i)
        s8[i]=(char)s[i];
    s8[i]=0;
    fputs(s8,f);
    fflush(f);
}
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

static void CopyTo(stream *Input, stream *Output, filepos_t StartPos, filepos_t Size)
{
    filepos_t SizeToCopy = Size;
    size_t readSize;
    err_t Err;

    Stream_Seek(Input, StartPos, SEEK_SET);
    while (SizeToCopy)
    {
        Err = Stream_ReadOneOrMore(Input, Data, min(SizeToCopy, sizeof(Data)), &readSize);
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
    stream *Input = NULL, *Output = NULL;
    tchar_t Path[MAXPATHFULL];
    ebml_master *EbmlHead = NULL;
    ebml_parser_context RContext;
    int i,UpperElement;
    array SegmentStarts;

    // Core-C init phase
    ParserContext_Init(&p,NULL,NULL,NULL);
	StdAfx_Init((nodemodule*)&p);
    ProjectSettings((nodecontext*)&p);

    // EBML & Matroska Init
    MATROSKA_Init((nodecontext*)&p);

    ArrayInit(&SegmentStarts);

    StdErr = &_StdErr;
    memset(StdErr,0,sizeof(_StdErr));
    StdErr->Stream = (stream*)NodeSingleton(&p,STDERR_ID);
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
        TextWrite(StdErr,T("mkparts v") PROJECT_VERSION T(", Copyright (c) 2011 Matroska Foundation\r\n"));
        if (argc < 2 || ShowUsage)
        {
            Result = OutputError(1,T("Usage: mkparts [options] <matroska_src>"));
		    TextWrite(StdErr,T("Options:\r\n"));
		    TextWrite(StdErr,T("  --split     split concatenated segments into different files\r\n"));
            TextWrite(StdErr,T("  --quiet     don't ouput progress and file info\r\n"));
            TextWrite(StdErr,T("  --version   show the version of mkparts\r\n"));
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
    RContext.Context = &MATROSKA_ContextStream;
    RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = NULL;
    RContext.Profile = 0;

    EbmlHead = (ebml_master*)EBML_FindNextElement(Input, &RContext, &UpperElement, 0);
    while (EbmlHead!=NULL)
    {
        // parse the list of EBML header+segment start points in memory
        ebml_master *Segment = NULL;

        if (EL_Type(EbmlHead, &MATROSKA_ContextSegment))
        {
            Result |= OutputWarning(3,T("Matroska Segment at %") TPRId64 T(" in %s without an EBML header"),EbmlHead->Base.ElementPosition, Path);
            // TODO: if there is an existing EBML head before in the file, use it
            //       otherwise create an EBML head with best guess
        }
        else if (!EL_Type(EbmlHead, &EBML_ContextHead))
        {
            Result |= OutputWarning(3,T("Unknown element %x at %") TPRId64 T(" in %s"), EbmlHead->Base.Context->Id, EbmlHead->Base.ElementPosition, Path);
            // TODO: if there is an existing EBML head before in the file, use it
            //       otherwise create an EBML head with best guess
        }

        if (!Quiet) TextWrite(StdErr,T("."));

        do {
            Segment = (ebml_master*)EBML_FindNextElement(Input, &RContext, &UpperElement, 0);
            if (EL_Type(Segment, &EBML_ContextHead))
            {
                Result |= OutputWarning(3,T("Found a new EBML header at %") TPRId64 T(" instead of a segment after EBML Header at %") TPRId64 T(" in %s"), Segment->Base.ElementPosition, EbmlHead->Base.ElementPosition, Path);
                EbmlHead = Segment;
            }
        } while (Segment!=NULL && !EL_Type(Segment, &MATROSKA_ContextSegment));

        if (Segment!=NULL && EL_Type(Segment, &MATROSKA_ContextSegment) && EbmlHead!=NULL && EL_Type(EbmlHead, &EBML_ContextHead))
        {
            SegmentStart currSegment;
            currSegment.EbmlHeadPos = EbmlHead->Base.ElementPosition;
            currSegment.SegmentPos = Segment->Base.ElementPosition;

            if (EBML_ElementIsFiniteSize(EbmlHead))
                currSegment.EbmlSize = EBML_ElementFullSize(EbmlHead, 1);
            else
                currSegment.EbmlSize = Segment->Base.ElementPosition - EbmlHead->Base.ElementPosition; // TODO: we should write the proper size in the header if there is enough room
            NodeDelete(EbmlHead);

            EbmlHead = EBML_ElementSkipData(Segment, Input, &RContext, NULL, 1);

            if (EBML_ElementIsFiniteSize(Segment))
                currSegment.SegmentSize = EBML_ElementFullSize(Segment, 1);
            else { // TODO: we should write the proper size in the segment if there is enough room
                if (EbmlHead!=NULL)
                    currSegment.SegmentSize = EbmlHead->Base.ElementPosition - Segment->Base.ElementPosition;
                else
                    currSegment.SegmentSize = Stream_Seek(Input,0,SEEK_CUR);
            }
            NodeDelete(Segment);

            ArrayAppend(&SegmentStarts, &currSegment, sizeof(currSegment), 0);
        }

        if (EbmlHead==NULL)
            EbmlHead = (ebml_master*)EBML_FindNextElement(Input, &RContext, &UpperElement, 0);
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
        for (i=0; i<ARRAYCOUNT(SegmentStarts, SegmentStart); ++i)
        {
            SegmentStart *seg = ARRAYBEGIN(SegmentStarts, SegmentStart)+i;
            if (!Quiet) TextPrintf(StdErr,T("write segment %d from %s %") TPRId64 T(" size %") TPRId64 T(" in %s\r\n"), i, Path, seg->EbmlHeadPos, seg->EbmlSize+seg->SegmentSize, Path);

            SplitPath(Path,Original,TSIZEOF(Original),String,TSIZEOF(String),Ext,TSIZEOF(Ext));
            if (!Original[0])
                OutPath[0] = 0;
            else
            {
                tcscpy_s(OutPath,TSIZEOF(OutPath),Original);
                AddPathDelimiter(OutPath,TSIZEOF(OutPath));
            }
            if (Ext[0])
                stcatprintf_s(OutPath,TSIZEOF(OutPath),T("%s.%d.%s"),String,i,Ext);
            else
                stcatprintf_s(OutPath,TSIZEOF(OutPath),T("%d.%s"),i,String);

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

    // EBML & Matroska ending
    MATROSKA_Done((nodecontext*)&p);

    // Core-C ending
	StdAfx_Done((nodemodule*)&p);
    ParserContext_Done(&p);

    return Result;
}
