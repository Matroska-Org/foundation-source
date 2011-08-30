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

#include "str.h"
#if defined(COREC_PARSER)
#include "corec/helpers/parser/parser.h"
#endif

static INLINE int ascii_upper(int ch)
{
    if (ch>='a' && ch<='z')
        ch += 'A'-'a';
    return ch;
}

size_t tcsbytes(const tchar_t* p)
{
    if (p)
        return (tcslen(p)+1)*sizeof(tchar_t);
    else
        return 0;
}

#define icmp_ascii(a,b,i) \
    ca=a[i]; \
    cb=b[i]; \
    cb ^= ca; \
    if (cb) /* not equal? */\
    { \
        cb &= ~0x20; /* only allow the 0x20 bit to be different */ \
        ca &= ~0x20; /* and only in the 'A'..'Z' range */ \
        if (cb || ca<'A' || ca>'Z') \
        { \
            ca=a[i]; \
            cb=b[i]; \
            break; \
        } \
    } \
    if (!ca) /* both zero? */ \
        return 0;

int tcsicmp_ascii(const tchar_t* a,const tchar_t* b)
{
    int ca,cb;
    for (;;a+=4,b+=4)
    {
        icmp_ascii(a,b,0)
        icmp_ascii(a,b,1)
        icmp_ascii(a,b,2)
        icmp_ascii(a,b,3)
    }
    return ascii_upper(ca)-ascii_upper(cb);
}

#define isame_ascii(a,b,i) \
    ca=a[i]; \
    cb=b[i]; \
    cb ^= ca; \
    if (cb) /* not equal? */\
    { \
        cb &= ~0x20; /* only allow the 0x20 bit to be different */ \
        ca &= ~0x20; /* and only in the 'A'..'Z' range */ \
        if (cb || ca<'A' || ca>'Z') \
            return 0; \
    } \
    if (!ca) /* both zero? */ \
        break;

bool_t tcsisame_ascii(const tchar_t* a,const tchar_t* b)
{
    int ca,cb;
    for (;;a+=4,b+=4)
    {
        isame_ascii(a,b,0)
        isame_ascii(a,b,1)
        isame_ascii(a,b,2)
        isame_ascii(a,b,3)
    }
    return 1;
}

int tcsnicmp_ascii(const tchar_t* a,const tchar_t* b,size_t n)
{
    int ca,cb;
    for (;;++a,++b)
    {
        if (!n--) return 0;
        icmp_ascii(a,b,0)
    }
    return ascii_upper(ca)-ascii_upper(cb);
}

tchar_t* tcscpy_s(tchar_t* Out,size_t OutLen,const tchar_t* In)
{
    assert(In != NULL);
	if (OutLen>0)
	{
		size_t n = min(tcslen(In),OutLen-1);
		memcpy(Out,In,n*sizeof(tchar_t));
		Out[n] = 0;
	}
    return Out;
}

tchar_t* tcsncpy_s(tchar_t* Out,size_t OutLen,const tchar_t* In,size_t n)
{
    assert(In != NULL);
	if (OutLen>0)
	{
		n = min(min(tcslen(In),n),OutLen-1);
		memcpy(Out,In,n*sizeof(tchar_t));
		Out[n] = 0;
	}
    return Out;
}

tchar_t* tcscat_s(tchar_t* Out,size_t OutLen,const tchar_t* In)
{
    assert(In != NULL);
	if (OutLen>0)
	{
		size_t n = tcslen(Out);
        if (OutLen>n)
		    tcscpy_s(Out+n,OutLen-n,In);
	}
    return Out;
}

tchar_t* tcsncat_s(tchar_t* Out,size_t OutLen,const tchar_t* In,size_t InLen)
{
    assert(In != NULL);
	if (OutLen>0)
	{
		size_t n = tcslen(Out);
        if (OutLen>n)
    		tcsncpy_s(Out+n,OutLen-n,In,InLen);
	}
    return Out;
}

void stprintf_s(tchar_t* Out,size_t OutLen,const tchar_t* Mask, ...)
{
	va_list Arg;
	va_start(Arg, Mask);
	vstprintf_s(Out,OutLen,Mask,Arg);
	va_end(Arg);
}

void stcatprintf_s(tchar_t* Out,size_t OutLen,const tchar_t* Mask, ...)
{
    size_t n = tcslen(Out);
    if (OutLen>n)
    {
	    va_list Arg;
	    va_start(Arg, Mask);
	    vstprintf_s(Out+n,OutLen-n,Mask,Arg);
	    va_end(Arg);
    }
}

static int get_integers(int64_t IntValue)
{
    int Result = 0;
    int64_t TmpVal = 1;
    while (IntValue >= TmpVal && TmpVal < MAX_INT64/10)
    {
        TmpVal *= 10;
        Result++;
    }
    return Result;
}

static int get_decimals(int Decimals, int64_t IntValue)
{
    int Result = 0;
    int64_t TmpVal = IntValue+1;
    while (TmpVal < MAX_INT64/10)
    {
        TmpVal *= 10;
        Result++;
    }
    return min(Decimals,Result);
}

void vstprintf_s(tchar_t* Out,size_t OutLen,const tchar_t* Mask,va_list Arg)
{
    int64_t vl;
	int vs;
    double dvs, _dvs;
	intptr_t Width,ptr;
	unsigned int v,w,q,w0;
	bool_t ZeroFill;
	bool_t Unsigned;
	bool_t Sign;
	bool_t AlignLeft;
    bool_t Long;
    int Decimals;
	const tchar_t *In;
	const char *InA;
	size_t n;
    int64_t lvs,_lvs,ww;

	while (OutLen>1 && *Mask)
	{
		switch (*Mask)
		{
		case '%':
			++Mask;

			if (*Mask == '%')
			{
				*(Out++) = *(Mask++);
				--OutLen;
				break;
			}

			AlignLeft = *Mask=='-';
			if (AlignLeft)
				++Mask;

			ZeroFill = *Mask=='0';
			if (ZeroFill)
				++Mask;

			Width = -1;
			if (IsDigit(*Mask))
			{
				Width = 0;
				for (;IsDigit(*Mask);++Mask)
					Width = Width*10 + (*Mask-'0');
			}

            Decimals = *Mask=='.';
			if (Decimals)
            {
				++Mask;
                Decimals = 0;
			    if (IsDigit(*Mask))
			    {
				    for (;IsDigit(*Mask);++Mask)
					    Decimals = Decimals*10 + (*Mask-'0');
			    }
            }

            Long = 0;
            if (*Mask == '*')
			{
				++Mask;
				Width = va_arg(Arg,int);
			}

            while (*Mask == 'l')
            {
                Long=1;
				++Mask; // long
            }

            if (*Mask=='I' && *(Mask+1)=='6' && *(Mask+2)=='4')
            {
                Long=1;
				Mask += 3;
            }

			Unsigned = *Mask=='u';
			if (Unsigned)
				++Mask;

			switch (*Mask)
			{
			case 'd':
			case 'i':
			case 'X':
			case 'x':
                if (Long)
				    vl = va_arg(Arg,int64_t);
                else
				    vl = va_arg(Arg,int);

				if (*Mask=='x' || *Mask=='X')
				{
					Unsigned = 1;
					q = 16;
					ww = 0x10000000;
				}
				else
				{
					q = 10;
					ww = 100000000000;
				}

				Sign = vl<0 && !Unsigned;
				if (Sign)
				{
					vl=-vl;
					--Width;
				}

				w0 = 1;
				for (;Width>1;--Width)
					w0 *= q;

				v = vl;
				while (vl<ww && ww>w0)
					ww/=q;

				while (ww>0)
				{
					unsigned int i = (unsigned int)(vl/ww); // between 0 and q-1
					vl-=i*ww;
					if (OutLen>1 && Sign && (ww==1 || ZeroFill || i>0))
					{
						*(Out++) = '-';
						--OutLen;
						Sign = 0;
					}
					if (OutLen>1)
					{
						if (i==0 && !ZeroFill && ww!=1)
							i = ' ';
						else
						{
							ZeroFill = 1;
							if (i>=10)
							{
								if (*Mask == 'X')
									i += 'A'-10;
								else
									i += 'a'-10;
							}
							else
								i+='0';
						}
						*(Out++) = (tchar_t)i;
						--OutLen;
					}
					ww/=q;
				}

				break;

			case 'c':
				Width -= 1;
				if (!AlignLeft)
					while (--Width>=0 && OutLen>1)
					{
						*Out++ = ' ';
						--OutLen;
					}
				*(Out++) = (tchar_t)va_arg(Arg,int);
				--OutLen;
				while (--Width>=0 && OutLen>1)
				{
					*Out++ = ' ';
					--OutLen;
				}
				break;

			case 's':
				In = va_arg(Arg,const tchar_t*);
                if (In)
                {
				    n = min(tcslen(In),OutLen-1);
				    Width -= n;
				    if (!AlignLeft)
					    while (--Width>=0 && OutLen>1)
					    {
						    *Out++ = ' ';
						    --OutLen;
					    }
				    memcpy(Out,In,n*sizeof(tchar_t));
				    Out += n;
				    OutLen -= n;
				    while (--Width>=0 && OutLen>1)
				    {
					    *Out++ = ' ';
					    --OutLen;
				    }
                }
				break;
			case 'S':
				InA = va_arg(Arg,const char*);
				n = min(strlen(InA),OutLen-1);
				Width -= n;
				if (!AlignLeft)
					while (--Width>=0 && OutLen>1)
					{
						*Out++ = ' ';
						--OutLen;
					}
                for (;n>0 && OutLen>1 && *InA;++InA,++Out,--OutLen,--n)
                    *Out = *InA;
    			while (--Width>=0 && OutLen>1)
				{
					*Out++ = ' ';
					--OutLen;
				}
				break;
            case 'g':
            case 'G':
            case 'f':
                dvs = va_arg(Arg,double);
                if (OutLen)
                {
			        Sign = dvs<0;
                    if (Sign && OutLen>1)
                        dvs = -dvs;

                    if (Width >= 0)
                    {
                        // count integer digits and decimal digits to adjust the alignment
                        int max_integer_digits = get_integers((int64_t)dvs);
                        int max_decimal_digits = get_decimals(Width - max_integer_digits - 1 - (Sign?1:0),(int64_t)dvs);
                        int max_total_chars;

                        vs = (int)dvs;
                        lvs = 1;
                        _lvs = vs;
                        w = w0 = 0; // decimals in use
                        while (max_decimal_digits-->0)
                        {
                            lvs *= 10;
                            _lvs *= 10;
                            q = (int)(dvs * lvs - (double)_lvs);
                            _lvs += q;
                            w0++;
                            if (q)
                                w = w0;
                        }

                        if (Decimals>0)
                            max_decimal_digits = max(Decimals,(int)w);
                        else
                            max_decimal_digits = w;

                        max_total_chars = max_integer_digits + max_decimal_digits + (Sign?1:0) + (max_decimal_digits?1:0);

                        if (max_integer_digits + (Sign?1:0) + (max_decimal_digits?1:0) > Width)
                            goto do_exponent;

                        while (!AlignLeft && Width > max_total_chars && OutLen > 1)
                        {
                            *Out++ = ' ';
                            --Width;
                            --OutLen;
                        }
                    }

                    // integer part
                    vs = (int)dvs;
			        v = vs;

                    if (Sign && dvs!=0.0 && OutLen > 1)
                    {
                        *Out++ = '-';
                        --OutLen;
                    }

				    w = 1000000000;
			        while (w>v && w>1)
				        w/=10;

                    // integer filling
                    while (w>0)
			        {
				        unsigned int i = v/w;
				        v-=i*w;
				        if (OutLen>1 && (Width<0 || Width-->0))
				        {
					        i+='0';
					        *(Out++) = (tchar_t)i;
					        --OutLen;
                        }
				        w/=10;
                    }

                    // decimal part
                    // dry run to count the number of necessary decimals
                    v = Decimals;
                    if (!Decimals)
                        Decimals=30;
                    Decimals = get_decimals(Decimals,vs);
                    lvs = 1;
                    _lvs = vs;
                    w = w0 = 0; // decimals in use
                    while (Decimals-->0)
                    {
                        lvs *= 10;
                        _lvs *= 10;
                        q = (int)(dvs * lvs - (double)_lvs);
                        _lvs += q;
                        w0++;
                        if (q)
                            w = w0;
                    }

                    if (*Mask==T('f') && w)
                        w = max(w,6);
                    if (w && OutLen>1)
                    {
                        *Out++ = T('.');
                        --OutLen;
                        --Width;

                        v -= w;

                        lvs = 1;
                        _lvs = vs;
                        while (w-->0 && OutLen>1)
                        {
                            lvs *= 10;
                            _lvs *= 10;
                            q = (int)(dvs * lvs - (double)_lvs);
                            _lvs += q;
                            if (w==0) // last one, do some rounding
                            {
                                if ((dvs * lvs - (double)_lvs) >= 5)
                                    q++;
                            }
                            *Out++ = (tchar_t)(q + '0');
                            --OutLen;
                            --Width;
                        }
                        while (Width-- > 0 && OutLen > 1)
                        {
                            *Out++ = ' ';
                            --OutLen;
                        }
                    }
                    break;
do_exponent:
                    // TODO
                    *Out++ = T('e');
                    OutLen--;
                }
                break;
            case 'e':
            case 'E':
                dvs = va_arg(Arg,double);
				Sign = dvs<0;
                if (Sign && OutLen>1)
                {
                    *Out++ = '-';
                    OutLen--;
                    dvs = -dvs;
                }
                vs = 0; // exponent
                _dvs = 1.0; // factor
                if (dvs >= 1.0)
                {
                    while (dvs*_dvs > 1.0)
                    {
                        _dvs *= 10.0;
                        vs++;
                    }
                    dvs *= 10.0;
                    vs--;
                }
                else
                {
                    while (dvs*_dvs < 1.0)
                    {
                        _dvs *= 10.0;
                        vs--;
                    }
                }

                dvs *= _dvs;
                if (OutLen > 4)
                {
                    *Out++ = (tchar_t)((int)dvs + T('0'));
                    *Out++ = T('.');
                    dvs -= (int)dvs;
                    dvs *= 10.0;
                    if (dvs - (int)dvs > 0.5)
                        *Out++ = (tchar_t)((int)(dvs+1) + T('0'));
                    else
                        *Out++ = (tchar_t)((int)(dvs) + T('0'));
                    *Out++ = *Mask;
                    OutLen -= 4;
                }
                if (OutLen > 1 && vs < 0)
                {
                    *Out++ = T('-');
                    OutLen--;
                    vs = -vs;
                }
                if (OutLen > 3)
                {
                    *Out++ = (tchar_t)((vs / 100) + T('0'));
                    vs %= 100;
                    *Out++ = (tchar_t)((vs / 10) + T('0'));
                    vs %= 10;
                    *Out++ = (tchar_t)(vs + T('0'));
                    OutLen -= 3;
                }
                break;
            case 'p':
                while (Width > 8 && !AlignLeft && OutLen > 1)
                {
                    Width--;
                    *Out++ = ' ';
                    OutLen--;
                }
                ptr = va_arg(Arg,intptr_t);
                for (vs=28;OutLen>1 && vs>=0;vs-=4)
                {
                    q = (ptr>>vs) & 0x0F;
                    if (q<10)
                        *Out++ = (tchar_t)(q+T('0'));
                    else
                        *Out++ = (tchar_t)(q+T('A')-10);
                    OutLen--;
                }
                while (Width > 8 && OutLen > 1)
                {
                    Width--;
                    *Out++ = ' ';
                    OutLen--;
                }
                break;
            case 'r':
                {
#if !defined(COREC_PARSER)
                    va_arg(Arg,fourcc_t); // skip the param
#else
                    fourcc_t FourCC = va_arg(Arg,fourcc_t);
                    if (OutLen > 4)
                    {
                        size_t Written = FourCCToString(Out,OutLen,FourCC);
                        Out+=Written;
                        OutLen+=Written;
                    }
#endif
                }
                break;
			}

			++Mask;
			break;

		default:
			*(Out++) = *(Mask++);
			--OutLen;
			break;
		}
	}

	assert(OutLen>0);
    if (OutLen>0)
        *Out=0;
}

static int var_stscanf_s(const tchar_t* In, size_t *InLen, const tchar_t* Mask, va_list Arg)
{
	int n = 0;
	int Sign;
    bool_t Long=0;
    size_t inlen = *InLen;
	int64_t v;
	int Width;
	const tchar_t* In0;

	while (inlen && In && *Mask)
	{
		switch (*Mask)
		{
		case '%':
			++Mask;

			Width = -1;
			if (IsDigit(*Mask))
			{
				Width = 0;
				for (;IsDigit(*Mask);++Mask)
					Width = Width*10 + (*Mask-'0');
			}

            while (*Mask=='l')
            {
                ++Long;
                ++Mask;
            }

			switch (*Mask)
			{
			case 'X':
			case 'x':

				for (;inlen && IsSpace(*In);++In,--inlen) {}
				Sign = *In == '-';
				In0 = In;
				if (Sign) { ++In; --inlen; --Width; }
				v = 0;
				for (;inlen && Width!=0 && *In;++In,--inlen,--Width)
				{
					int h = Hex(*In);
					if (h<0) break;
					v=v*16+h;
				}
				if (Sign) v=-v;
				if (In != In0)
				{
					*va_arg(Arg,int*) = v;
					++n;
				}
				else
					In = NULL;
				break;

			case 'd':
			case 'i':

				for (;inlen && IsSpace(*In);++In,--inlen) {}
				Sign = *In == '-';
				In0 = In;
                if (Sign) {++In; --inlen;}
				v = 0;
				for (;inlen && Width!=0 && IsDigit(*In);++In,--inlen,--Width)
					v=v*10+(*In-'0');
				if (Sign) v=-v;
				if (In != In0)
				{
					if (Long)
						*va_arg(Arg,int64_t*) = v;
					else
						*va_arg(Arg,int*) = v;
					++n;
				}
				else
					In = NULL;
				break;

			case 'o':

				for (;inlen && IsSpace(*In);++In,--inlen) {}
				Sign = *In == '-';
				In0 = In;
                if (Sign) {++In; --inlen;}
				v = 0;
				for (;inlen && Width!=0 && *In;++In,--inlen,--Width)
				{
					if (*In >= '0' && *In <= '7')
						v=v*8+(*In-'0');
					else
						break;
				}
				if (Sign) v=-v;
				if (In != In0)
				{
					*va_arg(Arg,int*) = v;
					++n;
				}
				else
					In = NULL;
				break;
            case 'f':
				for (;inlen && IsSpace(*In);++In,--inlen) {}
				Sign = *In == '-';
				In0 = In;
                if (Sign) {++In; --inlen;}
                v = 0;
				for (;inlen && IsDigit(*In);++In,--inlen)
					v=v*10+(*In-'0');
                if (Sign) v=-v;
				if (In != In0)
				{
                    int64_t Decimal=0;
                    double Decimals=1.0;

                    if (inlen && *In=='.')
                    {
                        ++In;
                        --inlen;
				        for (;inlen && IsDigit(*In);++In,--inlen,Decimals/=10.0)
					        Decimal=Decimal*10+(*In-'0');
                    }
                    if (Long)
				        *va_arg(Arg,double*) = (double)v + (double)(Decimal*Decimals);
                    else
                        *va_arg(Arg,float*) = (float)v + (float)(Decimal*Decimals);
					++n;
				}
				else
					In = NULL;
                break;
			}
			break;
		case 9:
		case ' ':
			for (;inlen && IsSpace(*In);++In,--inlen) {}
			break;
		default:
			if (*Mask == *In)
            {
				++In;
                --inlen;
            }
			else
				In = NULL;
		}
		++Mask;
	}
    *InLen -= inlen;

	return n;
}

int stscanf_s(const tchar_t* In, size_t *InLen, const tchar_t* Mask, ...)
{
    int n;
	va_list Arg;
	va_start(Arg, Mask);
    n = var_stscanf_s(In,InLen,Mask,Arg);
	va_end(Arg);
    return n;
}

int stscanf(const tchar_t* In, const tchar_t* Mask, ...)
{
    int n;
    size_t InLen = tcslen(In);
	va_list Arg;
	va_start(Arg, Mask);
    n = var_stscanf_s(In,&InLen,Mask,Arg);
	va_end(Arg);
    return n;
}

bool_t IsSpace(int ch) { return ch==' ' || ch==9 || ch == 13 || ch == 10; }
bool_t IsAlpha(int ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }
bool_t IsDigit(int ch) { return ch>='0' && ch<='9'; }

int Hex(int ch) 
{
	if (IsDigit(ch))
		return ch-'0';
	if (ch >= 'a' && ch <= 'f')
		return ch-'a'+10;
	if (ch >= 'A' && ch <= 'F')
		return ch-'A'+10;
	return -1;
}

tchar_t* tcsupr(tchar_t* p) 
{
	tchar_t* i=p;
	for (;*i;++i)
		*i = (tchar_t)toupper(*i);
	return p;
}

tchar_t* tcslwr(tchar_t* p) 
{
  tchar_t* i=p;
  for (;*i;++i)
    *i = (tchar_t)tolower(*i);
  return p;
}

int StrListIndex(const tchar_t* s, const tchar_t* List)
{
	if (List && *List)
	{
		size_t n = tcslen(s);
		if (n)
		{
			int i=0;
			while (List)
			{
				if (tcsnicmp_ascii(List,s,n)==0 && (!List[n] || List[n]==',' || List[n]==' '))
					return i;
				List = tcschr(List,T(','));
				if (List) ++List;
				++i;
			}
		}
	}
	return -1;
}

size_t utf16len(const utf16_t *In)
{
    size_t Result=0;
    while (*In++)
        ++Result;
    return Result;
}

tchar_t* tcsreplace(tchar_t* Out, size_t OutLen, const tchar_t *Src, const tchar_t *Dst)
{
    tchar_t *s = tcsstr(Out,Src);
    if (s)
    {
        size_t SrcLen,DstLen,Remain;
        SrcLen = tcslen(Src);
        DstLen = tcslen(Dst);
        do
        {
            Remain = tcslen(s);
            if (SrcLen < DstLen)
            {
                if (Remain + (DstLen-SrcLen) >= OutLen - (s-Out))
                    return 0; // not enough room
                memmove(s+DstLen-SrcLen,s,(Remain+1)*sizeof(tchar_t));
            }
            else
                memmove(s,s+SrcLen-DstLen,(Remain+1)*sizeof(tchar_t));
            memcpy(s,Dst,DstLen*sizeof(tchar_t));
            s += DstLen;
        } while ((s=tcsstr(s,Src)));
    }
    return Out;
}

tchar_t* tcsreplacechar(tchar_t *ts, tchar_t From, tchar_t To)
{
    tchar_t *p;
    for (p=ts;*p;++p)
        if (*p == From)
            *p = To;
    return ts;
}
