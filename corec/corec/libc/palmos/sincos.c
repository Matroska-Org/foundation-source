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

#define PI 3.14159265358979324

NOINLINE double floor(double i);
NOINLINE double ceil(double i)
{
	if (i<0)
		return -floor(-i);
	else
	{
		double d = floor(i);
		if (d!=i)
			d+=1.0;
		return d;
	}
}

NOINLINE double floor(double i)
{
	if (i<0) 
		return -ceil(-i);
	else
		return (double)(int)i;
}

NOINLINE double sin(double i)
{
	int n,k;
	double j,sum;
	int sign = 0;

	if (i<0)
	{
		sign = 1;
		i = -i;
	}

	i *= 1.0/(2*PI);
	i -= floor(i);
	if (i>=0.5)
	{
		i -= 0.5;
		sign ^= 1;
	}

	i *= 2*PI;
	k = 1;
	j = sum = i;
	for (n=3;n<14;n+=2)
	{
		k *= (n-1)*n;
		j *= i*i;
		if (n & 2)
			sum -= j/k;
		else
			sum += j/k;
	}

	return sign ? -sum:sum;
}

NOINLINE double cos(double i)
{
	return sin(i+PI/2);
}
