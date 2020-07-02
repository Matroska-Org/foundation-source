/*
 * $Id$
 * Copyright (c) 2008-2010, Matroska (non-profit organisation)
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

#ifndef MATROSKA_PARSER_H
#define MATROSKA_PARSER_H

#include "MatroskaInfo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InputStream
{
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

//#if defined(NO_MATROSKA2_GLOBAL)
	anynode* AnyNode;
	void* ptr;
//#endif

} InputStream;

MATROSKA_DLL MatroskaFile *mkv_OpenInput(InputStream *io, char *err_msg, size_t err_msgSize);
MATROSKA_DLL void mkv_CloseInput(MatroskaFile *File);

MATROSKA_DLL SegmentInfo *mkv_GetFileInfo(MatroskaFile *File);
MATROSKA_DLL size_t mkv_GetNumTracks(MatroskaFile *File);
MATROSKA_DLL TrackInfo *mkv_GetTrackInfo(MatroskaFile *File, size_t n);
MATROSKA_DLL void mkv_SetTrackMask(MatroskaFile *File, int Mask);

MATROSKA_DLL int mkv_ReadFrame(MatroskaFile *File, int mask, unsigned int *track, ulonglong *StartTime, ulonglong *EndTime, ulonglong *FilePos, unsigned int *FrameSize,
                void** FrameRef, unsigned int *FrameFlags);

#define MKVF_SEEK_TO_PREV_KEYFRAME  1

MATROSKA_DLL void mkv_Seek(MatroskaFile *File, timecode_t timecode, int flags);

MATROSKA_DLL void mkv_GetTags(MatroskaFile *File, Tag **, unsigned *Count);
MATROSKA_DLL void mkv_GetAttachments(MatroskaFile *File, Attachment **, unsigned *Count);
MATROSKA_DLL void mkv_GetChapters(MatroskaFile *File, Chapter **, unsigned *Count);

MATROSKA_DLL int mkv_TruncFloat(float f);

#ifdef __cplusplus
}
#endif

#endif /* MATROSKA_PARSER_H */
