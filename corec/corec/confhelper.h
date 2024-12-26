/*
  $Id$

  Copyright (c) 2010, CoreCodec, Inc.
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

#ifndef __CONFIG_HELPER_H
#define __CONFIG_HELPER_H

/* force some defines */

#if defined(TARGET_WIN32) || defined(TARGET_WIN64) || defined(TARGET_OSX) || defined(TARGET_LINUX)
#define CONFIG_FILEPOS_64 /* platforms where 64 bits file position/size should be favoured */
#endif

/* forbid some defines */

#if defined(CONFIG_UNICODE_WCHAR)
#define UNICODE
#elif defined(CONFIG_UNICODE_BEST)
#if defined(TARGET_WIN) // on UNIX like we assume utf-8
#define UNICODE
#endif
#endif // CONFIG_UNICODE_BEST

#undef T
#define TSIZEOF(name)	(sizeof(name)/sizeof(tchar_t))

#if defined(UNICODE)

#if defined(__GNUC__)
#include <wchar.h>
#endif

#if defined(__GNUC__) && (__GNUC__<3) && defined(__cplusplus)
typedef __wchar_t tchar_t;
#else
typedef wchar_t tchar_t;
#endif

#define tcsstr wcsstr
#define tcslen wcslen
#define tcschr wcschr
#define tcsrchr wcsrchr
#define tcscoll wcscoll
#define tcstod wcstod
#define tcscspn wcscspn
#define tcspbrk wcspbrk
#define tcstoul wcstoul
#define tcsftime wcsftime
#define T(a) L ## a
#else /* UNICODE */
typedef char tchar_t;
#define tcsstr strstr
#define tcslen strlen
#define tcschr strchr
#define tcsrchr strrchr
#define tcscoll strcoll
#define tcstod strtod
#define tcscspn strcspn
#define tcspbrk strpbrk
#define tcstoul strtoul
#define tcsftime strftime
#define T(a) a
#endif /* UNICODE */

#endif /* __CONFIG_HELPER_H */
