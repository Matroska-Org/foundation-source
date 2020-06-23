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

NODE_DLL size_t STR_FourCCToString(tchar_t* Out, size_t OutLen, fourcc_t FourCC);
NODE_DLL fourcc_t STR_StringToFourCC(const tchar_t* In, bool_t Upper);
NODE_DLL void STR_FractionToString(tchar_t* Out, size_t OutLen, const cc_fraction* Fraction, int Percent, int Decimal);
NODE_DLL void STR_StringToFraction(const tchar_t* In, cc_fraction* Out, bool_t Percent);
NODE_DLL int STR_StringToInt(const tchar_t* In, int Hex);
NODE_DLL int64_t STR_StringToInt64(const tchar_t* In);
NODE_DLL void STR_IntToString(tchar_t* Out, size_t OutLen, int32_t Int, bool_t Hex);
NODE_DLL void STR_Int64ToString(tchar_t* Out, size_t OutLen, int64_t Int, bool_t Hex);
NODE_DLL void STR_TickToString(tchar_t* Out, size_t OutLen, tick_t Tick, bool_t MS, bool_t Extended, bool_t Fix);
NODE_DLL void STR_SysTickToString(tchar_t* Out, size_t OutLen, systick_t Tick, bool_t MS, bool_t Extended, bool_t Fix);
NODE_DLL tick_t STR_StringToTick(const tchar_t* In);
NODE_DLL systick_t STR_StringToSysTick(const tchar_t* In);
NODE_DLL void STR_RGBToString(tchar_t* Out, size_t OutLen, rgbval_t RGB);
NODE_DLL rgbval_t STR_StringToRGB(const tchar_t* In);
NODE_DLL void STR_GUIDToString(tchar_t* Out, size_t OutLen, const cc_guid*);
NODE_DLL bool_t STR_StringToGUID(const tchar_t* In, cc_guid*);
NODE_DLL void STR_StringToURL(anynode* AnyNode, tchar_t* Out, size_t OutLen, const tchar_t *URL);
NODE_DLL void STR_URLToString(tchar_t* Out, size_t OutLen, const tchar_t *URL);
NODE_DLL void STR_LangToIso639_1(tchar_t *Out, size_t OutLen, fourcc_t Lang);
NODE_DLL void STR_ByteRateToString(tchar_t* Out, size_t OutLen, int ByteRate);
NODE_DLL datetime_t STR_RFC822ToRel(const tchar_t *);
NODE_DLL datetime_t STR_ISO8601ToRel(const tchar_t *);
NODE_DLL size_t STR_StrFTime(tchar_t* Out, size_t OutLen, const tchar_t *Format, datepack_t *dp);

#endif
