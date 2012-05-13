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

static err_t MemRead(memstream* p,void* Data,size_t Size,size_t* Readed)
{
    err_t Err = ERR_NONE;
	size_t Pos = p->Pos;
    assert(Pos <= p->Size);

    if (Size > p->Size - Pos)
    {
        Size = p->Size - Pos;
        Err = ERR_END_OF_FILE;
    }

	memcpy(Data,p->Ptr+Pos,Size);
	p->Pos = Pos+Size;

    if (Readed)
        *Readed = Size;

	return Err;
}

static err_t MemWrite(memstream* p, const void* Data, size_t Size, size_t* Written)
{
    *Written = 0;
    if (Size + p->Pos > p->Size)
        return ERR_INVALID_PARAM;
    assert(p->Ptr!=NULL);
	memcpy((uint8_t*)p->Ptr + p->Pos, Data, Size);
    p->Pos += Size;
    *Written = Size;
    return ERR_NONE;
}

static filepos_t MemSeek(memstream* p,filepos_t VirtualPos,int SeekMode)
{
	switch (SeekMode)
	{
	default:
	case SEEK_SET: break;
	case SEEK_CUR: VirtualPos += p->Pos + p->VirtualOffset; break;
	case SEEK_END: VirtualPos += p->Size + p->VirtualOffset; break;
	}

    // TODO: these safety checks should return an error
	if (VirtualPos<0)
    {
        assert(VirtualPos>=0);
		VirtualPos=0;
    }

    if (VirtualPos>(filepos_t)p->Size + p->VirtualOffset)
    {
        assert(VirtualPos<=(filepos_t)p->Size);
        VirtualPos=p->Size + p->VirtualOffset;
    }

    p->Pos = (size_t)(VirtualPos - p->VirtualOffset);
	return VirtualPos;
}

static err_t MemLength(memstream* p, dataid UNUSED_PARAM(Id), filepos_t* Data, size_t UNUSED_PARAM(Size))
{
    *Data = p->Size;
    return ERR_NONE;
}

static err_t MemData(memstream* p, dataid UNUSED_PARAM(Id), const void* Data, size_t Size)
{
    assert(Size==0 || Data!=NULL);
	p->Ptr = (const uint8_t*)Data;
	p->Size = Size;
    p->Pos = 0;
	return ERR_NONE;
}

META_START(MemStream_Class,MEMSTREAM_CLASS)
META_CLASS(SIZE,sizeof(memstream))
META_VMT(TYPE_FUNC,stream_vmt,Read,MemRead)
META_VMT(TYPE_FUNC,stream_vmt,Write,MemWrite)
META_VMT(TYPE_FUNC,stream_vmt,Seek,MemSeek)
META_PARAM(GET,STREAM_LENGTH,MemLength)
META_PARAM(SET,MEMSTREAM_DATA,MemData)
META_DATA(TYPE_FILEPOS,MEMSTREAM_OFFSET,memstream,VirtualOffset)
META_DATA_RDONLY(TYPE_PTR,MEMSTREAM_PTR,memstream,Ptr)
META_END(STREAM_CLASS)
