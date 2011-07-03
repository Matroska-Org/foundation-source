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

#ifndef __PORTAB_H
#define __PORTAB_H

#if defined(__GNUC__) && defined(__palmos__)
#undef __WCHAR_TYPE__
#define __WCHAR_TYPE__ unsigned short
#endif

#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>

#undef INLINE
#undef NOINLINE
#undef IS_LITTLE_ENDIAN
#undef IS_BIG_ENDIAN

#if defined(__palmos__)

#define TARGET_PALMOS
#define NO_FLOATINGPOINT
#define CONFIG_BLOCK_RDONLY

#elif defined(_EE)

#define TARGET_PS2SDK
#define TARGET_DESKTOP
#define RESOURCE_COREC

#elif defined(__SYMBIAN32__)

#if defined(__SERIES60_3X__)
#define SYMBIAN90
#define SERIES60
#elif defined(__SERIES60_10__)
#define SYMBIAN60
#define SERIES60
#elif defined(__SERIES60_20__)
#define SYMBIAN70
#define SERIES60
#elif defined(__SERIES80__)
#define SYMBIAN70
#define SERIES80
#elif defined(__SERIES90__)
#define SYMBIAN70
#define SERIES90
#elif defined(__UIQ3__)
#define SYMBIAN90
#define UIQ
#elif defined(__UIQ__)
#define SYMBIAN70
#define UIQ
#endif

#define TARGET_SYMBIAN
#ifndef UNICODE
#define UNICODE
#endif

#ifdef __MARM__
#define ARM
#endif

#elif defined(_WIN32_WCE)

#if !defined(CONFIG_WINCE2) && (_WIN32_WCE>=200 && _WIN32_WCE<300)
#define CONFIG_WINCE2
#endif

#if defined(WIN32_PLATFORM_WFSP)
#define TARGET_SMARTPHONE
#endif

#define TARGET_WINCE
#define TARGET_WIN

#ifndef UNICODE
#define UNICODE
#endif

#elif defined(_WIN64)

#define TARGET_WIN64
#define TARGET_WIN
#define TARGET_DESKTOP

#elif defined(_WIN32)

#define TARGET_WIN32
#define TARGET_WIN
#define TARGET_DESKTOP

#elif defined(__APPLE__)

#ifndef TARGET_IPHONE
#include <TargetConditionals.h> // iPhone information is defined within the SDK in which Xcode is pointed to
#endif

#define TARGET_OSX
#undef UNICODE // tchar_t is UTF-8 for OS X

#ifdef TARGET_OS_IPHONE // defined in TargetConditionals.h
#define TARGET_IPHONE
#define TARGET_IPHONE_SDK

#if !TARGET_IPHONE_SIMULATOR
#define TARGET_IPHONE_DEVICE
#endif


#if defined(__arm__)
#define ARM
#endif

#endif

#ifndef TARGET_IPHONE
#define TARGET_DESKTOP // only for dekstop (not iPhone or Apple TV)
#endif

#elif defined(ANDROID)

#define TARGET_ANDROID

#if defined(__arm__)
#define ARM
#endif

#elif defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)

#define TARGET_LINUX

#ifndef TARGET_QTOPIA // qtopia defines desktop manually
#define TARGET_DESKTOP // not necessarily
#endif

#else
#error Platform not supported by Core-C!
#endif

#if defined(__powerpc__) || defined(__PPC__) || defined(__POWERPC__)
#define POWERPC
#define IS_BIG_ENDIAN
#endif

#if defined(__sparc)
#if defined(_LP64)
#define SPARC64
#define CPU_64BITS
#else
#define SPARC32
#endif
#define IS_BIG_ENDIAN
#endif

#if defined(TARGET_LINUX) || defined(TARGET_ANDROID)
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#include <sys/endian.h>
#else
#include <endian.h>
#endif
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define IS_LITTLE_ENDIAN
#elif __BYTE_ORDER == __BIG_ENDIAN
#define IS_BIG_ENDIAN
#endif
#endif

#if defined(_M_X64) || defined(__amd64__)
#define IX86_64
#define CPU_64BITS
#endif

#if defined(_M_IA64)
#define IA64
#define CPU_64BITS
#endif

#if defined(__mips) && !defined(MIPS)
#define MIPS
#endif

#ifdef MIPS
#if defined(__mips64) || defined(CONFIG_MIPS64)
#define MIPS64
#else
#define MIPS32
#endif
#endif

#if defined(_M_IX86) || defined(__i386) || defined(__i686)
#define IX86
#endif

#if !defined(IS_LITTLE_ENDIAN) && !defined(IS_BIG_ENDIAN)
#define IS_LITTLE_ENDIAN
#endif

#define TICKSPERSEC			16384
#define TIMEPERSEC          (TICKSPERSEC>>4)

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288
#endif

#ifdef _MSC_VER

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <malloc.h>
#ifndef TARGET_WINCE
#include <crtdbg.h>
#endif

#ifndef strncasecmp
#define strncasecmp(x,y,z) _strnicmp(x,y,z)
#endif
#ifndef strcasecmp
#define strcasecmp(x,y)    _stricmp(x,y)
#endif
#ifndef stricmp
#define stricmp(x,y)       _stricmp(x,y)
#endif
#ifndef strnicmp
#define strnicmp(x,y,z)    _strnicmp(x,y,z)
#endif

#if _MSC_VER >= 1400
#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")
//#pragma comment(linker, "/nodefaultlib:oldnames.lib")
#elif defined(_WIN32_WCE)
// allow evc3/evc4 compiling with vs2005 object files
#pragma comment(linker, "/nodefaultlib:libcmt.lib")
#pragma comment(linker, "/nodefaultlib:oldnames.lib")
#endif

#ifndef alloca
#define alloca _alloca
#endif

#ifndef inline
#define inline __inline
#endif

#ifndef _UINTPTR_T_DEFINED
typedef unsigned int uintptr_t;
#define _UINTPTR_T_DEFINED
#endif

#ifndef _INTPTR_T_DEFINED
typedef signed int intptr_t;
#define _INTPTR_T_DEFINED
#endif

#if _MSC_VER >= 1400
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE
#endif

#define INLINE __forceinline

#ifndef STDCALL
#define STDCALL __stdcall
#endif

#ifndef CDECL
#define CDECL __cdecl
#endif

#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC
#endif

#if defined(__cplusplus)
#define SORTPP_PASS // to avoid a problem if winnt.h is included after us
#define NOMINMAX // conflict with <algorithm>
#endif

#else /* _MSC_VER */

#include <stdlib.h>

#if defined(TARGET_SYMBIAN)

typedef signed long int32_t;
typedef unsigned long uint32_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
typedef uint32_t uintptr_t;
typedef int32_t intptr_t;
typedef signed int int_fast32_t;
typedef unsigned int uint_fast32_t;
typedef signed int int_fast16_t;
typedef unsigned int uint_fast16_t;
typedef signed char int_fast8_t;
typedef unsigned char uint_fast8_t;
typedef signed long long int_fast64_t;
typedef unsigned long long uint_fast64_t;

#elif !defined(__GLIBC__) && !defined(__MINGW32__) && !defined(TARGET_PS2SDK) && !defined(TARGET_IPHONE) && !defined(TARGET_ANDROID) && !defined(__FreeBSD__)

#include <inttypes.h>

#ifdef SPARC64
  typedef int64_t int_fast32_t;
  typedef uint64_t uint_fast32_t;
  typedef int64_t int_fast16_t;
  typedef uint64_t uint_fast16_t;
#else
  typedef int32_t int_fast32_t;
  typedef uint32_t uint_fast32_t;
# ifndef __APPLE__
  typedef int32_t int_fast16_t;
  typedef uint32_t uint_fast16_t;
# endif
#endif

typedef int8_t int_fast8_t;
typedef uint8_t uint_fast8_t;
typedef int64_t int_fast64_t;
typedef uint64_t uint_fast64_t;

#else
#include <stdint.h>
#endif

#if defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0) && !defined(always_inline)
#define INLINE __attribute__((always_inline)) inline
#else
#define INLINE inline
#endif

#if __GNUC__ >= 3
  #define NOINLINE __attribute__((noinline))
#else
  #define NOINLINE
#endif

#if !defined(IX86) || defined(__CW32__)
#define __stdcall
#define __cdecl
#define STDCALL
#define CDECL
#else
#ifndef STDCALL
#define STDCALL __attribute__((stdcall))
#endif
#ifndef CDECL
#define CDECL __attribute__((cdecl))
#endif
#endif

#if !defined(COMPILER_GCC) && defined(__GNUC__)
#define COMPILER_GCC
#endif

#endif /* _MSC_VER */

#if !defined(_STDINT_H) && !defined(_STDINT_H_) && !defined(_UINT64_T_DECLARED) // could be used elsewhere

typedef signed long int32_t;
typedef unsigned long uint32_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;

#if defined(IX86_64) || defined(IA64)
typedef signed __int64 int_fast32_t;
typedef unsigned __int64 uint_fast32_t;
typedef signed __int64 int_fast16_t;
typedef unsigned __int64 uint_fast16_t;
typedef signed char int_fast8_t;
typedef unsigned char uint_fast8_t;
typedef signed __int64 int_fast64_t;
typedef unsigned __int64 uint_fast64_t;
#else
#if _MSC_VER >= 1400
typedef __w64 signed int int_fast32_t;
typedef __w64 unsigned int uint_fast32_t;
typedef __w64 signed int int_fast16_t;
typedef __w64 unsigned int uint_fast16_t;
#else
typedef signed int int_fast32_t;
typedef unsigned int uint_fast32_t;
typedef signed int int_fast16_t;
typedef unsigned int uint_fast16_t;
#endif
typedef signed char int_fast8_t;
typedef unsigned char uint_fast8_t;
typedef signed __int64 int_fast64_t;
typedef unsigned __int64 uint_fast64_t;
#endif
#endif

#ifdef _MSC_VER
#define LL(x)   x##i64
#define ULL(x)  x##ui64
#define PRId64  "I64d"
#define PRIu64  "I64u"
#define PRIx64  "I64x"
#define TPRId64  L"I64d"
#define TPRIu64  L"I64u"
#define TPRIx64  L"I64x"
#else
#define LL(x)   x##ll
#define ULL(x)  x##ull
#ifndef PRId64
#define PRId64  "lld"
#endif
#ifndef PRIu64
#define PRIu64  "llu"
#endif
#ifndef PRIx64
#define PRIx64  "llx"
#endif
#define TPRId64  PRId64
#define TPRIu64  PRIu64
#define TPRIx64  PRIx64
#endif

#define MAX_INT64 LL(0x7fffffffffffffff)

#define MAX_TICK INT_MAX

typedef int_fast32_t err_t;
typedef int_fast32_t bool_t;
typedef int_fast32_t tick_t;
typedef uint8_t boolmem_t; /* unsigned so ":1" bitmode should work */
typedef uint32_t fourcc_t;
typedef uint32_t rgbval_t;

typedef struct cc_guid
{
    uint32_t v1;
    uint16_t v2;
    uint16_t v3;
    uint8_t v4[8];
} cc_guid;

typedef struct cc_fraction
{
	int_fast32_t Num;
	int_fast32_t Den;
} cc_fraction;

typedef struct cc_fraction64
{
	int_fast64_t Num;
	int_fast64_t Den;
} cc_fraction64;

typedef struct cc_point
{
	int x;
	int y;

} cc_point;

typedef struct cc_point16
{
	int16_t x;
	int16_t y;

} cc_point16;

typedef struct cc_rect
{
	int x;
	int y;
	int Width;
	int Height;

} cc_rect;

#ifndef ZLIB_INTERNAL

#undef T
#define TSIZEOF(name)	(sizeof(name)/sizeof(tchar_t))

#if defined(UNICODE)

#if defined(__GNUC__) && !defined(TARGET_SYMBIAN) && !defined(TARGET_PALMOS)
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

#define T__(x) T(x)
#endif

#ifndef SIZEOF_WCHAR
#if defined(TARGET_OSX) || defined(TARGET_LINUX) || defined(TARGET_PS2SDK)
#define SIZEOF_WCHAR    4
#else
#define SIZEOF_WCHAR    2
#endif
#endif

#if SIZEOF_WCHAR==2
typedef wchar_t utf16_t;
#else
typedef uint16_t utf16_t;
#endif

#define tcscpy !UNSAFE!
#define tcscat !UNSAFE!
#define stprintf !UNSAFE!
#define vstprintf !UNSAFE!

#if defined(_WIN32) || defined(TARGET_SYMBIAN)
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#define DLLHIDDEN
#elif defined(__GNUC__) && ((__GNUC__*100 + __GNUC_MINOR__) >= 340)
#define DLLEXPORT __attribute__ ((visibility("default")))
#define DLLIMPORT __attribute__ ((visibility("default")))
#define DLLHIDDEN __attribute__ ((visibility("hidden")))
#else
#define DLLEXPORT
#define DLLIMPORT
#define DLLHIDDEN
#endif

#ifdef __cplusplus
#define INTERNAL_C_API extern "C"
#else
#define INTERNAL_C_API extern
#endif


#if defined(TARGET_WINCE) && (defined(SH3) || defined(SH4) || defined(MIPS))
#define _INLINE
#define _CONST
#else
#define _INLINE INLINE
#define _CONST const
#endif

/* todo: needs more testing, that nothing broke... */
#if defined(MAX_PATH)
#define MAXPATH MAX_PATH
#elif defined(PATH_MAX)
#define MAXPATH PATH_MAX
#else
#define MAXPATH 256
#endif
#define MAXPROTOCOL 16

#if MAXPATH < 1024
#define MAXPATHFULL 1024
#else
#define MAXPATHFULL MAXPATH
#endif

#define MAXPLANES 4
typedef void* planes[MAXPLANES];
typedef const void* constplanes[MAXPLANES];
#define CONST_CONSTPLANES(name) const void* const (name)[MAXPLANES]
#define CONSTPLANES(name) const void* (name)[MAXPLANES]
#define CONST_PLANES(name) void* const (name)[MAXPLANES]

#define FOURCCBE(a,b,c,d) \
	(((uint8_t)(a) << 24) | ((uint8_t)(b) << 16) | \
	((uint8_t)(c) << 8) | ((uint8_t)(d) << 0))

#define FOURCCLE(a,b,c,d) \
	(((uint8_t)(a) << 0) | ((uint8_t)(b) << 8) | \
	((uint8_t)(c) << 16) | ((uint8_t)(d)<< 24))

#ifdef IS_BIG_ENDIAN
#define FOURCC(a,b,c,d) (fourcc_t)FOURCCBE(a,b,c,d)
#else
#define FOURCC(a,b,c,d) (fourcc_t)FOURCCLE(a,b,c,d)
#endif

#if defined(TARGET_WINCE)
#undef strdup
#define strdup(x) _strdup(x)
#define wcscoll(x,y) tcscmp(x,y)
#endif

#if defined(__CW32__)
void * __alloca(size_t size);
#ifndef alloca
#define alloca(size) __alloca(size)
#endif
#endif

#if defined(__GNUC__)

#ifndef alloca
#define alloca(size) __builtin_alloca(size)
#endif

#if defined(TARGET_PALMOS)
extern int rand();
extern void srand(unsigned int);
extern void qsort(void* const base,size_t,size_t,int(*cmp)(const void*,const void*));
#endif

#if defined(ARM) && !defined(TARGET_WINCE)
//fixed size stack:
//  symbian
//  palm os
#define SWAPSP
static INLINE void* SwapSP(void* in)
{
	void* out;
	asm volatile(
		"mov %0, sp\n\t"
		"mov sp, %1\n\t"
		: "=&r"(out) : "r"(in) : "cc");
	return out;
}
#endif

#endif /* __GNUC__ */

#if defined(TARGET_PALMOS) && defined(IX86)
extern void* malloc_palmos(size_t);
extern void* realloc_palmos(void*,size_t);
extern void free_palmos(void*);
#define malloc(n) malloc_palmos(n)
#define realloc(p,n) realloc_palmos(p,n)
#define free(p) free_palmos(p)
#endif

#if defined(_MSC_VER) && defined(TARGET_WIN)
#define TRY_BEGIN __try {
#define TRY_END   ;} __except (1) {}
#define TRY_END_FUNC(func) ;} __except (1) { func; }
#else
#define TRY_BEGIN {
#define TRY_END   }
#define TRY_END_FUNC(func) }
#endif

#ifndef NDEBUG
#if defined(TARGET_OSX)
#include </usr/include/assert.h>
#elif !defined(TARGET_WINCE) && !defined(TARGET_PALMOS)
#include <assert.h>
#else
#ifdef LIBC_EXPORTS
#define ASSERT_DLL DLLEXPORT
#else
#define ASSERT_DLL
#endif
ASSERT_DLL void _Assert(const char* Exp, const char* File, int Line);
#define assert(x)   ((x) ? (void)0 : _Assert(#x, __FILE__, __LINE__))
#endif
#else // NDEBUG
#ifndef assert
#define assert(x)   ((void)0)
#endif
#endif // NDEBUG

#if defined(__palmos__)
#if _MSC_VER > 1000
#pragma warning( disable:4068 4204 )
#endif
#undef BIG_ENDIAN
#define USE_TRAPS 0
#endif

#if defined(COMPILER_GCC) && (!defined(TARGET_SYMBIAN) || defined(SYMBIAN90))
#define UNUSED_PARAM(x) (x) __attribute__ ((unused))
#elif (defined(TARGET_SYMBIAN) && !defined(ARM)) || defined(__cplusplus)
#define UNUSED_PARAM(x)
#else
#define UNUSED_PARAM(x) (x)
#endif

#include "config.h"
#if defined(COREMAKE_CONFIG_HELPER)
#include "config_helper.h"
#else /* COREMAKE_CONFIG_HELPER */
#include "confhelper.h"
#endif /* COREMAKE_CONFIG_HELPER */

#if defined(TARGET_IPHONE) && !defined(__ARM_NEON__)
#undef CONFIG_NEON
#endif

#ifdef CONFIG_FILEPOS_64
typedef int_fast64_t filepos_t;
#define MAX_FILEPOS MAX_INT64
#else
typedef int_fast32_t filepos_t;
#define MAX_FILEPOS INT_MAX
#endif

#define INVALID_FILEPOS_T  ((filepos_t)-1)

typedef int systick_t;
#define INVALID_SYSTICK_T  ((int)-1)

#if defined(CONFIG_SAFE_C)
// change the low level APIs to be safer on windows and other OSes
#include "banned.h"
#endif /* CONFIG_SAFE_C */

#endif
