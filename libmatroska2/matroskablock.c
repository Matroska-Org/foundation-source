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
#include "matroska/matroska.h"
#include "matroska/matroska_sem.h"
#include "matroska/matroska_internal.h"
#if defined(CONFIG_CODEC_HELPER)
#include "ivorbiscodec.h"
#include "codec_internal.h"
#include "misc.h"
#endif

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

static const int A_DTS_freq[16] = {
   0, 8000, 16000, 32000, 0, 0, 11025, 22050, 44100, 0, 0, 12000, 24000, 48000, 0, 0
};


err_t MATROSKA_BlockProcessFrameDurations(matroska_block *Block, stream *Input)
{
    ebml_master *Track=NULL;
    ebml_element *Elt;
    tchar_t CodecID[MAXPATH];
    err_t Err;
    bool_t ReadData;
    uint8_t *Cursor;
    size_t Frame;
    int Version, Layer, SampleRate, Samples, fscod, fscod2;

    Err = Node_GET(Block,MATROSKA_BLOCK_READ_TRACK,&Track);
    if (Err==ERR_NONE)
    {
        assert(Track!=NULL);
        Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextTrackType);
        if (!Elt || EBML_IntegerValue((ebml_integer*)Elt)!=TRACK_TYPE_AUDIO) // other track types not supported for now
            Err = ERR_INVALID_DATA;
        else
        {
            if (Block->FirstFrameLocation==0)
                Err = ERR_READ;
            else
            {
                Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextCodecID);
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

                    if (tcsisame_ascii(CodecID,T("A_MPEG/L3")) || tcsisame_ascii(CodecID,T("A_MPEG/L2")) || tcsisame_ascii(CodecID,T("A_MPEG/L1")))
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
                            if (fscod > 10 || fscod < 8)
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
                    else if (tcsisame_ascii(CodecID,T("A_DTS")))
                    {
                        Block->IsKeyframe = 1; // safety
                        ArrayResize(&Block->Durations,sizeof(timecode_t)*ARRAYCOUNT(Block->SizeList,int32_t),0);
                        Cursor = ARRAYBEGIN(Block->Data,uint8_t);
                        for (Frame=0;Frame<ARRAYCOUNT(Block->SizeList,int32_t);++Frame)
                        {
                            Samples = (((Cursor[4] & 1) << 7) + (Cursor[5] >> 2) + 1) * 32;
                            // TODO: handle the frame termination
                            SampleRate = A_DTS_freq[(Cursor[8] >> 2) & 0x0F];
                            if (Samples==0 || SampleRate==0)
                            {
                                Err = ERR_INVALID_DATA;
                                ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = INVALID_TIMECODE_T;
                                //goto exit;
                            }
                            else
                                ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = Scale64(1000000000,Samples,SampleRate);
                            Cursor += ARRAYBEGIN(Block->SizeList,int32_t)[Frame];
                        }
                    }
                    else if (tcsisame_ascii(CodecID,T("A_AAC")) || tcsncmp(CodecID,T("A_AAC/"),6)==0)
                    {
                        Block->IsKeyframe = 1; // safety
                        Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextAudio);
                        if (Elt)
                        {
                            Elt = EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextSamplingFrequency);
                            if (Elt)
                            {
                                ArrayResize(&Block->Durations,sizeof(timecode_t)*ARRAYCOUNT(Block->SizeList,int32_t),0);
                                Samples = 1024;
                                SampleRate = (int)((ebml_float*)Elt)->Value;
                                for (Frame=0;Frame<ARRAYCOUNT(Block->SizeList,int32_t);++Frame)
                                    ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = Scale64(1000000000,Samples,SampleRate);
                            }
                        }
                    }
#if defined(CONFIG_CODEC_HELPER)
                    else if (tcsisame_ascii(CodecID,T("A_VORBIS")))
                    {
                        Block->IsKeyframe = 1; // safety
                        Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextCodecPrivate);
                        if (Elt)
                        {
                            vorbis_info vi;
                            vorbis_comment vc;
                            ogg_packet OggPacket;
                        	ogg_reference OggRef;
                        	ogg_buffer OggBuffer;
                            int n,i,j;
                            codec_setup_info *ci;

		                    vorbis_info_init(&vi);
		                    vorbis_comment_init(&vc);
                            memset(&OggPacket,0,sizeof(ogg_packet));

		                    OggBuffer.data = (uint8_t*)EBML_BinaryGetData((ebml_binary*)Elt);
		                    OggBuffer.size = (long)Elt->DataSize;
		                    OggBuffer.refcount = 1;

                            memset(&OggRef,0,sizeof(OggRef));
		                    OggRef.buffer = &OggBuffer;
		                    OggRef.next = NULL;

		                    OggPacket.packet = &OggRef;
		                    OggPacket.packetno = -1; 

		                    n = OggBuffer.data[0];
		                    i = 1+n;
		                    j = 1;

		                    while (OggPacket.packetno < 3 && n>=j)
		                    {
			                    OggRef.begin = i;
			                    OggRef.length = 0;
			                    do
			                    {
				                    OggRef.length += OggBuffer.data[j];
			                    }
			                    while (OggBuffer.data[j++] == 255 && n>=j);
			                    i += OggRef.length;

			                    if (i > OggBuffer.size)
				                    return ERR_INVALID_DATA;

	                            ++OggPacket.packetno;
	                            OggPacket.b_o_s = OggPacket.packetno == 0;
	                            OggPacket.bytes = OggPacket.packet->length;
			                    if (!(vorbis_synthesis_headerin(&vi,&vc,&OggPacket) >= 0) && OggPacket.packetno==0)
				                    return ERR_INVALID_DATA;
		                    }

		                    if (OggPacket.packetno < 3)
		                    {
			                    OggRef.begin = i;
			                    OggRef.length = OggBuffer.size - i; 

	                            ++OggPacket.packetno;
	                            OggPacket.b_o_s = OggPacket.packetno == 0;
	                            OggPacket.bytes = OggPacket.packet->length;

                                if (!(vorbis_synthesis_headerin(&vi,&vc,&OggPacket) >= 0) && OggPacket.packetno==0)
				                    return ERR_INVALID_DATA;
                            }

                            SampleRate = vi.rate;
                            ArrayResize(&Block->Durations,sizeof(timecode_t)*ARRAYCOUNT(Block->SizeList,int32_t),0);
                            Cursor = ARRAYBEGIN(Block->Data,uint8_t);
                            ci = vi.codec_setup;
                            for (Frame=0;Frame<ARRAYCOUNT(Block->SizeList,int32_t);++Frame)
                            {
                                fscod = _ilog(ci->modes-1);
                                fscod = (Cursor[0] & 0x7F) >> (7-fscod);
                                if (fscod > ci->modes)
                                {
                                    Err = ERR_INVALID_DATA;
                                    ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = INVALID_TIMECODE_T;
                                    //goto exit;
                                }
                                else
                                {
                                    Samples = ci->blocksizes[ci->mode_param[fscod]->blockflag];
                                    ARRAYBEGIN(Block->Durations,timecode_t)[Frame] = Scale64(1000000000,Samples,SampleRate);
                                }
                                Cursor += ARRAYBEGIN(Block->SizeList,int32_t)[Frame];
                            }
                            vorbis_comment_clear(&vc);
                            vorbis_info_clear(&vi);
                        }
                    }
#endif

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

size_t MATROSKA_BlockGetLength(const matroska_block *Block, size_t FrameNum)
{
	if (FrameNum >= ARRAYCOUNT(Block->SizeList,int32_t))
		return 0;
	return ARRAYBEGIN(Block->SizeList,int32_t)[FrameNum];
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
        timecode_t Start = MATROSKA_BlockTimecode((matroska_block*)Block);
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
