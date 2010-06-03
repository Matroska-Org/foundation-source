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

#include "pace.h"

typedef struct vfs
{
	stream Stream;

	UInt16 Vol;
	FileRef	File;
	filepos_t Length;
	filepos_t Pos;
	int Flags;

    UInt32 Iter;

	tchar_t URL[MAXPATH];

} vfs;

static err_t Open(vfs* p, const tchar_t* URL, int Flags);

static err_t Read(vfs* p,void* Data,size_t Size,size_t* Readed)
{
    err_t Result;
	uint32_t n = 0;
	Err Error = VFSFileRead(p->File,Size,Data,&n);

	if ((Error==errNone && n>0) || Error==vfsErrFileEOF)
	{
		p->Pos += n;
        Result = Error==vfsErrFileEOF ? ERR_END_OF_FILE : ERR_NONE;
	}
    else
    {
        n = 0;
	    if (Error == vfsErrFileBadRef)
		    Open(p,p->URL,p->Flags | SFLAG_REOPEN);

        Result = ERR_READ;
    }

    if (Readed)
        *Readed = n;

	return Result;
}

static err_t Write(vfs* p,const void* Data,size_t Size,size_t* Written)
{
    err_t Result;
	uint32_t n = 0;
	Err Error = VFSFileWrite(p->File,Size,Data,&n);

	if (Error==errNone)
    {
		p->Pos += n;
        Result = (n == Size) ? ERR_NONE : ERR_WRITE;
    }
    else
    {
        n = 0;
        Result = ERR_WRITE;
	}

    if (Written)
        *Written = n;

	return Result;
}

static err_t ReadBlock(vfs* p,block* Block,size_t Ofs,size_t Size,size_t* Readed)
{
    err_t Result;
	uint32_t n = 0; 
	Err Error;

	if (IsHeapStorage(Block))
		Error = VFSFileReadData(p->File,Size,(void*)Block->Ptr,Ofs,&n);
	else
		Error = VFSFileRead(p->File,Size,(void*)(Block->Ptr+Ofs),&n);

	if ((Error==errNone && n>0) || Error==vfsErrFileEOF)
	{
		p->Pos += n;
        Result = Error==vfsErrFileEOF ? ERR_END_OF_FILE : ERR_NONE;
	}
    else
    {
        n = 0;
	    if (Error == vfsErrFileBadRef)
		    Open(p,p->URL,p->Flags | SFLAG_REOPEN);

        Result = ERR_READ;
    }

    if (Readed)
        *Readed = n;

	return Result;
}

static filepos_t Seek(vfs* p,filepos_t Pos,int SeekMode)
{
	FileOrigin Origin;
	Err Error;

	switch (SeekMode)
	{
	default:
	case SEEK_SET: Origin = vfsOriginBeginning; break;
	case SEEK_CUR: Origin = vfsOriginCurrent; break;
	case SEEK_END: Origin = vfsOriginEnd; break;
	}

	Error = VFSFileSeek(p->File,Origin,Pos);

	if (Error == errNone || Error == vfsErrFileEOF)
	{
		UInt32 Pos;
		if (VFSFileTell(p->File,&Pos) == errNone)
			p->Pos = Pos;
		return p->Pos;
	}

	return INVALID_FILEPOS_T;
}

static err_t SetLength(vfs* p,dataid Id,const filepos_t* Data,size_t Size)
{
	err_t Result = ERR_NONE;
	UInt32 Pos;

    if (Size != sizeof(filepos_t))
        return ERR_INVALID_DATA;

	if (VFSFileResize(p->File,*Data) != errNone)
		Result = ERR_BUFFER_FULL;

	if (VFSFileTell(p->File,&Pos) == errNone)
		p->Pos = Pos;

	return Result;
}

static UInt16 FindVol(int Slot)
{
	UInt16 Ref;
	UInt32 Iter = vfsIteratorStart;
	while (Iter != vfsIteratorStop && VFSVolumeEnumerate(&Ref,&Iter)==errNone)
	{
		VolumeInfoType Info;
		VFSVolumeInfo(Ref,&Info);

		if (Slot==0 && Info.mediaType == 'pose')
			return Ref;
 
		if (Slot<0 && Info.mountClass == vfsMountClass_Simulator && Info.slotRefNum==0xFFFF+Slot)
			return Ref;

		if (Slot>0 && Info.mountClass == vfsMountClass_SlotDriver && Info.slotRefNum==Slot)
			return Ref;
	}
	return vfsInvalidVolRef;
}

const tchar_t* VFSToVol(const tchar_t* URL,uint16_t* Vol)
{
	int Slot = 0;
	bool_t Found = 0;
	tchar_t Mime[MAXPATH];
	const tchar_t* Name = GetProtocol(URL,Mime,TSIZEOF(Mime),NULL);

	if (Name != URL)
		--Name; // need the '/'

	if (tcsncmp(Mime,"pose",4)==0 || stscanf(Mime,"slot%d",&Slot)==1 || stscanf(Mime,"vol%d",&Slot)==1)
		Found = 1;
	else
	if (stscanf(Mime,"simu%d",&Slot)==1)
	{
		Found = 1;
		Slot = -Slot;
	}
		
	if (Found && (*Vol=FindVol(Slot))!=vfsInvalidVolRef)
		return Name;

	return NULL;
}

bool_t VFSFromVol(uint16_t Vol,const tchar_t* Path,tchar_t* URL,int URLLen)
{
	int Slot;
	VolumeInfoType Info;
	VFSVolumeInfo(Vol,&Info);
	Slot = Info.slotRefNum;
	if (Slot > 0xFFF0)
		Slot = 0xFFFF-Slot;

	URL[0] = 0;
	if (!Path) Path = T("/");
	if (Info.mediaType == 'pose')
	{
		stprintf_s(URL,URLLen,T("pose:/%s"),Path);
		return 1;
	}
	if (Info.mountClass == vfsMountClass_Simulator)
	{
		stprintf_s(URL,URLLen,T("simu%d:/%s"),Slot,Path);
		return 1;
	}
	if (Info.mountClass == vfsMountClass_SlotDriver)
	{
		stprintf_s(URL,URLLen,(Info.attributes & vfsVolumeAttrNonRemovable)?T("vol%d:/%s"):T("slot%d:/%s"),Slot,Path);
		return 1;
	}
	return 0;
}

extern datetime_t PalmToDateTime(UInt32);

static bool_t IsMem(const tchar_t* URL)
{
	tchar_t Mime[MAXPROTOCOL];
	GetProtocol(URL,Mime,TSIZEOF(Mime),NULL);
	return tcsisame_ascii(Mime,T("mem"));
}

datetime_t FileDateTime(nodecontext *p,const tchar_t* URL)
{
	// only VFS support...
	const tchar_t* Name;
	UInt16 Vol;
	UInt32 Value;
	datetime_t Date = INVALID_DATETIME_T;
	FileRef	File = 0;

	Name = VFSToVol(URL,&Vol);
	if (Name)
	{
		VFSFileOpen(Vol,Name,vfsModeRead,&File);

		if (File)
		{
			if (VFSFileGetDate(File,vfsFileDateModified,&Value)==errNone)
				Date = PalmToDateTime(Value);
		
			VFSFileClose(File);
		}
	}
	return Date;
}

bool_t FileExists(nodecontext *p,const tchar_t* URL)
{
	if (IsMem(URL))
	{
        FileHand File = FileOpen(0,GetProtocol(URL,NULL,0,NULL), 0, 0, fileModeReadOnly | fileModeAnyTypeCreator, NULL);
		if (File)
			FileClose(File);
		return File!=NULL;
	}
	else
	{
		const tchar_t* Name;
		UInt16 Vol;
		FileRef	File = 0;

		Name = VFSToVol(URL,&Vol);
		if (!Name)
			return 0;

		VFSFileOpen(Vol,Name,vfsModeRead,&File);
		if (!File)
			return 0;

		VFSFileClose(File);
		return 1;
	}
}

bool_t FileErase(nodecontext *p,const tchar_t* URL, bool_t Force, bool_t Safe)
{
	if (IsMem(URL))
	{
        const tchar_t *Name = GetProtocol(URL,NULL,0,NULL);
        if (Force)
        {
            LocalID Id;
            Id = DmFindDatabase(0,Name);
            if (Id)
            {
	            UInt16 Attr;
	            DmDatabaseInfo(0,Id,NULL,&Attr,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
                if ((Attr & dmHdrAttrReadOnly)!=0)
                {
	                Attr ^= dmHdrAttrReadOnly;
	                DmSetDatabaseInfo(0,Id,NULL,&Attr,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
                }
            }
        }
        return FileDelete(0,Name) == errNone;
	}
	else
	{
		const tchar_t* Name;
		UInt16 Vol;

		Name = VFSToVol(URL,&Vol);
		if (!Name)
			return 0;

        if (Force)
        {
            FileRef	File = 0;
            UInt32 Attrib;

            VFSFileOpen(Vol,Name,vfsModeRead,&File);
            if (File)
            {
                if (VFSFileGetAttributes(File,&Attrib))
                {
                    if ((Attrib & vfsFileAttrReadOnly) != 0)
                    {
                        Attrib |= vfsFileAttrReadOnly;
                        VFSFileSetAttributes(File,Attrib);
                    }
                }
                VFSFileClose(File);
            }
        }

		if (errNone == VFSFileDelete(Vol,Name))
			return 1;
		return 0;
	}
}

bool_t FolderErase(nodecontext *p,const tchar_t* URL, bool_t Force, bool_t Safe)
{
    return FileErase(p,URL,Force,Safe); // a folder is a file with folder attribute
}

bool_t PathIsFolder(nodecontext *p,const tchar_t* URL)
{
	if (IsMem(URL))
	{
		return 0;
	}
	else
	{
		const tchar_t* Name;
		UInt16 Vol;
		FileRef	File = 0;
		UInt32 Attrib;

		Name = VFSToVol(URL,&Vol);
		if (!Name)
			return 0;

		VFSFileOpen(Vol,Name,vfsModeRead,&File);
		if (!File || !VFSFileGetAttributes(File,&Attrib))
			return 0;
		VFSFileClose(File);
    
		return (Attrib & vfsFileAttrDirectory)==vfsFileAttrDirectory;
	}
}

bool_t FolderCreate(nodecontext *p,const tchar_t* URL)
{
	if (IsMem(URL))
	{
		return 0;
	}
	else
	{
		const tchar_t* Name;
		UInt16 Vol;

		Name = VFSToVol(URL,&Vol);
		if (!Name)
			return 0;

		return VFSDirCreate(Vol,Name)==errNone;
	}
}

bool_t FileMove(nodecontext *p,const tchar_t* In,const tchar_t* Out)
{
	// only VFS support...
	const tchar_t *NameIn,*NameOut;
	UInt16 VolIn;
	UInt16 VolOut;

	NameIn = VFSToVol(In,&VolIn);
	NameOut = VFSToVol(Out,&VolOut);

	if (!NameIn || !NameOut || VolIn!=VolOut)
		return 0;

    return VFSFileRename(VolIn,NameIn,NameOut) == errNone;
}

static bool_t InternalOpen(vfs* p,const tchar_t* URL, int Flags)
{
	const tchar_t* Name;

	if (p->File)
	{
		VFSFileClose(p->File);
		p->File = 0;
	}

	Name = VFSToVol(URL,&p->Vol);
	if (Name)
    {
        uint_fast16_t Mode = 0;

        if (Flags & SFLAG_WRONLY && !(Flags & SFLAG_RDONLY))
            Mode = vfsModeWrite;
        else if (Flags & SFLAG_RDONLY && !(Flags & SFLAG_WRONLY))
            Mode = vfsModeRead;
        else
            Mode = vfsModeReadWrite;

        if (Flags & SFLAG_CREATE)
            Mode |= vfsModeTruncate|vfsModeCreate;

        VFSFileOpen(p->Vol,Name,(UInt16)Mode,&p->File);
    }

	if (!p->File)
		return 0;

	tcscpy_s(p->URL,TSIZEOF(p->URL),URL);
    p->Flags = Flags & ~SFLAG_REOPEN;

	return 1;
}

static err_t Open(vfs* p, const tchar_t* URL, int Flags)
{
	if (p->File)
	{
		VFSFileClose(p->File);
		p->File = 0;
	}

	p->Length = INVALID_FILEPOS_T;
	if (URL != p->URL)
		p->URL[0] = 0;

	if (URL)
	{
		UInt32 Pos = 0;

		if (!InternalOpen(p,URL,Flags))
		{
			if ((Flags & (SFLAG_REOPEN|SFLAG_SILENT))==0)
				NodeReportError(p,NULL,ERR_ID,ERR_FILE_NOT_FOUND,URL);
			return ERR_FILE_NOT_FOUND;
		}

		if (Flags & SFLAG_REOPEN)
			VFSFileSeek(p->File,vfsOriginBeginning,p->Pos);
		else
		{
			if (VFSFileSize(p->File,&Pos)==errNone)
				p->Length = Pos;
		}

		if (VFSFileTell(p->File,&Pos) == errNone)
			p->Pos = Pos;
	}
	return ERR_NONE;
}

static err_t OpenDir(vfs* p,const tchar_t* URL,int UNUSED_PARAM(Flags))
{
	UInt32 Attrib = 0;

	if (!InternalOpen(p,URL,SFLAG_RDONLY))
		return ERR_FILE_NOT_FOUND;

	VFSFileGetAttributes(p->File,&Attrib);
	
	if (!(Attrib & vfsFileAttrDirectory))
		return ERR_NOT_DIRECTORY;

	p->Iter = vfsIteratorStart;
    return ERR_NONE;
}

static err_t EnumDir(vfs* p,const tchar_t* Exts,bool_t ExtFilter,streamdir* Item)
{
	tchar_t Path[MAXPATH];
	FileInfoType Info;

	Item->DisplayName[0] = 0;

	if (p->File)
	{
		Info.nameP = Item->FileName;
		Info.nameBufLen = sizeof(Item->FileName);

		while (p->Iter != vfsIteratorStop && VFSDirEntryEnumerate(p->File,&p->Iter,&Info)==errNone)
		{
			UInt32 Value;
			FileRef File = 0;

			if (Item->FileName[0]=='.' || (Info.attributes & vfsFileAttrHidden)) // skip unix/mac hidden files
				continue;
 
			AbsPath(Path,TSIZEOF(Path),Item->FileName,GetProtocol(p->URL,NULL,0,NULL));
			
			//currently Date and Size is not needed
			//VFSFileOpen(p->Vol,Path,vfsModeRead,&File);

			Item->ModifiedDate = INVALID_DATETIME_T;
			if (File && VFSFileGetDate(File,vfsFileDateModified,&Value)==errNone)
				Item->ModifiedDate = PalmToDateTime(Value);

			if (Info.attributes & vfsFileAttrDirectory)
            {
				Item->Size = INVALID_FILEPOS_T;
                Item->Type = FTYPE_DIR;
            }
			else
			{
				if (File && VFSFileSize(File,&Value)==errNone)
					Item->Size = Value;
				else
					Item->Size = 0;
				Item->Type = CheckExts(Item->FileName,Exts);
				if (!Item->Type && ExtFilter)
				{
					if (File)
						VFSFileClose(File);
					continue;
				}
			}

			if (File)
				VFSFileClose(File);

			return ERR_NONE;
		}

		VFSFileClose(p->File);
		p->File = 0;
	}

	Item->FileName[0] = 0;
	return ERR_END_OF_FILE;
}

static void Delete(vfs* p)
{
	Open(p,NULL,0);
}

static err_t CheckVFS(fourcc_t ClassId, void* VMT)
{
	UInt32 Version;
    UInt32 CompanyID;

	if (FtrGet(sysFileCExpansionMgr, expFtrIDVersion, &Version) == errNone && Version >= 1 &&
	    FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &Version) == errNone && Version >= 1)
        return ERR_NONE;

    // simulator?
   	FtrGet(sysFtrCreator, sysFtrNumOEMCompanyID, &CompanyID);
    if (CompanyID == 'psys')
        return ERR_NONE;

    return ERR_NOT_SUPPORTED;
}

META_START(VFS_Class,VFS_CLASS)
META_CLASS(SIZE,sizeof(vfs))
META_CLASS(PRIORITY,PRI_MINIMUM)
META_CLASS(VMT_CREATE,CheckVFS)
META_CLASS(DELETE,Delete)
META_VMT(TYPE_FUNC,stream_vmt,Open,Open)
META_VMT(TYPE_FUNC,stream_vmt,Read,Read)
META_VMT(TYPE_FUNC,stream_vmt,ReadBlock,ReadBlock)
META_VMT(TYPE_FUNC,stream_vmt,Write,Write)
META_VMT(TYPE_FUNC,stream_vmt,Seek,Seek)
META_VMT(TYPE_FUNC,stream_vmt,OpenDir,OpenDir)
META_VMT(TYPE_FUNC,stream_vmt,EnumDir,EnumDir)
META_DATA_RDONLY(TYPE_INT,STREAM_FLAGS,vfs,Flags)
META_DATA_RDONLY(TYPE_STRING,STREAM_URL,vfs,URL)
META_PARAM(SET,STREAM_LENGTH,SetLength)
META_DATA(TYPE_FILEPOS,STREAM_LENGTH,vfs,Length)
META_PARAM(STRING,NODE_PROTOCOL,T("pose,slot1,slot2,slot3,slot4,slot5,slot6,slot7,slot8,simu1,simu2,simu3,simu4,simu5,simu6,simu7,simu8,vol1,vol2,vol3,vol4,vol5,vol6,vol7,vol8"))
META_END(STREAM_CLASS)

#endif

