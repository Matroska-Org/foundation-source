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

#if defined(corec_EXPORTS)
#define FILE_DLL DLLEXPORT
#elif defined(FILE_IMPORTS)
#define FILE_DLL DLLIMPORT
#else
#define FILE_DLL
#endif

#define FILE_CLASS      FOURCC('F','I','L','E')
#define VFS_CLASS       FOURCC('V','F','S','_')

FILE_DLL void CoreC_FileInit(nodemodule* Module);

FILE_DLL bool_t PathIsFolder(nodecontext*, const tchar_t*);
// \param Force erase even if the file is read-only
// \param Safe put in the OS trash rather than a permanent erase
FILE_DLL bool_t FileErase(nodecontext*, const tchar_t*, bool_t Force, bool_t Safe);

FILE_DLL void RemovePathDelimiter(tchar_t* Path);
FILE_DLL void AddPathDelimiter(tchar_t* Path,size_t PathLen);
FILE_DLL const tchar_t* GetProtocol(const tchar_t* URL, tchar_t *_Protocol, int ProtoLen, bool_t* HasHost);
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

#include "corec/helpers/file/streams.h"

#ifdef __cplusplus
}
#endif

#endif
