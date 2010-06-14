/*
 * $Id$
 * Copyright (c) 2008, Matroska Foundation
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

#include "MatroskaParser.h"

#if defined(TARGET_WIN)
#define snprintf _snprintf
#endif

#define HAALI_STREAM_CLASS FOURCC('H','A','L','S')
typedef struct haali_stream
{
	stream Base;
	InputStream *io;

} haali_stream;

struct MatroskaFile
{
	haali_stream *Input;
	ebml_element *Segment;
	ebml_element *SegmentInfo;
	SegmentInfo Seg;

	filepos_t pSegmentInfo;
	filepos_t pTracks;
	filepos_t pCues;
	filepos_t pAttachments;
	filepos_t pChapters;
	filepos_t pTags;
};

void mkv_SetTrackMask(MatroskaFile *File, int Mask)
{
	assert(0); // not supported yet
}

void mkv_GetTags(MatroskaFile *File, Tag **pTags, unsigned *Count)
{
	assert(0); // not supported yet
}

void mkv_GetAttachments(MatroskaFile *File, Attachment **pAttachements, unsigned *Count)
{
	assert(0); // not supported yet
}

void mkv_GetChapters(MatroskaFile *File, Chapter **pChapters, unsigned *Count)
{
	assert(0); // not supported yet
}

TrackInfo *mkv_GetTrackInfo(MatroskaFile *File, size_t n)
{
	assert(0); // not supported yet
	return NULL;
}

SegmentInfo *mkv_GetFileInfo(MatroskaFile *File)
{
	assert(0); // not supported yet
	return NULL;
}

size_t mkv_GetNumTracks(MatroskaFile *File)
{
	assert(0); // not supported yet
	return 0;
}

static bool_t CheckMatroskaHead(ebml_element *Head, char *err_msg, size_t err_msgSize)
{
	tchar_t DocType[MAXPATH];
	ebml_element *Elt;
	Elt = EBML_MasterFindFirstElt(Head,&EBML_ContextReadVersion,1,1);
	if (!Elt)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return 0;
	}
	if (EBML_IntegerValue(Elt) > EBML_MAX_VERSION)
	{
		snprintf(err_msg,err_msgSize,"File requires version %d EBML parser",(int)EBML_IntegerValue(Elt));
		return 0;
	}

	Elt = EBML_MasterFindFirstElt(Head,&EBML_ContextDocTypeReadVersion,1,1);
	if (!Elt)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return 0;
	}
	if (EBML_IntegerValue(Elt) > MATROSKA_VERSION)
	{
		snprintf(err_msg,err_msgSize,"File requires version %d Matroska parser",(int)EBML_IntegerValue(Elt));
		return 0;
	}

	Elt = EBML_MasterFindFirstElt(Head,&EBML_ContextMaxIdLength,1,1);
	if (!Elt)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return 0;
	}
	if (EBML_IntegerValue(Elt) > EBML_MAX_ID)
	{
		snprintf(err_msg,err_msgSize,"File has identifiers longer than %d",(int)EBML_IntegerValue(Elt));
		return 0;
	}

	Elt = EBML_MasterFindFirstElt(Head,&EBML_ContextMaxSizeLength,1,1);
	if (!Elt)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return 0;
	}
	if (EBML_IntegerValue(Elt) > EBML_MAX_SIZE)
	{
		snprintf(err_msg,err_msgSize,"File has integers longer than %d",(int)EBML_IntegerValue(Elt));
		return 0;
	}

	Elt = EBML_MasterFindFirstElt(Head,&EBML_ContextDocType,1,1);
	if (!Elt)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return 0;
	}
	EBML_StringGet((ebml_string*)Elt,DocType,TSIZEOF(DocType));
	if (!tcsisame_ascii(DocType,T("matroska")) && !tcsisame_ascii(DocType,T("webm")))
	{
		snprintf(err_msg,err_msgSize,"Unsupported DocType: %s",((ebml_string*)Elt)->Buffer);
		return 0;
	}
	return 1;
}

static bool_t parseSeekHead(ebml_element *SeekHead, MatroskaFile *File, char *err_msg, size_t err_msgSize)
{
	ebml_parser_context RContext;
	ebml_element *Elt,*EltId;
	fourcc_t EltID;
	filepos_t SegStart = EBML_ElementPositionData(File->Segment);
	assert(SegStart!=INVALID_FILEPOS_T);

	RContext.Context = SeekHead->Context;
	if (EBML_ElementIsFiniteSize(SeekHead))
		RContext.EndPosition = EBML_ElementPositionEnd(SeekHead);
	else
		RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = NULL;
	if (EBML_ElementReadData(SeekHead,(stream*)File->Input,&RContext,1,SCOPE_ALL_DATA)!=ERR_NONE)
	{
		strncpy(err_msg,"Failed to read the EBML head",err_msgSize);
		return 0;
	}

	Elt = EBML_MasterFindFirstElt(SeekHead,&MATROSKA_ContextSeek,0,0);
	while (Elt)
	{
		EltId = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextSeekId,0,0);
		if (EltId && EltId->DataSize > EBML_MAX_ID)
		{
			snprintf(err_msg,err_msgSize,"Invalid ID size in parseSeekEntry: %d",(int)EltId->DataSize);
			return 0;
		}
		EltID = MATROSKA_MetaSeekID((matroska_seekpoint *)Elt);
		if (EltID == MATROSKA_ContextSegmentInfo.Id)
			File->pSegmentInfo = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		else if (EltID == MATROSKA_ContextTracks.Id)
			File->pTracks = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		else if (EltID == MATROSKA_ContextCues.Id)
			File->pCues = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		else if (EltID == MATROSKA_ContextAttachments.Id)
			File->pAttachments = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		else if (EltID == MATROSKA_ContextChapters.Id)
			File->pChapters = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		else if (EltID == MATROSKA_ContextTags.Id)
			File->pTags = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		Elt = EBML_MasterFindNextElt(SeekHead,Elt,0,0);
	}

	return 1;
}

static bool_t parseSegmentInfo(ebml_element *SegmentInfo, MatroskaFile *File, char *err_msg, size_t err_msgSize)
{
	ebml_parser_context RContext;
	ebml_element *Elt;

	RContext.Context = SegmentInfo->Context;
	if (EBML_ElementIsFiniteSize(SegmentInfo))
		RContext.EndPosition = EBML_ElementPositionEnd(SegmentInfo);
	else
		RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = NULL;
	if (EBML_ElementReadData(SegmentInfo,(stream*)File->Input,&RContext,1,SCOPE_ALL_DATA)!=ERR_NONE)
	{
		strncpy(err_msg,"Failed to read the Segment Info",err_msgSize);
		File->pSegmentInfo = INVALID_FILEPOS_T;
		return 0;
	}
	File->pSegmentInfo = SegmentInfo->ElementPosition;
	File->SegmentInfo = SegmentInfo;
	File->Seg.TimecodeScale = MATROSKA_ContextTimecodeScale.DefaultValue;

	for (Elt = EBML_MasterChildren(SegmentInfo);Elt;Elt = EBML_MasterNext(Elt))
	{
		if (Elt->Context->Id == MATROSKA_ContextTimecodeScale.Id)
		{
			File->Seg.TimecodeScale = EBML_IntegerValue(Elt);
			if (File->Seg.TimecodeScale==0)
			{
				strncpy(err_msg,"Segment timecode scale is zero",err_msgSize);
				return 0;
			}
		}
		else if (Elt->Context->Id == MATROSKA_ContextSegmentUid.Id)
		{
			if (Elt->DataSize!=16)
			{
				snprintf(err_msg,err_msgSize,"SegmentUID size is not %d bytes",(int)Elt->DataSize);
				return 0;
			}
			memcpy(File->Seg.UID,EBML_BinaryGetData((ebml_binary*)Elt),sizeof(File->Seg.UID));
		}
		else if (Elt->Context->Id == MATROSKA_ContextPrevUid.Id)
		{
			if (Elt->DataSize!=16)
			{
				snprintf(err_msg,err_msgSize,"PrevUID size is not %d bytes",(int)Elt->DataSize);
				return 0;
			}
			memcpy(File->Seg.PrevUID,EBML_BinaryGetData((ebml_binary*)Elt),sizeof(File->Seg.PrevUID));
		}
		else if (Elt->Context->Id == MATROSKA_ContextNextUid.Id)
		{
			if (Elt->DataSize!=16)
			{
				snprintf(err_msg,err_msgSize,"NextUID size is not %d bytes",(int)Elt->DataSize);
				return 0;
			}
			memcpy(File->Seg.NextUID,EBML_BinaryGetData((ebml_binary*)Elt),sizeof(File->Seg.NextUID));
		}
		else if (Elt->Context->Id == MATROSKA_ContextSegmentFilename.Id)
		{
			File->Seg.Filename = malloc((size_t)(Elt->DataSize+1));
			strcpy(File->Seg.Filename,((ebml_string*)Elt)->Buffer);
		}
		else if (Elt->Context->Id == MATROSKA_ContextPrevFilename.Id)
		{
			File->Seg.PrevFilename = malloc((size_t)(Elt->DataSize+1));
			strcpy(File->Seg.Filename,((ebml_string*)Elt)->Buffer);
		}
		else if (Elt->Context->Id == MATROSKA_ContextNextFilename.Id)
		{
			File->Seg.NextFilename = malloc((size_t)(Elt->DataSize+1));
			strcpy(File->Seg.Filename,((ebml_string*)Elt)->Buffer);
		}
	}

	return 1;
}

MatroskaFile *mkv_Open(InputStream *io, char *err_msg, size_t err_msgSize)
{
    ebml_parser_context RContext,L1Context;
	int UpperLevel;
	ebml_element *Head;

	MatroskaFile *File = io->memalloc(io,sizeof(*File));
	if (!File)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return NULL;
	}

	File->pSegmentInfo = INVALID_FILEPOS_T;
	File->pTracks = INVALID_FILEPOS_T;
	File->pCues = INVALID_FILEPOS_T;
	File->pAttachments = INVALID_FILEPOS_T;
	File->pChapters = INVALID_FILEPOS_T;
	File->pTags = INVALID_FILEPOS_T;

	memset(File,0,sizeof(*File));

	io->progress(io,0,0);
	io->ioseek(io,0,SEEK_SET);

	// find a segment
	File->Input = (haali_stream*)NodeCreate(io->AnyNode,HAALI_STREAM_CLASS);
	if (!File->Input)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return NULL;
	}
	File->Input->io = io;

    RContext.Context = &MATROSKA_ContextStream;
    RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = NULL;
	UpperLevel = 0;
	Head = EBML_FindNextElement((stream*)File->Input,&RContext,&UpperLevel,0);
	if (!Head)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return NULL;
	}

	if (Head->Context->Id == MATROSKA_ContextSegment.Id)
	{
		strncpy(err_msg,"First element in file is not EBML",err_msgSize);
		return NULL;
	}

	if (EBML_ElementReadData(Head,(stream*)File->Input,&RContext,1,SCOPE_ALL_DATA)!=ERR_NONE)
	{
		strncpy(err_msg,"Failed to read the EBML head",err_msgSize);
		NodeDelete((node*)Head);
		return NULL;
	}
	if (!CheckMatroskaHead(Head,err_msg,err_msgSize))
	{
		NodeDelete((node*)Head);
		return NULL;
	}
	NodeDelete((node*)Head);

	File->Segment = EBML_FindNextElement((stream*)File->Input,&RContext,&UpperLevel,0);
	if (!File->Segment)
	{
		strncpy(err_msg,"No segments found in the file",err_msgSize);
		return NULL;
	}

	// we want to read data until we find a seekhead or a trackinfo
    L1Context.Context = File->Segment->Context;
	if (EBML_ElementIsFiniteSize(File->Segment))
		L1Context.EndPosition = EBML_ElementPositionEnd(File->Segment);
	else
		L1Context.EndPosition = INVALID_FILEPOS_T;
    L1Context.UpContext = &RContext;
	UpperLevel = 0;
	Head = EBML_FindNextElement((stream*)File->Input,&L1Context,&UpperLevel,0);
	while (Head && (!EBML_ElementIsFiniteSize(File->Segment) || EBML_ElementPositionEnd(File->Segment) >= EBML_ElementPositionEnd(Head)))
	{
		if (Head->Context->Id == MATROSKA_ContextSeekHead.Id)
		{
			parseSeekHead(Head, File, err_msg, err_msgSize);
			NodeDelete((node*)Head);
		}
		else if (Head->Context->Id == MATROSKA_ContextSegmentInfo.Id)
			parseSegmentInfo(Head, File, err_msg, err_msgSize);
		else NodeDelete((node*)Head);
		Head = EBML_FindNextElement((stream*)File->Input,&L1Context,&UpperLevel,0);
	}

	return File;
}

void mkv_Close(MatroskaFile *File)
{
	if (File->Input)
		NodeDelete((node*)File->Input);
	if (File->SegmentInfo)
		NodeDelete((node*)File->SegmentInfo);
	if (File->Segment)
		NodeDelete((node*)File->Segment);
	assert(0); // not supported yet
}

size_t mkv_ReadFrame(MatroskaFile *File, int mask, unsigned int *track, ulonglong *StartTime, ulonglong *EndTime, ulonglong *FilePos, unsigned int *FrameSize,
                void** FrameRef, unsigned int *FrameFlags)
{
	assert(0); // not supported yet
	return 0;
}

void mkv_Seek(MatroskaFile *File, timecode_t timecode, int flags)
{
	assert(0); // not supported yet
}

int mkv_TruncFloat(float f)
{
	assert(0); // not supported yet
	return 0;
}

static filepos_t Seek(haali_stream *p ,filepos_t Pos,int SeekMode)
{
	p->io->ioseek(p->io,Pos,SeekMode);
	return p->io->iotell(p->io);
}

static err_t Read(haali_stream* p,void* Data,size_t Size,size_t* pReaded)
{
	size_t Readed;
	if (!pReaded)
		pReaded = &Readed;
	*pReaded = p->io->ioread(p->io,Data,Size);
	return ERR_NONE;
}

META_START(HaaliStream_Class,HAALI_STREAM_CLASS)
META_CLASS(SIZE,sizeof(haali_stream))
META_VMT(TYPE_FUNC,stream_vmt,Seek,Seek)
META_VMT(TYPE_FUNC,stream_vmt,Read,Read)
META_END(STREAM_CLASS)
