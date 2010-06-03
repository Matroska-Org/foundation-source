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

// TODO: don't depend on this or a part of it
#include "common.h"
//end TODO

#if defined(TARGET_PALMOS)

#define FILEDB_ID		FOURCC('F','I','D','B')

#include "pace.h"

typedef struct filedb
{
	stream Stream;

	FileHand File;
	filepos_t Length;
	int Flags;

	DmSearchStateType SearchState;
	LocalID CurrentDB; 
	Boolean NewEnum;

  	tchar_t URL[MAXPATH];

} filedb;

static err_t Read(filedb* p,void* Data,size_t Size,size_t* Readed)
{
    err_t Result;
	Err Error;
	Int32 n = FileRead(p->File,Data,1,Size,&Error);

    if (n<0)
        n=0;

    if ((size_t)n != Size)
        Result = (Error == fileErrEOF) ? ERR_END_OF_FILE : ERR_READ;
    else
        Result = ERR_NONE;

    if (Readed)
        *Readed = n;

	return Result;
}

static err_t ReadBlock(filedb* p,block* Block,size_t Ofs,size_t Size,size_t* Readed)
{
    err_t Result;
	Err Error;
    Int32 n;

	if (IsHeapStorage(Block))
		n = FileDmRead(p->File,(void*)Block->Ptr,Ofs,1,Size,&Error);
	else
		n = FileRead(p->File,(void*)(Block->Ptr+Ofs),1,Size,&Error);

    if (n<0)
        n=0;

    if ((size_t)n != Size)
    {
        Result = (Error == fileErrEOF) ? ERR_END_OF_FILE : ERR_READ;
        n=0;
    }
    else
        Result = ERR_NONE;

    if (Readed)
        *Readed = n;

	return Result;
}

static err_t Write(filedb* p,const void* Data,size_t Size,size_t* Written)
{
    err_t Result;
	Err Error;
	Int32 n = FileWrite(p->File,Data,1,Size,&Error);

	if (n<=0)
    {
        Result = ERR_WRITE;
        n = 0;
    }
    else
        Result = ((size_t)n == Size) ? ERR_NONE : ERR_WRITE;
    
    if (Written)
        *Written = n;

	return Result;
}

static filepos_t Seek(filedb* p,filepos_t Pos,int SeekMode)
{
	FileOriginEnum Origin;
	Err Error;

	switch (SeekMode)
	{
	default:
	case SEEK_SET: Origin = fileOriginBeginning; break;
	case SEEK_CUR: Origin = fileOriginCurrent; break;
	case SEEK_END: Origin = fileOriginEnd; break;
	}

	Error = FileSeek(p->File,Pos,Origin);

	if (Error == errNone || Error == fileErrEOF)
		return FileTell(p->File,NULL,NULL);

	return INVALID_FILEPOS_T;
}

static err_t Open(filedb* p, const tchar_t* URL, int Flags)
{
	if (p->File)
	{
		FileClose(p->File);
		p->File = NULL;
	}
	
	p->Length = INVALID_FILEPOS_T;
    p->URL[0] = 0;

	if (URL)
	{
		UInt32 Mode;

		if (Flags & SFLAG_RDONLY && !(Flags & SFLAG_WRONLY))
			Mode = fileModeReadOnly;
		else
			Mode = fileModeUpdate; // readwrite mode, no writeonly

		Mode | = fileModeAnyTypeCreator;

		p->File = FileOpen(0,GetProtocol(URL,NULL,0,NULL), 0, 0, Mode, NULL);

		if (!p->File)
		{
			if (!(Flags & SFLAG_SILENT))
				NodeReportError(p,NULL,ERR_ID,ERR_FILE_NOT_FOUND,URL);
			return ERR_FILE_NOT_FOUND;
		}

        p->Flags = Flags;
		tcscpy_s(p->URL,TSIZEOF(p->URL),URL);

        if (Flags & SFLAG_CREATE)
            FileTruncate(p->File,0);
        else
        {
			if (FileSeek(p->File,0,fileOriginEnd) == errNone)
				p->Length = FileTell(p->File,NULL,NULL);

			FileSeek(p->File,0,fileOriginBeginning);
        }
	}
	return ERR_NONE;
}

bool_t DBFrom(uint16_t Card,uint32_t DB,tchar_t* URL,int URLLen)
{
	char Name[48];
	UInt16 Attr = 0;

	DmDatabaseInfo(Card,DB,Name,&Attr,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
	if (Attr & dmHdrAttrStream)
	{
		stprintf_s(URL,URLLen,T("mem://%s"),Name);
		return 1;
	}
	return 0;
}

static err_t OpenDir(filedb* p,const tchar_t* URL,int UNUSED_PARAM(Flags))
{
    const tchar_t* Name = GetProtocol(URL,NULL,0,NULL);
    if (Name[0])
    {
        FileHand File = FileOpen(0,GetProtocol(URL,NULL,0,NULL), 0, 0, fileModeReadOnly | fileModeAnyTypeCreator, NULL);
		if (File)
			FileClose(File);
        return File ? ERR_NOT_DIRECTORY:ERR_FILE_NOT_FOUND;
    }
	p->CurrentDB = 0;
	p->NewEnum = 1;
    return ERR_NONE;
}

extern datetime_t PalmToDateTime(UInt32);

static err_t EnumDir(filedb* p,const tchar_t* Exts,bool_t ExtFilter,streamdir* Item)
{
	UInt16 card; 

	Item->FileName[0] = 0;
	Item->DisplayName[0] = 0;

	while (DmGetNextDatabaseByTypeCreator(p->NewEnum, &p->SearchState, 0, 0, 0, &card, &p->CurrentDB)==errNone)
	{
		char Name[48];
		UInt16 Attr = 0;
		UInt32 Date = 0;
		UInt32 Size = 0;

		DmDatabaseInfo(card,p->CurrentDB,Name,&Attr,NULL,NULL,&Date,NULL,NULL,NULL,NULL,NULL,NULL);

		if (Attr & dmHdrAttrStream)
		{
			Item->Type = CheckExts(Name,Exts);
			if (Item->Type || !ExtFilter)
			{
				DmDatabaseSize(card,p->CurrentDB,NULL,NULL,&Size);

				tcscpy_s(Item->FileName,TSIZEOF(Item->FileName),Name);
				Item->ModifiedDate = PalmToDateTime(Date);
				Item->Size = Size;
				return ERR_NONE;
			}
		}

		p->NewEnum = 0;
	}

	return ERR_END_OF_FILE;
}

static void Delete(filedb* p)
{
	Open(p,NULL,0);
}

META_START(FileDb_Class,FILEDB_ID)
META_CLASS(SIZE,sizeof(filedb))
META_CLASS(PRIORITY,PRI_MINIMUM)
META_CLASS(DELETE,Delete)
META_VMT(TYPE_FUNC,stream_vmt,Open,Open)
META_VMT(TYPE_FUNC,stream_vmt,Read,Read)
META_VMT(TYPE_FUNC,stream_vmt,ReadBlock,ReadBlock)
META_VMT(TYPE_FUNC,stream_vmt,Write,Write)
META_VMT(TYPE_FUNC,stream_vmt,Seek,Seek)
META_VMT(TYPE_FUNC,stream_vmt,OpenDir,OpenDir)
META_VMT(TYPE_FUNC,stream_vmt,EnumDir,EnumDir)
META_DATA_RDONLY(TYPE_INT,STREAM_FLAGS,filedb,Flags)
META_DATA_RDONLY(TYPE_STRING,STREAM_URL,filedb,URL)
META_DATA_RDONLY(TYPE_FILEPOS,STREAM_LENGTH,filedb,Length)
META_PARAM(STRING,NODE_PROTOCOL,T("mem,conf"))
META_END(STREAM_CLASS)

#endif

