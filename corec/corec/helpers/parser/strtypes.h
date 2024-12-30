/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * Copyright (C) CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
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
