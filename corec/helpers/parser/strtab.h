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

#ifndef __STRTAB_H
#define __STRTAB_H

typedef struct strtab
{
    const cc_memheap* Heap;
    array Table;
    void* Lock;

} strtab;

#define STRTAB_INVALID_POS  ((size_t)-1)
#define STRTAB_ITEMSIZE     8

void StrTab_Init(strtab*, const cc_memheap* Heap, size_t Alloc);
void StrTab_Done(strtab*);
void StrTab_Clear(strtab*);
void StrTab_Add(strtab*, bool_t Secondary, fourcc_t Class, int32_t Id, const tchar_t* s);
NODE_DLL int StrTab_Enum(strtab*, fourcc_t Class, size_t No);
const tchar_t* StrTab_Find(strtab*, fourcc_t Class, int Id);
bool_t StrTab_Get(strtab*, fourcc_t Class, int Id, tchar_t* Out, size_t OutLen);
size_t StrTab_Pos(strtab*, fourcc_t Class, int Id);
const tchar_t* StrTab_GetPos(strtab*, size_t Pos);
void StrTab_GetPosName(strtab *, size_t Pos, tchar_t *Out, size_t OutLen);

#endif
