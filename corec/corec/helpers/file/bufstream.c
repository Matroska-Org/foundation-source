/*****************************************************************************
 * 
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of CoreCodec, Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY CoreCodec, Inc. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CoreCodec, Inc. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "file.h"

#define BUFSTREAM_SIZE      4096

typedef struct bufstream
{
	stream Base;
	stream* Stream;
    size_t ReadPos;
    size_t ReadSize;
    size_t WritePos;
	uint8_t Buffer[BUFSTREAM_SIZE];

} bufstream;

static NOINLINE err_t BufFlush(bufstream* p)
{
    err_t Err = ERR_NONE;
    if (p->Stream && p->WritePos>0)
    {
        Err = Stream_Write(p->Stream,p->Buffer,p->WritePos,NULL);
        if (Err == ERR_NONE)
            p->WritePos = 0;
    }
    return Err;
}

static err_t BufStream(bufstream* p,dataid UNUSED_PARAM(Id),stream** Data,size_t UNUSED_PARAM(Size))
{
    BufFlush(p);
    p->Stream = Data?*Data:NULL;
    p->ReadPos = 0;
    p->ReadSize = 0;
    p->WritePos = 0;
    return ERR_NONE;
}

static void BufDelete(bufstream* p)
{
    BufFlush(p);
    if (p->Stream)
    	NodeDelete((node*)p->Stream);
}

static err_t BufRead(bufstream* p,uint8_t* Data,size_t Size,size_t* Readed)
{
    err_t Err = ERR_NONE;
    size_t Pos = 0;
    size_t Left;

    while ((Left = (Size - Pos)) > 0)
    {
        if (p->ReadSize <= p->ReadPos)
        {
            if (Left > BUFSTREAM_SIZE)
            {
                Err = Stream_Read(p->Stream,Data+Pos,Left,&Left);
                if (Readed)
                    *Readed = Pos+Left;
                return Err;
            }

            p->ReadPos = 0;
            Err = Stream_Read(p->Stream,p->Buffer,BUFSTREAM_SIZE,&p->ReadSize);
            if (p->ReadSize <= 0)
                break;
        }

        if (Left > p->ReadSize - p->ReadPos)
            Left = p->ReadSize - p->ReadPos;

        memcpy(Data+Pos,p->Buffer+p->ReadPos,Left);
        Pos += Left;
        p->ReadPos += Left;
    }

    if (Readed)
        *Readed = Pos;

    if (Pos >= Size)
        Err = ERR_NONE; //override possible end of file in later of the buffer
	return Err;
}

static err_t BufWrite(bufstream* p,const uint8_t* Data,size_t Size,size_t* Written)
{
    err_t Err = ERR_NONE;
    size_t Pos = 0;
    size_t Left;

    while ((Left = (Size - Pos)) > 0)
    {
        if (p->WritePos >= BUFSTREAM_SIZE && (Err = BufFlush(p)) != ERR_NONE)
            break;

        if (!p->WritePos && Left > BUFSTREAM_SIZE)
        {
            Err = Stream_Write(p->Stream,Data+Pos,Left,&Left);
            Pos += Left;
            break;
        }

        if (Left > BUFSTREAM_SIZE - p->WritePos)
            Left = BUFSTREAM_SIZE - p->WritePos;

        memcpy(p->Buffer+p->WritePos,Data+Pos,Left);
        Pos += Left;
        p->WritePos += Left;
    }

    if (Written)
        *Written = Pos;
	return Err;
}

META_START(BufStream_Class,BUFSTREAM_CLASS)
META_CLASS(SIZE,sizeof(bufstream))
META_CLASS(DELETE,BufDelete)
META_VMT(TYPE_FUNC,stream_vmt,Read,BufRead)
META_VMT(TYPE_FUNC,stream_vmt,Write,BufWrite)
META_PARAM(SET,BUFSTREAM_STREAM,BufStream)
META_END(STREAM_CLASS)
