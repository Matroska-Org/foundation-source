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
#include "matroska/matroska.h"
#include "matroska_internal.h"

static int A_MPEG_freq[4][4] =
{
// v2.5   *    v2    v1
  {11025, 0, 22050, 44100},
  {12000, 0, 24000, 48000},
  { 8000, 0, 16000, 32000},
  {    0, 0,     0,     0}
};

static int A_MPEG_samples[4][4] =
{
// v2.5   *    v2    v1
  {   0,  0,    0,    0}, // unknown layer
  { 576,  0,  576, 1152}, // layer III
  {1152,  0, 1152, 1152}, // layer II
  { 384,  0,  384,  384}  // layer I
};

static const int A_AC3_freq[3][4] =
{
    {48000, 44100, 32000, 0}, // EEEE<=8
    {24000, 22050, 16000, 0}, // EEEE=9
    {12000, 11025,  8000, 0}, // EEEE=10
};

static const int A_EAC3_freq[6] = { 48000, 44100, 32000, 24000, 22050, 16000 };
static const int A_EAC3_samples[4] = { 256, 512, 768, 1536 };


err_t MATROSKA_BlockProcessDuration(matroska_block *Block, stream *Input)
{
    ebml_element *Track=NULL, *Elt;
    tchar_t CodecID[MAXPATH];
    err_t Err;
    bool_t ReadData;
    uint8_t *Cursor;
    size_t Frame;
    int Version, Layer, SampleRate, Samples, fscod, fscod2;

    Err = Node_GET(Block,MATROSKA_BLOCK_TRACK,&Track);
    if (Err==ERR_NONE)
    {
        assert(Track!=NULL);
        Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackType,0,0);
        if (!Elt || EBML_IntegerValue(Elt)!=TRACK_TYPE_AUDIO) // other track types not supported for now
            Err = ERR_INVALID_DATA;
        else
        {
            if (Block->FirstFrameLocation==0)
                Err = ERR_READ;
            else
            {
                Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackCodecID,0,0);
                if (!Elt) // missing codec ID
                    Err = ERR_INVALID_DATA;
                else
                {
                    EBML_StringGet((ebml_string*)Elt,CodecID,TSIZEOF(CodecID));
                    ReadData = 0;
                    if (!ArraySize(&Block->Data))
                    {
                        Err = MATROSKA_BlockReadData(Block,Input);
                        if (Err!=ERR_NONE)
                            goto exit;
                        ReadData = 1;
                    }

                    if (tcsisame_ascii(CodecID,T("A_MPEG/L3")))
                    {
                        Block->IsKeyframe = 1; // safety
                        ArrayResize(&Block->Durations,sizeof(timecode_t)*ARRAYCOUNT(Block->SizeList,int32_t),0);
                        Cursor = ARRAYBEGIN(Block->Data,uint8_t);
                        for (Frame=0;Frame<ARRAYCOUNT(Block->SizeList,int32_t);++Frame)
                        {
                            Version = (Cursor[1] >> 3) & 3;
                            Layer = (Cursor[1] >> 1) & 3;
                            SampleRate = (Cursor[2] >> 2) & 3;

                            Samples = A_MPEG_samples[Layer][Version];
                            SampleRate = A_MPEG_freq[SampleRate][Version];
                            if (SampleRate!=0 && Samples!=0)
                                ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = Scale64(1000000000,Samples,SampleRate);
                            else
                            {
                                Err = ERR_INVALID_DATA;
                                ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = INVALID_TIMECODE_T;
                                //goto exit;
                            }

                            Cursor += ARRAYBEGIN(Block->SizeList,int32_t)[Frame];
                        }
                    }
                    else if (tcsisame_ascii(CodecID,T("A_AC3")))
                    {
                        Block->IsKeyframe = 1; // safety
                        ArrayResize(&Block->Durations,sizeof(timecode_t)*ARRAYCOUNT(Block->SizeList,int32_t),0);
                        Cursor = ARRAYBEGIN(Block->Data,uint8_t);
                        for (Frame=0;Frame<ARRAYCOUNT(Block->SizeList,int32_t);++Frame)
                        {
                            fscod =  Cursor[5] >> 3;
                            SampleRate = Cursor[4] >> 6;
                            if (fscod > 10)
                            {
                                Err = ERR_INVALID_DATA;
                                ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = INVALID_TIMECODE_T;
                                //goto exit;
                            }
                            else
                            {
                                SampleRate = A_AC3_freq[fscod-8][SampleRate];
                                ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = Scale64(1000000000,1536,SampleRate);
                            }
                            Cursor += ARRAYBEGIN(Block->SizeList,int32_t)[Frame];
                        }
                    }
                    else if (tcsisame_ascii(CodecID,T("A_EAC3")))
                    {
                        Block->IsKeyframe = 1; // safety
                        ArrayResize(&Block->Durations,sizeof(timecode_t)*ARRAYCOUNT(Block->SizeList,int32_t),0);
                        Cursor = ARRAYBEGIN(Block->Data,uint8_t);
                        for (Frame=0;Frame<ARRAYCOUNT(Block->SizeList,int32_t);++Frame)
                        {
                            fscod =  Cursor[4] >> 6;
                            fscod2 = (Cursor[4] >> 4) & 0x03;
                            if ((0x03 == fscod) && (0x03 == fscod2))
                            {
                                Err = ERR_INVALID_DATA;
                                ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = INVALID_TIMECODE_T;
                                //goto exit;
                            }
                            else
                            {
                                SampleRate = A_EAC3_freq[0x03 == fscod ? 3 + fscod2 : fscod];
                                Samples = (0x03 == fscod) ? 1536 : A_EAC3_samples[fscod2];
                                ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = Scale64(1000000000,Samples,SampleRate);
                            }
                            Cursor += ARRAYBEGIN(Block->SizeList,int32_t)[Frame];
                        }
                    }

                    if (ReadData)
                    {
                        ArrayClear(&Block->Data);
                        Block->Base.Base.bValueIsSet = 0;
                    }
                }
            }
        }
    }
exit:
    return Err;
}

size_t MATROSKA_BlockGetFrameCount(const matroska_block *Block)
{
    return ARRAYCOUNT(Block->SizeList,int32_t);
}

timecode_t MATROSKA_BlockGetFrameDuration(const matroska_block *Block, size_t FrameNum)
{
    if (FrameNum >= ARRAYCOUNT(Block->Durations,timecode_t))
        return INVALID_TIMECODE_T;
    return ARRAYBEGIN(Block->Durations,timecode_t)[FrameNum];
}

timecode_t MATROSKA_BlockGetFrameStart(const matroska_block *Block, size_t FrameNum)
{
    if (FrameNum >= ARRAYCOUNT(Block->Durations,timecode_t))
        return INVALID_TIMECODE_T;
    else
    {
        size_t i;
        timecode_t Start = MATROSKA_BlockTimecode(Block);
        if (Start!=INVALID_TIMECODE_T)
        {
            for (i=0;i<FrameNum;++i)
            {
                if (ARRAYBEGIN(Block->Durations,timecode_t)[i]==INVALID_TIMECODE_T)
                    return INVALID_TIMECODE_T;
                Start += ARRAYBEGIN(Block->Durations,timecode_t)[i];
            }
        }
        return Start;
    }
}

timecode_t MATROSKA_BlockGetFrameEnd(const matroska_block *Block, size_t FrameNum)
{
    timecode_t Result = MATROSKA_BlockGetFrameStart(Block,FrameNum), a;
    if (Result==INVALID_TIMECODE_T)
        return INVALID_TIMECODE_T;
    a = MATROSKA_BlockGetFrameDuration(Block,FrameNum);
    if (a==INVALID_TIMECODE_T)
        return INVALID_TIMECODE_T;
    return Result + a;
}
