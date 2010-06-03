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

#include <sys/time.h>
#include <time.h>
#include <libcdvd.h>
#include <ps2timer.h>
#include <osd_config.h>

systick_t GetTimeTick()
{
    return EventTick;
} 

static int Bcd2Dec(int v)
{
    return ((v>>4) & 15)*10 + (v & 15); 
}

datetime_t GetTimeDate()
{
    datetime_t t;
    datepack_t tp;
	CdvdClock_t clock;
    if (!cdReadClock(&clock))
        return INVALID_DATETIME_T;

    tp.Year = Bcd2Dec(clock.year)+2000;
    tp.Month = Bcd2Dec(clock.month);
    tp.Day = Bcd2Dec(clock.day);
    tp.Hour = Bcd2Dec(clock.hour);
    tp.Minute = Bcd2Dec(clock.minute);
    tp.Second = Bcd2Dec(clock.second);
    tp.WeekDay = 0;

    t = TimePackToRel(&tp,0);
    if (t != INVALID_DATETIME_T)
    {
        t -= 9*60*60; //adjust JST to GMC
        if (t == INVALID_DATETIME_T)
            ++t;
    }
    return t;
}

static bool_t LeapYear(int v)
{
    return (v%4)==0 && ((v%100)!=0 || (v%400)==0);
}

datetime_t TimePackToRel(const datepack_t *tp, bool_t Local)
{
    static uint8_t Month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    datetime_t t;
    int a;

    if (tp->Year<1933 || tp->Year>=2069 ||
        tp->Month<1 || tp->Month>12 ||
        tp->Day<1 || tp->Day>31 ||
        tp->Hour<0 || tp->Hour>=24 ||
        tp->Minute<0 || tp->Minute>=60 ||
        tp->Second<0 || tp->Second>=60)
        return INVALID_DATETIME_T;

    t = -2145916800;
    
    t += (tp->Day-1)*24*60*60;
    t += tp->Hour*60*60;
    t += tp->Minute*60;
    t += tp->Second;

    for (a=1933;a<tp->Year;++a)
        t += 24*60*60*(LeapYear(a) ? 366:365);

	for (a=1;a<tp->Month;++a)
    {
        t += Month[a-1]*24*60*60;
        if (a==2 && LeapYear(tp->Year))
            t += 24*60*60;
    }

    if (Local)
    {
    	int Timezone = configGetTimezone();
	    int DaylightSaving = configIsDaylightSavingEnabled();
	    t -= (Timezone + DaylightSaving * 60)*60;
    }

	return t;
}

bool_t GetDatePacked(datetime_t t, datepack_t *tp, bool_t Local)
{
	static const uint16_t Days[12] = 
    {
		0,
		31,
		31+28,
		31+28+31,
		31+28+31+30,
		31+28+31+30+31,
		31+28+31+30+31+30,
		31+28+31+30+31+30+31,
		31+28+31+30+31+30+31+31,
		31+28+31+30+31+30+31+31+30,
		31+28+31+30+31+30+31+31+30+31,
		31+28+31+30+31+30+31+31+30+31+30,
	};

	unsigned int a;
	unsigned int b;
	unsigned int c;

	if (!tp || t == INVALID_DATETIME_T) return 0;

    if (t<-2145916800)
        return 0;

    if (Local)
    {
    	int Timezone = configGetTimezone();
	    int DaylightSaving = configIsDaylightSavingEnabled();
	    t += (Timezone + DaylightSaving * 60)*60;
    }

    a = t+2145916800; //2001 -> 1933

    b = a % (24*60*60);
	a = a / (24*60*60);

	tp->Second	= b %60; 
    b /= 60;
	tp->Minute	= b %60;
	tp->Hour	= b /60;

	tp->WeekDay = 1 + a % 7;

	for (b=1933;;++b)
    {
		c = LeapYear(b) ? 366:365;
		if (a >= c)
			a -= c;
		else
			break;
	}
	
	tp->Year = b;
	tp->Day = 1;

	if (LeapYear(b) && a>58) 
    {
		if (a==59)
		    tp->Day = 2;
		--a;
	}
	
    for (b = 11; b && (Days[b] > a); --b) {}

	tp->Month = b+1;
	tp->Day += a - Days[b];
	
    return 1;
}

bool_t GetIsDst(datetime_t t)
{
    //TODO: use t instead of current time
	return configIsDaylightSavingEnabled(); 
}
