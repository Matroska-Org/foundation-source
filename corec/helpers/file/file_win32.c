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

#if defined(TARGET_WIN)

#define FILE_FUNC_ID  FOURCC('F','L','I','D')

#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#ifndef FO_DELETE
#define FO_DELETE   3
#endif
#ifndef FOF_NO_UI
#define FOF_NO_UI (0x04|0x10|0x400|0x200)
#endif

#if defined(TARGET_WINCE)
static HMODULE CEShellDLL = NULL;
#endif
static int (WINAPI* FuncSHFileOperation)(SHFILEOPSTRUCT*) = NULL;

#ifndef ERROR_INVALID_DRIVE_OBJECT
#define ERROR_INVALID_DRIVE_OBJECT		4321L
#endif

#ifndef ERROR_DEVICE_NOT_AVAILABLE
#define ERROR_DEVICE_NOT_AVAILABLE		4319L
#endif

#ifndef ERROR_DEVICE_REMOVED
#define ERROR_DEVICE_REMOVED            1617L
#endif

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER        ((DWORD)-1)
#endif

typedef struct filestream
{
	stream Stream;
	tchar_t URL[MAXPATH];
	HANDLE Handle;
	filepos_t Length;
	filepos_t Pos;
	int Flags;

	void* Find;
	WIN32_FIND_DATA FindData;
    int DriveNo;

} filestream;

static filepos_t SetFilePointerFP(HANDLE hFile, filepos_t DistanceToMove, DWORD dwMoveMethod) {
  LARGE_INTEGER li;

  li.QuadPart = DistanceToMove;

  li.LowPart = SetFilePointer(hFile, li.LowPart, &li.HighPart, dwMoveMethod);

  if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
    return -1;

  return (filepos_t)li.QuadPart;
}

static err_t Open(filestream* p, const tchar_t* URL, int Flags)
{
	if (p->Handle)
		CloseHandle(p->Handle);
	p->Handle = 0;
	p->Length = INVALID_FILEPOS_T;

	if (URL != p->URL)
		p->URL[0] = 0;
	else
		Sleep(200);

	if (URL)
	{
        DWORD FileSizeLow;
        DWORD FileSizeHigh;
		HANDLE Handle;

        Handle = CreateFile(URL,((Flags & SFLAG_RDONLY || !(Flags & SFLAG_WRONLY))?GENERIC_READ:0)|
            ((Flags & SFLAG_WRONLY || !(Flags & SFLAG_RDONLY))?GENERIC_WRITE:0),
			FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,(Flags & SFLAG_CREATE)?CREATE_ALWAYS:OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,NULL);

		if (Handle == INVALID_HANDLE_VALUE)
		{
			if ((Flags & (SFLAG_REOPEN|SFLAG_SILENT))==0)
			{
				if (Flags & SFLAG_WRONLY)
					NodeReportError(p,NULL,ERR_ID,ERR_DEVICE_ERROR,URL);
				else
					NodeReportError(p,NULL,ERR_ID,ERR_FILE_NOT_FOUND,URL);
			}
			return ERR_FILE_NOT_FOUND;
		}

		tcscpy_s(p->URL,TSIZEOF(p->URL),URL);
        p->Flags = Flags & ~SFLAG_REOPEN;
		p->Handle = Handle;

		FileSizeLow = GetFileSize(Handle,&FileSizeHigh);
        if (FileSizeLow != INVALID_FILE_SIZE || GetLastError()==NO_ERROR)
            p->Length = (filepos_t)(((int64_t)FileSizeHigh << 32) | FileSizeLow);

		if (Flags & SFLAG_REOPEN)
			p->Pos = SetFilePointerFP(p->Handle,p->Pos,FILE_BEGIN);
		else
		{
			p->Pos = 0;

#if defined(TARGET_WINCE)

			// wince shortcut handling
			if (p->Length < MAXPATH && !(Flags & (SFLAG_CREATE|SFLAG_WRONLY)))
			{
				uint32_t Readed;
				char ShortCut[MAXPATH];
				tchar_t URL[MAXPATH];
				char* ch;

				if (ReadFile(p->Handle,ShortCut,(DWORD)p->Length,&Readed,NULL))
				{
					ShortCut[Readed] = 0;

					for (ch=ShortCut;*ch && *ch!='#';++ch)
						if (!IsDigit(*ch))
							break;

					if (ch[0] == '#' && ch[1]!=':')
					{
						char* Head = ++ch;
						char* Tail;

						if (*ch == '"')
						{
							Head++;
							ch = strchr(ch+1,'"');
							if (ch)
								*(ch++) = 0;
						}

						if (ch)
						{
							Tail = strchr(ch,13);
							if (Tail)
							{
								*Tail = 0;
								ch = Tail+1;
							}
							if (!strchr(ch,13))
							{
								ch = strchr(ch,10);
								if (!ch || !strchr(ch+1,10))
								{
									if (ch) *ch = 0;

									// we don't want to depend on parser and charconvert in file
									// Node_FromStr(p,URL,TSIZEOF(URL),Head);
#ifdef COREMAKE_UNICODE
									if (!MultiByteToWideChar(CP_ACP,0,Head,-1,URL,TSIZEOF(URL)))
									{
										tchar_t* Out = URL;
										size_t OutLen = TSIZEOF(URL);
										for (;OutLen>1 && *Head;++Head,--OutLen,++Out)
											*Out = (char)(*Head>255?'*':*Head);
										*Out = 0;	
									}
#endif
									return Open(p,URL,Flags);
								}
							}
						}
					}

					p->Pos = SetFilePointer(p->Handle,0,NULL,FILE_BEGIN);
				}
			}
#endif
		}
	}
	return ERR_NONE;
}

static err_t Read(filestream* p,void* Data,size_t Size,size_t* Readed)
{
    err_t Err;
	DWORD n;

	if (ReadFile(p->Handle,Data,(DWORD)Size,&n,NULL))
	{
		p->Pos += n;
        Err = (n == Size) ? ERR_NONE : ERR_END_OF_FILE;
	}
    else
    {
	    DWORD Error = GetLastError();
	    if (Error == ERROR_DEVICE_REMOVED || 
		    Error == ERROR_DEVICE_NOT_AVAILABLE ||
		    Error == ERROR_INVALID_HANDLE || 
		    Error == ERROR_INVALID_DRIVE_OBJECT ||
		    Error == ERROR_DEV_NOT_EXIST ||
		    Error == ERROR_GEN_FAILURE)
		    Open(p,p->URL,p->Flags|SFLAG_REOPEN);

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
	int ReTry=3;
	filepos_t Result;
	DWORD Error;

	switch (SeekMode)
	{
	default:
	case SEEK_SET: SeekMode = FILE_BEGIN; break;
	case SEEK_CUR: SeekMode = FILE_CURRENT; break;
	case SEEK_END: SeekMode = FILE_END; break;
	}

	do
	{
		Result = SetFilePointerFP(p->Handle,Pos,SeekMode);

		if (Result != -1)
		{
			p->Pos = Result;
			break;
		}

		Error = GetLastError();
		if (Error != ERROR_DEVICE_REMOVED && Error != ERROR_INVALID_HANDLE)
			break;

		Open(p,p->URL,p->Flags|SFLAG_REOPEN);
	}
	while (--ReTry>0);

    if (Result==-1)
        Result = INVALID_FILEPOS_T;

	return Result;
}

static err_t Write(filestream* p,const void* Data,size_t Size,size_t* Written)
{
	DWORD n;
    err_t Err;

	if (WriteFile(p->Handle,Data,(DWORD)Size,&n,NULL))
    {
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

static err_t SetLength(filestream* p,dataid UNUSED_PARAM(Id),const filepos_t* Data,size_t UNUSED_PARAM(Size))
{
	err_t Result = ERR_NONE;
	filepos_t Pos;

    if (Size != sizeof(filepos_t))
        return ERR_INVALID_DATA;

	Pos = SetFilePointerFP(p->Handle,0,SEEK_CUR);
	if (Pos == -1)
		return ERR_NOT_SUPPORTED;

	if (SetFilePointerFP(p->Handle,*Data,SEEK_SET) == -1)
		return ERR_NOT_SUPPORTED;

	if (!SetEndOfFile(p->Handle))
		Result = ERR_BUFFER_FULL;

	Pos = SetFilePointerFP(p->Handle,Pos,SEEK_SET);
	if (Pos != -1)
		p->Pos = Pos;

	return Result;
}

static err_t OpenDir(filestream* p,const tchar_t* URL,int UNUSED_PARAM(Flags))
{
	DWORD Attrib;
	tchar_t Path[MAXPATHFULL];

	if (p->Find != INVALID_HANDLE_VALUE)
	{
		FindClose(p->Find);
		p->Find = INVALID_HANDLE_VALUE;
	}
    p->DriveNo = -1;

#if !defined(TARGET_WINCE)
    if (!URL[0])
    {
        p->DriveNo = 0;
    }
    else
#endif
    {
		Attrib = GetFileAttributes(URL);
		if (Attrib == (DWORD)-1)
			return ERR_FILE_NOT_FOUND;

		if (!(Attrib & FILE_ATTRIBUTE_DIRECTORY))
			return ERR_NOT_DIRECTORY;

        tcscpy_s(Path,TSIZEOF(Path),URL);
        AddPathDelimiter(Path,TSIZEOF(Path));
        tcscat_s(Path,TSIZEOF(Path),T("*.*"));
		p->Find = FindFirstFile(Path, &p->FindData);
    }

    return ERR_NONE;
}

extern datetime_t FileTimeToRel(FILETIME*);

static err_t EnumDir(filestream* p,const tchar_t* Exts,bool_t ExtFilter,streamdir* Item)
{
	Item->FileName[0] = 0;
	Item->DisplayName[0] = 0;

#if !defined(TARGET_WINCE)
    if (p->DriveNo>=0)
    {
        size_t n = GetLogicalDriveStrings(0,NULL);
        tchar_t* Drives = alloca((n+1)*sizeof(tchar_t));
        if (GetLogicalDriveStrings((DWORD)n,Drives))
        {
            int No = p->DriveNo++;

            while (Drives[0] && --No>=0)
                Drives += tcslen(Drives)+1;

            if (Drives[0])
            {
                size_t n = tcslen(Drives);
                if (Drives[n-1] == '\\')
                    Drives[n-1] = 0;
                tcscpy_s(Item->FileName,TSIZEOF(Item->FileName),Drives);
                Item->ModifiedDate = INVALID_DATETIME_T;
                Item->Size = INVALID_FILEPOS_T;
                Item->Type = FTYPE_DIR;
            }
        }
    }
    else
#endif
    {
	    while (!Item->FileName[0] && p->Find != INVALID_HANDLE_VALUE)
	    {
		    if (p->FindData.cFileName[0]!='.' && // skip unix/mac hidden files and . .. directory entries
                !(p->FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) 
		    {
			    tcscpy_s(Item->FileName,TSIZEOF(Item->FileName),p->FindData.cFileName);

                Item->ModifiedDate = FileTimeToRel(&p->FindData.ftLastWriteTime);

			    if (p->FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
				    Item->Size = INVALID_FILEPOS_T;
                    Item->Type = FTYPE_DIR;
                }
			    else
			    {
				    Item->Size = (filepos_t)(((int64_t)p->FindData.nFileSizeHigh << 32) | p->FindData.nFileSizeLow);
				    Item->Type = CheckExts(Item->FileName,Exts);

				    if (!Item->Type && ExtFilter)
					    Item->FileName[0] = 0; // skip
			    }
		    }
			    
		    if (!FindNextFile(p->Find,&p->FindData))
		    {
			    FindClose(p->Find);
			    p->Find = INVALID_HANDLE_VALUE;
		    }
	    }
    }

	if (!Item->FileName[0])
	{
		if (p->Find != INVALID_HANDLE_VALUE)
		{
			FindClose(p->Find);
			p->Find = INVALID_HANDLE_VALUE;
		}
        p->DriveNo = -1;
		return ERR_END_OF_FILE;
	}

	return ERR_NONE;
}

static void Delete(filestream* p)
{
	Open(p,NULL,0);
	if (p->Find != INVALID_HANDLE_VALUE)
		FindClose(p->Find);
}

static err_t CreateFunc(node* UNUSED_PARAM(p))
{
#if defined(TARGET_WINCE)
	CEShellDLL = LoadLibrary(T("ceshell.dll"));
	if (CEShellDLL)
		*(FARPROC*)(void*)&FuncSHFileOperation = GetProcAddress(CEShellDLL,MAKEINTRESOURCE(14));
#else
    FuncSHFileOperation = SHFileOperation;
#endif

    return ERR_NONE;
}

static void DeleteFunc(node* UNUSED_PARAM(p))
{
#if defined(TARGET_WINCE)
	if (CEShellDLL) FreeLibrary(CEShellDLL);
#endif
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
META_CONST(TYPE_PTR,filestream,Find,INVALID_HANDLE_VALUE)
META_DATA_RDONLY(TYPE_INT,STREAM_FLAGS,filestream,Flags)
META_DATA_RDONLY(TYPE_STRING,STREAM_URL,filestream,URL)
META_PARAM(SET,STREAM_LENGTH,SetLength)
META_DATA(TYPE_FILEPOS,STREAM_LENGTH,filestream,Length)
META_PARAM(STRING,NODE_PROTOCOL,T("file"))
META_END_CONTINUE(STREAM_CLASS)

META_START_CONTINUE(FILE_FUNC_ID)
META_CLASS(FLAGS,CFLAG_SINGLETON)
META_CLASS(CREATE,CreateFunc)
META_CLASS(DELETE,DeleteFunc)
META_END(NODE_CLASS)

bool_t FolderCreate(nodecontext* UNUSED_PARAM(p),const tchar_t* Path)
{
	return CreateDirectory(Path,NULL) != FALSE;
}

bool_t FileExists(nodecontext* UNUSED_PARAM(p),const tchar_t* Path)
{
	return GetFileAttributes(Path) != (DWORD)-1;
}

static bool_t FileRecycle(const tchar_t* Path)
{
    tchar_t PathEnded[MAXPATHFULL];
    SHFILEOPSTRUCT DelStruct;
    int Ret;
    size_t l;

    memset(&DelStruct,0,sizeof(DelStruct));
    DelStruct.wFunc = FO_DELETE;
    l = min(tcslen(Path)+1,TSIZEOF(PathEnded)-1);
    tcscpy_s(PathEnded,TSIZEOF(PathEnded),Path);
    PathEnded[l]=0;
    DelStruct.pFrom = PathEnded;
    DelStruct.fFlags = FOF_ALLOWUNDO|FOF_NO_UI;
    Ret = FuncSHFileOperation(&DelStruct);
    return Ret == 0;
}

bool_t FileErase(nodecontext* UNUSED_PARAM(p),const tchar_t* Path, bool_t Force, bool_t Safe)
{
    if (Force)
    {
        DWORD attr = GetFileAttributes(Path);
        if ((attr != (DWORD)-1) && (attr & FILE_ATTRIBUTE_READONLY))
        {
            attr ^= FILE_ATTRIBUTE_READONLY;
            SetFileAttributes(Path,attr);
        }
    }

    if (!Safe || !FuncSHFileOperation)
    	return DeleteFile(Path) != FALSE;
    else
        return FileRecycle(Path);
}

bool_t FolderErase(nodecontext* UNUSED_PARAM(p),const tchar_t* Path, bool_t Force, bool_t Safe)
{
    if (Force)
    {
        DWORD attr = GetFileAttributes(Path);
        if ((attr != (DWORD)-1) && (attr & FILE_ATTRIBUTE_READONLY))
        {
            attr ^= FILE_ATTRIBUTE_READONLY;
            SetFileAttributes(Path,attr);
        }
    }

    if (!Safe || !FuncSHFileOperation)
    	return RemoveDirectory(Path) != FALSE;
    else
        return FileRecycle(Path);
}

bool_t PathIsFolder(nodecontext* UNUSED_PARAM(p),const tchar_t* Path)
{
    DWORD attr = GetFileAttributes(Path);
	return (attr != (DWORD)-1) && (attr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

datetime_t FileDateTime(nodecontext* UNUSED_PARAM(p),const tchar_t* Path)
{
	datetime_t Date = INVALID_DATETIME_T;
	HANDLE Find;
	WIN32_FIND_DATA FindData;

	Find = FindFirstFile(Path, &FindData);
	if (Find != INVALID_HANDLE_VALUE)
	{
		Date = FileTimeToRel(&FindData.ftLastWriteTime);
		FindClose(Find);
	}
	return Date;
}

bool_t FileMove(nodecontext* UNUSED_PARAM(p),const tchar_t* In,const tchar_t* Out)
{
    return MoveFile(In,Out) != 0;
}

void FindFiles(nodecontext* UNUSED_PARAM(p),const tchar_t* Path, const tchar_t* Mask, void(*Process)(const tchar_t*,void*),void* Param)
{
	WIN32_FIND_DATA FindData;
	tchar_t FindPath[MAXPATH];
	HANDLE Find;

	tcscpy_s(FindPath,TSIZEOF(FindPath),Path);
	tcscat_s(FindPath,TSIZEOF(FindPath),Mask);
	Find = FindFirstFile(FindPath,&FindData);

	if (Find != INVALID_HANDLE_VALUE)
	{
		do
		{
			tcscpy_s(FindPath,TSIZEOF(FindPath),Path);
			tcscat_s(FindPath,TSIZEOF(FindPath),FindData.cFileName);
			Process(FindPath,Param);
		}
		while (FindNextFile(Find,&FindData));

		FindClose(Find);
	}
}

stream *FileTemp(anynode* UNUSED_PARAM(Any))
{
#ifndef TODO
    assert(NULL); // not supported yet
#endif
    return NULL;
}

bool_t FileTempName(anynode* UNUSED_PARAM(Any),tchar_t* UNUSED_PARAM(Out), size_t UNUSED_PARAM(OutLen))
{
#ifndef TODO
    assert(NULL); // not supported yet
#endif
    return 0;
}

FILE_DLL int64_t GetPathFreeSpace(nodecontext* UNUSED_PARAM(p), const tchar_t* Path)
{
    ULARGE_INTEGER lpFreeBytesAvailable;
    ULARGE_INTEGER lpTotal;

    if (!GetDiskFreeSpaceEx(Path, &lpFreeBytesAvailable, &lpTotal, NULL))
        return -1;
    return (int64_t)lpFreeBytesAvailable.QuadPart;
}
#endif
