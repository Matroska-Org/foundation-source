/*
 * $Id$
 * Copyright (c) 2010, Matroska Foundation
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
#include "mkclean_stdafx.h"
#include "matroska/matroska.h"

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

static int DocVersion = 1;

static ebml_element *CheckMatroskaHead(const ebml_element *Head, const ebml_parser_context *Parser, stream *Input, textwriter *StdErr)
{
    ebml_parser_context SubContext;
    ebml_element *SubElement;
    int UpperElement;
    tchar_t String[MAXLINE];

    SubContext.UpContext = Parser;
    SubContext.Context = Head->Context;
    SubContext.EndPosition = EBML_ElementPositionEnd(Head);
    SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
    while (SubElement)
    {
        if (SubElement->Context->Id == EBML_ContextReadVersion.Id)
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (((ebml_integer*)SubElement)->Value > EBML_MAX_VERSION)
            {
                TextPrintf(StdErr,T("EBML Read version %ld not supported"),(long)((ebml_integer*)SubElement)->Value);
                break;
            }
            else
                DocVersion = (int)((ebml_integer*)SubElement)->Value;
        }
        else if (SubElement->Context->Id == EBML_ContextMaxIdLength.Id)
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (((ebml_integer*)SubElement)->Value > EBML_MAX_ID)
            {
                TextPrintf(StdErr,T("EBML Max ID Length %ld not supported"),(long)((ebml_integer*)SubElement)->Value);
                break;
            }
        }
        else if (SubElement->Context->Id == EBML_ContextMaxSizeLength.Id)
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (((ebml_integer*)SubElement)->Value > EBML_MAX_SIZE)
            {
                TextPrintf(StdErr,T("EBML Max Coded Size %ld not supported"),(long)((ebml_integer*)SubElement)->Value);
                break;
            }
        }
        else if (SubElement->Context->Id == EBML_ContextDocType.Id)
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else
            {
                Node_FromUTF8(Input,String,TSIZEOF(String),((ebml_string*)SubElement)->Buffer);
                if (tcscmp(String,T("matroska"))!=0)
                {
                    TextPrintf(StdErr,T("EBML DocType %s not supported"),(long)((ebml_integer*)SubElement)->Value);
                    break;
                }
            }
        }
        else if (SubElement->Context->Id == EBML_ContextDocTypeReadVersion.Id)
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (((ebml_integer*)SubElement)->Value > MATROSKA_VERSION)
            {
                TextPrintf(StdErr,T("EBML Read version %ld not supported"),(long)((ebml_integer*)SubElement)->Value);
                break;
            }
        }
        else if (SubElement->Context->Id == MATROSKA_ContextSegment.Id)
            return SubElement;
        NodeDelete((node*)SubElement);
        SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
    }

    return NULL;
}


int main(int argc, const char *argv[])
{
    int Result = 0;
    parsercontext p;
    textwriter StdErr;
    stream *Input = NULL,*Output = NULL;
    tchar_t Path[MAXPATHFULL];
    ebml_element *EbmlHead = NULL, *RSegment = NULL, *RLevel1 = NULL, **Cluster;
    ebml_element *RSegmentInfo = NULL, *RTrackInfo = NULL, *RChapters = NULL, *RTags = NULL, *RCues = NULL, *RAttachments = NULL;
    ebml_element *WSegment = NULL, *WMetaSeek = NULL, *WSeekPoint = NULL, *WSeekID = NULL;
    ebml_element *WSeekPosSegmentInfo = NULL, *WSeekPosTrackInfo = NULL, *WSeekPosChapters = NULL, *WSeekPosTags = NULL, *WSeekPosCues = NULL, *WSeekPosAttachments = NULL;
    array RClusters;
    ebml_parser_context RContext;
    ebml_parser_context RSegmentContext;
    int UpperElement;
	uint8_t IdBuffer[4]; // Class D ID
    size_t IdSize;
    filepos_t NextPos;

    // Core-C init phase
    ParserContext_Init(&p,NULL,NULL,NULL);
	StdAfx_Init((nodemodule*)&p);
    ProjectSettings((nodecontext*)&p);

    // EBML & Matroska Init
    MATROSKA_Init((nodecontext*)&p);

    ArrayInit(&RClusters);

    memset(&StdErr,0,sizeof(StdErr));
    StdErr.Stream = (stream*)NodeSingleton(&p,STDERR_ID);

    if (argc != 3)
    {
        TextWrite(&StdErr,T("Usage: mkclean [matroska_src] [matroska_dst]\r\n"));
        Result = -1;
        goto exit;
    }

    Node_FromStr(&p,Path,TSIZEOF(Path),argv[1]);
    Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    if (!Input)
    {
        TextPrintf(&StdErr,T("Could not open file \"%s\" for reading\r\n"),Path);
        Result = -2;
        goto exit;
    }

    Node_FromStr(&p,Path,TSIZEOF(Path),argv[2]);
    Output = StreamOpen(&p,Path,SFLAG_WRONLY|SFLAG_CREATE);
    if (!Output)
    {
        TextPrintf(&StdErr,T("Could not open file \"%s\" for writing\r\n"),Path);
        Result = -3;
        goto exit;
    }

    // parse the source file to determine if it's a Matroska file and determine the location of the key parts
    RContext.Context = &MATROSKA_ContextStream;
    RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = NULL;
    EbmlHead = EBML_FindNextElement(Input, &RContext, &UpperElement, 1);
    if (!EbmlHead)
    {
        TextPrintf(&StdErr,T("Could not find an EBML head!\r\n"),Path);
        Result = -4;
        goto exit;
    }

    RSegment = CheckMatroskaHead(EbmlHead,&RContext,Input,&StdErr);
    if (!RSegment)
    {
        Result = -5;
        goto exit;
    }

    // locate the Segment Info, Track Info, Chapters, Tags, Attachments, Cues Clusters*
    RSegmentContext.Context = &MATROSKA_ContextSegment;
    RSegmentContext.EndPosition = EBML_ElementPositionEnd(RSegment);
    RSegmentContext.UpContext = &RContext;
    RLevel1 = EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
    while (RLevel1)
    {
        EbmlHead = EBML_ElementSkipData(RLevel1, Input, &RSegmentContext, NULL, 1);
        assert(EbmlHead==NULL);
        if (RLevel1->Context->Id == MATROSKA_ContextSegmentInfo.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,0,SCOPE_ALL_DATA)==ERR_NONE)
                RSegmentInfo = RLevel1;
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextTracks.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,0,SCOPE_ALL_DATA)==ERR_NONE)
                RTrackInfo = RLevel1;
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextChapters.Id)
            RChapters = RLevel1;
        else if (RLevel1->Context->Id == MATROSKA_ContextTags.Id)
            RTags = RLevel1;
        else if (RLevel1->Context->Id == MATROSKA_ContextCues.Id)
            RCues = RLevel1;
        else if (RLevel1->Context->Id == MATROSKA_ContextAttachments.Id)
            RAttachments = RLevel1;
        else if (RLevel1->Context->Id == MATROSKA_ContextCluster.Id)
            ArrayAppend(&RClusters,&RLevel1,sizeof(RLevel1),256);
        else
            NodeDelete((node*)RLevel1);
        RLevel1 = EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
    }

    // Write the EBMLHead
    EbmlHead = EBML_ElementCreate(&p,&EBML_ContextHead,0,NULL);
    if (!EbmlHead)
        goto exit;
    // DocType
    RLevel1 = EBML_MasterGetChild(EbmlHead,&EBML_ContextDocType);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_STRING_CLASS));
    if (EBML_StringSetValue((ebml_string*)RLevel1,"matroska") != ERR_NONE)
        goto exit;

    // Doctype version
    RLevel1 = EBML_MasterGetChild(EbmlHead,&EBML_ContextDocTypeVersion);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_INTEGER_CLASS));
    ((ebml_integer*)RLevel1)->Value = DocVersion;

    // Doctype readable version
    RLevel1 = EBML_MasterGetChild(EbmlHead,&EBML_ContextDocTypeReadVersion);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_INTEGER_CLASS));
    ((ebml_integer*)RLevel1)->Value = DocVersion;

    if (EBML_ElementRender(EbmlHead,Output,1,0,0,NULL)!=ERR_NONE)
        goto exit;
    NodeDelete((node*)EbmlHead);
    EbmlHead = NULL;

    // Write the Matroska Segment Head
    WSegment = EBML_ElementCreate(&p,&MATROSKA_ContextSegment,0,NULL);
    WSegment->Size = RSegment->Size; // temporary value
    if (EBML_ElementRenderHead(WSegment,Output,0,NULL)!=ERR_NONE)
    {
        TextPrintf(&StdErr,T("Failed to write the (temporary) Segment head\r\n"),Path);
        Result = -10;
        goto exit;
    }

    //  Write the Meta Seek (TODO: add a higher level API to add elements directly)
    WMetaSeek = EBML_MasterAddElt(WSegment,&MATROSKA_ContextSeekHead,0);
    // segment info
    WSeekPoint = EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
    WSeekID = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekId,1,0);
    IdSize = EBML_FillBufferID(IdBuffer,sizeof(IdBuffer),MATROSKA_ContextSegmentInfo.Id);
    EBML_BinarySetData((ebml_binary*)WSeekID,IdBuffer,IdSize);
    WSeekPosSegmentInfo = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekPosition,1,0);
    ((ebml_integer*)WSeekPosSegmentInfo)->Value = 100; // dummy value
    WSeekPosSegmentInfo->bValueIsSet = 1;
    NextPos = ((ebml_integer*)WSeekPosSegmentInfo)->Value + RSegmentInfo->Size;
    // track info
    WSeekPoint = EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
    WSeekID = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekId,1,0);
    IdSize = EBML_FillBufferID(IdBuffer,sizeof(IdBuffer),MATROSKA_ContextTracks.Id);
    EBML_BinarySetData((ebml_binary*)WSeekID,IdBuffer,IdSize);
    WSeekPosTrackInfo = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekPosition,1,0);
    ((ebml_integer*)WSeekPosTrackInfo)->Value = NextPos; // dummy value
    WSeekPosTrackInfo->bValueIsSet = 1;
    NextPos += RTrackInfo->Size;
    // chapters
    if (RChapters)
    {
        WSeekPoint = EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
        WSeekID = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekId,1,0);
        IdSize = EBML_FillBufferID(IdBuffer,sizeof(IdBuffer),MATROSKA_ContextChapters.Id);
        EBML_BinarySetData((ebml_binary*)WSeekID,IdBuffer,IdSize);
        WSeekPosChapters = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekPosition,1,0);
        ((ebml_integer*)WSeekPosChapters)->Value = NextPos; // dummy value
        WSeekPosChapters->bValueIsSet = 1;
        NextPos += RChapters->Size;
    }
    // tags
    if (RTags)
    {
        WSeekPoint = EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
        WSeekID = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekId,1,0);
        IdSize = EBML_FillBufferID(IdBuffer,sizeof(IdBuffer),MATROSKA_ContextTags.Id);
        EBML_BinarySetData((ebml_binary*)WSeekID,IdBuffer,IdSize);
        WSeekPosTags = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekPosition,1,0);
        ((ebml_integer*)WSeekPosTags)->Value = NextPos; // dummy value
        WSeekPosTags->bValueIsSet = 1;
        NextPos += RTags->Size;
    }
    // cues
    WSeekPoint = EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
    WSeekID = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekId,1,0);
    IdSize = EBML_FillBufferID(IdBuffer,sizeof(IdBuffer),MATROSKA_ContextCues.Id);
    EBML_BinarySetData((ebml_binary*)WSeekID,IdBuffer,IdSize);
    WSeekPosCues = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekPosition,1,0);
    ((ebml_integer*)WSeekPosCues)->Value = NextPos; // dummy value
    WSeekPosCues->bValueIsSet = 1;
    if (RCues)
        NextPos += RCues->Size;
    else
        NextPos += 160*1024; // dummy value
    // attachements
    if (RAttachments)
    {
        WSeekPoint = EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
        WSeekID = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekId,1,0);
        IdSize = EBML_FillBufferID(IdBuffer,sizeof(IdBuffer),MATROSKA_ContextAttachments.Id);
        EBML_BinarySetData((ebml_binary*)WSeekID,IdBuffer,IdSize);
        WSeekPosAttachments = EBML_MasterFindFirstElt(WSeekPoint,&MATROSKA_ContextSeekPosition,1,0);
        ((ebml_integer*)WSeekPosAttachments)->Value = NextPos; // dummy value
        WSeekPosAttachments->bValueIsSet = 1;
        NextPos += RAttachments->Size;
    }

    // temporarily render the SeekHead
    if (EBML_ElementRender(WMetaSeek,Output,0,0,1,NULL)!=ERR_NONE)
    {
        TextPrintf(&StdErr,T("Failed to write the (temporary) Seek Head\r\n"),Path);
        Result = -10;
        goto exit;
    }


    //  Write the Segment Info
    if (EBML_ElementRender(RSegmentInfo,Output,0,0,1,NULL)!=ERR_NONE)
    {
        TextPrintf(&StdErr,T("Failed to write the Segment Info\r\n"),Path);
        Result = -11;
        goto exit;
    }
    ((ebml_integer*)WSeekPosSegmentInfo)->Value = RSegmentInfo->ElementPosition - EBML_ElementPositionData(WSegment);

    //  Write the Track Info
    if (EBML_ElementRender(RTrackInfo,Output,0,0,1,NULL)!=ERR_NONE)
    {
        TextPrintf(&StdErr,T("Failed to write the Segment Info\r\n"),Path);
        Result = -11;
        goto exit;
    }
    ((ebml_integer*)WSeekPosTrackInfo)->Value = RTrackInfo->ElementPosition - EBML_ElementPositionData(WSegment);

    //  Write the Chapters
    //  Write the Tags
    //  Write the Cues
    //  Write the Clusters*
    //  Write the Attachments
    // update the WSegment size

exit:
    if (WSegment)
        NodeDelete((node*)WSegment);

    for (Cluster = ARRAYBEGIN(RClusters,ebml_element*);Cluster != ARRAYEND(RClusters,ebml_element*); ++Cluster)
        NodeDelete((node*)*Cluster);
    ArrayClear(&RClusters);
    if (RAttachments)
        NodeDelete((node*)RAttachments);
    if (RTags)
        NodeDelete((node*)RTags);
    if (RCues)
        NodeDelete((node*)RCues);
    if (RChapters)
        NodeDelete((node*)RChapters);
    if (RTrackInfo)
        NodeDelete((node*)RTrackInfo);
    if (RSegmentInfo)
        NodeDelete((node*)RSegmentInfo);
    if (RLevel1)
        NodeDelete((node*)RLevel1);
    if (RSegment)
        NodeDelete((node*)RSegment);
    if (EbmlHead)
        NodeDelete((node*)EbmlHead);
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
