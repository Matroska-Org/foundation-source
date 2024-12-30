/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_CONFIG_H
#define _EBML2_EBML_CONFIG_H

#include "corec/corec.h"

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t binary;

#define countof(x)  (sizeof(x)/sizeof(*(x)))

#ifdef __GNUC__
#define EBML_PRETTYLONGINT(c) (c ## ll)
#else // __GNUC__
#define EBML_PRETTYLONGINT(c) (c)
#endif // __GNUC__

namespace libebml {

    enum endianess {
        big_endian,   ///< PowerPC, Alpha, 68000
        little_endian ///< x86, x64, (most) ARM, (most) MIPS
    };

    class big_int16
    {
    public:
        big_int16(int16_t);
        big_int16();
        void Fill(binary *Buffer) const;
        void Eval(const binary *Buffer);
        operator int16() const;
    private:
        int16_t Value;
    };
};

#endif // _EBML2_EBML_CONFIG_H
