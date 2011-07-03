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

#include "corec/corec.h"

int vsprintf(tchar_t *s0, const tchar_t *fmt, va_list args)
{
	tchar_t Num[80];
	tchar_t *s;

	for (s=s0;*fmt;++fmt)
	{
		if (fmt[0]=='%' && fmt[1])
		{
			const tchar_t *str;
			bool_t Left = 0;
			bool_t Sign = 0;
			bool_t Large = 0;
			bool_t ZeroPad = 0;
			int Width = -1;
			int Type = -1;
			int Base = 10;
			tchar_t ch,cs;
			int Len;
			long n;

			for (;;)
			{
				switch (*(++fmt)) 
				{
				case '-': Left = 1; continue;
				case '0': ZeroPad = 1; continue;
				default: break;
				}
				break;
			}

			if (*fmt>='0' && *fmt<='9')
			{
				Width = 0;
				for (;*fmt>='0' && *fmt<='9';++fmt)
					Width = Width*10 + (*fmt-'0');
			}
			else
			if (*fmt == '*')
			{
				++fmt;
				Width = va_arg(args, int);
				if (Width < 0)
				{
					Left = 1;
					Width = -Width;
				}
			}

			if (*fmt == 'h' || 
				*fmt == 'L' ||
				*fmt == 'l')
				Type = *(fmt++);

			switch (*fmt)
			{
			case 'c':
				for (;!Left && Width>1;--Width)
					*(s++) = ' ';
				*(s++) = (char)va_arg(args,int);
				for (;Width>1;--Width)
					*(s++) = ' ';
				continue;
			case 's':
				str = va_arg(args,const tchar_t*);
				if (!s)
					str = T("<NULL>");
				Len = tcslen(str);
				for (;!Left && Width>Len;--Width)
					*(s++) = ' ';
				for (;Len>0;--Len,--Width)
					*(s++) = *(str++);
				for (;Width>0;--Width)
					*(s++) = ' ';
				continue;
			case 'o':
				Base = 8;
				break;
			case 'X':
				Large = 1;
			case 'x':
				Base = 16;
				break;
			case 'i':
			case 'd':
				Sign = 1;
			case 'u':
				break;
			default:
				if (*fmt != '%')
					*(s++) = '%';
				*(s++) = *fmt;
				continue;
			}

			if (Type == 'l')
				n = va_arg(args,unsigned long);
			else
			if (Type == 'h')
				if (Sign)
					n = (short)va_arg(args,int);
				else
					n = (unsigned short)va_arg(args,unsigned int);
			else
			if (Sign)
				n = va_arg(args,int);
			else
				n= va_arg(args,unsigned int);

			if (Left)
				ZeroPad = 0;
			if (Large)
				str = T("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
			else
				str = T("0123456789abcdefghijklmnopqrstuvwxyz");

			ch = ' ';
			if (ZeroPad)
				ch = '0';
			cs = 0;

			if (n<0 && Sign)
			{
				cs = '-';
				n=-n;
			}

			Len = 0;
			if (n==0)
				Num[Len++] = '0';
			else
			{
				unsigned long un = n;
				while (un != 0)
				{
					Num[Len++] = str[un%Base];
					un /= Base;
				}
			}

			if (cs)
				++Len;

			for (;!Left && Width>Len;--Width)
				*(s++) = ch;

			if (cs)
			{
				*(s++) = cs;
				--Len;
				--Width;
			}

			for (;Len;--Width)
				*(s++) = Num[--Len];

			for (;Width>0;--Width)
				*(s++) = ' ';
		}
		else
			*(s++) = *fmt;
	}
	*(s++) = 0;
	return s-s0;
}

int sprintf(tchar_t *s, const tchar_t *fmt, ...)
{
	int i;
	va_list Args;
	va_start(Args,fmt);
	i=vsprintf(s,fmt,Args);
	va_end(Args);
	return i;
}
