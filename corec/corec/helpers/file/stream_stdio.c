/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include "file.h"
#include "streams.h"

#include <stdio.h>
#if defined(TARGET_WIN)
#include <fcntl.h>
#include <io.h>
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
#if defined(TARGET_WIN)
    _setmode(_fileno(stdin),_O_BINARY);
#endif
    p->Stream = stdin;
    return ERR_NONE;
}

static err_t GetTty(stdio_stream *p, dataid UNUSED_PARAM(Id), bool_t *Value, size_t UNUSED_PARAM(Size))
{
#if defined(TARGET_WIN)
    *Value = _isatty(_fileno(p->Stream))!=0;
#else
    *Value = isatty(fileno(p->Stream))!=0;
#endif
    return ERR_NONE;
}

static err_t CreateStdOut(stdio_stream *p)
{
#if defined(TARGET_WIN)
    _setmode(_fileno(stdout),_O_BINARY);
#endif
    p->Stream = stdout;
    return ERR_NONE;
}

static err_t CreateStdErr(stdio_stream *p)
{
#if defined(TARGET_WIN)
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
