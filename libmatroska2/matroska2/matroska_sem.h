/*
 * DO NOT EDIT, GENERATED WITH schema_2_matroska_sem_h.xsl
 * https://github.com/Matroska-Org/foundation-source/tree/master/spectool
 *
 * $Id$
 * Copyright (c) 2008-2022, Matroska (non-profit organisation)
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
#include "matroska2/matroska.h"

#ifndef MATROSKA_SEMANTIC_H
#define MATROSKA_SEMANTIC_H

void MATROSKA_InitSemantic();

MATROSKA_DLL const ebml_context *MATROSKA_getContextSeekID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSeekPosition();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSeek();

MATROSKA_DLL const ebml_context *MATROSKA_getContextSeekHead();

MATROSKA_DLL const ebml_context *MATROSKA_getContextSegmentUUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSegmentFilename();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrevUUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrevFilename();
MATROSKA_DLL const ebml_context *MATROSKA_getContextNextUUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextNextFilename();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSegmentFamily();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTranslateID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTranslateCodec();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTranslateEditionUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTranslate();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTimestampScale();
MATROSKA_DLL const ebml_context *MATROSKA_getContextDuration();
MATROSKA_DLL const ebml_context *MATROSKA_getContextDateUTC();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTitle();
MATROSKA_DLL const ebml_context *MATROSKA_getContextMuxingApp();
MATROSKA_DLL const ebml_context *MATROSKA_getContextWritingApp();
MATROSKA_DLL const ebml_context *MATROSKA_getContextInfo();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTimestamp();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSilentTrackNumber();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSilentTracks();

MATROSKA_DLL const ebml_context *MATROSKA_getContextPosition();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrevSize();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSimpleBlock();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlock();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockVirtual();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAdditional();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAddID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockMore();

MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAdditions();

MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockDuration();
MATROSKA_DLL const ebml_context *MATROSKA_getContextReferencePriority();
MATROSKA_DLL const ebml_context *MATROSKA_getContextReferenceBlock();
MATROSKA_DLL const ebml_context *MATROSKA_getContextReferenceVirtual();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCodecState();
MATROSKA_DLL const ebml_context *MATROSKA_getContextDiscardPadding();
MATROSKA_DLL const ebml_context *MATROSKA_getContextLaceNumber();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFrameNumber();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAdditionID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextDelay();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSliceDuration();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTimeSlice();

MATROSKA_DLL const ebml_context *MATROSKA_getContextSlices();

MATROSKA_DLL const ebml_context *MATROSKA_getContextReferenceOffset();
MATROSKA_DLL const ebml_context *MATROSKA_getContextReferenceTimestamp();
MATROSKA_DLL const ebml_context *MATROSKA_getContextReferenceFrame();

MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockGroup();

MATROSKA_DLL const ebml_context *MATROSKA_getContextEncryptedBlock();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCluster();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackNumber();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackType();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFlagEnabled();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFlagDefault();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFlagForced();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFlagHearingImpaired();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFlagVisualImpaired();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFlagTextDescriptions();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFlagOriginal();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFlagCommentary();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFlagLacing();
MATROSKA_DLL const ebml_context *MATROSKA_getContextMinCache();
MATROSKA_DLL const ebml_context *MATROSKA_getContextMaxCache();
MATROSKA_DLL const ebml_context *MATROSKA_getContextDefaultDuration();
MATROSKA_DLL const ebml_context *MATROSKA_getContextDefaultDecodedFieldDuration();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackTimestampScale();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackOffset();
MATROSKA_DLL const ebml_context *MATROSKA_getContextMaxBlockAdditionID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAddIDValue();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAddIDName();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAddIDType();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAddIDExtraData();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAdditionMapping();

MATROSKA_DLL const ebml_context *MATROSKA_getContextName();
MATROSKA_DLL const ebml_context *MATROSKA_getContextLanguage();
MATROSKA_DLL const ebml_context *MATROSKA_getContextLanguageBCP47();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCodecID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCodecPrivate();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCodecName();
MATROSKA_DLL const ebml_context *MATROSKA_getContextAttachmentLink();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCodecSettings();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCodecInfoURL();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCodecDownloadURL();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCodecDecodeAll();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackOverlay();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCodecDelay();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSeekPreRoll();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackTranslateTrackID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackTranslateCodec();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackTranslateEditionUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackTranslate();

MATROSKA_DLL const ebml_context *MATROSKA_getContextFlagInterlaced();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFieldOrder();
MATROSKA_DLL const ebml_context *MATROSKA_getContextStereoMode();
MATROSKA_DLL const ebml_context *MATROSKA_getContextAlphaMode();
MATROSKA_DLL const ebml_context *MATROSKA_getContextOldStereoMode();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPixelWidth();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPixelHeight();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPixelCropBottom();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPixelCropTop();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPixelCropLeft();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPixelCropRight();
MATROSKA_DLL const ebml_context *MATROSKA_getContextDisplayWidth();
MATROSKA_DLL const ebml_context *MATROSKA_getContextDisplayHeight();
MATROSKA_DLL const ebml_context *MATROSKA_getContextDisplayUnit();
MATROSKA_DLL const ebml_context *MATROSKA_getContextAspectRatioType();
MATROSKA_DLL const ebml_context *MATROSKA_getContextUncompressedFourCC();
MATROSKA_DLL const ebml_context *MATROSKA_getContextGammaValue();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFrameRate();
MATROSKA_DLL const ebml_context *MATROSKA_getContextMatrixCoefficients();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBitsPerChannel();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChromaSubsamplingHorz();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChromaSubsamplingVert();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCbSubsamplingHorz();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCbSubsamplingVert();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChromaSitingHorz();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChromaSitingVert();
MATROSKA_DLL const ebml_context *MATROSKA_getContextRange();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTransferCharacteristics();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrimaries();
MATROSKA_DLL const ebml_context *MATROSKA_getContextMaxCLL();
MATROSKA_DLL const ebml_context *MATROSKA_getContextMaxFALL();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrimaryRChromaticityX();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrimaryRChromaticityY();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrimaryGChromaticityX();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrimaryGChromaticityY();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrimaryBChromaticityX();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrimaryBChromaticityY();
MATROSKA_DLL const ebml_context *MATROSKA_getContextWhitePointChromaticityX();
MATROSKA_DLL const ebml_context *MATROSKA_getContextWhitePointChromaticityY();
MATROSKA_DLL const ebml_context *MATROSKA_getContextLuminanceMax();
MATROSKA_DLL const ebml_context *MATROSKA_getContextLuminanceMin();
MATROSKA_DLL const ebml_context *MATROSKA_getContextMasteringMetadata();

MATROSKA_DLL const ebml_context *MATROSKA_getContextColour();

MATROSKA_DLL const ebml_context *MATROSKA_getContextProjectionType();
MATROSKA_DLL const ebml_context *MATROSKA_getContextProjectionPrivate();
MATROSKA_DLL const ebml_context *MATROSKA_getContextProjectionPoseYaw();
MATROSKA_DLL const ebml_context *MATROSKA_getContextProjectionPosePitch();
MATROSKA_DLL const ebml_context *MATROSKA_getContextProjectionPoseRoll();
MATROSKA_DLL const ebml_context *MATROSKA_getContextProjection();

MATROSKA_DLL const ebml_context *MATROSKA_getContextVideo();

MATROSKA_DLL const ebml_context *MATROSKA_getContextSamplingFrequency();
MATROSKA_DLL const ebml_context *MATROSKA_getContextOutputSamplingFrequency();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChannels();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChannelPositions();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBitDepth();
MATROSKA_DLL const ebml_context *MATROSKA_getContextEmphasis();
MATROSKA_DLL const ebml_context *MATROSKA_getContextAudio();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackPlaneUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackPlaneType();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackPlane();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackCombinePlanes();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackJoinUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackJoinBlocks();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackOperation();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTrickTrackUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrickTrackSegmentUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrickTrackFlag();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrickMasterTrackUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrickMasterTrackSegmentUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentEncodingOrder();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentEncodingScope();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentEncodingType();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentCompAlgo();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentCompSettings();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentCompression();

MATROSKA_DLL const ebml_context *MATROSKA_getContextContentEncAlgo();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentEncKeyID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextAESSettingsCipherMode();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentEncAESSettings();

MATROSKA_DLL const ebml_context *MATROSKA_getContextContentSignature();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentSigKeyID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentSigAlgo();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentSigHashAlgo();
MATROSKA_DLL const ebml_context *MATROSKA_getContextContentEncryption();

MATROSKA_DLL const ebml_context *MATROSKA_getContextContentEncoding();

MATROSKA_DLL const ebml_context *MATROSKA_getContextContentEncodings();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackEntry();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTracks();

MATROSKA_DLL const ebml_context *MATROSKA_getContextCueTime();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueTrack();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueClusterPosition();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueRelativePosition();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueDuration();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueBlockNumber();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueCodecState();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueRefTime();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueRefCluster();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueRefNumber();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueRefCodecState();
MATROSKA_DLL const ebml_context *MATROSKA_getContextCueReference();

MATROSKA_DLL const ebml_context *MATROSKA_getContextCueTrackPositions();

MATROSKA_DLL const ebml_context *MATROSKA_getContextCuePoint();

MATROSKA_DLL const ebml_context *MATROSKA_getContextCues();

MATROSKA_DLL const ebml_context *MATROSKA_getContextFileDescription();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFileName();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFileMediaType();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFileData();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFileUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFileReferral();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFileUsedStartTime();
MATROSKA_DLL const ebml_context *MATROSKA_getContextFileUsedEndTime();
MATROSKA_DLL const ebml_context *MATROSKA_getContextAttachedFile();

MATROSKA_DLL const ebml_context *MATROSKA_getContextAttachments();

MATROSKA_DLL const ebml_context *MATROSKA_getContextEditionUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextEditionFlagHidden();
MATROSKA_DLL const ebml_context *MATROSKA_getContextEditionFlagDefault();
MATROSKA_DLL const ebml_context *MATROSKA_getContextEditionFlagOrdered();
MATROSKA_DLL const ebml_context *MATROSKA_getContextEditionString();
MATROSKA_DLL const ebml_context *MATROSKA_getContextEditionLanguageIETF();
MATROSKA_DLL const ebml_context *MATROSKA_getContextEditionDisplay();

MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterStringUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTimeStart();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTimeEnd();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterFlagHidden();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterFlagEnabled();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterSegmentUUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterSkipType();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterSegmentEditionUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterPhysicalEquiv();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTrackNumber();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTrack();

MATROSKA_DLL const ebml_context *MATROSKA_getContextChapString();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapLanguage();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapLanguageBCP47();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapCountry();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterDisplay();

MATROSKA_DLL const ebml_context *MATROSKA_getContextChapProcessCodecID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapProcessPrivate();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapProcessTime();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapProcessData();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapProcessCommand();

MATROSKA_DLL const ebml_context *MATROSKA_getContextChapProcess();

MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterAtom();

MATROSKA_DLL const ebml_context *MATROSKA_getContextEditionEntry();

MATROSKA_DLL const ebml_context *MATROSKA_getContextChapters();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTargetTypeValue();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTargetType();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagTrackUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagEditionUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagChapterUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagAttachmentUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTargets();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTagName();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagLanguage();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagLanguageBCP47();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagDefault();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagDefaultBogus();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagString();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagBinary();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSimpleTag();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTag();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTags();

MATROSKA_DLL const ebml_context *MATROSKA_getContextSegment();

/**
 *The `TrackType` defines the type of each frame found in the Track.
The value **SHOULD** be stored on 1 octet.
 */
typedef enum {
  MATROSKA_TRACK_TYPE_VIDEO            = 0x1, // An image.
  MATROSKA_TRACK_TYPE_AUDIO            = 0x2, // Audio samples.
  MATROSKA_TRACK_TYPE_COMPLEX          = 0x3, // A mix of different other TrackType. The codec needs to define how the `Matroska Player` should interpret such data.
  MATROSKA_TRACK_TYPE_LOGO             = 0x10, // An image to be rendered over the video track(s).
  MATROSKA_TRACK_TYPE_SUBTITLE         = 0x11, // Subtitle or closed caption data to be rendered over the video track(s).
  MATROSKA_TRACK_TYPE_BUTTONS          = 0x12, // Interactive button(s) to be rendered over the video track(s).
  MATROSKA_TRACK_TYPE_CONTROL          = 0x20, // Metadata used to control the player of the `Matroska Player`.
  MATROSKA_TRACK_TYPE_METADATA         = 0x21, // Timed metadata that can be passed on to the `Matroska Player`.
} MatroskaTrackType;

/**
 *The compression algorithm used.
 */
typedef enum {
  MATROSKA_TRACK_ENCODING_COMP_NONE             = -1,
  MATROSKA_TRACK_ENCODING_COMP_ZLIB             = 0, // zlib compression (RFC1950).
  MATROSKA_TRACK_ENCODING_COMP_BZLIB            = 1, // bzip2 compression (BZIP2), **SHOULD NOT** be used.
  MATROSKA_TRACK_ENCODING_COMP_LZO1X            = 2, // Lempel-Ziv-Oberhumer compression (LZO), **SHOULD NOT** be used.
  MATROSKA_TRACK_ENCODING_COMP_HEADERSTRIP      = 3, // Octets in `ContentCompSettings` ((#contentcompsettings-element)) have been stripped from each frame.
} MatroskaTrackEncodingCompAlgo;

/**
 *This `ChapterTranslate` applies to this chapter codec of the given chapter edition(s); see (#chapprocesscodecid-element).
 */
typedef enum {
  MATROSKA_CHAPTERTRANSLATECODEC_MATROSKA_SCRIPT  = 0, // Chapter commands using the Matroska Script codec.
  MATROSKA_CHAPTERTRANSLATECODEC_DVD_MENU         = 1, // Chapter commands using the DVD-like codec.
} MatroskaChapterTranslateCodec;

/**
 *This `TrackTranslate` applies to this chapter codec of the given chapter edition(s); see (#chapprocesscodecid-element).
 */
typedef enum {
  MATROSKA_TRACKTRANSLATECODEC_MATROSKA_SCRIPT  = 0, // Chapter commands using the Matroska Script codec.
  MATROSKA_TRACKTRANSLATECODEC_DVD_MENU         = 1, // Chapter commands using the DVD-like codec.
} MatroskaTrackTranslateCodec;

/**
 *Specify whether the video frames in this track are interlaced.
 */
typedef enum {
  MATROSKA_VIDEO_FLAGINTERLACED_UNDETERMINED     = 0, // Unknown status.
  MATROSKA_VIDEO_FLAGINTERLACED_INTERLACED       = 1, // Interlaced frames.
  MATROSKA_VIDEO_FLAGINTERLACED_PROGRESSIVE      = 2, // No interlacing.
} MatroskaVideoFlagInterlaced;

/**
 *Specify the field ordering of video frames in this track.
 */
typedef enum {
  MATROSKA_VIDEO_FIELDORDER_PROGRESSIVE      = 0, // Interlaced frames.
  MATROSKA_VIDEO_FIELDORDER_TOPFIELDFIRST    = 1, // Top field displayed first. Top field stored first.
  MATROSKA_VIDEO_FIELDORDER_UNDETERMINED     = 2, // Unknown field order.
  MATROSKA_VIDEO_FIELDORDER_BOTTOMFIELDFIRST = 6, // Bottom field displayed first. Bottom field stored first.
  MATROSKA_VIDEO_FIELDORDER_BOTTOMFIELDSWAPPED = 9, // Top field displayed first. Fields are interleaved in storage with the top line of the top field stored first.
  MATROSKA_VIDEO_FIELDORDER_TOPFIELDSWAPPED  = 14, // Bottom field displayed first. Fields are interleaved in storage with the top line of the top field stored first.
} MatroskaVideoFieldOrder;

/**
 *Stereo-3D video mode. There are some more details in (#multi-planar-and-3d-videos).
 */
typedef enum {
  MATROSKA_VIDEO_STEREO_MONO             = 0,
  MATROSKA_VIDEO_STEREO_LEFT_RIGHT       = 1,
  MATROSKA_VIDEO_STEREO_BOTTOM_TOP       = 2,
  MATROSKA_VIDEO_STEREO_TOP_BOTTOM       = 3,
  MATROSKA_VIDEO_STEREO_CHECKBOARD_RL    = 4,
  MATROSKA_VIDEO_STEREO_CHECKBOARD_LR    = 5,
  MATROSKA_VIDEO_STEREO_ROW_INTERLEAVED_RL = 6,
  MATROSKA_VIDEO_STEREO_ROW_INTERLEAVED_LR = 7,
  MATROSKA_VIDEO_STEREO_COL_INTERLEAVED_RL = 8,
  MATROSKA_VIDEO_STEREO_COL_INTERLEAVED_LR = 9,
  MATROSKA_VIDEO_STEREO_ANAGLYPH_CYAN_RED = 10,
  MATROSKA_VIDEO_STEREO_RIGHT_LEFT       = 11,
  MATROSKA_VIDEO_STEREO_ANAGLYPH_GREEN_MAG = 12,
  MATROSKA_VIDEO_STEREO_BOTH_EYES_BLOCK_LR = 13,
  MATROSKA_VIDEO_STEREO_BOTH_EYES_BLOCK_RL = 14,
} MatroskaVideoStereoMode;

/**
 *Indicate whether the BlockAdditional Element with BlockAddID of "1" contains Alpha data, as defined by to the Codec Mapping for the `CodecID`.
Undefined values **SHOULD NOT** be used as the behavior of known implementations is different (considered either as 0 or 1).
 */
typedef enum {
  MATROSKA_VIDEO_ALPHAMODE_NONE             = 0, // The BlockAdditional Element with BlockAddID of "1" does not exist or **SHOULD NOT** be considered as containing such data.
  MATROSKA_VIDEO_ALPHAMODE_PRESENT          = 1, // The BlockAdditional Element with BlockAddID of "1" contains alpha channel data.
} MatroskaVideoAlphaMode;

/**
 *Bogus StereoMode value used in old versions of libmatroska.
 */
typedef enum {
  MATROSKA_VIDEO_OLDSTEREOMODE_MONO             = 0,
  MATROSKA_VIDEO_OLDSTEREOMODE_RIGHT_EYE        = 1,
  MATROSKA_VIDEO_OLDSTEREOMODE_LEFT_EYE         = 2,
  MATROSKA_VIDEO_OLDSTEREOMODE_BOTH_EYES        = 3,
} MatroskaVideoOldStereoMode;

/**
 *How DisplayWidth & DisplayHeight are interpreted.
 */
typedef enum {
  MATROSKA_DISPLAY_UNIT_PIXELS           = 0,
  MATROSKA_DISPLAY_UNIT_CENTIMETERS      = 1,
  MATROSKA_DISPLAY_UNIT_INCHES           = 2,
  MATROSKA_DISPLAY_UNIT_DISPLAYASPECTRATIO = 3,
  MATROSKA_DISPLAY_UNIT_UNKNOWN          = 4,
} MatroskaVideoDisplayUnit;

/**
 *Specify the possible modifications to the aspect ratio.
 */
typedef enum {
  MATROSKA_VIDEO_ASPECTRATIOTYPE_FREE_RESIZING    = 0,
  MATROSKA_VIDEO_ASPECTRATIOTYPE_KEEP_ASPECT_RATIO = 1,
  MATROSKA_VIDEO_ASPECTRATIOTYPE_FIXED            = 2,
} MatroskaVideoAspectRatioType;

/**
 *The Matrix Coefficients of the video used to derive luma and chroma values from red, green, and blue color primaries.
For clarity, the value and meanings for MatrixCoefficients are adopted from Table 4 of ISO/IEC 23001-8:2016 or ITU-T H.273.
 */
typedef enum {
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_IDENTITY         = 0,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_BT709            = 1,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_UNSPECIFIED      = 2,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_RESERVED         = 3,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_US_FCC_73_682    = 4,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_BT470BG          = 5,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_SMPTE_170M       = 6,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_SMPTE_240M       = 7,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_YCOCG            = 8,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_BT2020_NCL       = 9,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_BT2020_CL        = 10,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_SMPTE_2085       = 11,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_CHROMA_DERIVED_NCL = 12,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_CHROMA_DERIVED_CL = 13,
  MATROSKA_VIDEO_MATRIXCOEFFICIENTS_ITU_R_BT_2100_0  = 14,
} MatroskaVideoMatrixCoefficients;

/**
 *How chroma is subsampled horizontally.
 */
typedef enum {
  MATROSKA_VIDEO_CHROMASITINGHORZ_UNSPECIFIED      = 0,
  MATROSKA_VIDEO_CHROMASITINGHORZ_LEFT             = 1,
  MATROSKA_VIDEO_CHROMASITINGHORZ_HALF             = 2,
} MatroskaColourChromaSitingHorz;

/**
 *How chroma is subsampled vertically.
 */
typedef enum {
  MATROSKA_VIDEO_CHROMASITINGVERT_UNSPECIFIED      = 0,
  MATROSKA_VIDEO_CHROMASITINGVERT_TOP              = 1,
  MATROSKA_VIDEO_CHROMASITINGVERT_HALF             = 2,
} MatroskaColourChromaSitingVert;

/**
 *Clipping of the color ranges.
 */
typedef enum {
  MATROSKA_VIDEO_RANGE_UNSPECIFIED      = 0,
  MATROSKA_VIDEO_RANGE_BROADCAST_RANGE  = 1,
  MATROSKA_VIDEO_RANGE_FULL_RANGE       = 2,
  MATROSKA_VIDEO_RANGE_DEFINED_BY_MATRIXCOEFFICIENTS = 3,
} MatroskaVideoRange;

/**
 *The transfer characteristics of the video. For clarity,
the value and meanings for TransferCharacteristics are adopted from Table 3 of ISO/IEC 23091-4 or ITU-T H.273.
 */
typedef enum {
  MATROSKA_TRANSFER_RESERVED         = 0,
  MATROSKA_TRANSFER_BT709            = 1,
  MATROSKA_TRANSFER_UNSPECIFIED      = 2,
  MATROSKA_TRANSFER_RESERVED2        = 3,
  MATROSKA_TRANSFER_GAMMA22          = 4,
  MATROSKA_TRANSFER_GAMMA28          = 5,
  MATROSKA_TRANSFER_SMPTE_170M       = 6,
  MATROSKA_TRANSFER_SMPTE_240M       = 7,
  MATROSKA_TRANSFER_LINEAR           = 8,
  MATROSKA_TRANSFER_LOG              = 9,
  MATROSKA_TRANSFER_LOG_SQRT         = 10,
  MATROSKA_TRANSFER_IEC_61966_2_4    = 11,
  MATROSKA_TRANSFER_BT1361_ECG       = 12,
  MATROSKA_TRANSFER_IEC_61966_2_1    = 13,
  MATROSKA_TRANSFER_BT2020_10_BIT    = 14,
  MATROSKA_TRANSFER_BT2020_12_BIT    = 15,
  MATROSKA_TRANSFER_BT2100_PQ        = 16,
  MATROSKA_TRANSFER_SMPTE_428_1      = 17,
  MATROSKA_TRANSFER_ARIB_STD_B67     = 18,
} MatroskaVideoTransferCharacteristics;

/**
 *The colour primaries of the video. For clarity,
the value and meanings for Primaries are adopted from Table 2 of ISO/IEC 23091-4 or ITU-T H.273.
 */
typedef enum {
  MATROSKA_VIDEO_PRIMARIES_RESERVED         = 0,
  MATROSKA_VIDEO_PRIMARIES_BT709            = 1,
  MATROSKA_VIDEO_PRIMARIES_UNSPECIFIED      = 2,
  MATROSKA_VIDEO_PRIMARIES_RESERVED2        = 3,
  MATROSKA_VIDEO_PRIMARIES_BT470M           = 4,
  MATROSKA_VIDEO_PRIMARIES_BT470BG          = 5,
  MATROSKA_VIDEO_PRIMARIES_BT601_525        = 6,
  MATROSKA_VIDEO_PRIMARIES_SMPTE_240M       = 7,
  MATROSKA_VIDEO_PRIMARIES_FILM             = 8,
  MATROSKA_VIDEO_PRIMARIES_BT2020           = 9,
  MATROSKA_VIDEO_PRIMARIES_SMPTE_428_1      = 10,
  MATROSKA_VIDEO_PRIMARIES_SMPTE_RP_432_2   = 11,
  MATROSKA_VIDEO_PRIMARIES_SMPTE_EG_432_2   = 12,
  MATROSKA_VIDEO_PRIMARIES_JEDEC_P22        = 22,
} MatroskaVideoPrimaries;

/**
 *Describes the projection used for this video track.
 */
typedef enum {
  MATROSKA_VIDEO_PROJECTIONTYPE_RECTANGULAR      = 0,
  MATROSKA_VIDEO_PROJECTIONTYPE_EQUIRECTANGULAR  = 1,
  MATROSKA_VIDEO_PROJECTIONTYPE_CUBEMAP          = 2,
  MATROSKA_VIDEO_PROJECTIONTYPE_MESH             = 3,
} MatroskaVideoProjectionType;

/**
 *Audio emphasis applied on audio samples. The player **MUST** apply the inverse emphasis to get the proper audio samples.
 */
typedef enum {
  MATROSKA_EMPHASIS_NO_EMPHASIS      = 0,
  MATROSKA_EMPHASIS_CD_AUDIO         = 1, // First order filter with zero point at 50 microseconds and a pole at 15 microseconds. Also found on DVD Audio and MPEG audio.
  MATROSKA_EMPHASIS_RESERVED         = 2,
  MATROSKA_EMPHASIS_CCIT_J_17        = 3, // Defined in (ITU-J.17).
  MATROSKA_EMPHASIS_FM_50            = 4, // FM Radio in Europe. RC Filter with a time constant of 50 microseconds.
  MATROSKA_EMPHASIS_FM_75            = 5, // FM Radio in the USA. RC Filter with a time constant of 75 microseconds.
  MATROSKA_EMPHASIS_PHONO_RIAA       = 10, // Phono filter with time constants of t1=3180, t2=318 and t3=75 microseconds. (NAB1964)
  MATROSKA_EMPHASIS_PHONO_IEC_N78    = 11, // Phono filter with time constants of t1=3180, t2=450 and t3=50 microseconds.
  MATROSKA_EMPHASIS_PHONO_TELDEC     = 12, // Phono filter with time constants of t1=3180, t2=318 and t3=50 microseconds.
  MATROSKA_EMPHASIS_PHONO_EMI        = 13, // Phono filter with time constants of t1=2500, t2=500 and t3=70 microseconds.
  MATROSKA_EMPHASIS_PHONO_COLUMBIA_LP = 14, // Phono filter with time constants of t1=1590, t2=318 and t3=100 microseconds.
  MATROSKA_EMPHASIS_PHONO_LONDON     = 15, // Phono filter with time constants of t1=1590, t2=318 and t3=50 microseconds.
  MATROSKA_EMPHASIS_PHONO_NARTB      = 16, // Phono filter with time constants of t1=3180, t2=318 and t3=100 microseconds.
} MatroskaAudioEmphasis;

/**
 *The kind of plane this track corresponds to.
 */
typedef enum {
  MATROSKA_TRACKPLANETYPE_LEFT_EYE         = 0,
  MATROSKA_TRACKPLANETYPE_RIGHT_EYE        = 1,
  MATROSKA_TRACKPLANETYPE_BACKGROUND       = 2,
} MatroskaTrackPlaneType;

/**
 *A bit field that describes which Elements have been modified in this way.
Values (big-endian) can be OR'ed.
 */
typedef enum {
  MATROSKA_CONTENTENCODINGSCOPE_BLOCK            = 1, // All frame contents, excluding lacing data.
  MATROSKA_CONTENTENCODINGSCOPE_PRIVATE          = 2, // The track's private data.
  MATROSKA_CONTENTENCODINGSCOPE_NEXT             = 4, // The next ContentEncoding (next `ContentEncodingOrder`. Either the data inside `ContentCompression` and/or `ContentEncryption`).
} MatroskaContentEncodingScope;

/**
 *A value describing what kind of transformation is applied.
 */
typedef enum {
  MATROSKA_CONTENTENCODINGTYPE_COMPRESSION      = 0,
  MATROSKA_CONTENTENCODINGTYPE_ENCRYPTION       = 1,
} MatroskaContentEncodingType;

/**
 *The encryption algorithm used.
 */
typedef enum {
  MATROSKA_CONTENTENCALGO_NOT_ENCRYPTED    = 0, // The data are not encrypted.
  MATROSKA_CONTENTENCALGO_DES              = 1, // Data Encryption Standard (DES) (FIPS.46-3).
  MATROSKA_CONTENTENCALGO_3DES             = 2, // Triple Data Encryption Algorithm (SP.800-67).
  MATROSKA_CONTENTENCALGO_TWOFISH          = 3, // Twofish Encryption Algorithm (Twofish).
  MATROSKA_CONTENTENCALGO_BLOWFISH         = 4, // Blowfish Encryption Algorithm (Blowfish).
  MATROSKA_CONTENTENCALGO_AES              = 5, // Advanced Encryption Standard (AES) (FIPS.197).
} MatroskaContentEncodingAlgo;

/**
 *The AES cipher mode used in the encryption.
 */
typedef enum {
  MATROSKA_AESSETTINGSCIPHERMODE_AES_CTR          = 1, // Counter (SP.800-38A).
  MATROSKA_AESSETTINGSCIPHERMODE_AES_CBC          = 2, // Cipher Block Chaining (SP.800-38A).
} MatroskaAESSettingsCipherMode;

/**
 *The algorithm used for the signature.
 */
typedef enum {
  MATROSKA_CONTENTSIGALGO_NOT_SIGNED       = 0,
  MATROSKA_CONTENTSIGALGO_RSA              = 1,
} MatroskaContentSignatureAlgo;

/**
 *The hash algorithm used for the signature.
 */
typedef enum {
  MATROSKA_CONTENTSIGHASHALGO_NOT_SIGNED       = 0,
  MATROSKA_CONTENTSIGHASHALGO_SHA1_160         = 1,
  MATROSKA_CONTENTSIGHASHALGO_MD5              = 2,
} MatroskaContentSigHashAlgo;

/**
 *Indicate what type of content the ChapterAtom contains and might be skipped. It can be used to automatically skip content based on the type.
If a `ChapterAtom` is inside a `ChapterAtom` that has a `ChapterSkipType` set, it **MUST NOT** have a `ChapterSkipType` or have a `ChapterSkipType` with the same value as it's parent `ChapterAtom`.
If the `ChapterAtom` doesn't contain a `ChapterTimeEnd`, the value of the `ChapterSkipType` is only valid until the next `ChapterAtom` with a `ChapterSkipType` value or the end of the file.
    
 */
typedef enum {
  MATROSKA_CHAPTERSKIPTYPE_NO_SKIPPING      = 0, // Content which should not be skipped.
  MATROSKA_CHAPTERSKIPTYPE_OPENING_CREDITS  = 1, // Credits usually found at the beginning of the content.
  MATROSKA_CHAPTERSKIPTYPE_END_CREDITS      = 2, // Credits usually found at the end of the content.
  MATROSKA_CHAPTERSKIPTYPE_RECAP            = 3, // Recap of previous episodes of the content, usually found around the beginning.
  MATROSKA_CHAPTERSKIPTYPE_NEXT_PREVIEW     = 4, // Preview of the next episode of the content, usually found around the end. It may contain spoilers the user wants to avoid.
  MATROSKA_CHAPTERSKIPTYPE_PREVIEW          = 5, // Preview of the current episode of the content, usually found around the beginning. It may contain spoilers the user want to avoid.
  MATROSKA_CHAPTERSKIPTYPE_ADVERTISEMENT    = 6, // Advertisement within the content.
} MatroskaChapterSkipType;

/**
 *Defines when the process command **SHOULD** be handled
 */
typedef enum {
  MATROSKA_CHAPPROCESSTIME_DURING           = 0,
  MATROSKA_CHAPPROCESSTIME_BEFORE           = 1,
  MATROSKA_CHAPPROCESSTIME_AFTER            = 2,
} MatroskaChapterProcessTime;

/**
 *A number to indicate the logical level of the target.
 */
typedef enum {
  MATROSKA_TARGET_TYPE_COLLECTION       = 70, // The highest hierarchical level that tags can describe.
  MATROSKA_TARGET_TYPE_EDITION          = 60, // A list of lower levels grouped together.
  MATROSKA_TARGET_TYPE_ALBUM            = 50, // The most common grouping level of music and video (equals to an episode for TV series).
  MATROSKA_TARGET_TYPE_PART             = 40, // When an album or episode has different logical parts.
  MATROSKA_TARGET_TYPE_TRACK            = 30, // The common parts of an album or movie.
  MATROSKA_TARGET_TYPE_SUBTRACK         = 20, // Corresponds to parts of a track for audio (like a movement).
  MATROSKA_TARGET_TYPE_SHOT             = 10, // The lowest hierarchy found in music or movies.
} MatroskaTargetTypeValue;

#endif // MATROSKA_SEMANTIC_H
