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

#if defined(TARGET_PALMOS)

#include <PalmOS.h>

int GetTimeFreq() 
{ 
	return SysTicksPerSecond();
}

systick_t GetTimeTick()
{
	return TimGetTicks();
}

datetime_t PalmToDateTime(UInt32 v)
{
	// reference is 1st January 2001 00:00:00.000 UTC
    datetime_t Date = (datetime_t)(v - 0xB6757900);
    if (Date==INVALID_DATETIME_T) ++Date;
    return Date;
}

UInt32 DateTimeToPalm(datetime_t v)
{
	// reference is 1st January 2001 00:00:00.000 UTC
	return (UInt32)(v + 0xB6757900);
}

datetime_t GetTimeDate()
{
    return PalmToDateTime(TimGetSeconds());
}

datetime_t TimePackToRel(const datepack_t *tp, bool_t Local)
{
	DateTimeType Date;
    UInt32 ot;
	if (!tp) return INVALID_DATETIME_T;
	Date.second = (int16_t)tp->Second;
	Date.minute = (int16_t)tp->Minute;
	Date.hour = (int16_t)tp->Hour;
	Date.day = (int16_t)tp->Day;
	Date.month = (int16_t)tp->Month;
	Date.year = (int16_t)tp->Year;
	Date.weekDay = (int16_t)(tp->WeekDay-1);

    ot = TimDateTimeToSeconds(&Date);

    if (Local)
        ot = TimTimeZoneToUTC(ot, PrefGetPreference(prefTimeZone), 
            PrefGetPreference(prefDaylightSavingAdjustment));

	return PalmToDateTime(ot);
}

bool_t GetDatePacked(datetime_t t, datepack_t *tp, bool_t Local)
{
	DateTimeType Date;
    UInt32 ot;
	if (!tp || t == INVALID_DATETIME_T) return 0;
	memset(&Date, 0, sizeof(DateTimeType));
	
    ot = DateTimeToPalm(t);
    if (Local)
        ot = TimUTCToTimeZone(ot, PrefGetPreference(prefTimeZone), 
            PrefGetPreference(prefDaylightSavingAdjustment));

	TimSecondsToDateTime(ot, &Date);
	tp->Second = Date.second;
	tp->Minute = Date.minute;
	tp->Hour = Date.hour;
	tp->Day = Date.day;
	tp->Month = Date.month;
	tp->Year = Date.year;
	tp->WeekDay = Date.weekDay;
	return 1;
}

bool_t GetIsDst(datetime_t t)
{
    UInt32 ot = DateTimeToPalm(t);
    return TimUTCToTimeZone(ot, PrefGetPreference(prefTimeZone), 0) !=
           TimUTCToTimeZone(ot, PrefGetPreference(prefTimeZone), 
                PrefGetPreference(prefDaylightSavingAdjustment));
}

#endif
