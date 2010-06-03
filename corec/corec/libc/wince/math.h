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

#ifndef __MATH_H
#define __MATH_H

#include "stdlib.h"

#define sinf(x) ((float)sin(x))
#define sinhf(x) ((float)sinh(x))
#define cosf(x) ((float)cos(x))
#define coshf(x) ((float)cosh(x))
#define tanf(x) ((float)tan(x))
#define tanhf(x) ((float)tanh(x))

#define asinf(x) ((float)asin(x))
#define acosf(x) ((float)acos(x))
#define atanf(x) ((float)atan(x))

#define atan2f(x,y) ((float)atan2(x,y))

#define powf(x,y) ((float)pow(x,y))
#define logf(x) ((float)log(x))
#define log10f(x) ((float)log10(x))
#define expf(x) ((float)exp(x))
#define frexpf(x,y) ((float)frexp(x,y))
#define ldexpf(x,y) ((float)ldexp(x,y))

#endif /* __MATH_H*/
