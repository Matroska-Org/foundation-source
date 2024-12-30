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
