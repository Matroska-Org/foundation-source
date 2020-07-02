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

#ifndef __ERR_H
#define __ERR_H

#define ERR_ID				FOURCC('E','R','R','_')

//----------------------------------------------------------------
// error codes

#define ERR_NONE			(err_t)0
#define ERR_BUFFER_FULL		(err_t)-1  // 0xFFFFFFFF
#define ERR_OUT_OF_MEMORY	(err_t)-2  // 0xFFFFFFFE
#define ERR_INVALID_DATA	(err_t)-3  // 0xFFFFFFFD
#define ERR_INVALID_PARAM	(err_t)-4  // 0xFFFFFFFC
#define ERR_NOT_SUPPORTED	(err_t)-5  // 0xFFFFFFFB
#define ERR_NEED_MORE_DATA	(err_t)-6  // 0xFFFFFFFA
                       // stream added    0xFFFFFFF9
#define ERR_FILE_NOT_FOUND	(err_t)-8  // 0xFFFFFFF8
#define ERR_END_OF_FILE		(err_t)-9  // 0xFFFFFFF7
#define ERR_DEVICE_ERROR	(err_t)-10 // 0xFFFFFFF6
#define ERR_SYNCED			(err_t)-11 // 0xFFFFFFF5
#define ERR_DATA_NOT_FOUND	(err_t)-12 // 0xFFFFFFF4
#define ERR_PROTO_NOT_FOUND (err_t)-13 // 0xFFFFFFF3
#define ERR_NOT_DIRECTORY	(err_t)-14 // 0xFFFFFFF2
#define ERR_NOT_COMPATIBLE	(err_t)-15 // 0xFFFFFFF1
#define ERR_CONNECT_FAILED	(err_t)-16 // 0xFFFFFFF0
#define ERR_DROPPING		(err_t)-17 // 0xFFFFFFEF
#define ERR_STOPPED			(err_t)-18 // 0xFFFFFFEE
#define ERR_UNAUTHORIZED	(err_t)-19 // 0xFFFFFFED
#define ERR_LOADING_HEADER	(err_t)-20 // 0xFFFFFFEC
#define ERR_READ            (err_t)-21 // 0xFFFFFFEB
#define ERR_WRITE           (err_t)-22 // 0xFFFFFFEA
#define ERR_UNRESOLVED_ADDR	(err_t)-23 // 0xFFFFFFE9
#define ERR_NO_NETWORK      (err_t)-24 // 0xFFFFFFE8
#define ERR_TIME_OUT        (err_t)-25 // 0xFFFFFFE7
#define ERR_KEY_NOT_UNIQUE  (err_t)-26 // 0xFFFFFFE6
#define ERR_NOT_CONST       (err_t)-27 // 0xFFFFFFE5
#define ERR_REDIRECTED      (err_t)-28 // 0xFFFFFFE4
#define ERR_CANCELED        (err_t)-29 // 0xFFFFFFE3
#define ERR_STREAM_CACHED   (err_t)-30 // 0xFFFFFFE2
#define ERR_SERVER_ERROR    (err_t)-31 // 0xFFFFFFE1
#define ERR_NOT_USABLE      (err_t)-32 // 0xFFFFFFE0

// buffer full: data is not processed, retry later
// need more data: data is processed, but need more to continue

#endif
