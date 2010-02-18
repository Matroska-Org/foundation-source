/*****************************************************************************
 * 
 * Copyright (c) 2008-2009, CoreCodec, Inc.
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

#ifndef __STREAM_H
#define __STREAM_H

#include "node/node.h"

#define FTYPE_DIR           'D'
#define FTYPE_UNDEFINED     'u'

#define MEDIA_CLASS			FOURCC('F','M','T','M') // anything with file extension

#define STREAM_CLASS		FOURCC('S','T','R','M')

#define STREAM_URL			0x91
#define STREAM_LENGTH		0x92  // filepos_t
#define STREAM_FLAGS		0x93
#define STREAM_CONTENTTYPE	0x95
#define STREAM_META		    0x96
#define STREAM_PRAGMA_SEND	0x97 // tchar_t*
#define STREAM_PRAGMA_GET	0x98
#define STREAM_ENUM_BASE    0x9A
#define STREAM_PLAY			0x9B
#define STREAM_DEFAULT_PORT	0x9C
#define STREAM_LOCAL_URL    0x9E
#define STREAM_PEER_URL     0x9F
#define STREAM_TIME         0xA0
#define STREAM_CLIENT_URL   0xA1
#define STREAM_MULTICAST_URL 0xA2
#define STREAM_MULTICAST_TTL 0xA3
#define STREAM_MULTICAST_ADD 0xA4
#define STREAM_BROADCAST    0xA5
#define STREAM_REUSE_IP     0xA6
#define STREAM_REUSE_PORT   0xA7
#define STREAM_NEW_CLIENT   0xA8
#define STREAM_BIND         0xA9
#define STREAM_SCOUTING     0xAA
#define STREAM_NO_PAUSE 	0xAB // TODO: find a better solution...
#define STREAM_USER_AGENT	0xAC
#define STREAM_KIND			0xAD
#define STREAM_CHANNEL_CLASS 0xAE
#define STREAM_ISTTY        0xAF // bool_t
#define STREAM_FULL_URL     0x99
#define STREAM_TIMEOUT      0x94
#define STREAM_NATIVE_HANDLER 0x8F
#define STREAM_CACHE_SEND   0x8B // tchar_t*
#define STREAM_DATE         0x8C // datetime_t
#define STREAM_EXPIRE       0x8D // datetime_t
#define STREAM_ETAG         0x8E // tchar_t*
#define STREAM_DATE_CHECK   0x89 // datetime_t
#define STREAM_ETAG_CHECK   0x8A // tchar_t*
#define STREAM_AUTH_COOKIE  0x86
#define STREAM_AUTH_SCHEME  0x87 // tchar_t*
#define STREAM_USERNAME     0x88 // tchar_t*
#define STREAM_PASSWORD     0x89 // tchar_t*
#define STREAM_PROXY_AUTH_SCHEME 0x90 // tchar_t*
#define STREAM_PROXY_USERNAME    0xC0 // tchar_t*
#define STREAM_PROXY_PASSWORD    0xC1 // tchar_t*

#define STREAM_POST_DATA    0xB0 // binary
#define STREAM_POST_DATA_STRING 0xB1 // tchar_t*
#define STREAM_POST_CONTENTTYPE 0xB2 // tchar_t*
#define STREAM_POST_CHARSET 0xB3 // tchar_t*

#define STREAM_CACHE_CLASS  0xB4 // fourcc_t
#define STREAM_CACHE_RANGE  0xB5 // array<filepos[2]>
#define STREAM_CACHING      0xB6 // bool_t

#define STREAM_TAG_DATE     0xB7 // datetime_t
#define STREAM_CACHE_ENDPOS 0xB8 // filepos_t

#define STREAM_KIND_LOCAL        0
#define STREAM_KIND_NETWORK      1
#define STREAM_KIND_LIVE_FEED    2

#define SFLAG_RDONLY               0x1
#define SFLAG_WRONLY               0x2
#define SFLAG_CREATE               0x4
#define SFLAG_SILENT               0x8
#define SFLAG_BUFFERED            0x10   // used only by StreamOpen helper function
#define SFLAG_REOPEN              0x20   // private inside stream
#define SFLAG_NO_PRAGMA           0x40
#define SFLAG_NO_PROXY            0x80
#define SFLAG_NO_DELAY           0x100
#define SFLAG_NO_RETRY           0x200
#define SFLAG_HIDDEN             0x400
#define SFLAG_NO_CACHING         0x800
#define SFLAG_NON_BLOCKING      0x1000   // used only by StreamOpen helper function
#define SFLAG_CREATE_BLOCKING   0x2000
#define SFLAG_FORCE_CACHING     0x4000
#define SFLAG_LONGTERM_CACHING  0x8000
#define SFLAG_RECONNECT        0x10000

#define MAX_NETWORK_PACKET      2048

#ifndef SEEK_SET
#define SEEK_SET        0
#define SEEK_CUR        1
#define SEEK_END        2
#endif

#ifndef EOF
#define EOF				(-1)
#endif

typedef struct stream
{
	node Base;
#if defined(CONFIG_DEBUGCHECKS)
    tchar_t URL[MAXPATH];
#endif

} stream;

typedef struct memstream
{
	stream Base;
	size_t Pos;
	size_t Size;
	const uint8_t* Ptr;

} memstream;

#define STDIN_ID		FOURCC('S','T','D','I')
#define STDOUT_ID		FOURCC('S','T','D','O')
#define STDERR_ID		FOURCC('S','T','D','E')

typedef struct streamdir
{
	tchar_t FileName[MAXPATH];
	tchar_t DisplayName[MAXPATH];
	int Type;					// from Exts, FTYPE_DIR for directory
	filepos_t Size;				
	datetime_t ModifiedDate;
	void* Private;

} streamdir;

typedef struct streamselect streamselect;
struct streamselect
{
    err_t (*Func)(streamselect*);
    int LibRef;
    int Max;
    void* Rd;
    void* Wr;
    void* Er;
	tick_t Timeout;
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

//--------------------------------------------------------------------------
 
#define STREAMPROCESS_CLASS		FOURCC('S','T','R','P')

#define STREAMPROCESS_INPUT		0xB0
#define STREAMPROCESS_DATAFEED	0xB1

//---------------------------------------------------------------------------

#define MEMSTREAM_CLASS		FOURCC('M','E','M','S')
#define MEMSTREAM_DATA		0x100
#define MEMSTREAM_PTR		0x101

//---------------------------------------------------------------------------

#define BUFSTREAM_CLASS		FOURCC('B','U','F','S')
#define BUFSTREAM_STREAM	0x100

//---------------------------------------------------------------------------

#define RESOURCEDATA_ID		FOURCC('R','E','S','F')
#define RESOURCEDATA_SIZE   0x100
#define RESOURCEDATA_PTR    0x101

//---------------------------------------------------------------------------

FILE_DLL stream* GetStream(anynode*, const tchar_t* URL, int Flags);
FILE_DLL stream* StreamOpen(anynode*, const tchar_t* URL, int Flags);
FILE_DLL void StreamClose(stream*);
FILE_DLL bool_t StreamGenExts(anynode*,array* Exts, fourcc_t ClassFilter, const tchar_t* TypeFilter);
FILE_DLL char StreamExtType(anynode*, fourcc_t ClassFilter, const tchar_t *Ext);
FILE_DLL int StreamProtocolPriority(anynode*, const tchar_t* URL);

#endif
