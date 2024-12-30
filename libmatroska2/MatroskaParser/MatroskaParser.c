/*
 * Copyright (c) 2008-2010, Matroska (non-profit organisation)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MatroskaParser.h"
#include "matroska2/matroska_sem.h"

#define MAX_TRACKS 32 // safety

#if defined(TARGET_WIN)
#define snprintf _snprintf
#endif

#ifndef EOF
#define EOF				(-1)
#endif


const nodemeta HaaliStream_Class[];

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
	ebml_element *TrackList;
	ebml_element *CueList;
	SegmentInfo Seg;

	ebml_parser_context L0Context;
	ebml_parser_context L1Context;

	ebml_element *CurrentCluster;
	matroska_block *CurrentBlock;
	size_t CurrentFrame;
	ebml_parser_context ClusterContext;

	filepos_t pSegmentInfo;
	filepos_t pTracks;
	filepos_t pCues;
	filepos_t pAttachments;
	filepos_t pChapters;
	filepos_t pTags;
	filepos_t pFirstCluster;

	int trackMask;
	int flags;

	array Tracks;
	array Tags;
	array Chapters;
	array Attachments;

	parsercontext p;
};

void mkv_SetTrackMask(MatroskaFile *File, int Mask)
{
	File->trackMask = Mask;
	// TODO: the original code is handling a queue
}

void mkv_GetTags(MatroskaFile *File, Tag **pTags, unsigned *Count)
{
	*pTags = ARRAYBEGIN(File->Tags,Tag);
	*Count = ARRAYCOUNT(File->Tags,Tag);
}

void mkv_GetAttachments(MatroskaFile *File, Attachment **pAttachements, unsigned *Count)
{
	*pAttachements = ARRAYBEGIN(File->Attachments,Attachment);
	*Count = ARRAYCOUNT(File->Attachments,Attachment);
}

void mkv_GetChapters(MatroskaFile *File, Chapter **pChapters, unsigned *Count)
{
	*pChapters = ARRAYBEGIN(File->Chapters,Chapter);
	*Count = ARRAYCOUNT(File->Chapters,Chapter);
}

TrackInfo *mkv_GetTrackInfo(MatroskaFile *File, size_t n)
{
	if (n>=ARRAYCOUNT(File->Tracks,TrackInfo))
		return NULL;
	return ARRAYBEGIN(File->Tracks,TrackInfo) + n;
}

SegmentInfo *mkv_GetFileInfo(MatroskaFile *File)
{
	return &File->Seg;
}

size_t mkv_GetNumTracks(MatroskaFile *File)
{
	return ARRAYCOUNT(File->Tracks,TrackInfo);
}

static bool_t CheckMatroskaHead(ebml_element *Head, char *err_msg, size_t err_msgSize)
{
	tchar_t DocType[MAXPATH];
	ebml_element *Elt;
	Elt = EBML_MasterFindFirstElt(Head,EBML_getContextReadVersion(),1,1);
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

	Elt = EBML_MasterFindFirstElt(Head,EBML_getContextDocTypeReadVersion(),1,1);
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

	Elt = EBML_MasterFindFirstElt(Head,EBML_getContextMaxIdLength(),1,1);
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

	Elt = EBML_MasterFindFirstElt(Head,EBML_getContextMaxSizeLength(),1,1);
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

	Elt = EBML_MasterFindFirstElt(Head,EBML_getContextDocType(),1,1);
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

	RContext.Context = EBMLElement_Context(SeekHead);
	if (EBML_ElementIsFiniteSize(SeekHead))
		RContext.EndPosition = EBML_ElementPositionEnd(SeekHead);
	else
		RContext.EndPosition = INVALID_FILEPOS_T;
	RContext.UpContext = &File->L1Context;
	if (EBML_ElementReadData(SeekHead,(struct stream*)File->Input,&RContext,1,SCOPE_ALL_DATA)!=ERR_NONE)
	{
		strncpy(err_msg,"Failed to read the EBML head",err_msgSize);
		return 0;
	}

	Elt = EBML_MasterFindFirstElt(SeekHead,MATROSKA_getContextSeek(),0,0);
	while (Elt)
	{
		EltId = EBML_MasterFindFirstElt(Elt,MATROSKA_getContextSeekId(),0,0);
		if (EltId && EBML_ElementDataSize(EltId, 1) > EBML_MAX_ID)
		{
			snprintf(err_msg,err_msgSize,"Invalid ID size in parseSeekEntry: %d",(int)EBML_ElementDataSize(EltId, 1));
			return 0;
		}
		EltID = MATROSKA_MetaSeekID((matroska_seekpoint *)Elt);
		if (EltID == MATROSKA_getContextSegmentInfo()->Id)
			File->pSegmentInfo = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		else if (EltID == MATROSKA_getContextTracks()->Id)
			File->pTracks = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		else if (EltID == MATROSKA_getContextCues()->Id)
			File->pCues = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		else if (EltID == MATROSKA_getContextAttachments()->Id)
			File->pAttachments = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		else if (EltID == MATROSKA_getContextChapters()->Id)
			File->pChapters = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		else if (EltID == MATROSKA_getContextTags()->Id)
			File->pTags = MATROSKA_MetaSeekPosInSegment((matroska_seekpoint *)Elt) + SegStart;
		Elt = EBML_MasterFindNextElt(SeekHead,Elt,0,0);
	}

	return 1;
}

static bool_t parseSegmentInfo(ebml_element *SegmentInfo, MatroskaFile *File, char *err_msg, size_t err_msgSize)
{
	ebml_parser_context RContext;
	ebml_element *Elt;
	double duration = -1.0;

	RContext.Context = EBMLElement_Context(SegmentInfo);
	if (EBML_ElementIsFiniteSize(SegmentInfo))
		RContext.EndPosition = EBML_ElementPositionEnd(SegmentInfo);
	else
		RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = &File->L1Context;
	if (EBML_ElementReadData(SegmentInfo,(struct stream*)File->Input,&RContext,1,SCOPE_ALL_DATA)!=ERR_NONE)
	{
		strncpy(err_msg,"Failed to read the Segment Info",err_msgSize);
		File->pSegmentInfo = INVALID_FILEPOS_T;
		return 0;
	}
	File->pSegmentInfo = EBML_ElementPosition(SegmentInfo);
	File->SegmentInfo = SegmentInfo;
	File->Seg.TimestampScale = MATROSKA_getContextTimestampScale()->DefaultValue;

	for (Elt = EBML_MasterChildren(SegmentInfo);Elt;Elt = EBML_MasterNext(Elt))
	{
		if (EBML_ElementClassID(Elt) == MATROSKA_getContextTimestampScale()->Id)
		{
			File->Seg.TimestampScale = EBML_IntegerValue(Elt);
			if (File->Seg.TimestampScale==0)
			{
				strncpy(err_msg,"Segment timestamp scale is zero",err_msgSize);
				return 0;
			}
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextDuration()->Id)
		{
			duration = ((ebml_float*)Elt)->Value;
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextSegmentDate()->Id)
		{
			File->Seg.DateUTC = EBML_DateTime((ebml_date*)Elt);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextSegmentTitle()->Id)
		{
			File->Seg.Title = File->Input->io->memalloc(File->Input->io, (size_t)(EBML_ElementDataSize(Elt, 1)+1));
			strcpy(File->Seg.Title,((ebml_string*)Elt)->Buffer);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextMuxingApp()->Id)
		{
			File->Seg.MuxingApp = File->Input->io->memalloc(File->Input->io, (size_t)(EBML_ElementDataSize(Elt, 1)+1));
			strcpy(File->Seg.MuxingApp,((ebml_string*)Elt)->Buffer);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextWritingApp()->Id)
		{
			File->Seg.WritingApp = File->Input->io->memalloc(File->Input->io, (size_t)(EBML_ElementDataSize(Elt, 1)+1));
			strcpy(File->Seg.WritingApp,((ebml_string*)Elt)->Buffer);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextSegmentUid()->Id)
		{
			if (EBML_ElementDataSize(Elt, 1)!=16)
			{
				snprintf(err_msg,err_msgSize,"SegmentUID size is not %d bytes",(int)EBML_ElementDataSize(Elt, 1));
				return 0;
			}
			memcpy(File->Seg.UID,EBML_BinaryGetData((ebml_binary*)Elt),sizeof(File->Seg.UID));
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextPrevUid()->Id)
		{
			if (EBML_ElementDataSize(Elt, 1)!=16)
			{
				snprintf(err_msg,err_msgSize,"PrevUID size is not %d bytes",(int)EBML_ElementDataSize(Elt, 1));
				return 0;
			}
			memcpy(File->Seg.PrevUID,EBML_BinaryGetData((ebml_binary*)Elt),sizeof(File->Seg.PrevUID));
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextNextUid()->Id)
		{
			if (EBML_ElementDataSize(Elt, 1)!=16)
			{
				snprintf(err_msg,err_msgSize,"NextUID size is not %d bytes",(int)EBML_ElementDataSize(Elt, 1));
				return 0;
			}
			memcpy(File->Seg.NextUID,EBML_BinaryGetData((ebml_binary*)Elt),sizeof(File->Seg.NextUID));
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextSegmentFilename()->Id)
		{
			File->Seg.Filename = File->Input->io->memalloc(File->Input->io, (size_t)(EBML_ElementDataSize(Elt, 1)+1));
			strcpy(File->Seg.Filename,((ebml_string*)Elt)->Buffer);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextPrevFilename()->Id)
		{
			File->Seg.PrevFilename = File->Input->io->memalloc(File->Input->io, (size_t)(EBML_ElementDataSize(Elt, 1)+1));
			strcpy(File->Seg.Filename,((ebml_string*)Elt)->Buffer);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextNextFilename()->Id)
		{
			File->Seg.NextFilename = File->Input->io->memalloc(File->Input->io, (size_t)(EBML_ElementDataSize(Elt, 1)+1));
			strcpy(File->Seg.Filename,((ebml_string*)Elt)->Buffer);
		}
	}
	if (duration > 0.0)
		File->Seg.Duration = (mkv_timestamp_t)(duration * File->Seg.TimestampScale);

	return 1;
}

static void releaseTrackEntry(TrackInfo *track, InputStream *io)
{
	if (track->CodecPrivate) io->memfree(io, track->CodecPrivate);
	if (track->Name) io->memfree(io, track->Name);
}

static bool_t parseTrackEntry(ebml_element *Track, MatroskaFile *File, char *err_msg, size_t err_msgSize)
{
	TrackInfo track,*Tracks;
	ebml_element *Elt,*TElt;

	if (ARRAYCOUNT(File->Tracks,TrackInfo) >= MAX_TRACKS)
		return 0;

	memset(&track,0,sizeof(track));
	track.DefaultDuration = INVALID_TIMESTAMP_T;
	track.Enabled = MATROSKA_getContextTrackEnabled()->DefaultValue;
	track.Default = MATROSKA_getContextTrackDefault()->DefaultValue;
	track.Lacing = MATROSKA_getContextTrackLacing()->DefaultValue;
	track.DecodeAll = MATROSKA_getContextTrackCodecDecodeAll()->DefaultValue;
	track.TimestampScale = (float)MATROSKA_getContextTrackTimestampScale()->DefaultValue;
	memcpy(track.Language, "eng", 4);

	for (Elt = EBML_MasterChildren(Track);Elt;Elt = EBML_MasterNext(Elt))
	{
		if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackNumber()->Id)
			track.Number = (int)EBML_IntegerValue(Elt);
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackNumber()->Id)
			track.UID = EBML_IntegerValue(Elt);
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackType()->Id)
		{
			if (EBML_IntegerValue(Elt)==0 || EBML_IntegerValue(Elt)>254)
			{
				snprintf(err_msg,err_msgSize,"Invalid track type: %d",(int)EBML_IntegerValue(Elt));
				goto fail;
			}
			track.Type = (uint8_t)EBML_IntegerValue(Elt);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackEnabled()->Id)
			track.Enabled = EBML_IntegerValue(Elt)!=0;
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackDefault()->Id)
			track.Default = EBML_IntegerValue(Elt)!=0;
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackLacing()->Id)
			track.Lacing = EBML_IntegerValue(Elt)!=0;
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackCodecDecodeAll()->Id)
			track.DecodeAll = EBML_IntegerValue(Elt)!=0;
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackMinCache()->Id)
		{
			if (EBML_IntegerValue(Elt) > 0xFF)
			{
				snprintf(err_msg,err_msgSize,"MinCache is too large: %d",(int)EBML_IntegerValue(Elt));
				goto fail;
			}
			track.MinCache = (uint8_t)EBML_IntegerValue(Elt);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackMaxCache()->Id)
		{
			if (EBML_IntegerValue(Elt) > 0x7FFFFFFF)
			{
				snprintf(err_msg,err_msgSize,"MaxCache is too large: %d",(int)EBML_IntegerValue(Elt));
				goto fail;
			}
			track.MaxCache = (size_t)EBML_IntegerValue(Elt);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackDefaultDuration()->Id)
			track.DefaultDuration = EBML_IntegerValue(Elt);
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackTimestampScale()->Id)
			track.TimestampScale = (float)((ebml_float*)Elt)->Value;
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackMaxBlockAdditionID()->Id)
			track.MaxBlockAdditionID = (size_t)EBML_IntegerValue(Elt);
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackLanguage()->Id)
		{
			size_t copy = (EBML_ElementDataSize(Elt, 1)>3) ? 3 : (size_t)EBML_ElementDataSize(Elt, 1);
			memcpy(track.Language,((ebml_string*)Elt)->Buffer,copy);
			memset(track.Language + copy,0,4-copy);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackCodecID()->Id)
		{
			track.CodecID = File->Input->io->memalloc(File->Input->io, (size_t)(EBML_ElementDataSize(Elt, 1)+1));
			strcpy(track.CodecID,((ebml_string*)Elt)->Buffer);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackCodecPrivate()->Id)
		{
			if (EBML_ElementDataSize(Elt, 1) > 256*1024)
			{
				snprintf(err_msg,err_msgSize,"CodecPrivate is too large: %d",(int)EBML_IntegerValue(Elt));
				goto fail;
			}
			track.CodecPrivateSize = (size_t)EBML_ElementDataSize(Elt, 1);
			track.CodecPrivate = File->Input->io->memalloc(File->Input->io, track.CodecPrivateSize);
			memcpy(track.CodecPrivate,EBML_BinaryGetData((ebml_binary*)Elt),track.CodecPrivateSize);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackOverlay()->Id)
		{
			if (EBML_IntegerValue(Elt)==0 || EBML_IntegerValue(Elt)>254)
			{
				snprintf(err_msg,err_msgSize,"Track number in TrackOverlay is too large: %d",(int)EBML_IntegerValue(Elt));
				goto fail;
			}
			track.TrackOverlay = (uint8_t)EBML_IntegerValue(Elt);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackVideo()->Id)
		{
			for (TElt = EBML_MasterChildren(Elt);TElt;TElt = EBML_MasterNext(TElt))
			{
				if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoInterlaced()->Id)
					track.AV.Video.Interlaced = EBML_IntegerValue(TElt)!=0;
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoStereo()->Id)
				{
					if (EBML_ElementDataSize(TElt, 1) > 3)
					{
						snprintf(err_msg,err_msgSize,"Invalid stereo mode: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.StereoMode = (uint8_t)EBML_IntegerValue(TElt);
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoPixelWidth()->Id)
				{
					if (EBML_IntegerValue(TElt) > 0xFFFFFFFF)
					{
						snprintf(err_msg,err_msgSize,"PixelWidth is too large: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.PixelWidth = (uint32_t)EBML_IntegerValue(TElt);
					if (!track.AV.Video.DisplayWidth)
						track.AV.Video.DisplayWidth = track.AV.Video.PixelWidth;
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoPixelHeight()->Id)
				{
					if (EBML_IntegerValue(TElt) > 0xFFFFFFFF)
					{
						snprintf(err_msg,err_msgSize,"PixelHeight is too large: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.PixelHeight = (uint32_t)EBML_IntegerValue(TElt);
					if (!track.AV.Video.DisplayHeight)
						track.AV.Video.DisplayHeight = track.AV.Video.PixelHeight;
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoDisplayWidth()->Id)
				{
					if (EBML_IntegerValue(TElt) > 0xFFFFFFFF)
					{
						snprintf(err_msg,err_msgSize,"DisplayWidth is too large: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.DisplayWidth = (uint32_t)EBML_IntegerValue(TElt);
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoDisplayHeight()->Id)
				{
					if (EBML_IntegerValue(TElt) > 0xFFFFFFFF)
					{
						snprintf(err_msg,err_msgSize,"DisplayHeight is too large: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.DisplayHeight = (uint32_t)EBML_IntegerValue(TElt);
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoDisplayUnit()->Id)
				{
					if (EBML_IntegerValue(TElt) > 2)
					{
						snprintf(err_msg,err_msgSize,"DisplayUnit is too large: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.DisplayUnit = (uint8_t)EBML_IntegerValue(TElt);
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoDisplayUnit()->Id)
				{
					if (EBML_IntegerValue(TElt) > 2)
					{
						snprintf(err_msg,err_msgSize,"AspectRatioType is too large: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.AspectRatioType = (uint8_t)EBML_IntegerValue(TElt);
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoPixelCropBottom()->Id)
				{
					if (EBML_IntegerValue(TElt) > 0xFFFFFFFF)
					{
						snprintf(err_msg,err_msgSize,"PixelCropBottom is too large: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.CropB = (uint32_t)EBML_IntegerValue(TElt);
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoPixelCropTop()->Id)
				{
					if (EBML_IntegerValue(TElt) > 0xFFFFFFFF)
					{
						snprintf(err_msg,err_msgSize,"PixelCropTop is too large: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.CropT = (uint32_t)EBML_IntegerValue(TElt);
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoPixelCropLeft()->Id)
				{
					if (EBML_IntegerValue(TElt) > 0xFFFFFFFF)
					{
						snprintf(err_msg,err_msgSize,"PixelCropLeft is too large: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.CropL = (uint32_t)EBML_IntegerValue(TElt);
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoPixelCropRight()->Id)
				{
					if (EBML_IntegerValue(TElt) > 0xFFFFFFFF)
					{
						snprintf(err_msg,err_msgSize,"PixelCropRight is too large: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Video.CropR = (uint32_t)EBML_IntegerValue(TElt);
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoColourSpace()->Id)
				{
					if (EBML_ElementDataSize(TElt, 1) != 4)
					{
						snprintf(err_msg,err_msgSize,"ColourSpace is too large: %d",EBML_ElementDataSize(TElt, 1));
						goto fail;
					}
					track.AV.Video.ColourSpace = LOAD32LE(EBML_BinaryGetData((ebml_binary*)TElt));
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackVideoGammaValue()->Id)
					track.AV.Video.GammaValue = (float)((ebml_float*)TElt)->Value;
			}
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackAudio()->Id)
		{
			track.AV.Audio.Channels = (uint8_t)MATROSKA_getContextTrackAudioChannels()->DefaultValue;
			track.AV.Audio.SamplingFreq = (float)MATROSKA_getContextTrackAudioSamplingFreq()->DefaultValue;

			for (TElt = EBML_MasterChildren(Elt);TElt;TElt = EBML_MasterNext(TElt))
			{
				if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackAudioSamplingFreq()->Id)
				{
					track.AV.Audio.SamplingFreq = (float)((ebml_float*)TElt)->Value;
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackAudioOutputSamplingFreq()->Id)
					track.AV.Audio.OutputSamplingFreq = (float)((ebml_float*)TElt)->Value;
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackAudioChannels()->Id)
				{
					if (EBML_IntegerValue(TElt)==0 || EBML_IntegerValue(TElt)>0xFF)
					{
						snprintf(err_msg,err_msgSize,"Invalid Channels value: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Audio.Channels = (uint8_t)EBML_IntegerValue(TElt);
				}
				else if (EBML_ElementClassID(TElt) == MATROSKA_getContextTrackAudioBitDepth()->Id)
				{
					if (EBML_IntegerValue(TElt)==0 || EBML_IntegerValue(TElt)>0xFF)
					{
						snprintf(err_msg,err_msgSize,"Invalid BitDepth value: %d",(int)EBML_IntegerValue(TElt));
						goto fail;
					}
					track.AV.Audio.BitDepth = (uint8_t)EBML_IntegerValue(TElt);
				}
			}
			if (!track.AV.Audio.OutputSamplingFreq)
				track.AV.Audio.OutputSamplingFreq = track.AV.Audio.SamplingFreq;
		}
		// TODO: ContentEncoding
	}

	if (!track.CodecID)
	{
		strncpy(err_msg,"Track has no Codec ID",err_msgSize);
		goto fail;
	}

	// check for duplicate track UID entries
    if (track.UID)
    {
        for (Tracks=ARRAYBEGIN(File->Tracks,TrackInfo);Tracks!=ARRAYEND(File->Tracks,TrackInfo);++Tracks)
        {
            if (Tracks->UID == track.UID)
            {
                snprintf(err_msg,err_msgSize,"A track with UID 0x" PRIx64 " already exists",track.UID);
                goto fail;
            }
        }
    }

	ArrayAppend(&File->Tracks,&track,sizeof(track),256);
	return 1;

fail:
	releaseTrackEntry(&track, File->Input->io);
	return 0;
}

static bool_t parseTracks(ebml_element *Tracks, MatroskaFile *File, char *err_msg, size_t err_msgSize)
{
	ebml_parser_context RContext;
	ebml_element *Elt;

	RContext.Context = Tracks->Context;
	if (EBML_ElementIsFiniteSize(Tracks))
		RContext.EndPosition = EBML_ElementPositionEnd(Tracks);
	else
		RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = &File->L1Context;
	if (EBML_ElementReadData(Tracks,(struct stream*)File->Input,&RContext,1,SCOPE_ALL_DATA)!=ERR_NONE)
	{
		strncpy(err_msg,"Failed to read the Tracks",err_msgSize);
		File->pTracks = INVALID_FILEPOS_T;
		return 0;
	}
	File->pTracks = EBML_ElementPosition(Tracks);

	for (Elt = EBML_MasterChildren(Tracks);Elt;Elt = EBML_MasterNext(Elt))
	{
		if (EBML_ElementClassID(Elt) == MATROSKA_getContextTrackEntry()->Id)
			if (!parseTrackEntry(Elt, File, err_msg, err_msgSize))
				break;
	}
	File->TrackList = Tracks;

	return 1;
}

static bool_t parseCues(ebml_element *Cues, MatroskaFile *File, char *err_msg, size_t err_msgSize)
{
	ebml_parser_context RContext;

	RContext.Context = Cues->Context;
	if (EBML_ElementIsFiniteSize(Cues))
		RContext.EndPosition = EBML_ElementPositionEnd(Cues);
	else
		RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = &File->L1Context;
	if (EBML_ElementReadData(Cues,(struct stream*)File->Input,&RContext,1,SCOPE_ALL_DATA)!=ERR_NONE)
	{
		strncpy(err_msg,"Failed to read the Cues",err_msgSize);
		File->pCues = INVALID_FILEPOS_T;
		return 0;
	}
	File->pCues = EBML_ElementPosition(Cues);
	File->CueList = Cues;

	return 1;
}

static bool_t parseAttachments(ebml_element *Attachments, MatroskaFile *File, char *err_msg, size_t err_msgSize)
{
	ebml_parser_context RContext;
    ebml_element *Elt, *Elt2;
    size_t Count;
    Attachment *At;

	RContext.Context = Attachments->Context;
	if (EBML_ElementIsFiniteSize(Attachments))
		RContext.EndPosition = EBML_ElementPositionEnd(Attachments);
	else
		RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = &File->L1Context;
	if (EBML_ElementReadData(Attachments,(struct stream*)File->Input,&RContext,1,SCOPE_PARTIAL_DATA)!=ERR_NONE)
	{
		strncpy(err_msg,"Failed to read the Attachments",err_msgSize);
		File->pAttachments = INVALID_FILEPOS_T;
		return 0;
	}
	File->pAttachments = EBML_ElementPosition(Attachments);

    Count=0;
    Elt = EBML_MasterFindFirstElt(Attachments, MATROSKA_getContextAttachedFile(), 0,0);
    while (Elt)
    {
        ++Count;
        Elt = EBML_MasterFindNextElt(Attachments, Elt, 0,0);
    }
    ArrayResize(&File->Attachments,Count*sizeof(Attachment),0);
    ArrayZero(&File->Attachments);

    for (Elt = EBML_MasterFindFirstElt(Attachments, MATROSKA_getContextAttachedFile(), 0,0),At=ARRAYBEGIN(File->Attachments,Attachment);
        At!=ARRAYEND(File->Attachments,Attachment); ++At, Elt = EBML_MasterFindNextElt(Attachments, Elt, 0,0))
    {
        At->Length = INVALID_FILEPOS_T;
        At->Position = INVALID_FILEPOS_T;
        for (Elt2=EBML_MasterChildren(Elt);Elt2;Elt2=EBML_MasterNext(Elt2))
        {
            if (Elt2->Context->Id == MATROSKA_getContextAttachedFileName()->Id)
            {
                At->Name = File->Input->io->memalloc(File->Input->io, (size_t)(Elt2->DataSize+1));
			    strcpy(At->Name,((ebml_string*)Elt2)->Buffer);
            }
            else if (Elt2->Context->Id == MATROSKA_getContextAttachedFileData()->Id)
            {
                At->Position = EBML_ElementPositionData(Elt2);
                At->Length = Elt2->DataSize;
            }
            else if (Elt2->Context->Id == MATROSKA_getContextAttachedFileUID()->Id)
                At->UID = EBML_IntegerValue(Elt2);
            else if (Elt2->Context->Id == MATROSKA_getContextAttachedFileMimeType()->Id)
            {
                At->MimeType = File->Input->io->memalloc(File->Input->io, (size_t)(Elt2->DataSize+1));
			    strcpy(At->MimeType,((ebml_string*)Elt2)->Buffer);
            }
            else if (Elt2->Context->Id == MATROSKA_getContextAttachedFileDescription()->Id)
            {
                At->Description = File->Input->io->memalloc(File->Input->io, (size_t)(Elt2->DataSize+1));
			    strcpy(At->Description,((ebml_string*)Elt2)->Buffer);
            }
        }
    }

	return 1;
}

static bool_t addChapterDisplay(ebml_element *ChapterDisplay, MatroskaFile *File, struct Chapter *Chapter)
{
	struct ChapterDisplay *pDisplay,Display;
	ebml_element *Elt;

	if (!ArrayAppend(&Chapter->aDisplays,&Display,sizeof(struct ChapterDisplay),512))
		return 0;
	pDisplay = ARRAYEND(Chapter->aDisplays,struct ChapterDisplay)-1;
	memset(pDisplay,0,sizeof(*pDisplay));
	memcpy(pDisplay->Language, (char*)MATROSKA_getContextChapterLanguage()->DefaultValue, 4);

	for (Elt=EBML_MasterChildren(ChapterDisplay); Elt; Elt=EBML_MasterNext(Elt))
	{
		if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterString()->Id)
		{
			pDisplay->String = File->Input->io->memalloc(File->Input->io, (size_t)(EBML_ElementDataSize(Elt, 1)+1));
			strcpy(pDisplay->String,((ebml_string*)Elt)->Buffer);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterLanguage()->Id)
		{
			size_t copy = (EBML_ElementDataSize(Elt, 1)>3) ? 3 : (size_t)EBML_ElementDataSize(Elt, 1);
			memcpy(pDisplay->Language,((ebml_string*)Elt)->Buffer,copy);
			memset(pDisplay->Language + copy,0,4-copy);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterCountry()->Id)
		{
			size_t copy = (EBML_ElementDataSize(Elt, 1)>3) ? 3 : (size_t)EBML_ElementDataSize(Elt, 1);
			memcpy(pDisplay->Country,((ebml_string*)Elt)->Buffer,copy);
			memset(pDisplay->Country+ copy,0,4-copy);
		}
	}

	return 1;
}

static bool_t parseChapter(ebml_element *Chapter, MatroskaFile *File, char *err_msg, size_t err_msgSize, struct Chapter *Parent, array *Editions)
{
	struct Chapter *pChapter,Chap;
	ebml_element *Elt;

	pChapter = &Chap;
	for (Elt=EBML_MasterChildren(Chapter); Elt; Elt=EBML_MasterNext(Elt))
	{
		if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterAtom()->Id)
		{
			if (!ArrayAppend(&Parent->aChildren,&Chap,sizeof(Chap),512))
				return 0;
			pChapter = ARRAYEND(Parent->aChildren,struct Chapter)-1;
			memset(pChapter,0,sizeof(*pChapter));
			pChapter->Start = INVALID_TIMESTAMP_T;
			if (!parseChapter(Elt,File,err_msg,err_msgSize,pChapter,NULL))
				ArrayRemove(&Parent->aChildren,struct Chapter,pChapter,NULL,NULL);
		}
		// Atom
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterUID()->Id)
			Parent->UID = EBML_IntegerValue(Elt);
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterTimeStart()->Id)
			Parent->Start = EBML_IntegerValue(Elt);
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterTimeEnd()->Id)
			Parent->End = EBML_IntegerValue(Elt);
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterHidden()->Id)
			Parent->Hidden = EBML_IntegerValue(Elt)!=0;
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterEnabled()->Id)
			Parent->Enabled = EBML_IntegerValue(Elt)!=0;
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterDisplay()->Id)
			addChapterDisplay(Elt, File, Parent);
		// Edition
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextEditionUID()->Id)
			Parent->UID = EBML_IntegerValue(Elt);
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextEditionHidden()->Id)
			Parent->Hidden = EBML_IntegerValue(Elt)!=0;
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextEditionDefault()->Id)
			Parent->Default = EBML_IntegerValue(Elt)!=0;
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextEditionOrdered()->Id)
			Parent->Ordered = EBML_IntegerValue(Elt)!=0;
	}

	Parent->nChildren = ARRAYCOUNT(Parent->aChildren,struct Chapter);
	Parent->Children = ARRAYBEGIN(Parent->aChildren,struct Chapter);
	Parent->nDisplay = ARRAYCOUNT(Parent->aDisplays,struct ChapterDisplay);
	Parent->Display = ARRAYBEGIN(Parent->aDisplays,struct ChapterDisplay);

	return 1;
}

static bool_t parseChapters(ebml_element *Chapters, MatroskaFile *File, char *err_msg, size_t err_msgSize)
{
	ebml_parser_context RContext;
	struct Chapter *pChapter,Chap;
	ebml_element *Elt;

	RContext.Context = Chapters->Context;
	if (EBML_ElementIsFiniteSize(Chapters))
		RContext.EndPosition = EBML_ElementPositionEnd(Chapters);
	else
		RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = &File->L1Context;
	if (EBML_ElementReadData(Chapters,(struct stream*)File->Input,&RContext,1,SCOPE_ALL_DATA)!=ERR_NONE)
	{
		strncpy(err_msg,"Failed to read the Chapters",err_msgSize);
		File->pChapters = INVALID_FILEPOS_T;
		return 0;
	}
	File->pChapters = EBML_ElementPosition(Chapters);

	pChapter = &Chap;
	memset(pChapter,0,sizeof(*pChapter));
	Chap.Start = INVALID_TIMESTAMP_T;
	Chap.Ordered = MATROSKA_getContextEditionOrdered()->DefaultValue;
	Chap.Hidden = MATROSKA_getContextEditionHidden()->DefaultValue;
	Chap.Default = MATROSKA_getContextEditionDefault()->DefaultValue;
	for (Elt=EBML_MasterChildren(Chapters); Elt; Elt=EBML_MasterNext(Elt))
	{
		if (EBML_ElementClassID(Elt) == MATROSKA_getContextChapterEntry()->Id)
		{
			if (!ArrayAppend(&File->Chapters,&Chap,sizeof(Chap),512))
				return 0;
			pChapter = ARRAYEND(File->Chapters,struct Chapter)-1;
			if (!parseChapter(Elt, File, err_msg, err_msgSize, pChapter, &File->Chapters))
				ArrayRemove(&File->Chapters,struct Chapter,pChapter,NULL,NULL);
		}
	}

	return 1;
}

static bool_t parseTargets(ebml_element *Targets, MatroskaFile *File, char *err_msg, size_t err_msgSize, struct Tag *Parent)
{
	ebml_element *Elt;
	uint8_t Level;
	struct Target *pTarget,Target;

	Elt = EBML_MasterFindFirstElt(Targets, MATROSKA_getContextTagTargetTypeValue(), 1, 1);
	if (!Elt || EBML_IntegerValue(Elt) > 0xFF)
		return 0;

	Level = (uint8_t)EBML_IntegerValue(Elt);

	pTarget = &Target;
	memset(pTarget,0,sizeof(*pTarget));
	for (Elt=EBML_MasterChildren(Targets); Elt; Elt=EBML_MasterNext(Elt))
	{
		if (EBML_ElementClassID(Elt) == MATROSKA_getContextTagTargetTrackUID()->Id)
		{
			if (!ArrayAppend(&Parent->aTargets,&Target,sizeof(Target),512))
				return 0;
			pTarget = ARRAYEND(Parent->aTargets,struct Target)-1;
			pTarget->Type = TARGET_TRACK;
			pTarget->UID = EBML_IntegerValue(Elt);
			pTarget->Level = Level;
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTagTargetChapterUID()->Id)
		{
			if (!ArrayAppend(&Parent->aTargets,&Target,sizeof(Target),512))
				return 0;
			pTarget = ARRAYEND(Parent->aTargets,struct Target)-1;
			pTarget->Type = TARGET_CHAPTER;
			pTarget->UID = EBML_IntegerValue(Elt);
			pTarget->Level = Level;
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTagTargetAttachmentUID()->Id)
		{
			if (!ArrayAppend(&Parent->aTargets,&Target,sizeof(Target),512))
				return 0;
			pTarget = ARRAYEND(Parent->aTargets,struct Target)-1;
			pTarget->Type = TARGET_ATTACHMENT;
			pTarget->UID = EBML_IntegerValue(Elt);
			pTarget->Level = Level;
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTagTargetEditionUID()->Id)
		{
			if (!ArrayAppend(&Parent->aTargets,&Target,sizeof(Target),512))
				return 0;
			pTarget = ARRAYEND(Parent->aTargets,struct Target)-1;
			pTarget->Type = TARGET_EDITION;
			pTarget->UID = EBML_IntegerValue(Elt);
			pTarget->Level = Level;
		}
	}

	return 1;
}

static bool_t parseSimpleTag(ebml_element *SimpleTag, MatroskaFile *File, char *err_msg, size_t err_msgSize, struct Tag *Parent)
{
	ebml_element *Elt;
	struct SimpleTag simpleTag;

	memset(&simpleTag,0,sizeof(simpleTag));
	simpleTag.Default = MATROSKA_getContextTagDefault()->DefaultValue;
	memcpy(simpleTag.Language, (const char*)MATROSKA_getContextTagLanguage()->DefaultValue, 4);
	for (Elt=EBML_MasterChildren(SimpleTag); Elt; Elt=EBML_MasterNext(Elt))
	{
		if (EBML_ElementClassID(Elt) == MATROSKA_getContextTagName()->Id)
		{
			simpleTag.Name = File->Input->io->memalloc(File->Input->io, (size_t)(EBML_ElementDataSize(Elt, 1)+1));
			strcpy(simpleTag.Name,((ebml_string*)Elt)->Buffer);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTagString()->Id)
		{
			simpleTag.Value = File->Input->io->memalloc(File->Input->io, (size_t)(EBML_ElementDataSize(Elt, 1)+1));
			strcpy(simpleTag.Value,((ebml_string*)Elt)->Buffer);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTagLanguage()->Id)
		{
			size_t copy = (EBML_ElementDataSize(Elt, 1)>3) ? 3 : (size_t)EBML_ElementDataSize(Elt, 1);
			memcpy(simpleTag.Language,((ebml_string*)Elt)->Buffer,copy);
			memset(simpleTag.Language + copy,0,4-copy);
		}
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextTagString()->Id)
			simpleTag.Default = EBML_IntegerValue(Elt)!=0;
	}

	if (!simpleTag.Value || !simpleTag.Name || !ArrayAppend(&Parent->aSimpleTags,&simpleTag,sizeof(simpleTag),256))
	{
		if (simpleTag.Value) File->Input->io->memfree(File->Input->io, simpleTag.Value);
		if (simpleTag.Name) File->Input->io->memfree(File->Input->io, simpleTag.Name);
		return 0;
	}

	Elt = EBML_MasterFindFirstElt(SimpleTag, MATROSKA_getContextSimpleTag(), 0, 0);
	while (Elt)
	{
		parseSimpleTag(Elt, File, err_msg, err_msgSize, Parent);
		Elt = EBML_MasterFindNextElt(SimpleTag, Elt, 0, 0);
	}

	return 1;
}

static bool_t parseTag(ebml_element *Tag, MatroskaFile *File, char *err_msg, size_t err_msgSize, struct Tag *Parent)
{
	ebml_element *Elt;

	for (Elt=EBML_MasterChildren(Tag); Elt; Elt=EBML_MasterNext(Elt))
	{
		if (EBML_ElementClassID(Elt) == MATROSKA_getContextTagTargets()->Id)
			parseTargets(Elt,File,err_msg,err_msgSize,Parent);
		else if (EBML_ElementClassID(Elt) == MATROSKA_getContextSimpleTag()->Id)
			parseSimpleTag(Elt,File,err_msg,err_msgSize,Parent);
	}

	Parent->Targets = ARRAYBEGIN(Parent->aTargets,struct Target);
	Parent->nTargets = ARRAYCOUNT(Parent->aTargets,struct Target);
	Parent->SimpleTags = ARRAYBEGIN(Parent->aSimpleTags,struct SimpleTag);
	Parent->nSimpleTags = ARRAYCOUNT(Parent->aSimpleTags,struct SimpleTag);

	return 1;
}

static bool_t parseTags(ebml_element *Tags, MatroskaFile *File, char *err_msg, size_t err_msgSize)
{
	ebml_parser_context RContext;
	struct Tag *pTag,Tag;
	ebml_element *Elt;

	RContext.Context = Tags->Context;
	if (EBML_ElementIsFiniteSize(Tags))
		RContext.EndPosition = EBML_ElementPositionEnd(Tags);
	else
		RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = &File->L1Context;
	if (EBML_ElementReadData(Tags,(struct stream*)File->Input,&RContext,1,SCOPE_ALL_DATA)!=ERR_NONE)
	{
		strncpy(err_msg,"Failed to read the Tags",err_msgSize);
		File->pTags = INVALID_FILEPOS_T;
		return 0;
	}
	File->pTags = EBML_ElementPosition(Tags);

	pTag = &Tag;
	memset(pTag,0,sizeof(*pTag));
	for (Elt=EBML_MasterChildren(Tags); Elt; Elt=EBML_MasterNext(Elt))
	{
		if (EBML_ElementClassID(Elt) == MATROSKA_getContextTag()->Id)
		{
			if (!ArrayAppend(&File->Tags,&Tag,sizeof(Tag),512))
				return 0;
			pTag = ARRAYEND(File->Tags,struct Tag)-1;
			if (!parseTag(Elt, File, err_msg, err_msgSize, pTag))
				ArrayRemove(&File->Tags,struct Tag,pTag,NULL,NULL);
		}
	}


	return 1;
}

MatroskaFile *mkv_Open(InputStream *io, char *err_msg, size_t err_msgSize)
{
	int UpperLevel;
	ebml_element *Head, *Elt;

	MatroskaFile *File = io->memalloc(io,sizeof(*File));
	if (!File)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return NULL;
	}

	memset(File,0,sizeof(*File));
	// Core-C, EBML & Matroska Init
	ParserContext_Init(&File->p,NULL,NULL,NULL);
	MATROSKA_Init(&File->p);
	NodeRegisterClassEx(&File->p.Base.Base, HaaliStream_Class);

	File->pSegmentInfo = INVALID_FILEPOS_T;
	File->pTracks = INVALID_FILEPOS_T;
	File->pCues = INVALID_FILEPOS_T;
	File->pAttachments = INVALID_FILEPOS_T;
	File->pChapters = INVALID_FILEPOS_T;
	File->pTags = INVALID_FILEPOS_T;
	File->pFirstCluster = INVALID_FILEPOS_T;

	io->progress(io,0,0);
	io->ioseek(io,0,SEEK_SET);

	// find a segment
	File->Input = (haali_stream*)NodeCreate(&File->p,HAALI_STREAM_CLASS);
	if (!File->Input)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return NULL;
	}
	File->Input->io = io;

	File->L0Context.Context = MATROSKA_getContextStream();
    File->L0Context.EndPosition = File->Input->io->getfilesize(File->Input->io);
    File->L0Context.UpContext = NULL;
	UpperLevel = 0;
	Head = EBML_FindNextElement((struct stream*)File->Input,&File->L0Context,&UpperLevel,0);
	if (!Head)
	{
		strncpy(err_msg,"Out of memory",err_msgSize);
		return NULL;
	}

	if (EBML_ElementClassID(Head) == MATROSKA_getContextSegment()->Id)
	{
		strncpy(err_msg,"First element in file is not EBML",err_msgSize);
		return NULL;
	}

	if (EBML_ElementReadData(Head,(struct stream*)File->Input,&File->L0Context,1,SCOPE_ALL_DATA)!=ERR_NONE)
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

	File->Segment = EBML_FindNextElement((struct stream*)File->Input,&File->L0Context,&UpperLevel,0);
	if (!File->Segment)
	{
		strncpy(err_msg,"No segments found in the file",err_msgSize);
		return NULL;
	}

	// we want to read data until we find a seekhead or a trackinfo
    File->L1Context.Context = File->Segment->Context;
	if (EBML_ElementIsFiniteSize(File->Segment))
		File->L1Context.EndPosition = EBML_ElementPositionEnd(File->Segment);
	else
		File->L1Context.EndPosition = INVALID_FILEPOS_T;
    File->L1Context.UpContext = &File->L0Context;
	UpperLevel = 0;
	Head = EBML_FindNextElement((struct stream*)File->Input,&File->L1Context,&UpperLevel,0);
	while (Head && (!EBML_ElementIsFiniteSize(File->Segment) || EBML_ElementPositionEnd(File->Segment) >= EBML_ElementPositionEnd(Head)))
	{
		if (EBML_ElementClassID(Head) == MATROSKA_getContextSeekHead()->Id)
		{
			parseSeekHead(Head, File, err_msg, err_msgSize);
			NodeDelete((node*)Head);
		}
		else if (EBML_ElementClassID(Head) == MATROSKA_getContextSegmentInfo()->Id)
			parseSegmentInfo(Head, File, err_msg, err_msgSize);
		else if (EBML_ElementClassID(Head) == MATROSKA_getContextTracks()->Id)
			parseTracks(Head, File, err_msg, err_msgSize);
		else if (EBML_ElementClassID(Head) == MATROSKA_getContextCues()->Id)
			parseCues(Head, File, err_msg, err_msgSize);
		else if (EBML_ElementClassID(Head) == MATROSKA_getContextAttachments()->Id)
        {
			parseAttachments(Head, File, err_msg, err_msgSize);
			NodeDelete((node*)Head);
		}
		else if (EBML_ElementClassID(Head) == MATROSKA_getContextChapters()->Id)
		{
			parseChapters(Head, File, err_msg, err_msgSize);
			NodeDelete((node*)Head);
		}
		else if (EBML_ElementClassID(Head) == MATROSKA_getContextTags()->Id)
		{
			parseTags(Head, File, err_msg, err_msgSize);
			NodeDelete((node*)Head);
		}
		else
		{
			if (EBML_ElementClassID(Head)==MATROSKA_getContextCluster()->Id && File->pFirstCluster==INVALID_FILEPOS_T)
				File->pFirstCluster = EBML_ElementPosition(Head);
			Elt = EBML_ElementSkipData(Head,(struct stream*)File->Input,&File->L1Context,NULL,1);
			NodeDelete((node*)Head);
			if (Elt)
			{
				Head = Elt;
				continue;
			}
		}
		Head = EBML_FindNextElement((struct stream*)File->Input,&File->L1Context,&UpperLevel,0);
	}

	if (File->CueList && File->SegmentInfo)
	{
		for (Elt = EBML_MasterChildren(File->CueList);Elt;Elt = EBML_MasterNext(Elt))
		{
			if (EBML_ElementClassID(Elt) == MATROSKA_getContextCuePoint()->Id)
				MATROSKA_LinkCueSegmentInfo((matroska_cuepoint*)Elt, File->SegmentInfo);
		}

		MATROSKA_CuesSort(File->CueList);
	}

	return File;
}

static void releaseAttachments(array *Attachments, MatroskaFile *File)
{
    Attachment *At;
    for (At=ARRAYBEGIN(File->Attachments,Attachment); At!=ARRAYEND(File->Attachments,Attachment); ++At)
    {
        if (At->Name) File->Input->io->memfree(File->Input->io, At->Name);
        if (At->MimeType) File->Input->io->memfree(File->Input->io, At->MimeType);
        if (At->Description) File->Input->io->memfree(File->Input->io, At->Description);
    }
	ArrayClear(Attachments);
}

static void releaseChapterDisplays(array *ChapterDisplays, MatroskaFile *File)
{
	struct ChapterDisplay *pChapter;

	for (pChapter = ARRAYBEGIN(*ChapterDisplays,struct ChapterDisplay); pChapter != ARRAYEND(*ChapterDisplays,struct ChapterDisplay); ++pChapter)
		File->Input->io->memfree(File->Input->io, pChapter->String);
	ArrayClear(ChapterDisplays);
}

static void releaseChapters(array *Chapters, MatroskaFile *File)
{
	struct Chapter *pChapter;

	for (pChapter = ARRAYBEGIN(*Chapters,struct Chapter); pChapter != ARRAYEND(*Chapters,struct Chapter); ++pChapter)
	{
		releaseChapterDisplays(&pChapter->aDisplays, File);
		releaseChapters(&pChapter->aChildren,File);
	}

	ArrayClear(Chapters);
}

static void releaseSimpleTags(array *SimpleTags, MatroskaFile *File)
{
	struct SimpleTag *pSimpleTag;
	for (pSimpleTag = ARRAYBEGIN(*SimpleTags,struct SimpleTag); pSimpleTag != ARRAYEND(*SimpleTags,struct SimpleTag); ++pSimpleTag)
	{
		if (pSimpleTag->Name) File->Input->io->memfree(File->Input->io, pSimpleTag->Name);
		if (pSimpleTag->Value) File->Input->io->memfree(File->Input->io, pSimpleTag->Value);
	}
	ArrayClear(SimpleTags);
}

static void releaseTags(array *Tags, MatroskaFile *File)
{
	struct Tag *pTag;

	for (pTag = ARRAYBEGIN(*Tags,struct Tag); pTag != ARRAYEND(*Tags,struct Tag); ++pTag)
	{
		releaseSimpleTags(&pTag->aSimpleTags, File);
		ArrayClear(&pTag->aTargets);
	}

	ArrayClear(Tags);
}

void mkv_Close(MatroskaFile *File)
{
	haali_stream *Input = File->Input;
	if (File->Seg.Filename) Input->io->memfree(Input->io, File->Seg.Filename);
	if (File->Seg.PrevFilename) Input->io->memfree(Input->io, File->Seg.PrevFilename);
	if (File->Seg.NextFilename) Input->io->memfree(Input->io, File->Seg.NextFilename);
	if (File->Seg.Title) Input->io->memfree(Input->io, File->Seg.Title);
	if (File->Seg.MuxingApp) Input->io->memfree(Input->io, File->Seg.MuxingApp);
	if (File->Seg.WritingApp) Input->io->memfree(Input->io, File->Seg.WritingApp);

	ArrayClear(&File->Tracks);
    releaseAttachments(&File->Attachments, File);
	releaseChapters(&File->Chapters, File);
	releaseTags(&File->Tags, File);

	if (File->TrackList) NodeDelete((node*)File->TrackList);
	if (File->CueList) NodeDelete((node*)File->CueList);
	if (File->SegmentInfo) NodeDelete((node*)File->SegmentInfo);
	if (File->Segment) NodeDelete((node*)File->Segment);

	// Core-C Done
	ParserContext_Done(&File->p);

	Input->io->memfree(Input, File);
}

int mkv_ReadFrame(MatroskaFile *File, int mask, unsigned int *track, ulonglong *StartTime, ulonglong *EndTime, ulonglong *FilePos, unsigned int *FrameSize,
                void** FrameRef, unsigned int *FrameFlags)
{
	ebml_element *Elt = NULL,*Elt2;
	TrackInfo *tr;
	int16_t TrackNum;
	matroska_frame Frame;
	bool_t Skip;
	unsigned int Track;
	int UpperLevel = 0;

	if (FrameFlags)
		*FrameFlags = 0;
	if (!track)
		track = &Track;
	*track = (unsigned int)-1;

	for (;;)
	{
		if (!File->CurrentCluster)
		{
			for (;;)
			{
				if (!Elt)
					Elt = EBML_FindNextElement((struct stream*)File->Input,&File->L1Context,&UpperLevel,0);
				File->CurrentCluster = Elt;
				if (!File->CurrentCluster)
					return EOF;
				if (UpperLevel)
				{
					// TODO: changing segments not supported yet
					NodeDelete((node*)File->CurrentCluster);
					return EOF;
				}
				if (EBML_ElementClassID(File->CurrentCluster) == MATROSKA_getContextCluster()->Id)
					break;

				Elt = File->CurrentCluster;
				File->CurrentCluster = EBML_ElementSkipData(File->CurrentCluster,(struct stream*)File->Input,&File->L1Context,NULL,1);
				NodeDelete((node*)Elt);
				Elt = NULL;
			}

			MATROSKA_LinkClusterReadSegmentInfo((matroska_cluster*)File->CurrentCluster,File->SegmentInfo,1);
			File->ClusterContext.Context = MATROSKA_getContextCluster();
			if (EBML_ElementIsFiniteSize(File->CurrentCluster))
				File->ClusterContext.EndPosition = EBML_ElementPositionEnd(File->CurrentCluster);
			else
				File->ClusterContext.EndPosition = INVALID_FILEPOS_T;
			File->ClusterContext.UpContext = &File->L1Context;

			File->CurrentBlock = NULL;
			File->CurrentFrame = 0;
		}

		Elt = NULL;
		while (!File->CurrentBlock)
		{
			if (!Elt)
			{
				UpperLevel = 0;
				Elt = EBML_FindNextElement((struct stream*)File->Input,&File->ClusterContext,&UpperLevel,1);
			}
			if (!Elt || UpperLevel>0)
			{
				NodeDelete((node*)File->CurrentCluster);
				File->CurrentCluster = NULL;
				--UpperLevel;
				break; // go to the next Cluster
			}

			Skip = 0;
			if (EBML_ElementClassID(Elt) == MATROSKA_getContextClusterTimestamp()->Id)
			{
				if (EBML_ElementReadData(Elt,(struct stream*)File->Input,&File->ClusterContext,1, SCOPE_ALL_DATA)!=ERR_NONE)
					return EOF; // TODO: memory leak
				Elt2 = EBML_MasterFindFirstElt(File->CurrentCluster,MATROSKA_getContextClusterTimestamp(),1,1);
				if (!Elt2)
					return EOF; // TODO: memory leak
				EBML_IntegerSetValue((ebml_integer*)Elt2,EBML_IntegerValue(Elt));
				NodeDelete((node*)Elt);
				Elt = NULL;
			}
			else if (EBML_ElementClassID(Elt) == MATROSKA_getContextClusterSimpleBlock()->Id)
			{
				if (EBML_ElementReadData(Elt,(struct stream*)File->Input,&File->ClusterContext,1, SCOPE_PARTIAL_DATA)!=ERR_NONE)
					return EOF; // TODO: memory leak

				TrackNum = MATROSKA_BlockTrackNum((matroska_block*)Elt);
				for (*track=0, tr=ARRAYBEGIN(File->Tracks,TrackInfo);tr!=ARRAYEND(File->Tracks,TrackInfo);++tr,(*track)++)
					if (TrackNum==tr->Number)
						break;
				if (tr==ARRAYEND(File->Tracks,TrackInfo) || (1<<*track & File->trackMask)!=0)
					Skip = 1;
				else
				{
					EBML_MasterAppend(File->CurrentCluster, Elt);
					MATROSKA_LinkBlockWithReadTracks((matroska_block*)Elt,File->TrackList,1);
					MATROSKA_LinkBlockReadSegmentInfo((matroska_block*)Elt,File->SegmentInfo,1);
					File->CurrentBlock = (matroska_block*)Elt;
				}
			}
			else if (EBML_ElementClassID(Elt) == MATROSKA_getContextClusterBlockGroup()->Id)
			{
				if (EBML_ElementReadData(Elt,(struct stream*)File->Input,&File->ClusterContext,1, SCOPE_PARTIAL_DATA)!=ERR_NONE)
					return EOF; // TODO: memory leak

				Elt2 = EBML_MasterFindFirstElt(Elt, MATROSKA_getContextClusterBlock(), 0, 0);
				if (!Elt2)
					Skip = 1;
				else
				{
					TrackNum = MATROSKA_BlockTrackNum((matroska_block*)Elt2);
					for (*track=0, tr=ARRAYBEGIN(File->Tracks,TrackInfo);tr!=ARRAYEND(File->Tracks,TrackInfo);++tr,(*track)++)
						if (TrackNum==tr->Number)
							break;

					if (tr==ARRAYEND(File->Tracks,TrackInfo) || (1<<*track & File->trackMask)!=0)
						Skip = 1;
					else
					{
						EBML_MasterAppend(File->CurrentCluster, Elt);
						MATROSKA_LinkBlockWithReadTracks((matroska_block*)Elt2,File->TrackList,1);
						MATROSKA_LinkBlockReadSegmentInfo((matroska_block*)Elt2,File->SegmentInfo,1);
						File->CurrentBlock = (matroska_block*)Elt2;
					}
				}
			}
			else Skip = 1;

			if (Skip)
			{
				Elt2 = Elt;
				Elt = EBML_ElementSkipData(Elt2,(struct stream*)File->Input,&File->L1Context,NULL,1);
				NodeDelete((node*)Elt2);
			}
		}
		if (File->CurrentCluster && File->CurrentBlock)
			break;
	}

	assert(File->CurrentBlock!=NULL);
	if (MATROSKA_BlockGetFrame(File->CurrentBlock,File->CurrentFrame,&Frame,0)!=ERR_NONE)
		return -1; // TODO: memory leaks

	if (*track==(unsigned int)-1)
	{
		TrackNum = MATROSKA_BlockTrackNum(File->CurrentBlock);
		for (*track=0, tr=ARRAYBEGIN(File->Tracks,TrackInfo);tr!=ARRAYEND(File->Tracks,TrackInfo);++tr,(*track)++)
			if (TrackNum==tr->Number)
				break;
	}

	if (Frame.Timestamp!=INVALID_TIMESTAMP_T)
	{
		if (StartTime)
			*StartTime = Frame.Timestamp;
		if (EndTime && Frame.Duration != INVALID_TIMESTAMP_T)
			*EndTime = Frame.Timestamp + Frame.Duration;
	}

	if (FilePos)
		*FilePos = EBML_ElementPosition((ebml_element*)File->CurrentBlock);
	if (FrameFlags)
	{
		if (Frame.Timestamp == INVALID_TIMESTAMP_T)
			*FrameFlags |= FRAME_UNKNOWN_START;
		if (Frame.Duration == INVALID_TIMESTAMP_T)
			*FrameFlags |= FRAME_UNKNOWN_END;
		if (MATROSKA_BlockKeyframe(File->CurrentBlock))
			*FrameFlags |= FRAME_KF;
	}
	MATROSKA_BlockSkipToFrame(File->CurrentBlock, (struct stream*)File->Input, File->CurrentFrame);
	*FrameRef = File->Input->io->makeref(File->Input->io,MATROSKA_BlockGetLength(File->CurrentBlock,File->CurrentFrame++));

	if (File->CurrentFrame >= MATROSKA_BlockGetFrameCount(File->CurrentBlock))
	{
		File->CurrentBlock = NULL;
		File->CurrentFrame = 0;
	}

	return 0;
}

static void SeekToPos(MatroskaFile *File, filepos_t SeekPos)
{
	if (File->CurrentBlock)
	{
		NodeDelete((node*)File->CurrentBlock);
		File->CurrentBlock = NULL;
	}
	File->CurrentFrame = 0;
	if (File->CurrentCluster && EBML_ElementPosition(File->EBML_ElementPosition)!=SeekPos)
	{
		NodeDelete((node*)File->CurrentCluster);
		File->CurrentCluster = NULL;
	}
	File->Input->io->ioseek(File->Input->io,SeekPos,SEEK_SET);
}

void mkv_Seek(MatroskaFile *File, mkv_timestamp_t timestamp, int flags)
{
	matroska_cuepoint *CuePoint;
	filepos_t SeekPos;

	if (File->flags & MKVF_AVOID_SEEKS || File->pFirstCluster==INVALID_FILEPOS_T || timestamp==INVALID_TIMESTAMP_T)
		return;

	if (timestamp==0)
	{
		SeekToPos(File, File->pFirstCluster);
		return;
	}
	if (!File->CueList)
		return;

	CuePoint = MATROSKA_CuesGetTimestampStart(File->CueList,timestamp);
	if (CuePoint==NULL)
		return;

	SeekPos = MATROSKA_CuePosInSegment(CuePoint) + EBML_ElementPositionData(File->Segment);
	SeekToPos(File, SeekPos);
}

int mkv_TruncFloat(float f)
{
	return (int)f;
}

static filepos_t Seek(haali_stream *p ,filepos_t Pos,int SeekMode)
{
	if (!(Pos == 0 && SeekMode==SEEK_CUR))
		p->io->ioseek(p->io,Pos,SeekMode);
	return p->io->iotell(p->io);
}

static err_t Read(haali_stream* p,void* Data,size_t Size,size_t* pReaded)
{
	size_t Readed;
	if (!pReaded)
		pReaded = &Readed;
	*pReaded = p->io->ioread(p->io,Data,Size);
	if (Size && *pReaded!=Size)
		return ERR_END_OF_FILE;
	return ERR_NONE;
}

META_START(HaaliStream_Class,HAALI_STREAM_CLASS)
META_CLASS(SIZE,sizeof(haali_stream))
META_VMT(TYPE_FUNC,stream_vmt,Seek,Seek)
META_VMT(TYPE_FUNC,stream_vmt,Read,Read)
META_END(STREAM_CLASS)
