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

//TODO: include from somewhere else or not depend on them at all
#ifdef CONFIG_BLOCK_RDONLY
#include "common.h"
#endif
//end TODO

static err_t DummySkip(void* p, intptr_t* Skip)
{
    uint8_t Buf[1024];
    size_t Readed;
    intptr_t n = *Skip;
    err_t Err = ERR_NONE;
    while (n>0 && Err == ERR_NONE)
    {
        Err = Stream_Read(p,Buf,min(n,(intptr_t)sizeof(Buf)),&Readed);
        n -= Readed;
    }
    *Skip = n;
    return Err;
}

static err_t DummyOpen(void* UNUSED_PARAM(p),const tchar_t* Name, int UNUSED_PARAM(Flags))
{
    if (Name)
        return ERR_NOT_SUPPORTED;
    return ERR_NONE;
}

static stream* DummyDuplicate(void* p,int Flags)
{
    tchar_t URL[MAXPATHFULL];

	if (Node_Get(p,STREAM_URL,URL,sizeof(URL)) != ERR_NONE)
		return NULL;
	
    return StreamOpen(p,URL,Flags);
}

static err_t DummyBlocking(void* UNUSED_PARAM(p),bool_t State)
{ 
    if (!State)
        return ERR_NOT_SUPPORTED;
    return ERR_NONE;
}

static err_t DummyRead(void* UNUSED_PARAM(p),void* UNUSED_PARAM(Data),size_t UNUSED_PARAM(Size),size_t* Readed)
{ 
    if (Readed)
        *Readed = 0;
    return ERR_NOT_SUPPORTED; 
}

static err_t DummyReadOneOrMore(void* p,void* Data,size_t Size,size_t* Readed)
{ 
	return Stream_Read(p,Data,Size,Readed);
}

static filepos_t DummySeek(void* UNUSED_PARAM(p),filepos_t UNUSED_PARAM(Pos),int UNUSED_PARAM(SeekMode)) 
{ 
    return INVALID_FILEPOS_T;
}

static err_t DummyWrite(void* UNUSED_PARAM(This),const void* UNUSED_PARAM(Data),size_t UNUSED_PARAM(Size), size_t* Written)
{ 
    if (Written)
        *Written = 0;
    return ERR_NOT_SUPPORTED; 
}

static err_t DummyOpenDir(void* UNUSED_PARAM(p),const tchar_t* UNUSED_PARAM(URL), int UNUSED_PARAM(Flags)) 
{ 
    return ERR_NOT_DIRECTORY; 
}

static err_t DummyEnumDir(void* UNUSED_PARAM(p),const tchar_t* UNUSED_PARAM(Exts), bool_t UNUSED_PARAM(ExtFilter),streamdir* UNUSED_PARAM(Item))
{ 
    return ERR_END_OF_FILE; 
}

static err_t DummyReadBlock(stream* p,block* Block,size_t Ofs,size_t Size,size_t* Readed)
{
#ifdef CONFIG_BLOCK_RDONLY
    uint8_t Buf[2048]; // MAX_NETWORK_PACKET has to fit...
    err_t Err = ERR_NONE;
    size_t Pos = 0;
    size_t Left;

    while ((Left = (Size - Pos)) > 0)
    {
        if (Left > sizeof(Buf))
            Left = sizeof(Buf);

		Err = Stream_Read(p,Buf,Left,&Left);
		if (!Left)
            break;

		WriteBlock(Block,Ofs+Pos,Buf,Left);
		Pos += Left;

        if (Left < sizeof(Buf))
            break;
	}

    if (Readed)
        *Readed = Pos;
	return Err;
#else
	return Stream_Read(p,(uint8_t*)Block->Ptr+Ofs,Size,Readed);
#endif
}

static err_t DummyWait(void* UNUSED_PARAM(p),bool_t UNUSED_PARAM(Read),streamselect* UNUSED_PARAM(Select))
{ 
    return ERR_NOT_SUPPORTED;
}

static err_t DummyFlush(void* UNUSED_PARAM(p))
{
    return ERR_NOT_SUPPORTED;
}

static err_t DummyResetReadTimeout(void* UNUSED_PARAM(p))
{
    return ERR_NOT_SUPPORTED;
}

static err_t ProcessBlocking(void* p,bool_t State)
{ 
    stream* Input;
    if (Node_GET(p,STREAMPROCESS_INPUT,&Input) == ERR_NONE && Input)
        return Stream_Blocking(Input,State);
    return DummyBlocking(p,State);
}

static err_t ProcessWait(void* p,bool_t Read,streamselect* Select)
{ 
    stream* Input;
    if (Node_GET(p,STREAMPROCESS_INPUT,&Input) == ERR_NONE && Input)
        return Stream_Wait(Input,Read,Select);
    return ERR_NOT_SUPPORTED;
}

META_START(Streams_Class,MEDIA_CLASS)
META_CLASS(FLAGS,CFLAG_ABSTRACT)
META_END_CONTINUE(NODE_CLASS)

META_START_CONTINUE(STREAM_CLASS)
META_CLASS(VMT_SIZE,sizeof(stream_vmt))
META_CLASS(SIZE,sizeof(stream))
META_CLASS(FLAGS,CFLAG_ABSTRACT)
META_PARAM(TYPE,STREAM_KIND,TYPE_FOURCC|TFLAG_RDONLY)
META_PARAM(CUSTOM,STREAM_KIND,STREAM_KIND_LOCAL)
META_PARAM(NAME,STREAM_URL,T("URL"))
META_PARAM(TYPE,STREAM_URL,TYPE_STRING)
META_PARAM(NAME,STREAM_LENGTH,T("Length"))
META_PARAM(TYPE,STREAM_LENGTH,TYPE_FILEPOS)
META_PARAM(NAME,STREAM_FLAGS,T("Flags"))
META_PARAM(TYPE,STREAM_FLAGS,TYPE_INT)
META_PARAM(NAME,STREAM_CONTENTTYPE,T("ContentType"))
META_PARAM(TYPE,STREAM_CONTENTTYPE,TYPE_STRING|TFLAG_RDONLY)
META_PARAM(NAME,STREAM_PRAGMA_SEND,T("PragmaSet"))
META_PARAM(TYPE,STREAM_PRAGMA_SEND,TYPE_STRING|TFLAG_RDONLY)
META_PARAM(NAME,STREAM_PRAGMA_GET,T("PragmaGet"))
META_PARAM(TYPE,STREAM_PRAGMA_GET,TYPE_STRING|TFLAG_RDONLY)
META_PARAM(NAME,STREAM_META,T("Meta"))
META_PARAM(TYPE,STREAM_META,TYPE_META)
META_PARAM(NAME,STREAM_SCOUTING,T("Scouting"))
META_PARAM(TYPE,STREAM_SCOUTING,TYPE_BOOLEAN)
META_PARAM(NAME,STREAM_TIMEOUT,T("TimeOut"))
META_PARAM(TYPE,STREAM_TIMEOUT,TYPE_INT) // TODO: TYPE_SYSTICK
META_PARAM(TYPE,STREAM_DATE,TYPE_DATETIME)
META_PARAM(NAME,STREAM_FULL_URL,T("FullURL"))
META_PARAM(TYPE,STREAM_FULL_URL,TYPE_STRING)
META_PARAM(TYPE,STREAM_PLAYING,TYPE_BOOLEAN)
META_PARAM(TYPE,STREAM_ENUM_BASE,TYPE_STRING)
META_PARAM(TYPE,STREAM_TIME,TYPE_TICK)
META_PARAM(TYPE,STREAM_NATIVE_HANDLER,TYPE_PTR|TFLAG_RDONLY)
META_PARAM(TYPE,STREAM_CACHING,TYPE_BOOLEAN)
META_PARAM(TYPE,STREAM_USERNAME,TYPE_STRING)
META_DYNAMIC(TYPE_STRING,STREAM_USERNAME)
META_PARAM(TYPE,STREAM_PASSWORD,TYPE_STRING)
META_DYNAMIC(TYPE_STRING,STREAM_PASSWORD)
META_PARAM(TYPE,STREAM_PROXY_USERNAME,TYPE_STRING)
META_DYNAMIC(TYPE_STRING,STREAM_PROXY_USERNAME)
META_PARAM(TYPE,STREAM_PROXY_PASSWORD,TYPE_STRING)
META_DYNAMIC(TYPE_STRING,STREAM_PROXY_PASSWORD)
META_PARAM(TYPE,STREAM_CACHE_CLASS,TYPE_FOURCC|TFLAG_RDONLY)
META_VMT(TYPE_FUNC,stream_vmt,Open,DummyOpen)
META_VMT(TYPE_FUNC,stream_vmt,Duplicate,DummyDuplicate)
META_VMT(TYPE_FUNC,stream_vmt,Read,DummyRead)
META_VMT(TYPE_FUNC,stream_vmt,ReadOneOrMore,DummyReadOneOrMore)
META_VMT(TYPE_FUNC,stream_vmt,ReadBlock,DummyReadBlock)
META_VMT(TYPE_FUNC,stream_vmt,Write,DummyWrite)
META_VMT(TYPE_FUNC,stream_vmt,Seek,DummySeek)
META_VMT(TYPE_FUNC,stream_vmt,Blocking,DummyBlocking)
META_VMT(TYPE_FUNC,stream_vmt,OpenDir,DummyOpenDir)
META_VMT(TYPE_FUNC,stream_vmt,EnumDir,DummyEnumDir)
META_VMT(TYPE_FUNC,stream_vmt,Wait,DummyWait)
META_VMT(TYPE_FUNC,stream_vmt,Skip,DummySkip)
META_VMT(TYPE_FUNC,stream_vmt,Flush,DummyFlush)
META_VMT(TYPE_FUNC,stream_vmt,ResetReadTimeout,DummyResetReadTimeout)
META_END_CONTINUE(MEDIA_CLASS) // STREAMPROCESS_CLASS can have NODE_EXTS

META_START_CONTINUE(STREAMPROCESS_CLASS)
META_CLASS(FLAGS,CFLAG_ABSTRACT)
META_PARAM(NAME,STREAMPROCESS_INPUT,T("Input"))
META_PARAM(TYPE,STREAMPROCESS_INPUT,TYPE_NODE)
META_PARAM(CLASS,STREAMPROCESS_INPUT,STREAM_CLASS)
META_VMT(TYPE_FUNC,stream_vmt,Blocking,ProcessBlocking)
META_VMT(TYPE_FUNC,stream_vmt,Wait,ProcessWait)
META_END(STREAM_CLASS) 

stream* StreamOpen(anynode *AnyNode, const tchar_t* Path, int Flags)
{
	stream* File = GetStream(AnyNode,Path,Flags);
	if (File)
	{
		err_t Err = Stream_Open(File,Path,Flags);
        if (Err != ERR_NONE && Err != ERR_NEED_MORE_DATA)
		{
			NodeDelete((node*)File);
			File = NULL;
		}
        else
        {
            stream* Buf;
            if ((Flags & SFLAG_BUFFERED) && (Buf = (stream*)NodeCreate(AnyNode,BUFSTREAM_CLASS)) != NULL)
            {
                Node_SET(Buf,BUFSTREAM_STREAM,&File);
                File = Buf;
            }
        }
	}
	return File;
}

void StreamClose(stream* File)
{
	NodeDelete((node*)File);
}

bool_t StreamGenExts(anynode* AnyNode,array* Exts, fourcc_t ClassFilter, const tchar_t* TypeFilter)
{
	fourcc_t* i;
	array List;
    ArrayInit(Exts);

    if (TypeFilter && !TypeFilter[0])
        TypeFilter = NULL;

	NodeEnumClass(AnyNode,&List,ClassFilter);
	for (i=ARRAYBEGIN(List,fourcc_t);i!=ARRAYEND(List,fourcc_t);++i)
	{
		const tchar_t* s = NodeStr2(AnyNode,*i,NODE_EXTS);
		while (s && s[0])
		{
            size_t n;
            for (n=0;s[n] && s[n]!=';' && s[n]!=':';++n) {}

            if (!TypeFilter || (s[n]==':' && tcschr(TypeFilter,s[n+1])!=NULL))
            {
                while (s[n] && s[n]!=';')
                    ++n;

                if (n)
                {
                    if (!ARRAYEMPTY(*Exts))
                        ArrayAppend(Exts,T(";"),sizeof(tchar_t),64);
                    ArrayAppend(Exts,s,n*sizeof(tchar_t),64);
                }
            }

            s = tcschr(s,';');
            if (s) ++s;
		}
	}
	ArrayClear(&List);

    if (!ARRAYEMPTY(*Exts) && !ArrayAppend(Exts,T("\0"),sizeof(tchar_t),64))
        ArrayClear(Exts);

    return !ARRAYEMPTY(*Exts);
}

char StreamExtType(anynode* AnyNode, fourcc_t ClassFilter, const tchar_t *Ext)
{
    char Result = FTYPE_UNDEFINED;
    tchar_t *s;
    size_t i;
    array List;
    StreamGenExts(AnyNode,&List,ClassFilter,NULL);

    for (s=ARRAYBEGIN(List,tchar_t);s;)
    {
        for (i=0;s[i] && s[i]==Ext[i];i++) {}

        if (!Ext[i] && s[i] == ':')
        {
            Result = (char)s[i+1];
            break;
        }

        s = tcschr(s,';');
        if (s) ++s;
    }

    ArrayClear(&List);
    return Result;
}

stream* GetStream(anynode *AnyNode, const tchar_t* URL, int Flags)
{
	tchar_t Protocol[MAXPROTOCOL];
	stream* Stream = NULL;
    fourcc_t FourCC;

    GetProtocol(URL,Protocol,TSIZEOF(Protocol),NULL);

    FourCC = NodeEnumClassStr(AnyNode,NULL,STREAM_CLASS,NODE_PROTOCOL,Protocol);

#if defined(CONFIG_STREAM_CACHE)
    if ((Flags & (SFLAG_NO_CACHING|SFLAG_WRONLY|SFLAG_CREATE))==0)
        Stream = (stream*)NodeCreate(AnyNode,NodeClass_Meta(NodeContext_FindClass(AnyNode,FourCC),STREAM_CACHE_CLASS,META_PARAM_CUSTOM));
#endif

    if (!Stream)
        Stream = (stream*)NodeCreate(AnyNode,FourCC);

    if (Stream && (Flags & SFLAG_NON_BLOCKING))
        Stream_Blocking(Stream,0);

    if (!Stream && !(Flags & SFLAG_SILENT))
    {
	    tcsupr(Protocol);
	    NodeReportError(AnyNode,NULL,ERR_ID,ERR_PROTO_NOT_FOUND,Protocol);
    }
#if defined(CONFIG_DEBUGCHECKS)
    if (Stream)
        tcscpy_s(Stream->URL,TSIZEOF(Stream->URL),URL);
#endif
	return Stream;
}

int StreamProtocolPriority(anynode *AnyNode, const tchar_t* URL)
{
	tchar_t Protocol[MAXPROTOCOL];
    GetProtocol(URL,Protocol,TSIZEOF(Protocol),NULL);
    if (tcsicmp(Protocol,T("file"))==0) // override for local files
        return PRI_MAXIMUM;
    return NodeClass_Priority(NodeContext_FindClass(AnyNode,NodeEnumClassStr(AnyNode,NULL,STREAM_CLASS,NODE_PROTOCOL,Protocol)));
}
