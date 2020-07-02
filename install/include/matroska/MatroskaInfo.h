#ifdef _MSC_VER
#pragma once
#endif // 

#ifndef MATROSKA_INFO_H
#define MATROSKA_INFO_H

#include "matroska.h"

typedef int64_t longlong;
typedef uint64_t ulonglong;

typedef struct TrackInfo {
	int Number;
	uint8_t Type;
	uint64_t UID;

	ebml_element* entry;

	uint8_t* CodecPrivate;
	size_t CodecPrivateSize;
	timecode_t DefaultDuration;
	char* CodecID;
	char* Name;
	char Language[4];

	bool_t Enabled;
	bool_t Default;
	bool_t Forced;
	bool_t Lacing;
	bool_t DecodeAll;
	float TimecodeScale;

	int TrackOverlay;
	uint8_t MinCache;
	size_t MaxCache;
	size_t MaxBlockAdditionID;
	size_t SeekPreRoll;
	size_t CodecDelay;

	union {
		struct {
			uint8_t   StereoMode;
			uint8_t   DisplayUnit;
			uint8_t   AspectRatioType;
			uint32_t    PixelWidth;
			uint32_t    PixelHeight;
			uint32_t    DisplayWidth;
			uint32_t    DisplayHeight;
			uint32_t    CropL, CropT, CropR, CropB;
			unsigned int    ColourSpace;
			float           GammaValue;
			//struct {
			unsigned int  Interlaced : 1;
			unsigned int  FieldOrder : 4;
			struct {
				uint8_t MatrixCoefficient;
				uint8_t BitsPerChannel;
				uint8_t ChromaSubsamplingHorz;
				uint8_t ChromaSubsamplingVert;
				uint8_t CbSubsamplingHorz;
				uint8_t CbSubsamplingVert;
				uint8_t ChromaSitingHorz : 2;
				uint8_t ChromaSitingVert : 2;
				uint8_t Range : 2;
				uint8_t TransferCharacteristics;
				uint8_t Primaries;
				uint32_t MaxCLL;
				uint32_t MaxFALL;
			} Colour;
			  //};
		} Video;
		struct {
			float     SamplingFreq;
			float     OutputSamplingFreq;
			uint8_t   Channels;
			uint8_t   BitDepth;
		} Audio;
	} AV;
} TrackInfo;

typedef struct Attachment {
	filepos_t Position;
	filepos_t Length;
	uint64_t UID;
	char* Name;
	char* Description;
	char* MimeType;

} Attachment;

typedef struct ChapterDisplay {
	char* String;
	char Language[4];
	char Country[4];

} ChapterDisplay;

typedef struct Chapter {
	size_t nChildren;
	struct Chapter* Children;
	size_t nDisplay;
	struct ChapterDisplay* Display;

	timecode_t Start;
	timecode_t End;
	uint64_t UID;

	bool_t Enabled;
	bool_t Ordered;
	bool_t Default;
	bool_t Hidden;

	array aChildren; // Chapter
	array aDisplays;  // ChapterDisplay

} Chapter;

typedef struct Target {
	uint64_t UID;
	uint8_t  Type;
	uint8_t  Level;
} Target;
// Tag Target types
#define TARGET_TRACK      0
#define TARGET_CHAPTER    1
#define TARGET_ATTACHMENT 2
#define TARGET_EDITION    3

typedef struct SimpleTag {
	char* Name;
	char* Value;
	char Language[4];
	bool_t Default;
} SimpleTag;

typedef struct Tag {
	size_t nSimpleTags;
	SimpleTag* SimpleTags;
	size_t nTargets;
	struct Target* Targets;
	array aTargets; // Target
	array aSimpleTags; // SimpleTag
} Tag;

typedef struct SegmentInfo {
	uint8_t UID[16];
	uint8_t PrevUID[16];
	uint8_t NextUID[16];
	uint8_t SegmentFamily[16];
	//TODO: Add ChapterTranslate
	char* Filename;
	char* PrevFilename;
	char* NextFilename;
	char* Title;
	char* MuxingApp;
	char* WritingApp;
	timecode_t TimestampScale;
	timecode_t Duration;
	datetime_t DateUTC;
} SegmentInfo;

#define MKV_STREAM_INPUT  1
#define MKV_STREAM_OUTPUT 2
#define MKV_STREAM_NOSEEK 4

typedef struct mkv_stream {
	stream Base;
	void* io;
	int io_flag;
} mkv_stream;

typedef struct MatroskaFile {
	mkv_stream* IOStream;

	//Porfile: Set by user after
	//Open
	int Profile, DocVersion;

	ebml_element* Segment;
	ebml_element* Segment_Info;
	ebml_element* TrackList;
	ebml_element* CueList;
	ebml_element* ChapterList;
	ebml_element* AttachmentList;
	ebml_element* TagList;

	//Seg: Set by user after Open
	SegmentInfo Seg;

	//States used internally
	ebml_parser_context L0Context;
	ebml_parser_context L1Context;

	ebml_element* CurrentCluster;
	matroska_block* CurrentBlock;
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

	int cur_level;

	array Tracks;
	array Tags;
	array Chapters;
	array Attachments;

	filepos_t NextPos;
} MatroskaFile;


#define FRAME_UNKNOWN_START  0x00000001
#define FRAME_UNKNOWN_END    0x00000002
#define FRAME_KF             0x00000004

#define MKVF_AVOID_SEEKS    1 /* use sequential reading only */

#endif // !


