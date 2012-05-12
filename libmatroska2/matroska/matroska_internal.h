/*
 * $Id$
 * Copyright (c) 2010-2011, Matroska (non-profit organisation)
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

#ifndef LIBMATROSKA2_INTERNAL_H
#define LIBMATROSKA2_INTERNAL_H

/*
 * ONLY INCLUDE THIS FILE IF YOU PLAN TO CREATE YOUR OWN MATROSKA-BASED CLASS
 */

#include "ebml/ebml_internal.h"

#define MATROSKA_BLOCK_READ_TRACK        0x180
#define MATROSKA_BLOCK_READ_SEGMENTINFO  0x181
#if defined(CONFIG_EBML_WRITING)
#define MATROSKA_BLOCK_WRITE_TRACK       0x182
#define MATROSKA_BLOCK_WRITE_SEGMENTINFO 0x183
#endif

struct matroska_block
{
    ebml_binary Base;
	timecode_t GlobalTimecode;
    filepos_t FirstFrameLocation;
    array SizeList; // int32_t
    array SizeListIn; // int32_t
    array Data; // uint8_t
    array Durations; // timecode_t
    ebml_master *ReadTrack;
    ebml_master *ReadSegInfo;
#if defined(CONFIG_EBML_WRITING)
    ebml_master *WriteTrack;
    ebml_master *WriteSegInfo;
#endif
    bool_t IsKeyframe;
    bool_t IsDiscardable;
    bool_t Invisible;
    bool_t LocalTimecodeUsed;
    int16_t LocalTimecode;
    uint16_t TrackNumber;
    char Lacing;
};

#define MATROSKA_BLOCK_CLASS      FOURCC('M','K','B','L')
#define MATROSKA_BLOCKGROUP_CLASS FOURCC('M','K','B','G')
#define MATROSKA_CUEPOINT_CLASS   FOURCC('M','K','C','P')
#define MATROSKA_CLUSTER_CLASS    FOURCC('M','K','C','U')
#define MATROSKA_SEEKPOINT_CLASS  FOURCC('M','K','S','K')
#define MATROSKA_SEGMENTUID_CLASS FOURCC('M','K','I','D')
#define MATROSKA_BIGBINARY_CLASS  FOURCC('M','K','B','B')
#define MATROSKA_ATTACHMENT_CLASS FOURCC('M','K','A','T')
#define MATROSKA_TRACKENTRY_CLASS FOURCC('M','K','T','E')

#endif /* LIBMATROSKA2_INTERNAL_H */
