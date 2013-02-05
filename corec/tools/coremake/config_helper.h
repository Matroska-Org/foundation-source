/*
  $Id: config_helper.h 25 2010-07-07 14:49:45Z robux4 $

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

#if defined(TARGET_WINCE) || defined(TARGET_SYMBIAN)
#undef COREMAKE_UNICODE
#define COREMAKE_UNICODE /* platforms where Unicode is mandatory */
#endif

#if (defined(TARGET_PALMOS) && defined(IX86)) || defined(TARGET_SYMBIAN)
#undef COREMAKE_STATIC /* platforms where dynamic libraries are not supported */
#define COREMAKE_STATIC
#endif

#if (defined(ARM) || defined(MIPS) || defined(SH3) || defined(SH4)) && !defined(TARGET_IPHONE_SDK)
#undef CONFIG_DYNCODE
#define CONFIG_DYNCODE /* platforms when dynamic code can be used */
#endif

#if defined(IX86) || defined(IX86_64)
#undef CONFIG_UNALIGNED_ACCESS
#define CONFIG_UNALIGNED_ACCESS /* pointers can use unaligned memory */
#endif

#if defined(TARGET_WIN32) || defined(TARGET_WIN64) || defined(TARGET_WINCE) || defined(TARGET_OSX) || defined(TARGET_LINUX)
#define CONFIG_FILEPOS_64 /* platforms where 64 bits file position/size should be favoured */
#endif

#if defined(TARGET_WINCE) || defined(TARGET_SYMBIAN) || defined(TARGET_PALMOS) || defined(ARM) || defined(MIPS)
#define CONFIG_FIXED_POINT /* platforms where fixed point arithmetic processing should favoured */
#endif

/* forbid some defines */

#if defined(TARGET_PALMOS) || defined(TARGET_LINUX) || defined(TARGET_PS2SDK)
#undef COREMAKE_UNICODE /* platforms where Unicode is handled via UTF-8 strings */
#endif

#if defined(TARGET_PALMOS)
#undef CONFIG_FILEPOS_64 /* platforms where 64 bits file position/size should not be used */
#endif

#if !defined(ARM) || defined(TARGET_SYMBIAN) || defined(TARGET_IPHONE)
#undef CONFIG_WMMX /* platforms that don't support Wireless MMX CPUs/instructions */
#endif

#if !defined(ARM) || defined(TARGET_SYMBIAN) || defined(TARGET_PALMOS)
#undef CONFIG_ARMV6 /* platforms that don't support ARMv6 CPUs/instructions */
#endif

#if !defined(IX86) || defined(TARGET_SYMBIAN)
#undef CONFIG_MMX /* platforms that don't support MMX CPUs/instructions */
#endif

#if !defined(ARM) || (!defined(TARGET_IPHONE) && !defined(TARGET_ANDROID))
#undef CONFIG_NEON /* platforms that don't support NEON/ARMv7 instructions */
#endif

#if !defined(POWERPC)
#undef CONFIG_ALTIVEC
#endif

#endif /* __CONFIG_HELPER_H */
