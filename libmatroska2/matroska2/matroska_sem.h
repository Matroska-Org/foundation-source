/*
 * DO NOT EDIT, GENERATED WITH schema_2_matroska_sem_h.xsl
 * https://github.com/Matroska-Org/foundation-source/tree/master/spectool
 *
 * $Id$
 * Copyright (c) 2008-2017, Matroska (non-profit organisation)
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

MATROSKA_DLL const ebml_context *MATROSKA_getContextSegmentUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSegmentFilename();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrevUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextPrevFilename();
MATROSKA_DLL const ebml_context *MATROSKA_getContextNextUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextNextFilename();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSegmentFamily();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTranslateEditionUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTranslateCodec();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTranslateID();
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
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAddID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextBlockAdditional();
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
MATROSKA_DLL const ebml_context *MATROSKA_getContextLanguageIETF();
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
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackTranslateEditionUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackTranslateCodec();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTrackTranslateTrackID();
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
MATROSKA_DLL const ebml_context *MATROSKA_getContextColourSpace();
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
MATROSKA_DLL const ebml_context *MATROSKA_getContextFileMimeType();
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
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterStringUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTimeStart();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTimeEnd();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterFlagHidden();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterFlagEnabled();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterSegmentUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterSegmentEditionUID();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterPhysicalEquiv();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTrackNumber();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapterTrack();

MATROSKA_DLL const ebml_context *MATROSKA_getContextChapString();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapLanguage();
MATROSKA_DLL const ebml_context *MATROSKA_getContextChapLanguageIETF();
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
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagLanguageIETF();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagDefault();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagString();
MATROSKA_DLL const ebml_context *MATROSKA_getContextTagBinary();
MATROSKA_DLL const ebml_context *MATROSKA_getContextSimpleTag();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTag();

MATROSKA_DLL const ebml_context *MATROSKA_getContextTags();

MATROSKA_DLL const ebml_context *MATROSKA_getContextSegment();

#endif // MATROSKA_SEMANTIC_H
