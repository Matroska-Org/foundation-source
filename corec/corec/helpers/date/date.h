/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __DATE_H
#define __DATE_H

#include <corec/corec.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(corec_EXPORTS)
#define DATE_DLL DLLEXPORT
#elif defined(DATE_IMPORTS)
#define DATE_DLL DLLIMPORT
#else
#define DATE_DLL
#endif

DATE_DLL systick_t GetTimeTick(void);
#if defined(TARGET_WIN) || defined(TARGET_LINUX) || defined(TARGET_OSX) || defined(TARGET_ANDROID)
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
DATE_DLL bool_t GetDatePacked(datetime_t t, datepack_t *tp, bool_t ToLocal);

#ifdef __cplusplus
}
#endif

#endif
