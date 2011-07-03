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

#include "date.h"

#if defined(TARGET_WIN)

//#define TARGET_WIN2K
// currently we dont use the TzSpecific (Win2K+) implementation at all

#ifndef STRICT
#define STRICT
#endif
#include <windows.h>

#define MIN_CACHED_YEAR 1990
#define MAX_CACHED_YEAR 2015

static TIME_ZONE_INFORMATION fix_tz;
static int fix_tz_ready; 
static FILETIME fTimeCache[MAX_CACHED_YEAR - MIN_CACHED_YEAR + 2][2];

#ifndef TIME_ZONE_ID_INVALID
#define TIME_ZONE_ID_INVALID ((DWORD)0xFFFFFFFF)
#endif

#if defined(TARGET_WINCE)
systick_t GetTimeTick()
{
	return GetTickCount();
}
#else
systick_t GetTimeTick()
{
	return timeGetTime();
}
#endif

datetime_t FileTimeToRel(FILETIME *fTime)
{
    int64_t int64time;

    int64time = fTime->dwHighDateTime;
    int64time <<= 32;
    int64time += fTime->dwLowDateTime;
    /* Convert from 100ns to seconds. */
    int64time /= 10000000;
    int64time -= LL(0x2F0605980); // reference is 1st January 2001 00:00:00.000 UTC
    if (int64time==INVALID_DATETIME_T) ++int64time;
    return (datetime_t)int64time;
}

void RelToFileTime(datetime_t t, FILETIME *fTime)
{
    int64_t int64time = t;

    int64time += LL(0x2F0605980); // reference is 1st January 2001 00:00:00.000 UTC
    /* Convert from seconds to 100ns. */
    int64time *= 10000000;
    fTime->dwLowDateTime = (DWORD)int64time;
    int64time >>= 32;
    fTime->dwHighDateTime = (DWORD)int64time;   
}

static void GetFixedTZ()
{
    if (fix_tz_ready == 0) {
        fix_tz_ready = GetTimeZoneInformation(&fix_tz) == TIME_ZONE_ID_INVALID? -1 : 1;
        if (fix_tz_ready == -1) { // default US rules
            fix_tz.DaylightDate.wYear = 0;
            fix_tz.DaylightDate.wHour = 2; // 2AM
            fix_tz.DaylightDate.wMinute = 0;
            fix_tz.DaylightDate.wSecond = 0;
            fix_tz.DaylightDate.wDayOfWeek = 0;
            // month and dayofweek are only set when year is known in getisdst()
            memcpy(&fix_tz.StandardDate, &fix_tz.DaylightDate, sizeof(SYSTEMTIME));
            fix_tz.DaylightBias = -60;
            fix_tz.Bias = 0; // default timezone?
        }
    }
}

/*
static int CompareFileTime(FILETIME *fTime1, FILETIME *fTime2)
{
    if (fTime1->dwHighDateTime > fTime2->dwHighDateTime)
        return 1;
    else if (fTime1->dwHighDateTime > fTime2->dwHighDateTime)
        return -1;
    else if (fTime1->dwLowDateTime > fTime2->dwLowDateTime)
        return 1;
    else if (fTime1->dwLowDateTime < fTime2->dwLowDateTime)
        return -1;
    return 0;
}*/

static void ConvertDaylightDateToFileTime(int Year, SYSTEMTIME *Date, FILETIME *fTime)
{
    SYSTEMTIME TimeStruct = {0};
    int Day, Leap;
    int MonthDays[2][12] = { {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                             {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} };

    TimeStruct.wYear = (WORD) Year;
    TimeStruct.wMonth = Date->wMonth;
    TimeStruct.wHour = Date->wHour;
    TimeStruct.wMinute = Date->wMinute;
    TimeStruct.wSecond = Date->wSecond;
    if (Date->wYear > 0)
        TimeStruct.wDay = Date->wDay;
    else {
        TimeStruct.wDay = 1;
        SystemTimeToFileTime(&TimeStruct, fTime);
        FileTimeToSystemTime(fTime, &TimeStruct); // to calculate day of week
        Day = 1 + Date->wDayOfWeek - TimeStruct.wDayOfWeek;
        if (Day < 0)
            Day += 7;
        Day += (Date->wDay - 1) * 7;
        Leap = (Year % 4 == 0 && Year % 100 != 0) || (Year % 400 == 0);
        if (Date->wDay == 5 && Day > MonthDays[Leap][Date->wMonth])
            Day -= 7;
        TimeStruct.wDay = (WORD) Day;
    }
    SystemTimeToFileTime(&TimeStruct, fTime);
}

datetime_t GetTimeDate()
{
    SYSTEMTIME sysTimeStruct;
    FILETIME fTime;

    GetSystemTime( &sysTimeStruct );
    if ( SystemTimeToFileTime( &sysTimeStruct, &fTime ) )
        return FileTimeToRel(&fTime);

    return INVALID_DATETIME_T;
}

datetime_t TimePackToRel(const datepack_t *tp, bool_t Local)
{
    SYSTEMTIME TimeStruct = {0};
    FILETIME fTime;
    datetime_t t;
    
    if (!tp) 
        return INVALID_DATETIME_T;

    TimeStruct.wMilliseconds = (WORD)0;
    TimeStruct.wSecond = (WORD)tp->Second;
    TimeStruct.wMinute = (WORD)tp->Minute;
    TimeStruct.wHour   = (WORD)tp->Hour;
    TimeStruct.wDay    = (WORD)tp->Day;
    TimeStruct.wMonth  = (WORD)tp->Month;
    TimeStruct.wYear   = (WORD)tp->Year;
	TimeStruct.wDayOfWeek = (WORD)(tp->WeekDay ? tp->WeekDay-1:0);

#ifdef TARGET_WIN2K
    if (Local) 
    {
        SYSTEMTIME sysTimeStruct;
        TIME_ZONE_INFORMATION timeZoneInfo;
        GetTimeZoneInformation(&timeZoneInfo);
        if (!TzSpecificLocalTimeToSystemTime(&timeZoneInfo, &TimeStruct, &sysTimeStruct))
            return INVALID_DATETIME_T;
        if (!SystemTimeToFileTime( &sysTimeStruct, &fTime )) 
            return INVALID_DATETIME_T;
    }
    else 
#endif
        if (!SystemTimeToFileTime( &TimeStruct, &fTime )) 
            return INVALID_DATETIME_T;

    t = FileTimeToRel(&fTime);

#ifndef TARGET_WIN2K
    if (Local) {
        GetFixedTZ();
        t += fix_tz.Bias * 60;
        if (GetIsDst(t)) // test with UTC time without daylight (not perfect at the edges)
            t += fix_tz.DaylightBias * 60;
        else
            t += fix_tz.StandardBias * 60;
        if (t==INVALID_DATETIME_T) 
            ++t;
    }
#endif

    return t;
}

bool_t GetDatePacked(datetime_t t, datepack_t *tp, bool_t Local)
{
    SYSTEMTIME TimeStruct = {0};
    FILETIME fTime = {0};
    
    if (!tp || t == INVALID_DATETIME_T) 
        return 0;

#ifndef TARGET_WIN2K
    if (Local) {
        GetFixedTZ();
        if (GetIsDst(t))
            t -= fix_tz.DaylightBias * 60;
        else
            t -= fix_tz.StandardBias * 60;
        t -= fix_tz.Bias * 60;
    }
#endif

    RelToFileTime(t, &fTime);

#ifdef TARGET_WIN2K
    if (Local) 
    {
        SYSTEMTIME sysTimeStruct = {0};
        TIME_ZONE_INFORMATION timeZoneInfo;
        if (!FileTimeToSystemTime(&fTime, &sysTimeStruct)) 
            return 0;
        GetTimeZoneInformation(&timeZoneInfo);
        SystemTimeToTzSpecificLocalTime(&timeZoneInfo, &sysTimeStruct, &TimeStruct);
    }
    else 
#endif        
        if (!FileTimeToSystemTime(&fTime, &TimeStruct)) 
            return 0;

    tp->Year = TimeStruct.wYear;
    tp->Month = TimeStruct.wMonth;
    tp->Day = TimeStruct.wDay;
    tp->Hour = TimeStruct.wHour;
    tp->Minute = TimeStruct.wMinute;
    tp->Second = TimeStruct.wSecond;
    tp->WeekDay = TimeStruct.wDayOfWeek+1;
    return 1;
}

bool_t GetIsDst(datetime_t t)
{
#ifdef TARGET_WIN2K
    FILETIME fTime = {0};
    TIME_ZONE_INFORMATION timeZoneInfo;
    SYSTEMTIME sysTimeStruct = {0};
    SYSTEMTIME TimeStruct = {0};
    int Hour;
	
    if (t == INVALID_DATETIME_T) 
        return 0;

    RelToFileTime(t, &fTime);
    if (!FileTimeToSystemTime(&fTime, &sysTimeStruct)) 
        return 0;
    GetTimeZoneInformation(&timeZoneInfo);
    if (!SystemTimeToTzSpecificLocalTime(&timeZoneInfo, &sysTimeStruct, &TimeStruct))
        return 0;
    Hour = TimeStruct.wHour;
    timeZoneInfo.DaylightBias = 0;
    timeZoneInfo.DaylightDate.wMonth = 0;
    timeZoneInfo.StandardDate.wMonth = 0;
    if (!SystemTimeToTzSpecificLocalTime(&timeZoneInfo, &sysTimeStruct, &TimeStruct))
        return 0;
    return Hour != TimeStruct.wHour;
#else
    FILETIME fTime;
    FILETIME fTime1, fTime2;
    FILETIME *fTimeStart, *fTimeEnd;
    int Year;
    int YearIndex;
    SYSTEMTIME TimeStruct = {0};
    if (t == INVALID_DATETIME_T) 
        return 0;

    GetFixedTZ();
    if (fix_tz.DaylightBias == 0 || fix_tz.DaylightDate.wMonth == 0)
        return 0;

    t -= fix_tz.Bias * 60; // we need local (biased) time

    RelToFileTime(t, &fTime);
    if (!FileTimeToSystemTime(&fTime, &TimeStruct)) 
        return 0;
    Year = TimeStruct.wYear;

    if (fix_tz_ready == -1) { // default US rules
        if (Year >= 2007) {
            fix_tz.DaylightDate.wMonth = 3;
            fix_tz.DaylightDate.wDayOfWeek = 2;
            fix_tz.StandardDate.wMonth = 11;
            fix_tz.StandardDate.wDayOfWeek = 1;
        } else {
            fix_tz.DaylightDate.wMonth = 4;
            fix_tz.DaylightDate.wDayOfWeek = 1;
            fix_tz.StandardDate.wMonth = 10;
            fix_tz.StandardDate.wDayOfWeek = 5;
        }
    }

    if (Year < MIN_CACHED_YEAR || Year > MAX_CACHED_YEAR ) {
        YearIndex = 0;
        fTimeStart = &fTime1;
        fTimeEnd = &fTime2;
    }
    else {
        YearIndex = Year - MIN_CACHED_YEAR + 1;  
        fTimeStart = &(fTimeCache[YearIndex][0]);
        fTimeEnd = &(fTimeCache[YearIndex][1]);
    }
        
    if (YearIndex == 0 || (fTimeStart->dwLowDateTime == 0 && 
        fTimeStart->dwHighDateTime == 0)) {
        ConvertDaylightDateToFileTime(Year, &fix_tz.DaylightDate, fTimeStart);
        ConvertDaylightDateToFileTime(Year, &fix_tz.StandardDate, fTimeEnd);
    }

    if (CompareFileTime(&fTime, fTimeStart) < 0 || 
        CompareFileTime(&fTime, fTimeEnd) >= 0)
        return 0;

    return 1;
#endif
}

#endif
