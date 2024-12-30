/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __FILE_H
#define __FILE_H

#include <corec/node/node.h>

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

#ifdef __cplusplus
}
#endif

#endif
