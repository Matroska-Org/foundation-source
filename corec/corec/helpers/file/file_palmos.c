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

// this is not a real file reader 
// just directory listing for different storage places

#if defined(TARGET_PALMOS)

#include "pace.h"

typedef struct file
{
	stream Stream;
	bool_t FileDb;
	UInt32 Iter;

} file;

static err_t OpenDir(file* p,const tchar_t* URL,int UNUSED_PARAM(Flags))
{
	p->FileDb = 0;
	if (NodeEnumClass(p,NULL,VFS_CLASS))
		p->Iter = vfsIteratorStart;
	else
		p->Iter = vfsIteratorStop;

    return ERR_NONE;
}

static err_t EnumDir(file* p,const tchar_t* Exts,bool_t ExtFilter,streamdir* Item)
{
	UInt16 Ref;

	Item->FileName[0] = 0;
	Item->DisplayName[0] = 0;
	Item->Size = INVALID_FILEPOS_T;
	Item->ModifiedDate = INVALID_DATETIME_T;
    Item->Type = FTYPE_DIR;

	while (!Item->FileName[0] && p->Iter != vfsIteratorStop && VFSVolumeEnumerate(&Ref,&p->Iter)==errNone)
		VFSFromVol(Ref,NULL,Item->FileName,TSIZEOF(Item->FileName));

	if (!Item->FileName[0] && !p->FileDb)
	{
		p->FileDb = 1;
		tcscpy_s(Item->FileName,TSIZEOF(Item->FileName),T("mem://"));
	}

	return Item->FileName[0] ? ERR_NONE : ERR_END_OF_FILE;
}

META_START(File_Class,FILE_CLASS)
META_CLASS(SIZE,sizeof(file))
META_CLASS(PRIORITY,PRI_MINIMUM)
META_VMT(TYPE_FUNC,stream_vmt,OpenDir,OpenDir)
META_VMT(TYPE_FUNC,stream_vmt,EnumDir,EnumDir)
META_PARAM(STRING,NODE_PROTOCOL,T("file"))
META_END(STREAM_CLASS)

stream *FileTemp(anynode *Any)
{
#ifndef TODO
    assert(NULL); // not supported yet
#endif
    return NULL;
}

bool_t FileTempName(anynode *Any,tchar_t *Out, size_t OutLen)
{
#ifndef TODO
    assert(NULL); // not supported yet
#endif
    return 0;
}

int64_t GetPathFreeSpace(nodecontext* UNUSED_PARAM(p), const tchar_t* Path)
{
#ifndef TODO
    assert(NULL); // TODO: not supported yet
    UInt16 volRefNum;
    UInt32 volumeUsed, volumeTotal;
    if (!VFSToVol(Path, &volRefNum))
	    return -1;
    if (VFSVolumeSize(volRefNum, &volumeUsedP, &volumeTotalP) != errNone)
    	return -1;
    return volumeTotal - volumeUsed;
#else
    return -1;
#endif
}

#endif

