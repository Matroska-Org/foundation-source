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

#include "parser.h"

static size_t SizeAlign(size_t Total, size_t Align)
{
    if (!Align)
    {
        for (Align=16;Align<16384;Align<<=1)
            if (Align*8 > Total)
                break;
    }
    --Align;
	return (Total + Align) & ~Align;
}

void BufferClear(buffer* p)
{
	free(p->Begin);
	p->End = NULL;
	p->Begin = NULL;
	p->Read = NULL;
	p->Write = NULL;
}

bool_t BufferAlloc(buffer* p, size_t Total, size_t Align)
{
    size_t Read = p->Read - p->Begin;
    size_t Write = p->Write - p->Begin;
	uint8_t* Begin;

	Total = SizeAlign(Total + SAFETAIL,Align);
	Begin = (uint8_t*)realloc(p->Begin,Total);
	if (!Begin)
		return 0;

	p->Begin = Begin;
    p->Read = Begin + Read;
    p->Write = Begin + Write;
	p->End = Begin + Total - SAFETAIL;
	return 1;
}

void BufferDrop(buffer* p)
{
	p->Write = p->Begin;
	p->Read = p->Begin;
}

uint8_t* BufferWrite(buffer* p, const void* Ptr, size_t Length, size_t Align)
{
    uint8_t* Write = p->Write + Length;

	if (Write > p->End && !BufferAlloc(p,Write-p->Begin,Align))
		return 0;

    Write = p->Write;
    p->Write = Write+Length;

	if (Ptr)
		memcpy(Write,Ptr,Length);

	return Write;
}

const uint8_t* BufferRead(buffer* p, size_t Length)
{
    uint8_t* Read = p->Read;

	if (p->Write < Read + Length)
		return NULL;

    p->Read = Read + Length;
	return Read;
}

void BufferPack(buffer* p, size_t Length)
{
	uint8_t* Read = p->Read + Length;

	if (p->Write > Read)
	{
		if (Read != p->Begin)
        {
            // move end part to the beginning
			memmove(p->Begin,Read,p->Write - Read); 
    		p->Write -= Read - p->Begin;
        }
	}
	else
		p->Write = p->Begin;

	p->Read = p->Begin;
}
