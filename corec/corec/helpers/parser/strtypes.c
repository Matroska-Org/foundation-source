/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include <stdlib.h>
#include "strtypes.h"
#include "parser.h"
#include <corec/helpers/date/date.h>
#include <corec/str/str.h>
#include <limits.h>

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

void GUIDToString(tchar_t* Out, size_t OutLen, const cc_guid* p)
{
    stprintf_s(Out,OutLen,T("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
        (int)p->v1,p->v2,p->v3,p->v4[0],p->v4[1],p->v4[2],p->v4[3],
        p->v4[4],p->v4[5],p->v4[6],p->v4[7]);
}

void FractionToString(tchar_t* Out, size_t OutLen, const cc_fraction* p, int Percent, int Decimal)
{
    int a,b,i;
    int_fast32_t Num = p->Num;
    int_fast32_t Den = p->Den;

    if (Percent)
    {
        while (labs(Num) > INT_MAX/100)
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
        stprintf_s(Out,OutLen,T("%d"),a);

    if (Percent>0)
        tcscat_s(Out,OutLen,T("%"));
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
    stscanf(In,Hex ? T("%X"):T("%d"),&v);
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
        stprintf_s(Out,OutLen,T("%d"),(int32_t)p);
    else
        stprintf_s(Out,OutLen,Hex ? T("0x%08X%08X"):T("%d%d"),(uint32_t)(((uint64_t)p)>>32),(uint32_t)p);
}

void IntToString(tchar_t* Out, size_t OutLen, int32_t p, bool_t Hex)
{
    stprintf_s(Out,OutLen,Hex ? T("0x%08X"):T("%d"),p);
}

void RGBToString(tchar_t* Out, size_t OutLen, rgbval_t RGB)
{
    stprintf_s(Out,OutLen,T("#%08X"),(int)INT32BE(RGB));
    if (tcslen(Out)>=1+8 && Out[7]=='0' && Out[8]=='0')
        Out[7] = 0;
}

void SysTickToString(tchar_t* Out, size_t OutLen, systick_t Tick)
{
    tchar_t Sign[2] = {0};
    if (Tick<0)
    {
        Sign[0] = '-';
        Tick = -Tick;
    }

    int Hour,Min,Sec;
    //Tick += GetTimeFreq()/2000;
    Hour = (int)(Tick / 3600 / GetTimeFreq());
    Tick -= Hour * 3600 * GetTimeFreq();
    Min = (int)(Tick / 60 / GetTimeFreq());
    Tick -= Min * 60 * GetTimeFreq();
    Sec = (int)(Tick / GetTimeFreq());
    Tick -= Sec * GetTimeFreq();
    if (!Hour && !Min)
        stprintf_s(Out,OutLen,T("%s%d"),Sign,Sec);
    else
    {
        if (Hour)
            stprintf_s(Out,OutLen,T("%s%d:%02d"),Sign,Hour,Min);
        else
            stprintf_s(Out,OutLen,T("%s%d"),Sign,Min);
        stcatprintf_s(Out,OutLen,T(":%02d"),Sec);
    }
    stcatprintf_s(Out,OutLen,T(".%03d"),(int)((Tick*1000)/GetTimeFreq()));
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
