/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * Copyright (C) CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __STREAM_H
#define __STREAM_H

#include "file.h" // FILE_DLL

#define FTYPE_DIR           'D'
#define FTYPE_UNDEFINED     'u'

#define MEDIA_CLASS			FOURCC('F','M','T','M') // anything with file extension

#define STREAM_CLASS		FOURCC('S','T','R','M')

#define STREAM_URL			0x91 // tchar_t*
#define STREAM_LENGTH		0x92 // filepos_t
#define STREAM_FLAGS		0x93 // int
#define STREAM_ENUM_BASE    0x9A // tchar_t*
#define STREAM_ISTTY        0xAF // bool_t
#define STREAM_USERNAME     0x88 // tchar_t*
#define STREAM_PASSWORD     0x89 // tchar_t*
#define STREAM_PROXY_USERNAME    0xC0 // tchar_t*
#define STREAM_PROXY_PASSWORD    0xC1 // tchar_t*
#define STREAM_CACHE_CLASS  0xB4 // fourcc_t

#define SFLAG_RDONLY               0x1
#define SFLAG_WRONLY               0x2
#define SFLAG_CREATE               0x4
#define SFLAG_SILENT               0x8
#define SFLAG_BUFFERED            0x10   // used only by StreamOpen helper function
#define SFLAG_REOPEN              0x20   // private inside stream
#define SFLAG_NO_CACHING         0x800
#define SFLAG_NON_BLOCKING      0x1000   // used only by StreamOpen helper function

#define MAX_NETWORK_PACKET      2048

#ifndef SEEK_SET
#define SEEK_SET        0
#define SEEK_CUR        1
#define SEEK_END        2
#endif

typedef struct stream
{
	node Base;
#if defined(CONFIG_DEBUGCHECKS)
    tchar_t URL[MAXPATH];
#endif

} stream;

#if defined(CONFIG_STDIO)
#define STDIN_ID		FOURCC('S','T','D','I')
#define STDOUT_ID		FOURCC('S','T','D','O')
#define STDERR_ID		FOURCC('S','T','D','E')
#endif

typedef struct streamdir
{
	tchar_t FileName[MAXPATH];
	tchar_t DisplayName[MAXPATH];
	filepos_t Size;
	int Type;					// from Exts, FTYPE_DIR for directory
	datetime_t ModifiedDate;

} streamdir;

typedef struct streamselect streamselect;
struct streamselect
{
    err_t (*Func)(streamselect*);
    int Max;
    void* Rd;
    void* Wr;
    void* Er;
	tick_t Timeout; // TODO: change to systick_t
	stream* Selected[64];
};

typedef struct stream_vmt
{
    node_vmt Base;

    err_t (*Open)(thisnode,const tchar_t* URL, int Flags);
    stream* (*Duplicate)(thisnode,int Flags);
    err_t (*Read)(thisnode,void* Data,size_t Size,size_t* Readed);
    err_t (*ReadOneOrMore)(thisnode,void* Data,size_t Size,size_t* Readed);
    err_t (*ReadBlock)(thisnode,block* Block,size_t Ofs,size_t Size,size_t* Readed);
    err_t (*Write)(thisnode,const void* Data,size_t Size,size_t* Written);
    filepos_t (*Seek)(thisnode,filepos_t Pos,int SeekMode);
    err_t (*OpenDir)(thisnode,const tchar_t* URL, int Flags);
    err_t (*EnumDir)(thisnode,const tchar_t* Exts,bool_t ExtFilter,streamdir* Item);
    err_t (*Blocking)(thisnode,bool_t Blocking);
    err_t (*Wait)(thisnode,bool_t Read, streamselect* Select);
    err_t (*Skip)(thisnode,intptr_t* Skip);
    err_t (*Flush)(thisnode);
    err_t (*ResetReadTimeout)(thisnode,int Secs);

} stream_vmt;

#define Stream_Open(p,a,b)              VMT_FUNC(p,stream_vmt)->Open(p,a,b)
#define Stream_Duplicate(p,a)           VMT_FUNC(p,stream_vmt)->Duplicate(p,a)
#define Stream_Read(p,a,b,c)            VMT_FUNC(p,stream_vmt)->Read(p,a,b,c)
#define Stream_ReadOneOrMore(p,a,b,c)   VMT_FUNC(p,stream_vmt)->ReadOneOrMore(p,a,b,c)
#define Stream_ReadBlock(p,a,b,c,d)     VMT_FUNC(p,stream_vmt)->ReadBlock(p,a,b,c,d)
#define Stream_Write(p,a,b,c)           VMT_FUNC(p,stream_vmt)->Write(p,a,b,c)
#define Stream_Seek(p,a,b)              VMT_FUNC(p,stream_vmt)->Seek(p,a,b)
#define Stream_OpenDir(p,a,b)           VMT_FUNC(p,stream_vmt)->OpenDir(p,a,b)
#define Stream_EnumDir(p,a,b,c)         VMT_FUNC(p,stream_vmt)->EnumDir(p,a,b,c)
#define Stream_Blocking(p,a)            VMT_FUNC(p,stream_vmt)->Blocking(p,a)
#define Stream_Wait(p,a,b)              VMT_FUNC(p,stream_vmt)->Wait(p,a,b)
#define Stream_Skip(p,a)                VMT_FUNC(p,stream_vmt)->Skip(p,a)
#define Stream_Flush(p)                 VMT_FUNC(p,stream_vmt)->Flush(p)
#define Stream_ResetReadTimeout(p,a)    VMT_FUNC(p,stream_vmt)->ResetReadTimeout(p,a)

//--------------------------------------------------------------------------

#define STREAMPROCESS_CLASS		FOURCC('S','T','R','P')

#define STREAMPROCESS_INPUT		0xB0

//---------------------------------------------------------------------------

#define MEMSTREAM_CLASS		FOURCC('M','E','M','S')
#define MEMSTREAM_DATA		0x100
#define MEMSTREAM_PTR		0x101
#define MEMSTREAM_OFFSET    0x102

//---------------------------------------------------------------------------

#define BUFSTREAM_CLASS		FOURCC('B','U','F','S')
#define BUFSTREAM_STREAM	0x100

//---------------------------------------------------------------------------

FILE_DLL stream* StreamOpen(anynode*, const tchar_t* URL, int Flags);
FILE_DLL void StreamClose(stream*);

#endif
