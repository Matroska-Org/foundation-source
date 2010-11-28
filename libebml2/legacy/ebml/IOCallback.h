/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Matroska assocation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY the Matroska association ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL The Matroska Foundation BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _EBML2_IO_CALLBACK_H
#define _EBML2_IO_CALLBACK_H

#include <stdio.h>

#include "ebml/EbmlConfig.h"

typedef struct stream stream;
typedef struct stream_io stream_io;

namespace LIBEBML_NAMESPACE {

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
