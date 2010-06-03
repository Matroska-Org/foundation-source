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

#if defined(TARGET_SYMBIAN)

#undef T
#undef NULL
#include <e32std.h>
#include <e32hal.h>

#ifdef SYMBIAN90
#include <tz.h>
#include <tzconverter.h>
#endif

#ifndef SYMBIAN90
static INLINE TInt64 ToInt64(int64_t i) { return TInt64((int32_t)(i>>32),(uint32_t)i); }
#else
#define ToInt64(a) (a)
#endif

int GetTimeFreq()
{ 
    TTimeIntervalMicroSeconds32 n;
	UserHal::TickPeriod(n);
    return Scale32(1000000,1,n.Int());
}

systick_t GetTimeTick()
{
	return User::TickCount();
}

datetime_t SymbianToDateTime(TTime Time)
{
    // reference is 1st January 2001 00:00:00.000 UTC
	datetime_t Date = INVALID_DATETIME_T;
    TTime Reference(ToInt64(LL(0x00e05776f452a000)));
    TTimeIntervalSeconds Diff;

    if (Time.SecondsFrom(Reference,Diff) == KErrNone)
	{
		Date = Diff.Int();
		if (Date==INVALID_DATETIME_T) ++Date;
	}

	return Date;
}

TTime DateTimeToSymbian(datetime_t Time)
{
  	// reference is 1st January 2001 00:00:00.000 UTC
    TTime Reference(ToInt64(LL(0x00e05776f452a000)));
    TTimeIntervalSeconds Diff;
    Diff = Time;
	return Reference + Diff;
}

datetime_t GetTimeDate()
{
    TTime Now;
    Now.UniversalTime();
    return SymbianToDateTime(Now);
}

datetime_t TimePackToRel(const datepack_t *tp, bool_t Local)
{
    TDateTime Date;
    TTime ot;
	if (!tp) return INVALID_DATETIME_T;
    Date.SetYear(tp->Year);
    Date.SetMonth((TMonth)(tp->Month-1));
    Date.SetDay(tp->Day-1);
    Date.SetHour(tp->Hour);
    Date.SetMinute(tp->Minute);
    Date.SetSecond(tp->Second);

    ot = TTime(Date);

    if (Local) 
    {
#ifndef SYMBIAN90
        TLocale locale;
        TTimeIntervalSeconds universalTimeOffset(locale.UniversalTimeOffset());
        ot -= universalTimeOffset;
        if (locale.QueryHomeHasDaylightSavingOn())
        {
            TTimeIntervalHours daylightSaving(1);
            ot -= daylightSaving;
        }
#else
        RTz TzServer;
        if (TzServer.Connect()==KErrNone)
        {
            CTzConverter* Converter = CTzConverter::NewL(TzServer); 
            Converter->ConvertToUniversalTime(ot);
            delete Converter;
            TzServer.Close();
        }
#endif
    }

    return SymbianToDateTime(ot);
}

bool_t GetDatePacked(datetime_t t, datepack_t *tp, bool_t Local)
{
	TDateTime Date;
    TTime ot;
	if (!tp || t == INVALID_DATETIME_T) return 0;
	
    ot = DateTimeToSymbian(t);

    if (Local) 
    {
#ifndef SYMBIAN90
        TLocale locale;
        TTimeIntervalSeconds universalTimeOffset(locale.UniversalTimeOffset());
        ot += universalTimeOffset;
        if (locale.QueryHomeHasDaylightSavingOn())
        {
            TTimeIntervalHours daylightSaving(1);
            ot += daylightSaving;
        }
#else
        RTz TzServer;
        if (TzServer.Connect()==KErrNone)
        {
            CTzConverter* Converter = CTzConverter::NewL(TzServer); 
            Converter->ConvertToLocalTime(ot);
            delete Converter;
            TzServer.Close();
        }
#endif
    }

	Date = ot.DateTime();
	tp->Year = Date.Year();
	tp->Month = (int)Date.Month() + 1;
	tp->Day = Date.Day()+1;
	tp->Hour = Date.Hour();
	tp->Minute = Date.Minute();
	tp->Second = Date.Second();
	return 1;
}

bool_t GetIsDst(datetime_t UNUSED_PARAM(t))
{
#ifndef SYMBIAN90
    TLocale locale;
    return locale.QueryHomeHasDaylightSavingOn();
#else
    TBool IsDst=EFalse;
    RTz TzServer;
    if (TzServer.Connect()==KErrNone)
    {
        CTzConverter* Converter = CTzConverter::NewL(TzServer); 
        CTzId* TzId = CTzId::NewL(Converter->CurrentTzId());
        IsDst = TzServer.IsDaylightSavingOnL(*TzId);
        delete TzId;
        delete Converter;
        TzServer.Close();
    }
    return IsDst;
#endif
}

#endif
