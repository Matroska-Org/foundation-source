/*
Copyright (c) 2006-2016, CoreCodec, Inc.
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

#ifndef DIRECTORY_H
#define DIRECTORY_H

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <windows.h>

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
	DIR* p = (DIR*)malloc(sizeof(DIR));
	if (p)
	{
		sprintf(p->entry.d_name, "%s\\*", name);
		p->h = _findfirst(p->entry.d_name, &p->file);
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
	if (p->first || _findnext(p->h, &p->file) == 0)
	{
		p->first = 0;
		strcpy(p->entry.d_name, p->file.name);
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

#endif /* DIRECTORY_H */
