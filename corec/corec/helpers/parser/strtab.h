/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __STRTAB_H
#define __STRTAB_H


#define STRTAB_INVALID_POS  ((size_t)-1)
#define STRTAB_ITEMSIZE     8

void StrTab_Init(strtab*, const cc_memheap* Heap, size_t Alloc);
void StrTab_Done(strtab*);
const tchar_t* StrTab_Find(strtab*, fourcc_t Class, int Id);
size_t StrTab_Pos(strtab*, fourcc_t Class, int Id);
const tchar_t* StrTab_GetPos(strtab*, size_t Pos);
void StrTab_GetPosName(strtab *, size_t Pos, tchar_t *Out, size_t OutLen);

#endif
