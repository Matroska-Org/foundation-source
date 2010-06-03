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

#include <stdio.h>
#if defined(TARGET_WIN)
#if !defined(TARGET_WINCE)
#include <fcntl.h>
#include <io.h>
#endif
#else
#include <unistd.h>
#endif

#define STDIO_CLASS  FOURCC('S','T','I','O')

typedef struct stdio_stream
{
    stream Base;
    FILE *Stream;

} stdio_stream;

static err_t Write(stdio_stream *p,const void* Data,size_t Size,size_t* Written)
{
    size_t written;
    if (!Written)
        Written = &written;
    *Written = fwrite(Data,1,Size,p->Stream);
    return (*Written == Size) ? ERR_NONE : ERR_DEVICE_ERROR;
}

static err_t Read(stdio_stream *p, void* Data, size_t Size, size_t* Readed)
{
    size_t readed;
    if (!Readed)
        Readed = &readed;
    *Readed = fread(Data,1,Size,p->Stream);
    return (*Readed == Size) ? ERR_NONE : ERR_DEVICE_ERROR;
}

static err_t ReadOneOrMore(stdio_stream* p,char* Data,size_t UNUSED_PARAM(Size),size_t* Readed)
{
    *Data = (char)fgetc(p->Stream);
    if (Readed)
        *Readed = 1;
    return ERR_NONE;
}

static err_t Flush(stdio_stream *p)
{
    return fflush(p->Stream)==0 ? ERR_NONE : ERR_DEVICE_ERROR;
}

static err_t CreateStdIn(stdio_stream *p)
{
#if defined(TARGET_WIN) && !defined(TARGET_WINCE)
    _setmode(_fileno(stdin),_O_BINARY);
#endif
    p->Stream = stdin;
    return ERR_NONE;
}

static err_t GetTty(stdio_stream *p, dataid UNUSED_PARAM(Id), bool_t *Value, size_t UNUSED_PARAM(Size))
{
#if defined(TARGET_WINCE) || defined(TARGET_PS2SDK)
    *Value = 0;
#elif defined(TARGET_WIN)
    *Value = _isatty(_fileno(p->Stream))!=0;
#else
    *Value = isatty(fileno(p->Stream))!=0;
#endif
    return ERR_NONE;
}

static err_t CreateStdOut(stdio_stream *p)
{
#if defined(TARGET_WIN) && !defined(TARGET_WINCE)
    _setmode(_fileno(stdout),_O_BINARY);
#endif
    p->Stream = stdout;
    return ERR_NONE;
}

static err_t CreateStdErr(stdio_stream *p)
{
#if defined(TARGET_WIN) && !defined(TARGET_WINCE)
    _setmode(_fileno(stderr),_O_BINARY);
#endif
    p->Stream = stderr;
    return ERR_NONE;
}

META_START(Stdio_Class,STDIO_CLASS)
META_CLASS(FLAGS,CFLAG_ABSTRACT)
META_CLASS(SIZE,sizeof(stdio_stream))
META_VMT(TYPE_FUNC,stream_vmt,Write,Write)
META_VMT(TYPE_FUNC,stream_vmt,Read,Read)
META_VMT(TYPE_FUNC,stream_vmt,ReadOneOrMore,ReadOneOrMore)
META_VMT(TYPE_FUNC,stream_vmt,Flush,Flush)
META_END_CONTINUE(STREAM_CLASS)

META_START_CONTINUE(STDIN_ID)
META_CLASS(FLAGS,CFLAG_SINGLETON)
META_CLASS(CREATE,CreateStdIn)
META_PARAM(TYPE,STREAM_ISTTY,TYPE_BOOLEAN)
META_PARAM(GET,STREAM_ISTTY,GetTty)
META_END_CONTINUE(STDIO_CLASS)

META_START_CONTINUE(STDOUT_ID)
META_CLASS(FLAGS,CFLAG_SINGLETON)
META_CLASS(CREATE,CreateStdOut)
META_END_CONTINUE(STDIO_CLASS)

META_START_CONTINUE(STDERR_ID)
META_CLASS(FLAGS,CFLAG_SINGLETON)
META_CLASS(CREATE,CreateStdErr)
META_END(STDIO_CLASS)
