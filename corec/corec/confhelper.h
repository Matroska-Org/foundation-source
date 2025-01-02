/*
  $Id$

  Copyright (c) 2010, CoreCodec, Inc.
  SPDX-License-Identifier: BSD-3-Clause
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

#include <wchar.h>

typedef wchar_t tchar_t;

#define tcsstr wcsstr
#define tcslen wcslen
#define tcschr wcschr
#define tcsrchr wcsrchr
#define T(a) L ## a
#else /* UNICODE */
typedef char tchar_t;
#define tcsstr strstr
#define tcslen strlen
#define tcschr strchr
#define tcsrchr strrchr
#define T(a) a
#endif /* UNICODE */

#endif /* __CONFIG_HELPER_H */
