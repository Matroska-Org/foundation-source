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

#ifndef __NODETOOLS_H
#define __NODETOOLS_H

//some helper functions

static INLINE tchar_t* tcsdup(const tchar_t* s)
{
	size_t n = tcslen(s)+1;
	tchar_t* p = (tchar_t*)malloc(n*sizeof(tchar_t));
	if (p) tcscpy_s(p,n,s);
	return p;
}

static INLINE bool_t EqInt(const int* a, const int* b) { return *a == *b; }
static INLINE bool_t EqTick(const tick_t* a, const tick_t* b) { return *a == *b; }
static INLINE bool_t EqBool(const bool_t* a, const bool_t* b) { return *a == *b; }
static INLINE bool_t EqPtr(void** a, void** b) { return *a == *b; }
static INLINE bool_t EqFrac(const cc_fraction* a, const cc_fraction* b)
{
	if (a->Den == b->Den && a->Num == b->Num) 
		return 1;
	if (!a->Den) return b->Den==0;
	if (!b->Den) return 0;
	return (int64_t)b->Den * a->Num == (int64_t)a->Den * b->Num;
}

// variable names: Result, Data, Size

#define GETVALUE(Value,Type)								\
		{													\
			assert(Size == sizeof(Type));					\
			*(Type*)Data = Value;							\
			Result = ERR_NONE;								\
		}

#define GETVALUECOND(Value,Type,Cond)						\
		{													\
			assert(Size == sizeof(Type));					\
			if (Cond)										\
			{												\
				*(Type*)Data = Value;						\
				Result = ERR_NONE;							\
			}												\
		}
    
#define SETVALUE(Value,Type,Update)							\
		{													\
			assert(Size == sizeof(Type));					\
			Value = *(Type*)Data;							\
			Result = Update;								\
		}

#define SETVALUENULL(Value,Type,Update,Null)				\
		{													\
			if (Data)										\
			{												\
				assert(Size == sizeof(Type));				\
				Value = *(Type*)Data;						\
			}												\
			else											\
				Null;										\
			Result = Update;								\
		}

#define SETVALUECOND(Value,Type,Update,Cond)				\
		{													\
			assert(Size == sizeof(Type));					\
			if (Cond)										\
			{												\
				Value = *(Type*)Data;						\
				Result = Update;							\
			}												\
		}

#define SETVALUECMP(Value,Type,Update,EqFunc)				\
		{													\
			assert(Size == sizeof(Type));					\
			Result = ERR_NONE;								\
			if (!EqFunc(&Value,(Type*)Data))				\
			{												\
				Value = *(Type*)Data;						\
				Result = Update;							\
			}												\
		}

#define SETPACKETFORMAT(Value,Type,Update)					\
		{													\
			assert(Size == sizeof(Type) || !Data);			\
			Result = PacketFormatCopy(&Value,(Type*)Data);	\
			if (Result == ERR_NONE)							\
				Result = Update;							\
		}

#define SETPACKETFORMATCMP(Value,Type,Update)				\
		{													\
			assert(Size == sizeof(Type) || !Data);			\
			Result = ERR_NONE;								\
			if (!EqPacketFormat(&Value,(Type*)Data))		\
			{												\
				Result = PacketFormatCopy(&Value,(Type*)Data);\
				if (Result == ERR_NONE)						\
					Result = Update;						\
			}												\
		}

static INLINE tick_t ScaleTick(int64_t v, int64_t Num, int64_t Den)
{
    return (tick_t)Scale64(v,Num,Den);
}

static INLINE filepos_t ScalePos(int64_t v, int64_t Num, int64_t Den)
{
    return (filepos_t)Scale64(v,Num,Den);
}

NODE_DLL int ScaleRound(int_fast32_t v,int_fast32_t Num,int_fast32_t Den);

#endif
