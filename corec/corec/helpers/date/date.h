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

#ifndef __DATE_H
#define __DATE_H

#include "corec/corec.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DATE_EXPORTS)
#define DATE_DLL DLLEXPORT
#elif defined(DATE_IMPORTS)
#define DATE_DLL DLLIMPORT
#else
#define DATE_DLL
#endif

DATE_DLL systick_t GetTimeTick(void);
#if defined(TARGET_WIN) || defined(TARGET_PS2SDK) || defined(TARGET_LINUX) || defined(TARGET_OSX) || defined(TARGET_ANDROID)
#define GetTimeFreq()  1000
#else
DATE_DLL int GetTimeFreq();
#endif

typedef struct datepack_t 
{
    intptr_t Year; // 2001, 2007, etc
    intptr_t Month; // 1-12
    intptr_t Day; // 1-31
    intptr_t Hour; // 24-hour
    intptr_t Minute; // 0-59
    intptr_t Second; // 0-59
    intptr_t WeekDay; // 1 = Sunday, 2 = Monday, etc
} datepack_t;

DATE_DLL datetime_t GetTimeDate(void); // UTC in s (reference is 1st January 2001 00:00:00.000 UTC, use a helper to get the localized string value)
DATE_DLL datetime_t TimePackToRel(const datepack_t *tp, bool_t FromLocal);
DATE_DLL bool_t GetDatePacked(datetime_t t, datepack_t *tp, bool_t ToLocal);
DATE_DLL bool_t GetIsDst(datetime_t t); // may not be correct on all platforms

#ifdef __cplusplus
}
#endif

#endif
