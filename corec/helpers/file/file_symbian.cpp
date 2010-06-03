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

#if defined(TARGET_SYMBIAN)

#ifndef SYMBIAN90
#define EFileShareReadersOrWriters EFileShareAny
#endif

typedef struct filestream
{
	stream Stream;
	tchar_t URL[MAXPATH];
	RFile* File;
	filepos_t Length;
	filepos_t Pos;
	int Flags;

	CDir* Dir;
	TInt DirPos;
    TInt DrivePos;

} filestream;

static err_t Open(filestream* p, const tchar_t* URL, int Flags)
{
	if (p->File)
	{
		p->File->Close();
		delete p->File;
		p->File = NULL;
	}

	p->Length = INVALID_FILEPOS_T;
	if (URL != p->URL)
		p->URL[0] = 0;

	if (URL)
	{
        RFs& Session = Node_FsSession(p);
		TPtrC _URL((const TText*)URL);
		RFile* File = new RFile;
		TInt Result;
        TInt Mode;

        if (!File)
            return ERR_OUT_OF_MEMORY;

        if (Flags & SFLAG_RDONLY && !(Flags & SFLAG_WRONLY))
            Mode = EFileRead;
        else
            Mode = EFileWrite; // readwrite mode, no writeonly

        Mode |= EFileShareReadersOrWriters;

		if (Flags & SFLAG_CREATE)
			Result = File->Replace(Session,_URL,Mode);
		else
			Result = File->Open(Session,_URL,Mode);

		if (Result != KErrNone)
		{
			delete File;
			if ((Flags & (SFLAG_REOPEN|SFLAG_SILENT))==0)
				NodeReportError(p,NULL,ERR_ID,ERR_FILE_NOT_FOUND,URL);
			return ERR_FILE_NOT_FOUND;
		}

		if (Flags & SFLAG_CREATE)
		{
	        if (Flags & SFLAG_HIDDEN)
	            File->SetAtt(KEntryAttHidden,0);
	        else
	            File->SetAtt(0,KEntryAttHidden);
		}

		tcscpy_s(p->URL,TSIZEOF(p->URL),URL);
        p->Flags = Flags & ~SFLAG_REOPEN;
		p->File = File;

		if (File->Size(p->Length) != KErrNone)
			p->Length = INVALID_FILEPOS_T;

		if (Flags & SFLAG_REOPEN)
			File->Seek(ESeekStart,p->Pos);
		else
			p->Pos = 0;
	}

	return ERR_NONE;
}

static err_t Read(filestream* p,void* Data,size_t Size,size_t* Readed)
{
    if (Readed)
    	*Readed = 0;
    
    if (!p->File)
    	return ERR_INVALID_DATA;

    err_t Err;
    size_t n;
	TPtr8 Buffer((uint8_t*)Data,0,Size);
	TInt Result = p->File->Read(Buffer,Size);

	if (Result == KErrNone)
	{
		n = Buffer.Length();
		p->Pos += n;
        Err = (n == Size) ? ERR_NONE : ERR_END_OF_FILE;
	}
    else
    {
    	//todo: reopen when card reinserted...

        Err = ERR_READ;
        n = 0;
    }

    if (Readed)
        *Readed = n;

    return Err;
}

static err_t ReadBlock(filestream* p,block* Block,size_t Ofs,size_t Size,size_t* Readed)
{
	return Read(p,(void*)(Block->Ptr+Ofs),Size,Readed);
}

static filepos_t Seek(filestream* p,filepos_t Pos,int SeekMode)
{
	if (!p->File)
		return INVALID_FILEPOS_T;

	TSeek Mode;
	switch (SeekMode)
	{
	default:
	case SEEK_SET: Mode = ESeekStart; break;
	case SEEK_CUR: Mode = ESeekCurrent; break;
	case SEEK_END: Mode = ESeekEnd; break;
	}

	if (p->File->Seek(Mode,Pos) != KErrNone)
		return INVALID_FILEPOS_T;

    p->Pos = Pos;
	return Pos;
}

static err_t Write(filestream* p,const void* Data,size_t Size,size_t* Written)
{
    if (Written)
    	*Written = 0;

	if (!p->File)
		return ERR_INVALID_DATA;

	size_t n;
    err_t Err;
	TPtrC8 Buffer((const uint8_t*)Data,Size);
	int Result = p->File->Write(Buffer,Size);

	if (Result == KErrNone)
	{
        n = Buffer.Length();
    	p->Pos += n;
        Err = (n == Size) ? ERR_NONE : ERR_WRITE;
	}
    else
    {
        n = 0;
        Err = ERR_WRITE;
    }

    if (Written)
        *Written = n;

	return Err;
}

static err_t SetLength(filestream* p, dataid UNUSED_PARAM(Id),const filepos_t* Data,size_t Size)
{
    if (!p->File)
    	return ERR_INVALID_DATA;

	err_t Result = ERR_NONE;
	filepos_t Pos = 0;

    if (Size != sizeof(filepos_t))
        return ERR_INVALID_DATA;

	if (p->File->SetSize(*Data) != KErrNone)
		Result = ERR_BUFFER_FULL;

	if (p->File->Seek(ESeekCurrent,Pos) == KErrNone)
		p->Pos = Pos;

	return Result;
}

static err_t OpenDir(filestream* p,const tchar_t* URL,int UNUSED_PARAM(Flags))
{
	TInt Result;
	TBool Folder;

    if (p->Dir)
	{
		delete p->Dir;
		p->Dir = NULL;
	}
    p->DrivePos = -1;

    if (!URL[0])
    {
        p->DrivePos = EDriveA;
    }
    else
    {
		TFileName Path;
		Path = (const TText*)URL;

		if (EikFileUtils::IsFolder(Path,Folder)==KErrNone && !Folder)
			return ERR_NOT_DIRECTORY;

		Path.Append(_L("\\"));
		if (!EikFileUtils::PathExists(Path))
			return ERR_FILE_NOT_FOUND;

		Path.Append(_L("*.*"));

		TFindFile Find(Node_FsSession(p));
		Result = Find.FindWildByPath(Path,NULL,p->Dir);

		if (Result != KErrNone)
			return ERR_END_OF_FILE;

		p->DirPos = 0;
    }

    return ERR_NONE;
}

extern datetime_t SymbianToDateTime(TTime);

static err_t EnumDir(filestream* p,const tchar_t* Exts,bool_t ExtFilter,streamdir* Item)
{
	Item->FileName[0] = 0;
	Item->DisplayName[0] = 0;

    if (p->DrivePos>=0)
    {
        RFs& Session = Node_FsSession(p);

	    while (!Item->FileName[0] && p->DrivePos <= EDriveZ)
	    {
            TChar ch;
            TDriveInfo Info; 
            Session.Drive(Info,p->DrivePos);

            if ((TInt)Info.iDriveAtt != (TInt)KDriveAbsent && Session.DriveToChar(p->DrivePos,ch)==KErrNone)
            {
			    Item->FileName[0] = (tchar_t)ch;
			    Item->FileName[1] = ':';
			    Item->FileName[2] = 0;
			    Item->ModifiedDate = INVALID_DATETIME_T;
			    Item->Size = INVALID_FILEPOS_T;
                Item->Type = FTYPE_DIR;
		    }

            ++p->DrivePos;
	    }
    }
    else
    {
	    while (!Item->FileName[0] && p->Dir && p->DirPos<p->Dir->Count())
	    {
		    const TEntry* Entry = &(*p->Dir)[p->DirPos];

		    if (Entry->iName.Length()>0 && Entry->iName[0]!='.' && // skip unix/mac hidden files and . .. directory entries
				!Entry->IsHidden() &&
                Entry->iName.CompareF(_L("_PAlbTN"))!=0) // skip thumb directory
		    {
			    TPtr Name((TText*)Item->FileName,TSIZEOF(Item->FileName)-1);
			    Name = Entry->iName;
			    Item->FileName[Name.Length()] = 0;

			    Item->ModifiedDate = SymbianToDateTime(Entry->iModified);

			    if (Entry->IsDir())
                {
				    Item->Size = INVALID_FILEPOS_T;
                    Item->Type = FTYPE_DIR;
                }
			    else
			    {
				    Item->Size = Entry->iSize;
				    Item->Type = CheckExts(Item->FileName,Exts);

				    if (!Item->Type && ExtFilter)
					    Item->FileName[0] = 0; // skip
			    }
		    }
		    
		    ++p->DirPos;
	    }
    }

	if (!Item->FileName[0])
	{
		if (p->Dir)
		{
			delete p->Dir;
			p->Dir = NULL;
		}
        p->DrivePos = -1;
		return ERR_END_OF_FILE;
	}

	return ERR_NONE;
}

static void Delete(filestream* p)
{
	Open(p,NULL,0);
	if (p->Dir)
		delete p->Dir;
}

META_START(File_Class,FILE_CLASS)
META_CLASS(SIZE,sizeof(filestream))
META_CLASS(PRIORITY,PRI_MINIMUM)
META_CLASS(DELETE,Delete)
META_VMT(TYPE_FUNC,stream_vmt,Open,Open)
META_VMT(TYPE_FUNC,stream_vmt,Read,Read)
META_VMT(TYPE_FUNC,stream_vmt,ReadBlock,ReadBlock)
META_VMT(TYPE_FUNC,stream_vmt,Write,Write)
META_VMT(TYPE_FUNC,stream_vmt,Seek,Seek)
META_VMT(TYPE_FUNC,stream_vmt,OpenDir,OpenDir)
META_VMT(TYPE_FUNC,stream_vmt,EnumDir,EnumDir)
META_DATA_RDONLY(TYPE_INT,STREAM_FLAGS,filestream,Flags)
META_DATA_RDONLY(TYPE_STRING,STREAM_URL,filestream,URL)
META_PARAM(SET,STREAM_LENGTH,SetLength)
META_DATA(TYPE_FILEPOS,STREAM_LENGTH,filestream,Length)
META_PARAM(STRING,NODE_PROTOCOL,T("file"))
META_END(STREAM_CLASS)

datetime_t FileDateTime(nodecontext *p, const tchar_t* URL)
{
    datetime_t Date = INVALID_DATETIME_T;
    RFs& Session = Node_FsSession(p);
	TPtrC _URL((const TText*)URL);
    TTime Time;
	if (Session.Modified(_URL,Time) == KErrNone)
        Date = SymbianToDateTime(Time);
    return Date;
}

bool_t FileExists(nodecontext *p, const tchar_t* URL)
{
	return BaflUtils::FileExists(Node_FsSession(p),TPtrC((const TText*)URL)) != 0;
}

bool_t FileErase(nodecontext *p, const tchar_t* URL, bool_t Force, bool_t Safe)
{
    if (Force)
    {
        TUint Attribs;
        if (Node_FsSession(p).Att(TPtrC((const TText*)URL),Attribs) == KErrNone && (Attribs & KEntryAttReadOnly)!=0)
        {
            Node_FsSession(p).SetAtt(TPtrC((const TText*)URL),0,KEntryAttReadOnly);
        }
    }
	return BaflUtils::DeleteFile(Node_FsSession(p),TPtrC((const TText*)URL)) != KErrNone;
}

bool_t FolderErase(nodecontext *p, const tchar_t* URL, bool_t Force, bool_t Safe)
{
	TFileName Path;
	Path = (const TText*)URL;
	Path.Append(_L("\\"));
    if (Force)
    {
        TUint Attribs;
        if (Node_FsSession(p).Att(Path,Attribs) == KErrNone && (Attribs & KEntryAttReadOnly)!=0)
        {
            Node_FsSession(p).SetAtt(Path,0,KEntryAttReadOnly);
        }
    }
	return Node_FsSession(p).RmDir(Path) == KErrNone;
}

bool_t PathIsFolder(nodecontext *p, const tchar_t* URL)
{
    TBool IsFolder;
    if ((!IsAlpha(URL[0]) || URL[1]!=':') && (URL[0]!='\\'))
    	return 0;
	if (BaflUtils::IsFolder(Node_FsSession(p),TPtrC((const TText*)URL),IsFolder)!=KErrNone)
        return 0;
    return (IsFolder!=0);
}

bool_t FileMove(nodecontext *p, const tchar_t* In,const tchar_t* Out)
{
    TPtrC _In((const TText*)In);
    TPtrC _Out((const TText*)Out);
    return BaflUtils::RenameFile(Node_FsSession(p),_In,_Out) == KErrNone;
}

bool_t FolderCreate(nodecontext *p, const tchar_t* URL)
{
	TFileName Path;
	Path = (const TText*)URL;
	Path.Append(_L("\\"));
	return Node_FsSession(p).MkDir(Path) == KErrNone;
}

void FindFiles(nodecontext *p, const tchar_t* URL, const tchar_t* Mask,void(*Process)(const tchar_t*,void*),void* Param)
{
	tchar_t FindPath[MAXPATH];
	CDir* Dir = NULL;
	TFindFile Find(Node_FsSession(p));

    TPtrC _Mask((const TText*)Mask);
	TFileName Path;
	Path = (const TText*)URL;
	Path.Append(_Mask);

	if (Find.FindWildByPath(Path,NULL,Dir) == KErrNone && Dir)
    {
        for (TInt i=0;i<Dir->Count();++i)
        {
		    const TEntry* Entry = &(*Dir)[i];

		    if (Entry->iName.Length()>0 && Entry->iName[0]!='.') // skip unix/mac hidden files and . .. directory entries
		    {
    			tcscpy_s(FindPath,TSIZEOF(FindPath),URL);
                size_t n = tcslen(FindPath);
			    TPtr Name((TText*)(FindPath+n),TSIZEOF(FindPath)-1-n);
			    Name = Entry->iName;
    		    FindPath[n+Name.Length()] = 0;
	    	    Process(FindPath,Param);
            }
        }
    }
	if (Dir)
		delete Dir;
}

stream *FileTemp(anynode* UNUSED_PARAM(Any))
{
#ifndef TODO
    assert(NULL); // TODO: not supported yet
#endif
    return NULL;
}

bool_t FileTempName(anynode* UNUSED_PARAM(Any),tchar_t* UNUSED_PARAM(Out), size_t UNUSED_PARAM(OutLen))
{
#ifndef TODO
    assert(NULL); // TODO: not supported yet
#endif
    return 0;
}

int64_t GetPathFreeSpace(nodecontext* UNUSED_PARAM(p), const tchar_t* Path)
{
#ifndef TODO
    assert(NULL); // TODO: not supported yet
    //TVolumeInfo volumeInfo;
    //return volumeInfo.iFree;
    return -1;
#endif
}

#endif
