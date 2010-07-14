/*
  Copyright (c) 2006-2010, CoreCodec, Inc.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without 
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the CoreCodec, Inc. nor the names of its contributors 
    may be used to endorse or promote products derived from this software 
    without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
  THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>

#define MAX_REDIRECT    1024
#define MAX_HEADER      4096

#ifndef MAX_PATH
#define MAX_PATH		1024
#endif

#if !defined(_MSC_VER) && !defined(__MINGW32__)
# ifndef strcmpi
#  define strcmpi strcasecmp
# endif
# ifndef stricmp
#  define stricmp strcasecmp
# endif
# ifndef strncmpi
#  define strncmpi strncasecmp
# endif
# ifndef strnicmp
#  define strnicmp strncasecmp
# endif
# define make_dir(x) mkdir(x,S_IRWXU)
# define tmpnam_safe(x) tmpnam(x)
#else
static const char *tmpnam_safe(char *str)
{
	char *res = tmpnam(str);
	if (res && res[0] == '\\')
		++res;
	return res;
}
# define make_dir(x) mkdir(x)
#endif

int verbose = 0;

#ifdef _WIN32
#include <io.h>
#include <direct.h>

#ifndef _INTPTR_T_DEFINED
typedef signed int intptr_t;
#define _INTPTR_T_DEFINED
#endif

struct dirent
{
	char d_name[MAX_PATH];
};
typedef struct DIR
{
	intptr_t h;
	int first;
	struct _finddata_t file;
	struct dirent entry;

} DIR;

DIR* opendir(const char* name)
{
	DIR* p = malloc(sizeof(DIR));
	if (p)
	{
		sprintf(p->entry.d_name,"%s\\*",name);
		p->h = _findfirst(p->entry.d_name,&p->file);
		p->first = 1;
		if (p->h == -1)
		{
			free(p);
			p = NULL;
		}
	}
	return p;
}

struct dirent* readdir(DIR* p)
{
	if (p->first || _findnext(p->h,&p->file)==0)
	{
		p->first = 0;
		strcpy(p->entry.d_name,p->file.name);
		return &p->entry;
	}
	return NULL;
}

void closedir(DIR* p)
{
	_findclose(p->h);
	free(p);
}

#else
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

void trim(char* s)
{
    while (s[0] && isspace(s[strlen(s)-1]))
        s[strlen(s)-1]=0;
}

void addendpath(char* path)
{
	if (path[0] && path[strlen(path)-1]!='/') 
		strcat(path,"/");
}

void delendpath(char* path)
{
	if (path[0] && path[strlen(path)-1]=='/')
		path[strlen(path)-1] = 0;
}

void create_missing_dirs(const char *path)
{
	size_t strpos = 0, strpos_i;
	char new_path[MAX_PATH];
	char orig_path[MAX_PATH];
	int ret;
	struct stat dir_stats;

	getcwd(orig_path, sizeof(new_path));

	for (;;)
	{
		strpos_i = strcspn(&path[strpos], "/\\");
		if (strpos_i == strlen(&path[strpos]))
			break;
		strpos += strpos_i+1;

		strcpy(new_path,orig_path);
		strcat(new_path,"/");
		strncat(new_path, path, strpos-1);

		if (stat(new_path, &dir_stats) == 0)
		{
			if ((dir_stats.st_mode & S_IFDIR) != S_IFDIR)
			{
				printf("%s is not a directory\n",new_path);
				exit(1);
			}
		}
		else
		{
			ret = make_dir(new_path);
			
			if (ret != 0 && ret != EEXIST)
			{
				printf("can't create directory %s\n",new_path);
				exit(1);
			}
		}
	}
}

static char header[3][MAX_HEADER];
static char header_magic[MAX_HEADER];

static size_t redirect_count;
static char redirect_src[MAX_REDIRECT][MAX_PATH];
static char redirect_dst[MAX_REDIRECT][MAX_PATH];
static char redirect_process[MAX_REDIRECT][MAX_PATH];

int gethead(const char* line, int prev)
{
    if (prev<0 && strncmp(line,"/*",2)==0)
        return 3;

    if (prev==3)
    {
        const char* s = strrchr(line,'*');
        if (s && s[1]=='/')
            return 0;
        return 3;
    }

    if (strncmp(line,";*",2)==0)
        return 1;

    if (strncmp(line,"@*",2)==0)
        return 2;

    return -1;
}

int stristr(const char* s, const char* sub)
{
    size_t n = strlen(sub);
    for (;*s;++s)
        if (strnicmp(s,sub,n)==0)
            return 1;
    return 0;
}

int preprocess(const char* srcname, const char* dstname, const char* process)
{
    char line[MAX_HEADER];
    FILE* src;
    FILE* dst;
    int head=-1;
    int ret=0;

    src = fopen(srcname,"r");
    if (!src)
        return 0;

    fgets(line,sizeof(line),src);

    if (header[0][0])
    {
        int i;
        if ((i = gethead(line,-1))>=0)
        {
            int magic=0;
            do
            {
                head = i;
                if (!fgets(line,sizeof(line),src))
                    break;

                i = gethead(line,i);

                if (stristr(line,header_magic))
                    magic = 1;
            }
            while (i>=0);

            if (magic)
                ret = 1;
        }
    }

    if (process[0])
        ret = 1;

    if (!ret)
    {
        fclose(src);
        return 0;
    }

    dst = fopen(dstname,"w");
    if (!dst)
    {
        printf("file create error (%s)!\n",dstname);
        exit(1);
    }

    if (head>=0)
        fputs(header[head],dst);

    for (;;)
    {
		const char* i1;
		const char* i2;
		const char* i3;
		char* j;

		for (i1=process;(i1=strchr(i1,'{'))!=NULL && (i2=strchr(i1,'|'))!=NULL && (i3=strchr(i2,'}'))!=NULL;i1=i3)
		{
			size_t n=i2-(++i1);
			for (j=line;*j;)
				if (strncmp(j,i1,n)==0)
				{
                    char* k;
					memmove(j,j+n,strlen(j+n)+1);
					n=i3-(++i2);
					memmove(j+n,j,strlen(j)+1);
					memcpy(j,i2,n);
                    for (k=j;k<j+n;++k)
                        if (k[0]=='\\' && k[1]=='n')
                        {
        					memmove(k,k+1,strlen(k+1)+1);
                            k[0] = '\n';
                            --n;
                        }
					if (strcmp(line,"\n")==0 || strcmp(line,"\r\n")==0)
						line[0]=0;
				}
				else
					++j;
		}

        fputs(line,dst);
        if (!fgets(line,sizeof(line),src))
            break;
    }

    fclose(src);
    fclose(dst);
    return 1;
}

int compare(const char* srcname, const char* dstname, const char* process)
{
    char srcbuf[4096];
    char dstbuf[4096];
    const char* tmpname = NULL;
    FILE* src;
    FILE* dst = NULL;
    int ret=0;

    if (header[0][0] || process[0])
    {
        tmpname = tmpnam_safe(NULL);
        if (preprocess(srcname,tmpname,process))
            srcname = tmpname;
        else
            tmpname = NULL;
    }

    src = fopen(srcname,"rb");
    if (!src)
    {
        if (tmpname)
            remove(tmpname);
        printf("file open error (%s)!\n",srcname);
        exit(1);
    }

    dst = fopen(dstname,"rb");
    if (dst)
    {
        for (;;)
        {
            size_t n = fread(srcbuf,1,sizeof(srcbuf),src);
            size_t m = fread(dstbuf,1,sizeof(dstbuf),dst);
            if (n != m || memcmp(srcbuf,dstbuf,n)!=0)
            {
                ret=1;
                break;
            }
            if (!n)
                break;
        }
        fclose(dst);
    }
    else
        ret = 1;

    fclose(src);
    if (tmpname)
        remove(tmpname);

    return ret;
}

void copy(const char* srcname, const char* dstname, const char* process)
{
    char buf[4096];
    FILE* src;
    FILE* dst;
    const char* tmpname = NULL;
    struct stat s;

    printf("%s -> %s\n",srcname,dstname);

    stat(srcname,&s);

    if (header[0][0] || process[0])
    {
        tmpname = tmpnam_safe(NULL);
        if (preprocess(srcname,tmpname,process))
            srcname = tmpname;
        else
            tmpname = NULL;
    }

    src = fopen(srcname,"rb");
    if (!src)
    {
        printf("file open error (%s)!\n",srcname);
        exit(1);
    }

    create_missing_dirs(dstname);

    dst = fopen(dstname,"wb+");
    if (!dst)
    {
        printf("file create error (%s)!\n",dstname);
        exit(1);
    }

    for (;;)
    {
        size_t n = fread(buf,1,sizeof(buf),src);
        if (!n) break;
        fwrite(buf,1,n,dst);
    }

    fclose(src);
    fclose(dst);

    chmod(dstname,s.st_mode);

    if (tmpname)
        remove(tmpname);
}

int same_ch(int a, int b)
{
#ifdef _WIN32
    return toupper(a) == toupper(b);
#else
    return a==b;
#endif
}

int match(const char* name, const char* mask)
{
    if (*mask=='*')
    {
        ++mask;
        for (;!match(name,mask);++name)
        {
            if (*name==0)
                return 0;
        }
        return 1;
    }
    else
    {
        if (*mask==0)
            return *name==0;

        if (*name==0)
            return 0;

        return (mask[0]=='?' || same_ch(name[0],mask[0])) && match(name+1,mask+1);
    }
}

void refresh(const char* src, const char* dst, int force, const char* process)
{
    DIR* dir;
    size_t i;

    if (strcmp(dst,"-")==0 || strchr(src,'*') || strchr(src,'?'))
        return;

    if (!force)
        for (i=0;i<redirect_count;++i)
            if (match(src,redirect_src[i]))
                return;

	dir = opendir(src);
	if (dir)
	{
		struct dirent* entry;
        make_dir(dst);
        if (!force || dst[strlen(dst)-1]!='.')
        {
		    while ((entry = readdir(dir)) != NULL)
		    {
			    if (entry->d_name[0]!='.')
			    {
                    char src2[MAX_PATH];
                    char dst2[MAX_PATH];

                    strcpy(dst2,dst);
                    strcpy(src2,src);
        		    addendpath(dst2);
                    addendpath(src2);
        		    strcat(src2,entry->d_name);
                    strcat(dst2,entry->d_name);
                    refresh(src2,dst2,0,"");
			    }
		    }
        }
		closedir(dir);
	}
    else
    {
        if (compare(src,dst,process))
            copy(src,dst,process);
    }
}

int main(int argc, char** argv)
{
    FILE* f;
    char base[MAX_PATH];
    char line[MAX_PATH*2];
    int head=0;
    size_t i;

    if (argc<=1)
    {
        printf("usage: branch <config>\n");
        return 1;
    }

    strcpy(base,argv[1]);
    for (i=strlen(base);i>0;)
    {
        --i;
        if (base[i]=='/' || base[i]=='\\')
            break;
        base[i]=0;
    }

    memset(header,0,sizeof(header));

    f = fopen(argv[1],"r");
    if (!f)
    {
        printf("%s not found!\n",argv[1]);
        return 1;
    }

    redirect_count=0;
    for (;;)
    {

        char* s, *t;
        if (!fgets(line,sizeof(line),f))
            break;

        if (line[0]=='#')
            continue;

        if (!head && line[0]=='/' && line[1]=='*')
            head = 1;

        if (head)
        {
            strcat(header[0],line);
            s = strrchr(line,'*');
            if (s && s[1]=='/')
            {
                s[1]='*';
                head = 0;
                fgets(header_magic,sizeof(header_magic),f);
                trim(header_magic);
            }
            line[0]=';';
            strcat(header[1],line);
            line[0]='@';
            strcat(header[2],line);
        }
        else
        {
            trim(line);
            strcpy(redirect_src[redirect_count],(line[0]!='/')?base:"");
            strcat(redirect_src[redirect_count],line);

            s = redirect_src[redirect_count];
            s += (line[0]!='/')?strlen(base):0;
            while (*s && isspace(*s))
                ++s;
            if (*s == '\"')
            {
                memmove(s,s+1,strlen(s)+1);
                t = strchr(s+1,'\"');
                if (t)
                    *t=0;
                s = t+1;
            }
            else
            {
                s = strchr(redirect_src[redirect_count],' ');
            }

            if (s)
            {
                while (*s && isspace(*s))
                    *(s++)=0;
                if (*s == '\"')
                {
                    t = strchr(++s,'\"');
                    if (t)
                        *t=' ';
                    strcpy(redirect_dst[redirect_count],s);
                    s = strchr(t+1,' ');
                }
                else
                {
                    strcpy(redirect_dst[redirect_count],s);
                    s = strchr(redirect_dst[redirect_count],' ');
                }
                redirect_process[redirect_count][0]=0;

                if (s)
                {
                    while (*s && isspace(*s))
                        *(s++)=0;

				    if (*s)
                        strcpy(redirect_process[redirect_count],s);
                }

                ++redirect_count;
            }
        }
    }

    fclose(f);

    for (i=0;i<redirect_count;++i)
        refresh(redirect_src[i],redirect_dst[i],1,redirect_process[i]);

	return 0;
}
