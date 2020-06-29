#include "MatroskaWriter.h"

#include "ebml/ebml_internal.h"
#include "matroska/matroska_sem.h"
#include "matroska/matroska2.h"
//#include <stdio.h>

typedef struct write_stream {
	stream Base;
	OutputStream* io;
	int io_flag;
} write_stream;

#define MKV_WRITE_STREAM_CLASS FOURCC('M','K','V','W')
#define MAXLINE 1024

MatroskaFile* mkv_OpenOutput(OutputStream* io, int mode, char* err_msg, size_t err_msgSize)
{
	ebml_element* Head, * Elt;
	MatroskaFile* File = io->memalloc(io, sizeof(*File));
	if (!File) {
		strncpy(err_msg, "Out of memory", err_msgSize);
		return NULL;
	}

	memset(File, 0, sizeof(*File));
	File->pSegmentInfo = INVALID_FILEPOS_T;
	File->pTracks = INVALID_FILEPOS_T;
	File->pCues = INVALID_FILEPOS_T;
	File->pAttachments = INVALID_FILEPOS_T;
	File->pChapters = INVALID_FILEPOS_T;
	File->pTags = INVALID_FILEPOS_T;
	File->pFirstCluster = INVALID_FILEPOS_T;
	File->flags = 0;
	ArrayInit(&File->Tracks);
	io->progress(io, 0, 0);
	if (mode == MATROSKA_OUTPUT_DEFAULT && !(File->IOStream->io_flag & MKV_STREAM_NOSEEK)) {
		io->ioseek(io, 0, SEEK_SET);
		//currently only streaming output is supported
		//since write-frame requires the whole information.
		File->flags |= MKVF_AVOID_SEEKS;
	}
	else {
		File->flags |= MKVF_AVOID_SEEKS;
	}
	File->IOStream = (write_stream*)NodeCreate(io->AnyNode, MKV_WRITE_STREAM_CLASS);
	if (!File->IOStream) {
		mkv_CloseInput(File);
		strncpy(err_msg, "Out of memory", err_msgSize);
		return NULL;
	}
	File->IOStream->io = io;
	File->L0Context.Context = &MATROSKA_ContextStream;
	File->L0Context.EndPosition = INVALID_FILEPOS_T;
	File->L0Context.UpContext = NULL;
	return File;
}

int cmp_id(const void* Param, const void* a, const void* b) {
	return ((TrackInfo*)a)->Number == ((TrackInfo*)b)->Number ? ((TrackInfo*)a)->UID < ((TrackInfo*)b)->UID: ((TrackInfo*)a)->Number < ((TrackInfo*)b)->Number;
}

MATROSKA_DLL int mkv_AddTrack(MatroskaFile* File, TrackInfo* info)
{
	ArrayAppend(&File->Tracks,info,sizeof(TrackInfo),0);
	return 0;
}

MATROSKA_DLL int mkv_WriteHeader(MatroskaFile* File, char* err_msg, size_t err_msgSize)
{
	OutputStream*  io = (OutputStream*)File->IOStream->io;
	ebml_master* EbmlHead = (ebml_master*)EBML_ElementCreate(io->AnyNode, &EBML_ContextHead, 0, NULL);
	if (!EbmlHead) {
		mkv_CloseInput(File);
		strncpy(err_msg, "Out of memory", err_msgSize);
		return NULL;
	}
	//TODO: Add checksum
	EBML_MasterUseChecksum(EbmlHead, 0);
	NodeTree_Clear((nodetree*)EbmlHead); // remove the default values
	// DocType
	ebml_master* RLevel1 = (ebml_master*)EBML_MasterGetChild(EbmlHead, &EBML_ContextDocType);
	if (!EbmlHead) {
		mkv_CloseInput(File);
		strncpy(err_msg, "Out of memory", err_msgSize);
		return NULL;
	}

	assert(Node_IsPartOf(RLevel1, EBML_STRING_CLASS));
	if (File->Profile == PROFILE_WEBM) {
		if (EBML_StringSetValue((ebml_string*)RLevel1, "webm") != ERR_NONE) {
			mkv_CloseInput(File);
			strncpy(err_msg, "Error writing EBML_ContextDocType", err_msgSize);
			return NULL;
		}
	}
	else {
		if (EBML_StringSetValue((ebml_string*)RLevel1, "matroska") != ERR_NONE) {
			mkv_CloseInput(File);
			strncpy(err_msg, "Error writing EBML_ContextDocType", err_msgSize);
			return NULL;
		}
	}

	if (File->Profile == PROFILE_MATROSKA_V2 || File->Profile == PROFILE_WEBM)
		File->DocVersion = 2;
	if (File->Profile == PROFILE_MATROSKA_V3)
		File->DocVersion = 3;
	if (File->Profile == PROFILE_MATROSKA_V4)
		File->DocVersion = 4;
/*
	if (ARRAYCOUNT(Alternate3DTracks, block_info*) && DstProfile!=PROFILE_MATROSKA_V3 && DstProfile!=PROFILE_MATROSKA_V4)
    {
        NODE_TextPrintf(StdErr,T("Using --alt-3d in profile '%s' try \"--doctype %d\"\r\n"),GetProfileName(DstProfile),GetProfileId(PROFILE_MATROSKA_V3));
        goto exit;
    }
*/
	RLevel1 = (ebml_master*)EBML_MasterGetChild(EbmlHead, &EBML_ContextDocTypeVersion);
	if (!RLevel1) {
		mkv_CloseInput(File);
		strncpy(err_msg, "Out of memory", err_msgSize);
		return 1;
	}
	assert(Node_IsPartOf(RLevel1, EBML_INTEGER_CLASS));
	EBML_IntegerSetValue((ebml_integer*)RLevel1, File->DocVersion);

	// Doctype readable version
	RLevel1 = (ebml_master*)EBML_MasterGetChild(EbmlHead, &EBML_ContextDocTypeReadVersion);
	if (!RLevel1) {
		mkv_CloseInput(File);
		strncpy(err_msg, "Out of memory", err_msgSize);
		return 1;
	}
	assert(Node_IsPartOf(RLevel1, EBML_INTEGER_CLASS));
	EBML_IntegerSetValue((ebml_integer*)RLevel1, File->DocVersion);
	if (EBML_ElementRender((ebml_element*)EbmlHead, File->IOStream, 1, 0, 1, &File->NextPos) != ERR_NONE) {
		mkv_CloseInput(File);
		strncpy(err_msg, "Failed to write header", err_msgSize);
		return 1;
	}
	NodeDelete((node*)EbmlHead);
	return 0;
}

inline static bool_t all_null(uint8_t* data, size_t size)
{
	for (size_t i = 0; i < size; ++i) {
		if(data[size]) return 0;
	}
	return 1;
}

int mkv_TempHeaders(MatroskaFile* File)
{
	OutputStream* io = (OutputStream*)File->IOStream->io;
	File->Segment = (ebml_master*)EBML_ElementCreate(io->AnyNode, &MATROSKA_ContextSegment, 0, NULL);
	if (File->flags & MKVF_AVOID_SEEKS)
		EBML_ElementSetInfiniteSize((ebml_element*)File->Segment, 1);
	else {
		// temporary value
		EBML_ElementSetSizeLength((ebml_element*)File->Segment, EBML_MAX_SIZE);
	}
	if (EBML_ElementRenderHead((ebml_element*)File->Segment, File->IOStream, 0, NULL) != ERR_NONE) {
		return 1;
	}
	File->Segment_Info = EBML_MasterFindFirstElt(File->Segment,&MATROSKA_ContextInfo,1,0);
	ebml_string* LibName = (ebml_string*)EBML_MasterFindFirstElt(File->Segment_Info, &MATROSKA_ContextMuxingApp, 1, 0);
	if (!File->Seg.MuxingApp) {
		char String[MAXLINE];
		sprintf(String, "Modified %s + %s", Node_GetDataStr(io->AnyNode, CONTEXT_LIBEBML_VERSION), Node_GetDataStr(io->AnyNode, CONTEXT_LIBMATROSKA_VERSION));
		EBML_UniStringSetValue(LibName, String);
	}
	else {
		EBML_UniStringSetValue(LibName, File->Seg.MuxingApp);
	}
	ebml_string*  AppName = (ebml_string*)EBML_MasterFindFirstElt(File->Segment_Info, &MATROSKA_ContextWritingApp, 1, 0);
	if (!File->Seg.WritingApp) {
		char String[MAXLINE];
		sprintf(String, "Modified %s + %s dll", Node_GetDataStr(io->AnyNode, CONTEXT_LIBEBML_VERSION), Node_GetDataStr(io->AnyNode, CONTEXT_LIBMATROSKA_VERSION));
		EBML_UniStringSetValue(AppName, String);
	}
	else {
		EBML_UniStringSetValue(AppName, File->Seg.WritingApp);
	}
	ebml_master* RLevel1 = (ebml_master*)EBML_MasterGetChild(File->Segment_Info, &MATROSKA_ContextDateUTC);
	if (File->Seg.DateUTC) {
		EBML_DateSetDateTime((ebml_date*)RLevel1, File->Seg.DateUTC);
	}
	else {
		EBML_DateSetDateTime((ebml_date*)RLevel1, DATE_GetTimeDate());
	}
	if (!File->Seg.TimestampScale) {
		File->Seg.TimestampScale = 1000000;
	}
	ebml_element* segtimestamp = EBML_MasterAddElt(File->Segment_Info, &MATROSKA_ContextTimestampScale, 0);
	EBML_IntegerSetValue(segtimestamp,File->Seg.TimestampScale);
	filepos_t length;
	EBML_ElementRender(File->Segment_Info,File->IOStream,0,0,0,&length);
	File->NextPos+=length;
	File->TrackList = EBML_MasterAddElt(File->Segment,&MATROSKA_ContextTracks,0);
	for (size_t i = 0; i < ARRAYCOUNT(File->Tracks,TrackInfo); ++i) {
		TrackInfo* info = &ARRAYBEGIN(File->Tracks, TrackInfo)[i];
		info->entry = EBML_MasterAddElt(File->TrackList,&MATROSKA_ContextTrackEntry,0);
		//MATROSKA_TrackSetCompressionZlib(entry,);
		ebml_integer* track_number = EBML_MasterAddElt(info->entry,&MATROSKA_ContextTrackNumber,0);
		EBML_IntegerSetValue(track_number, i + 1);
		ebml_integer* track_uid = EBML_MasterAddElt(info->entry, &MATROSKA_ContextTrackUID, 0);
		if(info->UID)
			EBML_IntegerSetValue(track_uid, info->UID);
		else
			EBML_IntegerSetValue(track_uid, i + 1);
		ebml_integer* track_type = EBML_MasterAddElt(info->entry, &MATROSKA_ContextTrackType, 0);
		EBML_IntegerSetValue(track_type, info->Type);
		ebml_integer* track_enabled = EBML_MasterAddElt(info->entry, &MATROSKA_ContextFlagEnabled, 0);
		EBML_IntegerSetValue(track_enabled, info->Enabled);
		ebml_integer* track_default = EBML_MasterAddElt(info->entry, &MATROSKA_ContextFlagDefault, 0);
		EBML_IntegerSetValue(track_default, info->Default);
		ebml_integer* track_forced = EBML_MasterAddElt(info->entry, &MATROSKA_ContextFlagForced, 0);
		EBML_IntegerSetValue(track_forced, info->Forced);
		ebml_integer* track_lacing = EBML_MasterAddElt(info->entry, &MATROSKA_ContextFlagLacing, 0);
		EBML_IntegerSetValue(track_lacing, info->Lacing);
		//not supported
		ebml_integer* MaxBlockAdditionID = EBML_MasterAddElt(info->entry, &MATROSKA_ContextMaxBlockAdditionID, 0);
		EBML_IntegerSetValue(MaxBlockAdditionID, 0);
		ebml_string* Codec_ID = EBML_MasterAddElt(info->entry, &MATROSKA_ContextCodecID, 0);
		EBML_StringSetValue(Codec_ID, info->CodecID);
		if (info->CodecPrivate) {
			ebml_binary* codec_private = EBML_MasterAddElt(info->entry, &MATROSKA_ContextCodecPrivate, 0);
			EBML_BinarySetData(codec_private,info->CodecPrivate,info->CodecPrivateSize);
		}
		ebml_integer* decode_all = EBML_MasterAddElt(info->entry, &MATROSKA_ContextCodecDecodeAll, 0);
		EBML_IntegerSetValue(decode_all, info->DecodeAll);
		ebml_integer* SeekPreRoll = EBML_MasterAddElt(info->entry, &MATROSKA_ContextSeekPreRoll, 0);
		EBML_IntegerSetValue(SeekPreRoll, info->SeekPreRoll);
		if (info->Type == TRACK_TYPE_VIDEO) {
			ebml_master* VideoContext = EBML_MasterAddElt(info->entry, &MATROSKA_ContextVideo, 0);
			ebml_integer* flag_interlace = EBML_MasterAddElt(VideoContext, &MATROSKA_ContextFlagInterlaced, 0);
			EBML_IntegerSetValue(flag_interlace, info->AV.Video.Interlaced);
			ebml_integer* FieldOrder = EBML_MasterAddElt(VideoContext, &MATROSKA_ContextFieldOrder, 0);
			EBML_IntegerSetValue(FieldOrder, info->AV.Video.FieldOrder);
			ebml_integer* PixelWidth = EBML_MasterAddElt(VideoContext, &MATROSKA_ContextPixelWidth, 0);
			EBML_IntegerSetValue(PixelWidth, info->AV.Video.PixelWidth);
			ebml_integer* PixelHeight = EBML_MasterAddElt(VideoContext, &MATROSKA_ContextPixelHeight, 0);
			EBML_IntegerSetValue(PixelHeight, info->AV.Video.PixelHeight);
		}
		else if (info->Type == TRACK_TYPE_AUDIO) {
			ebml_master* AudioContext = EBML_MasterAddElt(info->entry, &MATROSKA_ContextAudio, 0);
			ebml_integer* SamplingFrequency = EBML_MasterAddElt(AudioContext, &MATROSKA_ContextSamplingFrequency, 0);
			EBML_IntegerSetValue(SamplingFrequency, info->AV.Audio.SamplingFreq);
			ebml_integer* Channels = EBML_MasterAddElt(AudioContext, &MATROSKA_ContextChannels, 0);
			EBML_IntegerSetValue(Channels, info->AV.Audio.Channels);
		}
	}
	EBML_ElementRender(File->TrackList,File->IOStream,0,0,0,&length);
	File->NextPos+=length;
	File->CueList = EBML_MasterAddElt(File->Segment,&MATROSKA_ContextCues,0);
	if (!(File->flags & MKVF_AVOID_SEEKS)) {
	//  Prepare the Meta Seek with average values
		ebml_master* WMetaSeek = (ebml_master*)EBML_MasterAddElt(File->Segment, &MATROSKA_ContextSeekHead, 0);
		//EBML_MasterUseChecksum(WMetaSeek, !Unsafe);
		EBML_ElementForcePosition((ebml_element*)WMetaSeek, Stream_Seek(File->IOStream, 0, SEEK_CUR)); // keep the position for when we need to write it
		//File->NextPos = 38 + 4 * (Unsafe ? 17 : 23); // raw estimation of the SeekHead size
		File->NextPos = 38 + 4 * 17;
		if (!ARRAYEMPTY(File->Attachments))
			//File->NextPos += Unsafe ? 18 : 24;
			File->NextPos += 18;
		if (!ARRAYEMPTY(File->Chapters))
			//File->NextPos += Unsafe ? 17 : 23;
			File->NextPos += 17;
		// segment info
		matroska_seekpoint* WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek, &MATROSKA_ContextSeek, 0);
		//EBML_MasterUseChecksum((ebml_master*)WSeekPoint, !Unsafe);
		EBML_ElementForcePosition(File->Segment_Info, File->NextPos);
		File->NextPos += EBML_ElementFullSize(File->Segment_Info, 0) + 60; // 60 for the extra string we add
		MATROSKA_LinkMetaSeekElement(WSeekPoint, File->Segment_Info);
		// track info
		if (!ARRAYEMPTY(File->Tracks)) {
			//TODO: Initialize track accoring to array
			WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek, &MATROSKA_ContextSeek, 0);
			//EBML_MasterUseChecksum((ebml_master*)WSeekPoint, !Unsafe);
			EBML_ElementForcePosition(File->TrackList, File->NextPos);
			EBML_ElementUpdateSize(File->TrackList, 0, 0);
			File->NextPos += EBML_ElementFullSize(File->TrackList, 0);
			MATROSKA_LinkMetaSeekElement(WSeekPoint, File->TrackList);
		}
		//else {
		//	NODE_TextWrite(StdErr, T("Warning: the source Segment has no Track Info section (can be a chapter file)\r\n"));
		//}

		// chapters
		if (!ARRAYEMPTY(File->Chapters)) {
			//TODO: Init chapters.
			//ReduceSize((ebml_element*)RChapters);
			//if (EBML_MasterUseChecksum(RChapters, !Unsafe))
			//	EBML_ElementUpdateSize(RChapters, 0, 0);
			WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek, &MATROSKA_ContextSeek, 0);
			//EBML_MasterUseChecksum((ebml_master*)WSeekPoint, !Unsafe);
			EBML_ElementForcePosition(File->ChapterList, File->NextPos);
			File->NextPos += EBML_ElementFullSize(File->ChapterList, 0);
			MATROSKA_LinkMetaSeekElement(WSeekPoint, File->ChapterList);
		}
		// attachments
		if (!ARRAYEMPTY(File->Attachments)) {
			MATROSKA_AttachmentSort(File->AttachmentList);
			//ReduceSize((ebml_element*)File->Attachments);
			//if (EBML_MasterUseChecksum(File->AttachmentList, !Unsafe))
			//	EBML_ElementUpdateSize(RAttachments, 0, 0);
			//if (!EBML_MasterCheckMandatory(RAttachments, 0)) {
			//	NODE_TextWrite(StdErr, T("The Attachments section is missing mandatory elements, skipping\r\n"));
			//	NodeDelete((node*)RAttachments);
			//	RAttachments = NULL;
			//}
			//else {
				WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek, &MATROSKA_ContextSeek, 0);
				//EBML_MasterUseChecksum((ebml_master*)WSeekPoint, !Unsafe);
				EBML_ElementForcePosition(File->AttachmentList, File->NextPos);
				File->NextPos += EBML_ElementFullSize(File->AttachmentList, 0);
				MATROSKA_LinkMetaSeekElement(WSeekPoint, File->AttachmentList);
			//}
		}

		// tags
		if (!ARRAYEMPTY(File->Tags)) {
			//ReduceSize((ebml_element*)RTags);
			//if (EBML_MasterUseChecksum(RTags, !Unsafe))
			//	EBML_ElementUpdateSize(RTags, 0, 0);
			//if (!EBML_MasterCheckMandatory(RTags, 0)) {
			//	NODE_TextWrite(StdErr, T("The Tags section is missing mandatory elements, skipping\r\n"));
			//	NodeDelete((node*)RTags);
			//	RTags = NULL;
			//}
			//else {
				WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek, &MATROSKA_ContextSeek, 0);
				//EBML_MasterUseChecksum((ebml_master*)WSeekPoint, !Unsafe);
				EBML_ElementForcePosition(File->TagList, File->NextPos);
				File->NextPos += EBML_ElementFullSize(File->TagList, 0);
				MATROSKA_LinkMetaSeekElement(WSeekPoint, File->TagList);
			//}
		}
	}
	else {
		char Val = 0x0A;
		int Amount = 134;
		size_t written = 0;
		while (Amount) {
			Stream_Write(File->IOStream, &Val, 1, &written);
			Amount-=written;
		}
		File->NextPos += 134;
	}
	return 0;
}

int mkv_WriteFrame(MatroskaFile* File, int track, ulonglong StartTime, ulonglong EndTime, unsigned int size, void* data, bool_t key_frame, bool_t can_discard)
{
	if(File->CurrentCluster)
		NodeDelete(File->CurrentCluster);
	File->CurrentCluster = EBML_MasterAddElt(File->Segment,&MATROSKA_ContextCluster,0);
	MATROSKA_LinkClusterReadSegmentInfo(File->CurrentCluster,File->Segment_Info,1);
	MATROSKA_LinkClusterWriteSegmentInfo(File->CurrentCluster,File->Segment_Info);
	MATROSKA_ClusterSetTimecode(File->CurrentCluster, StartTime);
/*	ebml_element* SilentTracks = EBML_MasterAddElt(File->CurrentCluster, &MATROSKA_ContextSilentTracks,0);
	for (int i = 0; i < ArraySize(&File->Tracks); ++i) {
		if (i != track) {
			ebml_element* Track_num = EBML_MasterAddElt(SilentTracks,&MATROSKA_ContextSilentTrackNumber,0);
			EBML_IntegerSetValue(Track_num,i);
		}
	}*/
	ebml_element* simpleblock = EBML_MasterAddElt(File->CurrentCluster,&MATROSKA_ContextSimpleBlock,0);
	MATROSKA_LinkBlockReadSegmentInfo(simpleblock, File->Segment_Info,1);
	MATROSKA_LinkBlockWriteSegmentInfo(simpleblock, File->Segment_Info);
	MATROSKA_LinkBlockReadTrack(simpleblock, ARRAYBEGIN(File->Tracks, TrackInfo)[track].entry,1);
	MATROSKA_LinkBlockWriteTrack(simpleblock, ARRAYBEGIN(File->Tracks, TrackInfo)[track].entry);
	MATROSKA_BlockSetTimecode(simpleblock,0,StartTime);
	MATROSKA_BlockSetTimecode(simpleblock,0,StartTime);
	MATROSKA_BlockSetDiscardable(simpleblock,can_discard);
	matroska_frame frame;
	frame.Duration = EndTime-StartTime;
	frame.Size = size;
	frame.Timecode = StartTime;
	frame.Data = data;
	MATROSKA_BlockAppendFrame(simpleblock,&frame,StartTime);
	filepos_t written;
//	EBML_ElementRenderHead(File->CurrentCluster,File->IOStream,1,0,0,&written);
	EBML_ElementRender(File->CurrentCluster,File->IOStream,0,0,0,&written);
	File->NextPos += written;
	NodeDelete(simpleblock);
	return 0;
}

void mkv_WriteTail(MatroskaFile* File)
{
	//maybe write cues here
}

//clean up handles
MATROSKA_DLL void mkv_CloseOutput(MatroskaFile* File)
{
	if (File->CurrentCluster) NodeDelete(File->CurrentCluster);
	for (int i = 0; i < ARRAYCOUNT(File->Tracks, TrackInfo); ++i) {
		NodeDelete(ARRAYBEGIN(File->Tracks,TrackInfo)[i].entry);
	}
	if (File->TrackList) NodeDelete((node*)File->TrackList);
	if (File->CueList) NodeDelete((node*)File->CueList);
	if (File->Segment_Info) NodeDelete((node*)File->Segment_Info);
	if (File->Segment) NodeDelete((node*)File->Segment);
	if(ARRAYCOUNT(File->Tracks, TrackInfo))
		ArrayClear(&File->Tracks);
	OutputStream* io = File->IOStream->io;
	NodeDelete(File->IOStream);
	io->memfree(io,File);
}

static filepos_t Seek(write_stream* p, filepos_t Pos, int SeekMode)
{
	if (!(Pos == 0 && SeekMode == SEEK_CUR))
		p->io->ioseek(p->io, Pos, SeekMode);
	return p->io->iotell(p->io);
}

static err_t Write(write_stream* p, const void* Data, size_t Size, size_t* pWritten)
{
	size_t Written;
	if (!pWritten)
		Written = &Written;
	*pWritten = p->io->iowrite(p->io, Data, Size);
	if (Size && *pWritten != Size)
		return ERR_END_OF_FILE;
	return ERR_NONE;
}

META_START(WriteStream_Class, MKV_WRITE_STREAM_CLASS)
META_CLASS(SIZE, sizeof(write_stream))
META_VMT(TYPE_FUNC, stream_vmt, Seek, Seek)
META_VMT(TYPE_FUNC, stream_vmt, Write, Write)
META_END(STREAM_CLASS)

