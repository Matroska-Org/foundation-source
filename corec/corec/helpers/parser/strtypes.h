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

#ifndef __STRTYPES_H
#define __STRTYPES_H

#include <corec/node/node.h> // NODE_DLL

fourcc_t StringToFourCC(const tchar_t* In, bool_t Upper);
void FractionToString(tchar_t* Out, size_t OutLen, const cc_fraction* Fraction, int Percent, int Decimal);
NODE_DLL int StringToInt(const tchar_t* In, int Hex);
NODE_DLL int64_t StringToInt64(const tchar_t* In);
void IntToString(tchar_t* Out, size_t OutLen, int32_t Int, bool_t Hex);
void Int64ToString(tchar_t* Out, size_t OutLen, int64_t Int, bool_t Hex);
void TickToString(tchar_t* Out, size_t OutLen, tick_t Tick, bool_t MS, bool_t Extended, bool_t Fix);
NODE_DLL void SysTickToString(tchar_t* Out, size_t OutLen, systick_t Tick, bool_t MS, bool_t Extended, bool_t Fix);
void RGBToString(tchar_t* Out, size_t OutLen, rgbval_t RGB);
void GUIDToString(tchar_t* Out, size_t OutLen, const cc_guid*);
void ByteRateToString(tchar_t* Out, size_t OutLen, int ByteRate);

#endif
