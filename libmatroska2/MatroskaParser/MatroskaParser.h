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

#ifndef MATROSKA_PARSER_H
#define MATROSKA_PARSER_H

#include "matroska/matroska.h"

typedef int64_t longlong;
typedef uint64_t ulonglong;

typedef struct InputStream
{
#if defined(NO_MATROSKA2_GLOBAL)
	anynode *AnyNode;
#endif

	int (*read)(struct InputStream *cc, filepos_t pos, void *buffer, size_t count);
	filepos_t (*scan)(struct InputStream *cc, filepos_t start, unsigned signature);
	size_t (*getcachesize)(struct InputStream *cc);
	filepos_t (*getfilesize)(struct InputStream *cc);
	const char *(*geterror)(struct InputStream *cc);
	int (*progress)(struct InputStream *cc, filepos_t cur, filepos_t max);

    int (*ioreadch)(struct InputStream *inf);
    int (*ioread)(struct InputStream *inf,void *buffer,int count);
    void (*ioseek)(struct InputStream *inf,longlong where,int how);
    filepos_t (*iotell)(struct InputStream *inf);
    void* (*makeref)(struct InputStream *inf,int count);
	void (*releaseref)(struct InputStream *inf,void* ref);

	void *(*memalloc)(struct InputStream *cc,size_t size);
	void *(*memrealloc)(struct InputStream *cc,void *mem,size_t newsize);
	void (*memfree)(struct InputStream *cc,void *mem);

} InputStream;

typedef struct TrackInfo
{
	uint8_t *CodecPrivate;
	size_t CodecPrivateSize;
	timecode_t DefaultDuration;
	char *CodecID;
	char *Name;
	char *Language;

  union {
    struct {
      unsigned char   StereoMode;
      unsigned char   DisplayUnit;
      unsigned char   AspectRatioType;
      unsigned int    PixelWidth;
      unsigned int    PixelHeight;
      unsigned int    DisplayWidth;
      unsigned int    DisplayHeight;
      unsigned int    CropL, CropT, CropR, CropB;
      unsigned int    ColourSpace;
      float           GammaValue;
      //struct {
    unsigned int  Interlaced:1;
      //};
    } Video;
    struct {
      float           SamplingFreq;
      float           OutputSamplingFreq;
      unsigned char   Channels;
      unsigned char   BitDepth;
    } Audio;
  } AV;

} TrackInfo;

typedef struct Attachment
{
	char* MimeType;
	filepos_t Position;
	size_t Length;

} Attachment;

typedef struct ChapterDisplay
{
	char *String;

} ChapterDisplay;

typedef struct Chapter
{
	size_t nChildren;
	struct Chapter *Children;
	size_t nDisplay;
	struct ChapterDisplay *Display;

	timecode_t Start;

} Chapter;

typedef struct SimpleTag
{
	char *Name;
	char *Value;

} SimpleTag;

typedef struct Tag
{
	size_t nSimpleTags;
	SimpleTag *SimpleTags;
} Tag;

typedef struct SegmentInfo
{
	float Duration;
	timecode_t TimecodeScale;

	char *Title;
	char *Filename;
	char *PrevFilename;
	char *NextFilename;
	uint8_t UID[16];
	uint8_t PrevUID[16];
	uint8_t NextUID[16];

} SegmentInfo;

typedef struct MatroskaFile MatroskaFile;

MatroskaFile *mkv_Open(InputStream *io, char *err_msg, size_t err_msgSize);
void mkv_Close(MatroskaFile *File);

SegmentInfo *mkv_GetFileInfo(MatroskaFile *File);
size_t mkv_GetNumTracks(MatroskaFile *File);
TrackInfo *mkv_GetTrackInfo(MatroskaFile *File, size_t n);
void mkv_SetTrackMask(MatroskaFile *File, int Mask);

#define FRAME_UNKNOWN_START  0x00000001
#define FRAME_UNKNOWN_END    0x00000002
#define FRAME_KF             0x00000004

size_t mkv_ReadFrame(MatroskaFile *File, int mask, unsigned int *track, ulonglong *StartTime, ulonglong *EndTime, ulonglong *FilePos, unsigned int *FrameSize,
                void** FrameRef, unsigned int *FrameFlags);

#define MKVF_SEEK_TO_PREV_KEYFRAME  1

void mkv_Seek(MatroskaFile *File, timecode_t timecode, int flags);

void mkv_GetTags(MatroskaFile *File, Tag **, unsigned *Count);
void mkv_GetAttachments(MatroskaFile *File, Attachment **, unsigned *Count);
void mkv_GetChapters(MatroskaFile *File, Chapter **, unsigned *Count);

int mkv_TruncFloat(float f);

#endif /* MATROSKA_PARSER_H */
