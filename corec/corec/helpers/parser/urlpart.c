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

#define	URLPART_ID        FOURCC('U','P','A','T')
#define URLPART_URL       0x100
#define URLPART_MIME      0x101

#define URLPART_SEPARATOR  T("$")
#define URLPART_SEP_CHAR   T('$')
#define URLPART_SEP_ESCAPE T("$$")

typedef struct urlpart
{
    stream Base;
    stream *Input;
    filepos_t Pos;
    size_t Length;
    bool_t Blocking;

} urlpart;

err_t CreateUrlPart(nodecontext *Context, tchar_t *Out, size_t OutLen, const tchar_t *URL, filepos_t Offset, size_t Length, const tchar_t *Mime)
{
    datetime_t FileDate;
    if (!URL || !URL[0])
        return ERR_INVALID_DATA;

    stprintf_s(Out,OutLen,T("urlpart://%s"),URL);
    tcsreplace(Out,OutLen,URLPART_SEPARATOR,URLPART_SEP_ESCAPE);
    tcscat_s(Out,OutLen,URLPART_SEPARATOR);
    if (Offset > 0)
        stcatprintf_s(Out,OutLen,T("ofs=%d#"),(int)Offset); // TODO: support 64 bits offset
    if (Length > 0)
        stcatprintf_s(Out,OutLen,T("len=%d#"),(int)Length);
    if (Mime)
        stcatprintf_s(Out,OutLen,T("mime=%s#"),Mime);
    FileDate = FileDateTime(Context,URL);
    if (FileDate != INVALID_DATETIME_T)
        stcatprintf_s(Out,OutLen,T("date=%d#"),(int)FileDate);
    return ERR_NONE;
}

static void Clear(urlpart* p)
{
    Node_RemoveData((node*)p,STREAM_URL,TYPE_STRING);
    Node_RemoveData((node*)p,URLPART_URL,TYPE_STRING);
    Node_RemoveData((node*)p,URLPART_MIME,TYPE_STRING);
    p->Length = (size_t)-1;
    p->Pos = INVALID_FILEPOS_T;
    p->Input = 0;
}

static err_t Get(urlpart* p,dataid Id, void* Data, size_t Size)
{
	if (!p->Input)
		return ERR_INVALID_DATA;
    switch (Id)
    {
    case STREAM_LENGTH: (*(filepos_t*)Data)=p->Length; return ERR_NONE;
    case STREAM_URL: return INHERITED(p,node_vmt,URLPART_ID)->Get(p,Id,Data,Size);
    case STREAM_CONTENTTYPE:
        {
            tchar_t *Mime = Node_GetData((node*)p,URLPART_MIME,TYPE_STRING);
            if (Mime)
            {
                tcscpy_s(Data,Size/sizeof(tchar_t),Mime);
                return ERR_NONE;
            }
        }
    }
	return Node_Get(p->Input,Id,Data,Size);
}

static err_t Set(urlpart* p,dataid Id, const void* Data, size_t Size)
{
	if (!p->Input)
		return ERR_INVALID_DATA;
	return Node_Set(p->Input,Id,Data,Size);
}

static err_t Open(urlpart* p, const tchar_t* URL, int Flags)
{
    err_t Result;
    const tchar_t *String, *Equal;
    tchar_t Value[MAXPATHFULL];
    datetime_t Date = INVALID_DATETIME_T;

    String = tcsrchr(URL,URLPART_SEP_CHAR);
    if (!String)
        return ERR_INVALID_DATA;
    
    Clear(p);
    Node_SetData((node*)p,STREAM_URL,TYPE_STRING,URL);

    Equal = GetProtocol(URL,NULL,0,NULL);
    tcsncpy_s(Value,TSIZEOF(Value),Equal,String-Equal);
    tcsreplace(Value,TSIZEOF(Value),URLPART_SEP_ESCAPE,URLPART_SEPARATOR);
    Node_SetData((node*)p,URLPART_URL,TYPE_STRING,Value);
    while (String++ && *String)
    {
        Equal = tcschr(String,T('='));
        if (Equal)
        {
            tchar_t *Sep = tcschr(Equal,T('#'));
            if (Sep)
                tcsncpy_s(Value,TSIZEOF(Value),Equal+1,Sep-Equal-1);
            else
                tcscpy_s(Value,TSIZEOF(Value),Equal+1);

            if (tcsncmp(String,T("ofs"),Equal-String)==0)
                p->Pos = StringToInt(Value,0);
            else if (tcsncmp(String,T("len"),Equal-String)==0)
                p->Length = StringToInt(Value,0);
            else if (tcsncmp(String,T("mime"),Equal-String)==0)
                Node_SetData((node*)p,URLPART_MIME,TYPE_STRING,Value);
            else if (tcsncmp(String,T("date"),Equal-String)==0)
                Date = StringToInt(Value,0);
        }
        String = tcschr(String,'#');
    }

    if (Date!=INVALID_DATETIME_T && Date != FileDateTime(Node_Context(p),Node_GetDataStr((node*)p,URLPART_URL)))
        return ERR_INVALID_DATA;

    p->Input = GetStream(p,Node_GetDataStr((node*)p,URLPART_URL),Flags);
    if (!p->Input)
        return ERR_NOT_SUPPORTED;
    Stream_Blocking(p->Input,p->Blocking);
    Result = Stream_Open(p->Input,Node_GetDataStr((node*)p,URLPART_URL),Flags);
    if (Result == ERR_NONE && p->Pos!=INVALID_FILEPOS_T) // TODO: support asynchronous stream opening
    {
        if (Stream_Seek(p->Input,p->Pos,SEEK_SET)!=p->Pos)
            return ERR_INVALID_DATA;
    }
    return Result;
}

static filepos_t Seek(urlpart* p,filepos_t Pos,int SeekMode)
{
	if (!p->Input)
		return INVALID_FILEPOS_T;

    switch (SeekMode)
    {
    case SEEK_SET:
        Pos = min(Pos,(filepos_t)p->Length);
        assert(p->Pos!=INVALID_FILEPOS_T);
        return Stream_Seek(p->Input,Pos+p->Pos,SeekMode);
    case SEEK_CUR:
        return Stream_Seek(p->Input,Pos,SeekMode);
    case SEEK_END:
        Pos = max(Pos,-(filepos_t)p->Length);
        assert(p->Pos!=INVALID_FILEPOS_T);
        return Stream_Seek(p->Input,Pos+p->Pos+p->Length,SeekMode);
    }
    return INVALID_FILEPOS_T;
}

static err_t Read(urlpart* p,void* Data,size_t Size,size_t* Readed)
{
    if (!p->Input) {
        if (Readed)
            *Readed=0;
        return ERR_INVALID_DATA;
    }
	return Stream_Read(p->Input,Data,Size,Readed);
}

static err_t ReadBlock(urlpart* p,block* Block,size_t Ofs,size_t Size,size_t* Readed)
{
    if (!p->Input) {
        if (Readed)
            *Readed=0;
        return ERR_INVALID_DATA;
    }
	return Stream_ReadBlock(p->Input,Block,Ofs,Size,Readed);
}

static err_t Blocking(urlpart* p,bool_t Blocking)
{
	p->Blocking = Blocking;
	if (p->Input)
		return Stream_Blocking(p->Input,Blocking);
	return Blocking ? ERR_NONE : ERR_NOT_SUPPORTED;
}

static err_t WaitStream(urlpart* p,bool_t Read, streamselect* Select)
{
	if (!p->Input)
		return ERR_NOT_SUPPORTED;
	return Stream_Wait(p->Input,Read,Select);
}

static err_t SkipStream(urlpart* p,intptr_t* Skip)
{
	if (!p->Input)
		return ERR_NOT_SUPPORTED;
	return Stream_Skip(p->Input,Skip);
}

static err_t Create(urlpart* p)
{
    p->Blocking = 1;
    return ERR_NONE;
}

static void Delete(urlpart* p)
{
    if (p->Input)
	    Node_Release(p->Input);
    Clear(p);
}

META_START(UrlPart_Class,URLPART_ID)
META_CLASS(SIZE,sizeof(urlpart))
META_PARAM(STRING,NODE_PROTOCOL,T("urlpart"))
META_CLASS(CREATE,Create)
META_CLASS(DELETE,Delete)
META_VMT(TYPE_FUNC,node_vmt,Get,Get)
META_VMT(TYPE_FUNC,node_vmt,Set,Set)
META_VMT(TYPE_FUNC,stream_vmt,Blocking,Blocking)
META_VMT(TYPE_FUNC,stream_vmt,Open,Open)
META_VMT(TYPE_FUNC,stream_vmt,Seek,Seek)
META_VMT(TYPE_FUNC,stream_vmt,Read,Read)
META_VMT(TYPE_FUNC,stream_vmt,ReadBlock,ReadBlock)
META_VMT(TYPE_FUNC,stream_vmt,Wait,WaitStream)
META_VMT(TYPE_FUNC,stream_vmt,Skip,SkipStream)
META_DYNAMIC(TYPE_STRING,STREAM_URL)
META_END(STREAM_CLASS)
