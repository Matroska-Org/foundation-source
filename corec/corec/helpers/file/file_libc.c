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

#ifdef CONFIG_FILEPOS_64
#define __USE_FILE_OFFSET64
#endif

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#if defined(TARGET_OSX)
#include <sys/mount.h>
#else
#include <sys/vfs.h>
#endif

#if defined(O_ACCMODE)
#define _RW_ACCESS_FILE  (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define _RW_ACCESS_DIR  (S_IRWXU|S_IRWXG|S_IRWXO)
#elif defined(S_IREAD) && defined(S_IWRITE)
#define _RW_ACCESS_FILE  (S_IREAD|S_IWRITE)
#define _RW_ACCESS_DIR  (S_IREAD|S_IWRITE|S_IEXEC)
#endif

typedef struct filestream
{
	stream Stream;
	int fd;
	tchar_t URL[MAXPATH]; // TODO: turn into a dynamic data
	filepos_t Length;
	int Flags;

	tchar_t DirPath[MAXPATH]; // TODO: turn into a dynamic data
	DIR *FindDir;

} filestream;

static err_t Open(filestream* p, const tchar_t* URL, int Flags)
{
	if (p->fd != -1)
		close(p->fd);

    p->Length = INVALID_FILEPOS_T;
	p->fd = -1;

	if (URL && URL[0])
	{
		struct stat file_stats;
        int mode = 0;

        if (Flags & SFLAG_WRONLY && !(Flags & SFLAG_RDONLY))
            mode = O_WRONLY;
        else if (Flags & SFLAG_RDONLY && !(Flags & SFLAG_WRONLY))
            mode = O_RDONLY;
        else
            mode = O_RDWR;

        if (Flags & SFLAG_CREATE)
            mode |= O_CREAT|O_TRUNC;

		//TODO: verify it works with Unicode files too
		p->fd = open(URL, mode, _RW_ACCESS_FILE);
		if (p->fd == -1)
		{
			if ((Flags & (SFLAG_REOPEN|SFLAG_SILENT))==0)
				NodeReportError(p,NULL,ERR_ID,ERR_FILE_NOT_FOUND,URL);
			return ERR_FILE_NOT_FOUND;
		}

		tcscpy_s(p->URL,TSIZEOF(p->URL),URL);
		
        if (stat(URL, &file_stats) == 0)
			p->Length = file_stats.st_size;

	}
	return ERR_NONE;
}

static err_t Read(filestream* p,void* Data,size_t Size,size_t* Readed)
{
    err_t Err;
    int n = read(p->fd, Data, (unsigned int)Size);
    if (n<0)
    {
        n=0;
        Err = ERR_READ;
    }
    else
        Err = ((size_t)n != Size) ? ERR_END_OF_FILE:ERR_NONE;

    if (Readed)
        *Readed = n;
    return Err;
}

static err_t ReadBlock(filestream* p,block* Block,size_t Ofs,size_t Size,size_t* Readed)
{
	return Read(p,(void*)(Block->Ptr+Ofs),Size,Readed);
}

static err_t Write(filestream* p,const void* Data,size_t Size,size_t* Written)
{
    err_t Err;
	int n = write(p->fd, Data, (unsigned int)Size);

    if (n<0)
    {
        n=0;
        Err = ERR_WRITE;
    }
    else
        Err = (n != Size) ? ERR_WRITE:ERR_NONE;

    if (Written)
        *Written = n;
    return Err;
}

static filepos_t Seek(filestream* p,filepos_t Pos,int SeekMode)
{
	off_t NewPos = lseek(p->fd, Pos, SeekMode);
    if (NewPos<0)
        return INVALID_FILEPOS_T;
    return NewPos;
}

static err_t SetLength(filestream* p,dataid Id,const filepos_t* Data,size_t Size)
{
    if (Size != sizeof(filepos_t))
        return ERR_INVALID_DATA;

	if (ftruncate(p->fd, *Data)!=0)
		return ERR_BUFFER_FULL;

	return ERR_NONE;
}

static err_t OpenDir(filestream* p,const tchar_t* Path,int UNUSED_PARAM(Flags))
{
	if (p->FindDir)
		closedir(p->FindDir);

    if (Path[0]==0)
        Path = T("/");

	p->FindDir = opendir(Path);
	if (!p->FindDir)
	{
		if (errno == ENOTDIR)
			return ERR_NOT_DIRECTORY;
		else
			return ERR_FILE_NOT_FOUND;
	}

	tcscpy_s(p->DirPath,TSIZEOF(p->DirPath),Path);
	AddPathDelimiter(p->DirPath,TSIZEOF(p->DirPath));
    return ERR_NONE;
}
	
extern datetime_t LinuxToDateTime(time_t);

static err_t EnumDir(filestream* p,const tchar_t* Exts,bool_t ExtFilter,streamdir* Item)
{
	struct dirent *Dirent;

	if (!p->FindDir)
		return ERR_END_OF_FILE;

	Item->FileName[0] = 0;
    Item->Size = INVALID_FILEPOS_T;

	while (!Item->FileName[0] && (Dirent = readdir(p->FindDir)) != NULL)
	{
	    tchar_t FilePath[MAXPATH];
	    struct stat file_stats;
        
        if (Dirent->d_name[0]=='.') // skip hidden files and current directory
            continue;

	    tcscpy_s(FilePath, TSIZEOF(FilePath), p->DirPath);
	    tcscat_s(FilePath, TSIZEOF(FilePath), Dirent->d_name);
	    tcscpy_s(Item->FileName,TSIZEOF(Item->FileName), Dirent->d_name);

	    stat(FilePath, &file_stats);

	    Item->ModifiedDate = LinuxToDateTime(file_stats.st_mtime);
	    if (S_ISDIR(file_stats.st_mode))
        {
            Item->Type = FTYPE_DIR;
		    Item->Size = INVALID_FILEPOS_T;
        }
	    else
	    {
		    Item->Size = file_stats.st_size;
		    Item->Type = CheckExts(Item->FileName,Exts);

			if (!Item->Type && ExtFilter)
				Item->FileName[0] = 0; // skip
	    }
	}

	if (!Item->FileName[0])
	{
		closedir(p->FindDir);
		p->FindDir = NULL;
		return ERR_END_OF_FILE;
	}

	return ERR_NONE;
}

static void Delete(filestream* p)
{
	if (p->fd != -1)
    {
		close(p->fd);
	    p->fd = -1;
    }
	if (p->FindDir)
		closedir(p->FindDir);
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
META_CONST(TYPE_INT,filestream,fd,-1)
META_DATA_RDONLY(TYPE_INT,STREAM_FLAGS,filestream,Flags)
META_DATA_RDONLY(TYPE_STRING,STREAM_URL,filestream,URL)
META_DATA_RDONLY(TYPE_STRING,STREAM_ENUM_BASE,filestream,DirPath)
META_PARAM(SET,STREAM_LENGTH,SetLength)
META_DATA(TYPE_FILEPOS,STREAM_LENGTH,filestream,Length)
META_PARAM(STRING,NODE_PROTOCOL,T("file"))
META_END(STREAM_CLASS)

bool_t FileExists(nodecontext *p,const tchar_t* Path)
{
	struct stat file_stats;
	return stat(Path, &file_stats) == 0;
}

bool_t FileErase(nodecontext *p,const tchar_t* Path, bool_t Force, bool_t Safe)
{
    if (Force)
    {
        struct stat file_stats;
        if (stat(Path, &file_stats) == 0)
        {
            if ((file_stats.st_mode & S_IWUSR)==0)
            {
                file_stats.st_mode |= S_IWUSR;
                chmod(Path,file_stats.st_mode);
            }
        }
    }
	return unlink(Path) == 0;
}

bool_t FolderErase(nodecontext *p,const tchar_t* Path, bool_t Force, bool_t Safe)
{
    if (Force)
    {
        struct stat file_stats;
        if (stat(Path, &file_stats) == 0)
        {
            if ((file_stats.st_mode & S_IWUSR)==0)
            {
                file_stats.st_mode |= S_IWUSR;
                chmod(Path,file_stats.st_mode);
            }
        }
    }
	return rmdir(Path) == 0;
}

bool_t PathIsFolder(nodecontext *p,const tchar_t* Path)
{
	struct stat file_stats;
	if (stat(Path, &file_stats) == 0)
	{
		return (file_stats.st_mode & S_IFDIR) == S_IFDIR;
	}
	return 0;
}

datetime_t FileDateTime(nodecontext *p,const tchar_t* Path)
{
	datetime_t Date = INVALID_DATETIME_T;
	struct stat file_stats;
	if (stat(Path, &file_stats) == 0)
		Date = LinuxToDateTime(file_stats.st_mtime);
	return Date;
}

bool_t FileMove(nodecontext *p,const tchar_t* In,const tchar_t* Out)
{
    return rename(In,Out) == 0;
}

bool_t FolderCreate(nodecontext *p,const tchar_t* Path)
{
	return mkdir(Path,_RW_ACCESS_DIR) == 0;
}

void FindFiles(nodecontext *p,const tchar_t* Path, const tchar_t* Mask,void(*Process)(const tchar_t*,void*),void* Param)
{
    DIR* Directory;
    struct dirent* DirectoryInfo;
    tchar_t TPathToFile[MAXPATH];
    
    Directory = opendir(Path);
    if (Directory)
    {
        while ( (DirectoryInfo = readdir(Directory)) != NULL )
        {
            char* FileExtension = 0;
            FileExtension = strrchr(DirectoryInfo->d_name, '.');
            if(FileExtension)
            {
                if (strcmp(Mask, FileExtension ) == 0 )
                {
                    tcscpy_s(TPathToFile, TSIZEOF(TPathToFile), Path);
                    tcscat_s(TPathToFile, TSIZEOF(TPathToFile), DirectoryInfo->d_name);
                    Process(TPathToFile, Param);
                }
            }
        }
        
        closedir(Directory);
    }
    
}

stream *FileTemp(anynode *Any)
{
#ifndef TODO
    assert(NULL); // not supported yet
#endif
    return NULL;
}

bool_t FileTempName(anynode *Any,tchar_t *Out, size_t OutLen)
{
#ifndef TODO
    assert(NULL); // not supported yet
#endif
    return 0;
}

int64_t GetPathFreeSpace(nodecontext* UNUSED_PARAM(p), const tchar_t* Path)
{
#ifndef TODO
    // need to an include (see at includes)
    struct statfs st;
    if (statfs(Path, &st) < 0)
    	return -1;
    return (int64_t)st.f_bsize * (int64_t)st.f_bavail;
#else
    return -1;
#endif
}
