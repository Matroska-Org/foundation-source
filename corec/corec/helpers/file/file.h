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

#ifndef __FILE_H
#define __FILE_H

#include "corec/node/node.h"
#include "corec/helpers/date/date.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(FILE_EXPORTS)
#define FILE_DLL DLLEXPORT
#elif defined(FILE_IMPORTS)
#define FILE_DLL DLLIMPORT
#else
#define FILE_DLL
#endif

#define FILE_CLASS      FOURCC('F','I','L','E')
#define VFS_CLASS       FOURCC('V','F','S','_')

FILE_DLL bool_t FileExists(nodecontext*, const tchar_t*);
FILE_DLL bool_t FileMove(nodecontext*, const tchar_t* In,const tchar_t* Out);
FILE_DLL datetime_t FileDateTime(nodecontext*, const tchar_t*);
FILE_DLL bool_t PathIsFolder(nodecontext*, const tchar_t*);
FILE_DLL bool_t FolderCreate(nodecontext*, const tchar_t*);
// \param Force erase even if the file is read-only
// \param Safe put in the OS trash rather than a permanent erase
FILE_DLL bool_t FileErase(nodecontext*, const tchar_t*, bool_t Force, bool_t Safe);
FILE_DLL bool_t FolderErase(nodecontext*, const tchar_t*, bool_t Force, bool_t Safe);
FILE_DLL void FindFiles(nodecontext*,const tchar_t* Path, const tchar_t* Mask,void(*Process)(const tchar_t*,void*),void* Param);
FILE_DLL int64_t GetPathFreeSpace(nodecontext*,const tchar_t* Path);

FILE_DLL void RemovePathDelimiter(tchar_t* Path);
FILE_DLL void AddPathDelimiter(tchar_t* Path,size_t PathLen);
FILE_DLL const tchar_t* GetProtocol(const tchar_t* URL, tchar_t *_Protocol, int ProtoLen, bool_t* HasHost);
FILE_DLL fourcc_t GetProtocolKind(anynode*, tchar_t *_Protocol);
FILE_DLL void SplitPath(const tchar_t* Path, tchar_t* Dir, int DirLen, tchar_t* Name, int NameLen, tchar_t* Ext, int ExtLen);
FILE_DLL void SplitURL(const tchar_t* URL, tchar_t* Mime, int MimeLen, tchar_t* Host, int HostLen, int* Port, tchar_t* Path, int PathLen);
FILE_DLL bool_t SplitAddr(const tchar_t* URL, tchar_t* Peer, int PeerLen, tchar_t* Local, int LocalLen);
FILE_DLL bool_t SetFileExt(tchar_t* URL, size_t URLLen, const tchar_t* Ext);
FILE_DLL int CheckExts(const tchar_t* URL, const tchar_t* Exts);
FILE_DLL void AbsPath(tchar_t* Abs, int AbsLen, const tchar_t* Path, const tchar_t* Base);
FILE_DLL void AbsPathNormalize(tchar_t* Abs, size_t AbsLen);
FILE_DLL void ReduceLocalPath(tchar_t* Abs, size_t AbsLen);
FILE_DLL void RelPath(tchar_t* Rel, int RelLen, const tchar_t* Path, const tchar_t* Base);
FILE_DLL bool_t UpperPath(tchar_t* Path, tchar_t* Last, size_t LastLen);
FILE_DLL void StreamLoginInfo(node* p, tchar_t* URL, bool_t Proxy); //URL updated

FILE_DLL tchar_t* FirstSepar(const tchar_t *Path);
FILE_DLL void SplitURLLogin(const tchar_t *URL, tchar_t *UserName, size_t UserNameLen, tchar_t *Password, size_t PasswordLen, tchar_t *URL2, size_t URL2Len);
FILE_DLL void SplitShare(const tchar_t *Path, tchar_t *Share, size_t ShareLen, tchar_t *Path2, size_t Path2Len);
FILE_DLL tchar_t *MergeURL(tchar_t *URL, size_t URLLen, const tchar_t *Proto, const tchar_t *Host, int Port, const tchar_t *Path);
FILE_DLL tchar_t *GetIP(tchar_t *sIP, size_t IPLen, long IP);
FILE_DLL void SplitURLParams(const tchar_t* URL, tchar_t* URL2, int URL2Len, tchar_t* Params, int ParamsLen);
FILE_DLL tchar_t *AddCacheURL(tchar_t* Out, size_t Len, const tchar_t *In);
FILE_DLL bool_t CheckRemoveCacheURL(const tchar_t** URL);
FILE_DLL bool_t RemoveURLParam(tchar_t* URL, const tchar_t* Param);

static INLINE size_t FileBlockSize(filepos_t Start,filepos_t End)
{
    End -= Start;
    if (End<0 || End>INT_MAX)
        return 0;
    return (size_t)End;
}

#include "corec/helpers/file/streams.h"

FILE_DLL stream *FileTemp(anynode*);
FILE_DLL bool_t FileTempName(anynode*,tchar_t *Out, size_t OutLen);
FILE_DLL err_t FileStat(nodecontext* p, const tchar_t* Path, streamdir* Item);

#if defined(TARGET_PALMOS)
FILE_DLL const tchar_t* VFSToVol(const tchar_t* URL,uint16_t* Vol);
FILE_DLL bool_t VFSFromVol(uint16_t Vol,const tchar_t* Path,tchar_t* URL,int URLLen);
FILE_DLL bool_t DBFrom(uint16_t Card,uint32_t DB,tchar_t* URL,int URLLen);
#endif

#ifdef __cplusplus
}
#endif

#if defined(TARGET_SYMBIAN) && defined(__cplusplus) // symbian headers are not compatible with C
#include <e32std.h>
#include <e32hal.h>
#include <f32file.h>
static INLINE RFs& Node_FsSession(anynode *AnyNode)
{
	return *static_cast<RFs*>(Node_Context(AnyNode)->FsSession);
}
#endif

#endif
