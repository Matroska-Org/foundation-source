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

#if defined(TARGET_PS2SDK)

#include <stdio.h>
#include <sys/stat.h>
#include <kernel.h>
#include <fileXio_rpc.h>
#include <loadfile.h>

typedef struct filestream
{
	stream Stream;
	int fd;
	tchar_t URL[MAXPATH]; // TODO: turn into a dynamic data
	filepos_t Length;
	int Flags;

	tchar_t DirPath[MAXPATH]; // TODO: turn into a dynamic data
	int FindDir;
    int DevNo;

} filestream;

const char* CdromPath(const char* Path, char* Tmp, size_t TmpLen)
{
    if (strnicmp(Path,"cdrom",5)==0)
    {
        // 8.3 upper case name
        size_t a,b,c;
        for (a=0,b=0,c=32;b+1<TmpLen && Path[a];++a)
        {
            char ch=Path[a];
            if (ch=='/' || ch=='\\' || ch==':')
            {
                c=0;
            }
            else
            if (ch=='.')
            {
                if (c>=16) continue;
                c=16;
            }
            else
            {
                if (c==8) continue;
                if (c==16+3) continue;
                if (c<32)
                    ch=toupper(ch);
                ++c;
            }
            Tmp[b++]=ch;
        }
        Tmp[b]=0;
        Path = Tmp;
    }
    return Path;
}

static err_t Open(filestream* p, const tchar_t* URL, int Flags)
{
	if (p->fd>=0)
		fileXioClose(p->fd);

    p->Length = INVALID_FILEPOS_T;
	p->fd = -1;

	if (URL && URL[0])
	{
        tchar_t Tmp[MAXPATH];
		int size;
        int mode = 0;

        URL = CdromPath(URL,Tmp,TSIZEOF(Tmp));

        if (Flags & SFLAG_WRONLY && !(Flags & SFLAG_RDONLY))
            mode = O_WRONLY;
        else if (Flags & SFLAG_RDONLY && !(Flags & SFLAG_WRONLY))
            mode = O_RDONLY;
        else
            mode = O_RDWR;

        if (Flags & SFLAG_CREATE)
            mode |= O_CREAT|O_TRUNC;

		p->fd = fileXioOpen(URL, mode, FIO_S_IRUSR | FIO_S_IWUSR | FIO_S_IXUSR | FIO_S_IRGRP | FIO_S_IWGRP | FIO_S_IXGRP | FIO_S_IROTH | FIO_S_IWOTH | FIO_S_IXOTH );
		if (p->fd<0)
		{
			if ((Flags & (SFLAG_REOPEN|SFLAG_SILENT))==0)
				NodeReportError(p,NULL,ERR_ID,ERR_FILE_NOT_FOUND,URL);
			return ERR_FILE_NOT_FOUND;
		}

		tcscpy_s(p->URL,TSIZEOF(p->URL),URL);

	    if ((size = fileXioLseek(p->fd, 0, SEEK_END)) >= 0)
        {
            fileXioLseek(p->fd, 0, SEEK_SET);
			p->Length = size;
        }
	}
	return ERR_NONE;
}

static err_t Read(filestream* p,void* Data,size_t Size,size_t* Readed)
{
    err_t Err;
    int n;

    n = fileXioRead(p->fd, Data, (unsigned int)Size);
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
    int n;

	n = fileXioWrite(p->fd, (void*)Data, (unsigned int)Size);
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
	int Pos = fileXioLseek(p->fd, Pos, SeekMode);
    if (Pos<0)
        return INVALID_FILEPOS_T;
    return Pos;
}

static err_t SetLength(filestream* p,dataid Id,const int* Data,size_t Size)
{
#ifndef TODO
    assert(NULL); // not supported yet
#endif
	return ERR_NOT_SUPPORTED;
}

static err_t OpenDir(filestream* p,const tchar_t* URL,int UNUSED_PARAM(Flags))
{
	if (p->FindDir>=0)
    {
    	fileXioDclose(p->FindDir);
        p->FindDir=-1;
    }

    if (!URL[0])
    {
        p->DevNo = 0;
        return ERR_NONE;
    }

    p->DevNo = -1;
	p->FindDir = fileXioDopen(URL);

	if (p->FindDir<0)
	{
        int fd = fileXioOpen(URL,O_RDONLY,0);
		if (fd >= 0)
        {
            fileXioClose(fd);
			return ERR_NOT_DIRECTORY;
        }
		else
			return ERR_FILE_NOT_FOUND;
	}

	tcscpy_s(p->DirPath,TSIZEOF(p->DirPath),URL);
	AddPathDelimiter(p->DirPath,TSIZEOF(p->DirPath));
    return ERR_NONE;
}
	
static datetime_t PS2ToDateTime(unsigned char time[8])
{
    datetime_t t;
    datepack_t p;
    p.Year = time[6] + (time[7]<<8);
    p.Month = time[5];
    p.Day = time[4];
    p.Hour = time[3];
    p.Minute = time[2];
    p.Second = time[1];
    t = TimePackToRel(&p,0);
    if (t != INVALID_DATETIME_T)
    {
        t -= 9*60*60; //adjust JST to GMC
        if (t == INVALID_DATETIME_T)
            ++t;
    }
    return t;
}

static err_t EnumDir(filestream* p,const tchar_t* Exts,bool_t ExtFilter,streamdir* Item)
{
	iox_dirent_t Dirent;

	Item->FileName[0] = 0;
	Item->DisplayName[0] = 0;

    if (p->DevNo>=0)
    {
        static const tchar_t* const Devices[] =
        {
            "mass:",
            //"cdrom:", //driver doesn't support directory listing
            "hdd:",
            //"host:",  //driver doesn't support directory listing
            "cdda:",
            "cdfs:",
            "smb:",
            NULL,
        };
        for (;!Item->FileName[0];++p->DevNo)
        {
            int fd;
            const tchar_t* URL = Devices[p->DevNo];
            if (!URL)
                break;

        	fd = fileXioDopen(URL);
            if (fd>=0)
            {
                fileXioDclose(fd);
    	        tcscpy_s(Item->FileName,TSIZEOF(Item->FileName), URL);
	            Item->ModifiedDate = INVALID_DATETIME_T;
                Item->Type = FTYPE_DIR;
		        Item->Size = INVALID_FILEPOS_T;
            }
        }
    }
    else
    {
	    if (p->FindDir<0)
		    return ERR_END_OF_FILE;

	    while (!Item->FileName[0])
	    {
            if (fileXioDread(p->FindDir,&Dirent)<=0)
                break;

			if (Dirent.name[0]=='.') // skip unix/mac hidden files
				continue;

	        tcscpy_s(Item->FileName,TSIZEOF(Item->FileName), Dirent.name);
          
	        Item->ModifiedDate = PS2ToDateTime(Dirent.stat.mtime);
	        if (FIO_S_ISDIR(Dirent.stat.mode))
            {
                Item->Type = FTYPE_DIR;
		        Item->Size = INVALID_FILEPOS_T;
            }
	        else
	        {
		        Item->Size = Dirent.stat.size;
		        Item->Type = CheckExts(Item->FileName,Exts);

			    if (!Item->Type && ExtFilter)
				    Item->FileName[0] = 0; // skip
	        }
	    }
    }

	if (!Item->FileName[0])
	{
        if (p->FindDir>=0)
        {
		    fileXioDclose(p->FindDir);
		    p->FindDir = -1;
        }
		return ERR_END_OF_FILE;
	}

	return ERR_NONE;
}

static void Delete(filestream* p)
{
	if (p->fd>=0)
    {
		fileXioClose(p->fd);
	    p->fd = -1;
    }
	if (p->FindDir>=0)
    {
		fileXioDclose(p->FindDir);
        p->FindDir = -1;
    }
}

bool_t LoadIRX(anynode* AnyNode,const char* Name,size_t ParamCount,const char* Param)
{
    int ret;
    fourcc_t Data;
    //tchar_t Path[MAXPATH];
    size_t ParamSize=0;
    const char* s = Param;
    for (;ParamCount>0;--ParamCount)
    {
        ParamSize += strlen(s)+1;
        s += strlen(s)+1;
    }

    /*
    // try external file
	if (GetResourcePath(AnyNode,0,Path,TSIZEOF(Path))==ERR_NONE)
    {
        tchar_t Tmp[MAXPATH];
	    tcscat_s(Path,TSIZEOF(Path),Name);
        if (SifExecModuleFile(CdromPath(Path,Tmp,TSIZEOF(Tmp)), ParamSize, Param, &ret)>=0)
            return ret>=0;
    }
    */

    // try core-c resource
    Data = NodeEnumClassStr(AnyNode,NULL,RESOURCEDATA_ID,NODE_NAME,Name);
    if (Data)
    {
        size_t size = (size_t)NodeClass_Meta(NodeContext_FindClass(AnyNode,Data),RESOURCEDATA_SIZE,META_PARAM_CUSTOM);
        void* ptr = (void*)NodeClass_Meta(NodeContext_FindClass(AnyNode,Data),RESOURCEDATA_PTR,META_PARAM_CUSTOM);
        if (ptr && size)
        {
            SifExecModuleBuffer(ptr, size, ParamSize, Param, &ret);
            return ret>=0;
        }
    }
    return 0;
}

void USB_Init(anynode* p)
{
    LoadIRX(p,"usbd.irx",0,NULL);
    LoadIRX(p,"usbhdfsd.irx",0,NULL);
}

void FileXio_Init(nodemodule* p)
{
    LoadIRX(p,"fastmem.irx",0,NULL);
    LoadIRX(p,"poweroff.irx",0,NULL);
    LoadIRX(p,"iomanx.irx",0,NULL);
    LoadIRX(p,"filexio.irx",0,NULL);
#ifndef CONFIG_NODRIVERS
    USB_Init(p);
#endif
}

void FileXio_Done(nodemodule* p)
{
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
META_CONST(TYPE_INT,filestream,FindDir,-1)
META_DATA_RDONLY(TYPE_INT,STREAM_FLAGS,filestream,Flags)
META_DATA_RDONLY(TYPE_STRING,STREAM_URL,filestream,URL)
META_PARAM(SET,STREAM_LENGTH,SetLength)
META_DATA(TYPE_FILEPOS,STREAM_LENGTH,filestream,Length)
META_PARAM(STRING,NODE_PROTOCOL,T("file"))
META_END_CONTINUE(STREAM_CLASS)

META_START_CONTINUE(RESOURCEDATA_ID)
META_CLASS(FLAGS,CFLAG_ABSTRACT)
META_END(NODE_CLASS)

bool_t FileExists(nodecontext *p,const tchar_t* Path)
{
    // not all file systems supports fileXioGetstat
    tchar_t Tmp[MAXPATH];
    int fd = fileXioOpen(CdromPath(Path,Tmp,TSIZEOF(Tmp)),O_RDONLY,0);
	if (fd >= 0)
    {
        fileXioClose(fd);
		return 1;
    }
	return 0;
}

bool_t FileErase(nodecontext *p,const tchar_t* Path, bool_t Force, bool_t Safe)
{
    bool_t Result = fileXioRemove(Path)==0;
    fileXioRmdir(Path); // workaround for the bug in the ROM that creates a directory right after removing the file
    return Result;
}

bool_t FolderErase(nodecontext *p,const tchar_t* Path, bool_t Force, bool_t Safe)
{
    return fileXioRmdir(Path) == 0;
}

bool_t PathIsFolder(nodecontext *p,const tchar_t* Path)
{
    // not all file systems supports fileXioGetstat
    if (tcsnicmp(Path,T("cdrom"),5)!=0)
    {
        int fd = fileXioDopen(Path);
	    if (fd >= 0)
        {
            fileXioDclose(fd);
		    return 1;
        }
    }
	return 0;
}

datetime_t FileDateTime(nodecontext *p,const tchar_t* Path)
{
    iox_stat_t Stat;
    if (fileXioGetStat(Path,&Stat)==0)
        return PS2ToDateTime(Stat.mtime);
	return INVALID_DATETIME_T;
}

bool_t FileMove(nodecontext *p,const tchar_t* In,const tchar_t* Out)
{
#ifndef TODO
    assert(NULL); // not supported yet
#endif
    return 0;
}

bool_t FolderCreate(nodecontext *p,const tchar_t* Path)
{
	return fileXioMkdir(Path,FIO_S_IRUSR | FIO_S_IWUSR | FIO_S_IXUSR | FIO_S_IRGRP | FIO_S_IWGRP | FIO_S_IXGRP | FIO_S_IROTH | FIO_S_IWOTH | FIO_S_IXOTH)==0;
}

stream *FileTemp(anynode *Any)
{
#ifndef TODO
    assert(NULL); // TODO: not supported yet
#endif
    return NULL;
}

bool_t FileTempName(anynode *Any,tchar_t *Out, size_t OutLen)
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
#endif
    return -1;
}

#endif
