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

#include "parser.h"

fourcc_t StringToFourCC(const tchar_t* In, bool_t Upper)
{
	tchar_t s[4+1];
	size_t i;

	if (!In[0])
		return 0;

	tcscpy_s(s,TSIZEOF(s),In);

	if (Upper)
		tcsupr(s);

	for (i=1;i<4;++i)
		if (!s[i])
			for (;i<4;++i)
				s[i] = '_';

	return FOURCC((uint8_t)s[0],(uint8_t)s[1],(uint8_t)s[2],(uint8_t)s[3]);
}

size_t FourCCToString(tchar_t* Out, size_t OutLen, fourcc_t FourCC)
{
    size_t i=0;
    if (OutLen)
    {
	    union
	    {
		    fourcc_t d;
		    uint8_t a[4];
	    } s;

	    s.d = FourCC;
	    for (i=0;i<4 && i<OutLen-1;++i)
            Out[i] = s.a[i];

        while (i>0 && Out[i-1]=='_')
            --i;

        Out[i] = 0;
    }
    return i;
}

void GUIDToString(tchar_t* Out, size_t OutLen, const cc_guid* p)
{
	stprintf_s(Out,OutLen,T("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
		(int)p->v1,p->v2,p->v3,p->v4[0],p->v4[1],p->v4[2],p->v4[3],
		p->v4[4],p->v4[5],p->v4[6],p->v4[7]);
}

bool_t StringToGUID(const tchar_t* In, cc_guid* p)
{
	int i,v[10];
	if (In[0]=='{') ++In;
	if (stscanf(In,T("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
			&p->v1,v+0,v+1,v+2,v+3,v+4,v+5,v+6,v+7,v+8,v+9) < 11)
	{
		memset(p,0,sizeof(cc_guid));
		return 0;
	}
	p->v2 = (uint16_t)v[0];
	p->v3 = (uint16_t)v[1];
	for (i=0;i<8;++i)
		p->v4[i] = (uint8_t)v[2+i];
	return 1;
}

// gcc 2.97 bug...
static const tchar_t mask_d[] = T("%d");
static const tchar_t mask_dd[] = T("%d%d");
static const tchar_t mask_X[] = T("%X");
static const tchar_t mask_02X[] = T("%02X");
static const tchar_t mask_0x08X[] = T("0x%08X");
static const tchar_t mask_0x08Xx08X[] = T("0x%08X%08X");
static const tchar_t mask_rgb[] = T("#%08X");

void FractionToString(tchar_t* Out, size_t OutLen, const cc_fraction* p, int Percent, int Decimal)
{
	int a,b,i;
	int_fast32_t Num = p->Num;
	int_fast32_t Den = p->Den;

	if (Percent)
	{
		while (_abs(Num) > INT_MAX/100)
		{
			Num >>= 1;
			Den >>= 1;
		}
		Num *= 100;
	}

	if (Den)
	{
		if (Den<0)
		{
			Num = -Num;
			Den = -Den;
		}
		for (i=0,b=1;i<Decimal;++i,b*=10) {}
		if (Num>0)
		{
			// rounding
			int_fast32_t r = Den/(2*b);
			if (Num < INT_MAX-r)
				Num += r;
			else
				Num = INT_MAX;
		}
		a=(int)(Num/Den);
		Num -= a*Den;
		b=(int)(((int64_t)Num*b)/Den);
	}
	else
		a=b=0;

	if (Decimal)
		stprintf_s(Out,OutLen,T("%d.%0*d"),a,Decimal,b);
	else
		stprintf_s(Out,OutLen,mask_d,a);

	if (Percent>0)
		tcscat_s(Out,OutLen,T("%"));
}

void StringToFraction(const tchar_t* In, cc_fraction* Out, bool_t Percent)
{
    ExprSkipSpace(&In);
    if (!ExprIsFrac(&In,Out))
    {
        Out->Num=0;
        Out->Den=0;
    }
    if (Percent)
        Out->Den *= 100;
}

int StringToInt(const tchar_t* In, int Hex)
{
	int v=0;
	if (Hex<0)
	{
        ExprSkipSpace(&In);
		Hex = In[0]=='0' && In[1]=='x';
		if (Hex) In+=2;
	}
	stscanf(In,Hex ? mask_X:mask_d,&v);
	return v;
}

int64_t StringToInt64(const tchar_t* In)
{
	int hi=0,lo=0;
	stscanf(In,T("0x%8x%8x"),&hi,&lo);
	return (((uint64_t)hi)<<32)+lo;
}

void Int64ToString(tchar_t* Out, size_t OutLen, int64_t p, bool_t Hex)
{
    if (!Hex && (p & 0xFFFFFFFF)==p)
        stprintf_s(Out,OutLen,mask_d,(int32_t)p);
    else
	    stprintf_s(Out,OutLen,Hex ? mask_0x08Xx08X:mask_dd,(uint32_t)(((uint64_t)p)>>32),(uint32_t)p);
}

void IntToString(tchar_t* Out, size_t OutLen, int32_t p, bool_t Hex)
{
	stprintf_s(Out,OutLen,Hex ? mask_0x08X:mask_d,p);
}

void RGBToString(tchar_t* Out, size_t OutLen, rgbval_t RGB)
{
	stprintf_s(Out,OutLen,mask_rgb,(int)INT32BE(RGB));
    if (tcslen(Out)>=1+8 && Out[7]=='0' && Out[8]=='0')
        Out[7] = 0;
}

rgbval_t StringToRGB(const tchar_t* In)
{
    int v;
    if (*In=='#') ++In;
    v = StringToInt(In,1);
    if (tcslen(In)<=6)
        v <<= 8;
    return INT32BE(v);
}

tick_t StringToTick(const tchar_t* In)
{
	// hour:min:sec.msec

	bool_t Sign = 0;
	tick_t Tick = 0;
	int Hour,Min,Sec;
	int n;

    if (*In=='-') 
	{ 
		Sign = 1; 
		++In; 
	}
	else
    if (*In=='+') 
		++In;
        
    n = stscanf(In,T("%d:%d:%d"),&Hour,&Min,&Sec);
	if (n>0)
	{
		Tick = Hour;
		if (n>1)
		{
			Tick *= 60;
			Tick += Min;
			if (n>2)
			{
				Tick *= 60;
				Tick += Sec;
			}
		}
		Tick *= TICKSPERSEC;
	}

	In = tcschr(In,T('.'));
	if (In)
	{
		int64_t Num = 0;
        int64_t Den = 1;
        ++In;
        for (;IsDigit(*In);++In)
        {
		    Num = Num*10 + (*In-'0');
            Den *= 10;
        }
		Tick += (tick_t)((Num*TICKSPERSEC+Den/2)/Den);
	}

	if (Sign)
		Tick = -Tick;
	
	return Tick;
}

systick_t StringToSysTick(const tchar_t* In)
{
	// hour:min:sec.msec

	bool_t Sign = 0;
	systick_t Tick = 0;
	int Hour,Min,Sec;
	int n;

    if (*In=='-') 
	{ 
		Sign = 1; 
		++In; 
	}
	else
    if (*In=='+') 
		++In;
        
    n = stscanf(In,T("%d:%d:%d"),&Hour,&Min,&Sec);
	if (n>0)
	{
		Tick = Hour;
		if (n>1)
		{
			Tick *= 60;
			Tick += Min;
			if (n>2)
			{
				Tick *= 60;
				Tick += Sec;
			}
		}
		Tick *= GetTimeFreq();
	}

	In = tcschr(In,T('.'));
	if (In)
	{
		int64_t Num = 0;
        int64_t Den = 1;
        ++In;
        for (;IsDigit(*In);++In)
        {
		    Num = Num*10 + (*In-'0');
            Den *= 10;
        }
		Tick += (systick_t)((Num*GetTimeFreq()+Den/2)/Den);
	}

	if (Sign)
		Tick = -Tick;
	
	return Tick;
}

void TickToString(tchar_t* Out, size_t OutLen, tick_t Tick, bool_t MS, bool_t Extended, bool_t Fix)
{
	tchar_t Sign[2] = {0};
	if (Tick<0)
	{
		Sign[0] = '-';
		Tick = -Tick;
	}
	if (!MS)
	{
		int Hour,Min,Sec;
		Tick += TICKSPERSEC/2000;
		Hour = (int)(Tick / 3600 / TICKSPERSEC);
		Tick -= Hour * 3600 * TICKSPERSEC;
		Min = (int)(Tick / 60 / TICKSPERSEC);
		Tick -= Min * 60 * TICKSPERSEC;
		Sec = (int)(Tick / TICKSPERSEC);
		Tick -= Sec * TICKSPERSEC;
        if (!Hour && !Min && !Fix && Extended)
    		stprintf_s(Out,OutLen,T("%s%d"),Sign,Sec);
        else
        {
		    if (Hour)
			    stprintf_s(Out,OutLen,T("%s%d:%02d"),Sign,Hour,Min);
		    else
			    stprintf_s(Out,OutLen,Fix?T("%s%02d"):T("%s%d"),Sign,Min);
		    stcatprintf_s(Out,OutLen,T(":%02d"),Sec);
        }
		if (Extended)
			stcatprintf_s(Out,OutLen,T(".%03d"),(int)((Tick*1000)/TICKSPERSEC));
	}
	else
	{
		int i = Scale32(Tick,100000,TICKSPERSEC);
        stprintf_s(Out,OutLen,T("%s%d.%02d%s"),Sign,i/100,i%100,Extended?T(" ms"):T(""));
	}
}

void SysTickToString(tchar_t* Out, size_t OutLen, systick_t Tick, bool_t MS, bool_t Extended, bool_t Fix)
{
	tchar_t Sign[2] = {0};
	if (Tick<0)
	{
		Sign[0] = '-';
		Tick = -Tick;
	}
	if (!MS)
	{
		int Hour,Min,Sec;
		//Tick += GetTimeFreq()/2000;
		Hour = (int)(Tick / 3600 / GetTimeFreq());
		Tick -= Hour * 3600 * GetTimeFreq();
		Min = (int)(Tick / 60 / GetTimeFreq());
		Tick -= Min * 60 * GetTimeFreq();
		Sec = (int)(Tick / GetTimeFreq());
		Tick -= Sec * GetTimeFreq();
        if (!Hour && !Min && !Fix && Extended)
    		stprintf_s(Out,OutLen,T("%s%d"),Sign,Sec);
        else
        {
		    if (Hour)
			    stprintf_s(Out,OutLen,T("%s%d:%02d"),Sign,Hour,Min);
		    else
			    stprintf_s(Out,OutLen,Fix?T("%s%02d"):T("%s%d"),Sign,Min);
		    stcatprintf_s(Out,OutLen,T(":%02d"),Sec);
        }
		if (Extended)
			stcatprintf_s(Out,OutLen,T(".%03d"),(int)((Tick*1000)/GetTimeFreq()));
	}
	else
	{
		int i = Scale32(Tick,1000,GetTimeFreq());
		stprintf_s(Out,OutLen,T("%s%d%s"),Sign,i,Extended?T(" ms"):T(""));
	}
}

void URLToString(tchar_t* Title, size_t TitleLen, const tchar_t* URL)
{
	const tchar_t *i;
	tchar_t *j;
    assert(TitleLen>0);
	// replace %20 and '_' with space
	for (j=Title,i=URL;*i && TitleLen>1;++i,--TitleLen)
	{
		if (*i=='_')
			*j++ = ' ';
		else
		if (i[0]=='%' && Hex(i[1])>=0 && Hex(i[2])>=0)
		{
			*j++ = (tchar_t)((Hex(i[1])<<4) + Hex(i[2]));
			i += 2;
		}
		else
			*j++ = *i;
	}
	*j=0;
}

static bool_t IsUrlSafe(tchar_t i)
{
    return (i == T('$') || i == T('-') || i == T('_') || i == T('.') || i == T('+'));
}

void StringToURL(anynode* AnyNode, tchar_t* Out, size_t OutLen, const tchar_t *URL)
{
    size_t Utf8Len = max(2*tcslen(URL),OutLen); // try to estimate
    char *Utf8 = malloc(Utf8Len);
    assert(OutLen>0);
    if (Utf8)
    {
	    const char *i;
	    Node_ToUTF8(AnyNode,Utf8,Utf8Len,URL);
	    for (i=Utf8;*i && OutLen>1;++i)
	    {
		    if (IsDigit(*i) || IsAlpha(*i) || IsUrlSafe(*i))
		    {
			    OutLen--;
			    *Out++ = *i;
		    }
		    else
            if (OutLen>3)
		    {
			    OutLen--;
			    *Out++ = T('%');
			    stprintf_s(Out,OutLen,mask_02X,*i & 0xFF);
                OutLen -= tcslen(Out);
			    Out += tcslen(Out);
		    }
	    }
        free(Utf8);
    }
	*Out = 0;
}

void LangToIso639_1(tchar_t *Out, size_t OutLen, fourcc_t Lang)
{
	FourCCToString(Out,OutLen,Lang);
    tcslwr(Out);
}

void ByteRateToString(tchar_t* Out, size_t OutLen, int ByteRate)
{
    int KB = Scale32(ByteRate,8,1000);
	if (KB>=1000)
	{
        cc_fraction f;
        f.Num = KB;
        f.Den = 1000;
		FractionToString(Out,OutLen,&f,0,2);
		tcscat_s(Out,OutLen,T(" Mbit/s"));
	}
	else
	{
		IntToString(Out,OutLen,KB,0);
		tcscat_s(Out,OutLen,T(" kbit/s"));
	}
}

datetime_t RFC822ToRel(const tchar_t *Date)
{
    datetime_t Result;
	datepack_t ResultPacked = {0};
    intptr_t Offset=0;
    const tchar_t *s = tcschr(Date,T(','));
    const tchar_t* const Token[12] = { T("Jan"), T("Feb"), T("Mar"), T("Apr"), T("May"), 
        T("Jun"), T("Jul"), T("Aug"), T("Sep"), T("Oct"), T("Nov"), T("Dec") };
    const tchar_t* const TokenLong[12] = { T("January"), T("February"), T("March"), T("April"), T("May"), 
        T("June"), T("July"), T("August"), T("September"), T("October"), T("November"), T("December") };

    if (s)
        s++;
    else
        s = Date;
    ExprSkipSpace(&s);
    if (!ExprIsTokenEx(&s,T("%d "),&ResultPacked.Day))
        return INVALID_DATETIME_T;

    ExprSkipSpace(&s);
    for (ResultPacked.Month=0;ResultPacked.Month<12;++ResultPacked.Month)
        if (ExprIsToken(&s,Token[ResultPacked.Month]))
            break;
    if (ResultPacked.Month==12)
		for (ResultPacked.Month=0;ResultPacked.Month<12;++ResultPacked.Month)
			if (ExprIsToken(&s,TokenLong[ResultPacked.Month]))
				break;
    if (ResultPacked.Month==12)
        return INVALID_DATETIME_T;
    ResultPacked.Month++;

    ExprSkipSpace(&s);
    if (!ExprIsTokenEx(&s,T("%d %d:%d:%d"),&ResultPacked.Year,&ResultPacked.Hour,&ResultPacked.Minute,&ResultPacked.Second)
        && !ExprIsTokenEx(&s,T("%d %d:%d"),&ResultPacked.Year,&ResultPacked.Hour,&ResultPacked.Minute))
        return INVALID_DATETIME_T;
    if (ResultPacked.Year < 100)
        ResultPacked.Year += 2000;

    ExprSkipSpace(&s);
    if (ExprIsTokenEx(&s,T("+%d"),&Offset))
        Offset = -Offset;
    else if (ExprIsTokenEx(&s,T("-%d"),&Offset))
    {}
    else if (ExprIsToken(&s,T("EDT")))
        Offset = 400;
    else if (ExprIsToken(&s,T("EST")))
        Offset = 500;
    else if (ExprIsToken(&s,T("CST")))
        Offset = 600;
    else if (ExprIsToken(&s,T("CDT")))
        Offset = 500;
    else if (ExprIsToken(&s,T("MST")))
        Offset = 600;
    else if (ExprIsToken(&s,T("MDT")))
        Offset = 500;
    else if (ExprIsToken(&s,T("PST")))
        Offset = 600;
    else if (ExprIsToken(&s,T("PDT")))
        Offset = 500;

	
	Result = TimePackToRel(&ResultPacked,0);

	if (Result != INVALID_DATETIME_T)
		Result += (datetime_t)(((Offset/100)*60)+(Offset%100))*60;

    return Result;
}

datetime_t ISO8601ToRel(const tchar_t *InDate)
{
    tchar_t Time[32],Date[32];
    datetime_t Result;
	datepack_t ResultPacked = {0};
    intptr_t Offset=0;
    const tchar_t *s;
    tchar_t *t;
    
    s = InDate;
    ExprSkipSpace(&s);
    tcscpy_s(Date,TSIZEOF(Date),s);
    s = Date;

    // split the time and date parts
    Time[0] = 0;
    t = tcsrchr(s,T('T'));
    if (!t)
        t = tcsrchr(s,T(' '));
    if (t)
    {
        tcscpy_s(Time,TSIZEOF(Time),t+1);
        t[0] = 0;
    }

    if (!*Time)
    {
        t = tcsrchr(s,T('Z'));
        if (t && t[1] == 0)
        {
            // assume HHMMSS for the time
            if (tcslen(Date)>=13)
            {
                tcscpy_s(Time,TSIZEOF(Time),max(t-6,s));
                Time[6] = 0;
                t[-6]=0;
            }
        }
    }

    if (!ExprIsTokenEx(&s,T("%d-%d-%d"),&ResultPacked.Year,&ResultPacked.Month,&ResultPacked.Day) && 
        !ExprIsTokenEx(&s,T("%d:%d:%d"),&ResultPacked.Year,&ResultPacked.Month,&ResultPacked.Day) &&
        !ExprIsTokenEx(&s,T("%4d%2d%2d"),&ResultPacked.Year,&ResultPacked.Month,&ResultPacked.Day) && 
        !ExprIsTokenEx(&s,T("%2d%2d%2d"),&ResultPacked.Year,&ResultPacked.Month,&ResultPacked.Day))
        return INVALID_DATETIME_T;

    if (ResultPacked.Year < 50)
        ResultPacked.Year += 2000;
    else if (ResultPacked.Year < 100)
        ResultPacked.Year += 1900;

    if (*Time)
    {
        s = Time;
        if (ExprIsTokenEx(&s,T("%d:%d:%d"),&ResultPacked.Hour,&ResultPacked.Minute,&ResultPacked.Second) ||
            ExprIsTokenEx(&s,T("%2d%2d%2d"),&ResultPacked.Hour,&ResultPacked.Minute,&ResultPacked.Second))
        {
            intptr_t MilliSeconds;
            ExprIsTokenEx(&s,T(".%d"),&MilliSeconds);
            if (s[0])
            {
                const tchar_t *o=s + 1;
                intptr_t OffsetH,OffsetM=0;
                if (ExprIsTokenEx(&o,T("%d:%d"),&OffsetH,&OffsetM) ||
                    ExprIsTokenEx(&o,T("%2d%2d"),&OffsetH,&OffsetM) || 
                    ExprIsTokenEx(&o,T("%2d"),&OffsetH))
                {
                    Offset = (OffsetH*60 + OffsetM)*60;
                }
                if (s[0] == T('+'))
                    Offset = -Offset;
            }
        }
    }

	Result = TimePackToRel(&ResultPacked,0);

	if (Result != INVALID_DATETIME_T)
		Result += (datetime_t)Offset;

    return Result;
}

#define BUFLEN 30

static void ZPad(tchar_t* Out, size_t Len, intptr_t v)
{
    Out[Len] = '\0';
    while (Len--) {
        Out[Len] = (tchar_t) ('0' + v % 10);
        v = v / 10;
    }
}

size_t StrFTime(tchar_t* Out, size_t OutLen, const tchar_t *Format, datepack_t *dp)
{
    const tchar_t* const Month[12] = { T("Jan"), T("Feb"), T("Mar"), T("Apr"), T("May"), 
        T("Jun"), T("Jul"), T("Aug"), T("Sep"), T("Oct"), T("Nov"), T("Dec") };
    const tchar_t* const MonthLong[12] = { T("January"), T("February"), T("March"), T("April"), T("May"), 
        T("June"), T("July"), T("August"), T("September"), T("October"), T("November"), T("December") };
    const tchar_t* const WeekDay[7] = { T("Sun"), T("Mon"), T("Tue"), T("Wed"), T("Thu"), T("Fri"), T("Sat") };
    const tchar_t* const WeekDayLong[7] = { T("Sunday"), T("Monday"), T("Tuesday"), T("Wednesday"), T("Thursday"), 
        T("Friday"), T("Saturday") };
    const tchar_t* const AMPM[2] = { T("am"), T("pm") };
    const tchar_t* const DateTimeFormat[3] = { T("%y/%m/%d"), T("%H:%M:%S"), T("%a %b %d %H:%M:%S %Y") };

    tchar_t buf[BUFLEN];
    tchar_t *p, *q;
    const tchar_t *r;

    p = Out;
    q = Out + OutLen - 1;
    while ((*Format != '\0'))
    {
        if (*Format++ == '%')
        {
              r = buf;
              switch (*Format++)
              {
              case '%' :
                    buf[0] = '%';
                    buf[1] = '\0';
                    break;
              case 'a' :
                    r = WeekDay[dp->WeekDay-1];
                    break;
              case 'A' :
                    r = WeekDayLong[dp->WeekDay-1];
                    break;
              case 'b' :
                    r = Month[dp->Month-1];
                    break;
              case 'B' :
                    r = MonthLong[dp->Month-1];
                    break;
              case 'c' :
                    StrFTime(buf, BUFLEN, DateTimeFormat[2], dp);
                    break;
              case 'd' :
                    ZPad(buf, 2, dp->Day);
                    break;
              case 'H' :
                    ZPad(buf, 2, dp->Hour);
                    break;
              case 'I' :
                    ZPad(buf, 2, dp->Hour==12 ? 12 : dp->Hour%12);
                    break;
              case 'm' :
                    ZPad(buf, 2, dp->Month);
                    break;
              case 'M' :
                    ZPad(buf, 2, dp->Minute);
                    break;
              case 'p' :
                    r = AMPM[dp->Hour/12];
                    break;
              case 'S' :
                    ZPad(buf, 2, dp->Second);
                    break;
              case 'w' :
                    ZPad(buf, 1, dp->WeekDay-1);
                    break;
              case 'x' :
                    StrFTime(buf, BUFLEN, DateTimeFormat[0], dp);
                    break;
              case 'X' :
                    StrFTime(buf, BUFLEN, DateTimeFormat[1], dp);
                    break;
              case 'y' :
                    ZPad(buf, 2, dp->Year % 100);
                    break;
              case 'Y' :
                    ZPad(buf, 4, dp->Year);
                    break;
              default:
                    buf[0] = '%';
                    buf[1] = Format[-1];
                    buf[2] = '\0';
                    if (buf[1] == 0)
                          Format--;
              }
              while (*r)
              {
                    if (p == q)
                    {
                          *q = '\0';
                          return 0;
                    }
                    *p++ = *r++;
              }
        }
        else
        {
              if (p == q)
              {
                    *q = '\0';
                    return 0;
              }
              *p++ = Format[-1];
        }
    }
    *p = '\0';
    return p - Out;
}
