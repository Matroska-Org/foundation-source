/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include "file.h"
#include "streams.h"
#include <corec/str/str.h>

#if defined(TARGET_WIN)

#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h> // for SHFileOperation

#ifndef FO_DELETE
#define FO_DELETE   3
#endif
#ifndef FOF_NO_UI
#define FOF_NO_UI (0x04|0x10|0x400|0x200)
#endif

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

static err_t SetLength(filestream* p,dataid UNUSED_PARAM(Id),const filepos_t* Data,size_t Size)
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

    if (!URL[0])
    {
        p->DriveNo = 0;
    }
    else
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

    if (p->DriveNo>=0)
    {
        size_t n = GetLogicalDriveStrings(0,NULL);
#if defined(_MSC_VER) && !defined (__clang__)
        tchar_t* Drives = _alloca((n+1)*sizeof(tchar_t));
#else
        tchar_t Drives_[n+1], *Drives = Drives_;
#endif
        if (GetLogicalDriveStrings((DWORD)n,Drives))
        {
            int No = p->DriveNo++;

            while (Drives[0] && --No>=0)
                Drives += tcslen(Drives)+1;

            if (Drives[0])
            {
                size_t len = tcslen(Drives);
                if (Drives[len-1] == '\\')
                    Drives[len-1] = 0;
                tcscpy_s(Item->FileName,TSIZEOF(Item->FileName),Drives);
                Item->ModifiedDate = INVALID_DATETIME_T;
                Item->Size = INVALID_FILEPOS_T;
                Item->Type = FTYPE_DIR;
            }
        }
    }
    else
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
META_END(STREAM_CLASS)


static bool_t FileRecycle(const tchar_t* Path)
{
    tchar_t PathEnded[MAXPATHFULL];
    SHFILEOPSTRUCT DelStruct;
    int Ret;
    size_t l;

    memset(&DelStruct,0,sizeof(DelStruct));
    DelStruct.wFunc = FO_DELETE;
    l = MIN(tcslen(Path)+1,TSIZEOF(PathEnded)-1);
    tcscpy_s(PathEnded,TSIZEOF(PathEnded),Path);
    PathEnded[l]=0;
    DelStruct.pFrom = PathEnded;
    DelStruct.fFlags = FOF_ALLOWUNDO|FOF_NO_UI;
    Ret = SHFileOperation(&DelStruct);
    return Ret == 0;
}

bool_t FileErase(const tchar_t* Path, bool_t Force, bool_t Safe)
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

    if (!Safe)
        return DeleteFile(Path) != FALSE;
    else
        return FileRecycle(Path);
}

bool_t PathIsFolder(const tchar_t* Path)
{
    DWORD attr = GetFileAttributes(Path);
    return (attr != (DWORD)-1) && (attr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}
#endif
