/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_IO_CALLBACK_H
#define _EBML2_IO_CALLBACK_H

#include <stdio.h>

#include "ebml/EbmlConfig.h"

typedef struct stream stream;
typedef struct stream_io stream_io;

namespace libebml {

    enum seek_mode
    {
	    seek_beginning = SEEK_SET,
	    seek_end = SEEK_END,
	    seek_current = SEEK_CUR
    };

    typedef enum open_mode {
        MODE_READ,
        MODE_WRITE,
        MODE_CREATE,
        MODE_SAFE
    } open_mode;

    class IOCallback {
    public:
        IOCallback();
        virtual ~IOCallback();
        virtual uint32_t read(void* Buffer, size_t Size)=0;
        virtual size_t write(const void *Buffer,size_t Size)=0;
        virtual void setFilePointer(int64_t Offset, seek_mode Mode=seek_beginning)=0;
        virtual uint64_t getFilePointer()=0;
        virtual void close()=0;

        void writeFully(const void* Buffer, size_t Size);
        stream *GetStream();
    private:
        stream_io *Stream;
    };
};

#endif // _EBML2_IO_CALLBACK_H
