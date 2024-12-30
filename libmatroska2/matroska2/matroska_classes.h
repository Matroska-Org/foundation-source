/*
 * Copyright (c) 2010-2011, Matroska (non-profit organisation)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBMATROSKA2_CLASSES_H
#define LIBMATROSKA2_CLASSES_H

/*
 * ONLY INCLUDE THIS FILE IF YOU PLAN TO CREATE YOUR OWN MATROSKA-BASED CLASS
 */

#include "ebml2/ebml_classes.h"

#define MATROSKA_BLOCK_READ_TRACK        0x180
#define MATROSKA_BLOCK_READ_SEGMENTINFO  0x181
#if defined(CONFIG_EBML_WRITING)
#define MATROSKA_BLOCK_WRITE_TRACK       0x182
#define MATROSKA_BLOCK_WRITE_SEGMENTINFO 0x183
#endif

struct matroska_block
{
    ebml_binary Base;
    mkv_timestamp_t GlobalTimestamp;
    filepos_t FirstFrameLocation;
    array SizeList; // int32_t
    array SizeListIn; // int32_t
    array Data; // uint8_t
    array Durations; // mkv_timestamp_t
    ebml_master *ReadTrack;
    ebml_master *ReadSegInfo;
#if defined(CONFIG_EBML_WRITING)
    ebml_master *WriteTrack;
    ebml_master *WriteSegInfo;
#endif
    bool_t IsKeyframe;
    bool_t IsDiscardable;
    bool_t Invisible;
    bool_t LocalTimestampUsed;
    int16_t LocalTimestamp;
    uint16_t TrackNumber;
    char Lacing;
};

#endif /* LIBMATROSKA2_CLASSES_H */
