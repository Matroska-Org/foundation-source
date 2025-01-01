/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include "date.h"

#define DATETIME_OFFSET     0x3A4FC880

#include <sys/time.h>
#include <time.h>

systick_t GetTimeTick(void)
{
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec*1000 + t.tv_usec/1000;
}

datetime_t LinuxToDateTime(time_t t)
{
	// reference is 1st January 2001 00:00:00.000 UTC
    t -= (time_t)DATETIME_OFFSET;
    if (t==INVALID_DATETIME_T) t++;
	return (datetime_t)t;
}

datetime_t GetTimeDate(void)
{
    return LinuxToDateTime(time(NULL));
}

bool_t GetDatePacked(datetime_t t, datepack_t *tp, bool_t Local)
{
	time_t ot;
	struct tm *date;

	if (!tp || t == INVALID_DATETIME_T)
        return 0;

	ot = t + DATETIME_OFFSET;
    if (Local)
        date = localtime(&ot);
    else
	    date = gmtime(&ot);

	if (date)
    {
		tp->Second = date->tm_sec;
		tp->Minute = date->tm_min;
		tp->Hour = date->tm_hour;
		tp->Day = date->tm_mday;
		tp->Month = date->tm_mon + 1;
		tp->Year = date->tm_year + 1900;
		tp->WeekDay = date->tm_wday + 1;
		return 1;
	}

	return 0;
}
