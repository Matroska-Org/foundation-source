/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * Copyright (C) CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include "charconvert.h"
#include <corec/str/str.h>

static const uint16_t codepage_1250[128] = {
0x20ac,0x0081,0x201a,0x0083,0x201e,0x2026,0x2020,0x2021,0x0088,0x2030,0x0160,0x2039,0x015a,0x0164,0x017d,0x0179,
0x0090,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0x0098,0x2122,0x0161,0x203a,0x015b,0x0165,0x017e,0x017a,
0x00a0,0x02c7,0x02d8,0x0141,0x00a4,0x0104,0x00a6,0x00a7,0x00a8,0x00a9,0x015e,0x00ab,0x00ac,0x00ad,0x00ae,0x017b,
0x00b0,0x00b1,0x02db,0x0142,0x00b4,0x00b5,0x00b6,0x00b7,0x00b8,0x0105,0x015f,0x00bb,0x013d,0x02dd,0x013e,0x017c,
0x0154,0x00c1,0x00c2,0x0102,0x00c4,0x0139,0x0106,0x00c7,0x010c,0x00c9,0x0118,0x00cb,0x011a,0x00cd,0x00ce,0x010e,
0x0110,0x0143,0x0147,0x00d3,0x00d4,0x0150,0x00d6,0x00d7,0x0158,0x016e,0x00da,0x0170,0x00dc,0x00dd,0x0162,0x00df,
0x0155,0x00e1,0x00e2,0x0103,0x00e4,0x013a,0x0107,0x00e7,0x010d,0x00e9,0x0119,0x00eb,0x011b,0x00ed,0x00ee,0x010f,
0x0111,0x0144,0x0148,0x00f3,0x00f4,0x0151,0x00f6,0x00f7,0x0159,0x016f,0x00fa,0x0171,0x00fc,0x00fd,0x0163,0x02d9
};

static const uint8_t codepage_1250_r[128] = {
0x01,0x03,0x08,0x10,0x18,0x20,0x24,0x26,0x27,0x28,0x29,0x2b,0x2c,0x2d,0x2e,0x30,
0x31,0x34,0x35,0x36,0x37,0x38,0x3b,0x41,0x42,0x44,0x47,0x49,0x4b,0x4d,0x4e,0x53,
0x54,0x56,0x57,0x5a,0x5c,0x5d,0x5f,0x61,0x62,0x64,0x67,0x69,0x6b,0x6d,0x6e,0x73,
0x74,0x76,0x77,0x7a,0x7c,0x7d,0x43,0x63,0x25,0x39,0x46,0x66,0x48,0x68,0x4f,0x6f,
0x50,0x70,0x4a,0x6a,0x4c,0x6c,0x45,0x65,0x3c,0x3e,0x23,0x33,0x51,0x71,0x52,0x72,
0x55,0x75,0x40,0x60,0x58,0x78,0x0c,0x1c,0x2a,0x3a,0x0a,0x1a,0x5e,0x7e,0x0d,0x1d,
0x59,0x79,0x5b,0x7b,0x0f,0x1f,0x2f,0x3f,0x0e,0x1e,0x21,0x22,0x7f,0x32,0x3d,0x16,
0x17,0x11,0x12,0x02,0x13,0x14,0x04,0x06,0x07,0x15,0x05,0x09,0x0b,0x1b,0x00,0x19
};

static const uint16_t codepage_1251[128] = {
0x0402,0x0403,0x201a,0x0453,0x201e,0x2026,0x2020,0x2021,0x20ac,0x2030,0x0409,0x2039,0x040a,0x040c,0x040b,0x040f,
0x0452,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0x0098,0x2122,0x0459,0x203a,0x045a,0x045c,0x045b,0x045f,
0x00a0,0x040e,0x045e,0x0408,0x00a4,0x0490,0x00a6,0x00a7,0x0401,0x00a9,0x0404,0x00ab,0x00ac,0x00ad,0x00ae,0x0407,
0x00b0,0x00b1,0x0406,0x0456,0x0491,0x00b5,0x00b6,0x00b7,0x0451,0x2116,0x0454,0x00bb,0x0458,0x0405,0x0455,0x0457,
0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,0x0418,0x0419,0x041a,0x041b,0x041c,0x041d,0x041e,0x041f,
0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,0x0428,0x0429,0x042a,0x042b,0x042c,0x042d,0x042e,0x042f,
0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,0x0438,0x0439,0x043a,0x043b,0x043c,0x043d,0x043e,0x043f,
0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,0x0448,0x0449,0x044a,0x044b,0x044c,0x044d,0x044e,0x044f
};

static const uint8_t codepage_1251_r[128] = {
0x18,0x20,0x24,0x26,0x27,0x29,0x2b,0x2c,0x2d,0x2e,0x30,0x31,0x35,0x36,0x37,0x3b,
0x28,0x00,0x01,0x2a,0x3d,0x32,0x2f,0x23,0x0a,0x0c,0x0e,0x0d,0x21,0x0f,0x40,0x41,
0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,
0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,
0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,
0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x38,0x10,
0x03,0x3a,0x3e,0x33,0x3f,0x3c,0x1a,0x1c,0x1e,0x1d,0x22,0x1f,0x25,0x34,0x16,0x17,
0x11,0x12,0x02,0x13,0x14,0x04,0x06,0x07,0x15,0x05,0x09,0x0b,0x1b,0x08,0x39,0x19
};

static const uint16_t codepage_1252[128] = {
0x20ac,0x0081,0x201a,0x0192,0x201e,0x2026,0x2020,0x2021,0x02c6,0x2030,0x0160,0x2039,0x0152,0x008d,0x017d,0x008f,
0x0090,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0x02dc,0x2122,0x0161,0x203a,0x0153,0x009d,0x017e,0x0178,
0x00a0,0x00a1,0x00a2,0x00a3,0x00a4,0x00a5,0x00a6,0x00a7,0x00a8,0x00a9,0x00aa,0x00ab,0x00ac,0x00ad,0x00ae,0x00af,
0x00b0,0x00b1,0x00b2,0x00b3,0x00b4,0x00b5,0x00b6,0x00b7,0x00b8,0x00b9,0x00ba,0x00bb,0x00bc,0x00bd,0x00be,0x00bf,
0x00c0,0x00c1,0x00c2,0x00c3,0x00c4,0x00c5,0x00c6,0x00c7,0x00c8,0x00c9,0x00ca,0x00cb,0x00cc,0x00cd,0x00ce,0x00cf,
0x00d0,0x00d1,0x00d2,0x00d3,0x00d4,0x00d5,0x00d6,0x00d7,0x00d8,0x00d9,0x00da,0x00db,0x00dc,0x00dd,0x00de,0x00df,
0x00e0,0x00e1,0x00e2,0x00e3,0x00e4,0x00e5,0x00e6,0x00e7,0x00e8,0x00e9,0x00ea,0x00eb,0x00ec,0x00ed,0x00ee,0x00ef,
0x00f0,0x00f1,0x00f2,0x00f3,0x00f4,0x00f5,0x00f6,0x00f7,0x00f8,0x00f9,0x00fa,0x00fb,0x00fc,0x00fd,0x00fe,0x00ff
};

static const uint8_t codepage_1252_r[128] = {
0x01,0x0d,0x0f,0x10,0x1d,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,
0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,
0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,
0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,
0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,
0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,
0x7b,0x7c,0x7d,0x7e,0x7f,0x0c,0x1c,0x0a,0x1a,0x1f,0x0e,0x1e,0x03,0x08,0x18,0x16,
0x17,0x11,0x12,0x02,0x13,0x14,0x04,0x06,0x07,0x15,0x05,0x09,0x0b,0x1b,0x00,0x19
};

static const uint16_t codepage_1253[128] = {
0x20ac,0x0081,0x201a,0x0192,0x201e,0x2026,0x2020,0x2021,0x0088,0x2030,0x008a,0x2039,0x008c,0x008d,0x008e,0x008f,
0x0090,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0x0098,0x2122,0x009a,0x203a,0x009c,0x009d,0x009e,0x009f,
0x00a0,0x0385,0x0386,0x00a3,0x00a4,0x00a5,0x00a6,0x00a7,0x00a8,0x00a9,0xf8f9,0x00ab,0x00ac,0x00ad,0x00ae,0x2015,
0x00b0,0x00b1,0x00b2,0x00b3,0x0384,0x00b5,0x00b6,0x00b7,0x0388,0x0389,0x038a,0x00bb,0x038c,0x00bd,0x038e,0x038f,
0x0390,0x0391,0x0392,0x0393,0x0394,0x0395,0x0396,0x0397,0x0398,0x0399,0x039a,0x039b,0x039c,0x039d,0x039e,0x039f,
0x03a0,0x03a1,0xf8fa,0x03a3,0x03a4,0x03a5,0x03a6,0x03a7,0x03a8,0x03a9,0x03aa,0x03ab,0x03ac,0x03ad,0x03ae,0x03af,
0x03b0,0x03b1,0x03b2,0x03b3,0x03b4,0x03b5,0x03b6,0x03b7,0x03b8,0x03b9,0x03ba,0x03bb,0x03bc,0x03bd,0x03be,0x03bf,
0x03c0,0x03c1,0x03c2,0x03c3,0x03c4,0x03c5,0x03c6,0x03c7,0x03c8,0x03c9,0x03ca,0x03cb,0x03cc,0x03cd,0x03ce,0xf8fb
};

static const uint8_t codepage_1253_r[128] = {
0x01,0x08,0x0a,0x0c,0x0d,0x0e,0x0f,0x10,0x18,0x1a,0x1c,0x1d,0x1e,0x1f,0x20,0x23,
0x24,0x25,0x26,0x27,0x28,0x29,0x2b,0x2c,0x2d,0x2e,0x30,0x31,0x32,0x33,0x35,0x36,
0x37,0x3b,0x3d,0x03,0x34,0x21,0x22,0x38,0x39,0x3a,0x3c,0x3e,0x3f,0x40,0x41,0x42,
0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x53,
0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,
0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,
0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x16,0x17,0x2f,0x11,0x12,
0x02,0x13,0x14,0x04,0x06,0x07,0x15,0x05,0x09,0x0b,0x1b,0x00,0x19,0x2a,0x52,0x7f
};

static const uint16_t codepage_1254[128] = {
0x20ac,0x0081,0x201a,0x0192,0x201e,0x2026,0x2020,0x2021,0x02c6,0x2030,0x0160,0x2039,0x0152,0x008d,0x008e,0x008f,
0x0090,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0x02dc,0x2122,0x0161,0x203a,0x0153,0x009d,0x009e,0x0178,
0x00a0,0x00a1,0x00a2,0x00a3,0x00a4,0x00a5,0x00a6,0x00a7,0x00a8,0x00a9,0x00aa,0x00ab,0x00ac,0x00ad,0x00ae,0x00af,
0x00b0,0x00b1,0x00b2,0x00b3,0x00b4,0x00b5,0x00b6,0x00b7,0x00b8,0x00b9,0x00ba,0x00bb,0x00bc,0x00bd,0x00be,0x00bf,
0x00c0,0x00c1,0x00c2,0x00c3,0x00c4,0x00c5,0x00c6,0x00c7,0x00c8,0x00c9,0x00ca,0x00cb,0x00cc,0x00cd,0x00ce,0x00cf,
0x011e,0x00d1,0x00d2,0x00d3,0x00d4,0x00d5,0x00d6,0x00d7,0x00d8,0x00d9,0x00da,0x00db,0x00dc,0x0130,0x015e,0x00df,
0x00e0,0x00e1,0x00e2,0x00e3,0x00e4,0x00e5,0x00e6,0x00e7,0x00e8,0x00e9,0x00ea,0x00eb,0x00ec,0x00ed,0x00ee,0x00ef,
0x011f,0x00f1,0x00f2,0x00f3,0x00f4,0x00f5,0x00f6,0x00f7,0x00f8,0x00f9,0x00fa,0x00fb,0x00fc,0x0131,0x015f,0x00ff
};

static const uint8_t codepage_1254_r[128] = {
0x01,0x0d,0x0e,0x0f,0x10,0x1d,0x1e,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,
0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
0x5a,0x5b,0x5c,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,
0x6c,0x6d,0x6e,0x6f,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,
0x7f,0x50,0x70,0x5d,0x7d,0x0c,0x1c,0x5e,0x7e,0x0a,0x1a,0x1f,0x03,0x08,0x18,0x16,
0x17,0x11,0x12,0x02,0x13,0x14,0x04,0x06,0x07,0x15,0x05,0x09,0x0b,0x1b,0x00,0x19
};

static const uint16_t codepage_1255[128] = {
0x20ac,0x0081,0x201a,0x0192,0x201e,0x2026,0x2020,0x2021,0x02c6,0x2030,0x008a,0x2039,0x008c,0x008d,0x008e,0x008f,
0x0090,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0x02dc,0x2122,0x009a,0x203a,0x009c,0x009d,0x009e,0x009f,
0x00a0,0x00a1,0x00a2,0x00a3,0x20aa,0x00a5,0x00a6,0x00a7,0x00a8,0x00a9,0x00d7,0x00ab,0x00ac,0x00ad,0x00ae,0x00af,
0x00b0,0x00b1,0x00b2,0x00b3,0x00b4,0x00b5,0x00b6,0x00b7,0x00b8,0x00b9,0x00f7,0x00bb,0x00bc,0x00bd,0x00be,0x00bf,
0x05b0,0x05b1,0x05b2,0x05b3,0x05b4,0x05b5,0x05b6,0x05b7,0x05b8,0x05b9,0x05ba,0x05bb,0x05bc,0x05bd,0x05be,0x05bf,
0x05c0,0x05c1,0x05c2,0x05c3,0x05f0,0x05f1,0x05f2,0x05f3,0x05f4,0xf88d,0xf88e,0xf88f,0xf890,0xf891,0xf892,0xf893,
0x05d0,0x05d1,0x05d2,0x05d3,0x05d4,0x05d5,0x05d6,0x05d7,0x05d8,0x05d9,0x05da,0x05db,0x05dc,0x05dd,0x05de,0x05df,
0x05e0,0x05e1,0x05e2,0x05e3,0x05e4,0x05e5,0x05e6,0x05e7,0x05e8,0x05e9,0x05ea,0xf894,0xf895,0x200e,0x200f,0xf896
};

static const uint8_t codepage_1255_r[128] = {
0x01,0x0a,0x0c,0x0d,0x0e,0x0f,0x10,0x1a,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,
0x25,0x26,0x27,0x28,0x29,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,
0x36,0x37,0x38,0x39,0x3b,0x3c,0x3d,0x3e,0x3f,0x2a,0x3a,0x03,0x08,0x18,0x40,0x41,
0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,
0x52,0x53,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x54,0x55,0x56,
0x57,0x58,0x7d,0x7e,0x16,0x17,0x11,0x12,0x02,0x13,0x14,0x04,0x06,0x07,0x15,0x05,
0x09,0x0b,0x1b,0x24,0x00,0x19,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x7b,0x7c,0x7f
};

static const uint16_t codepage_1256[128] = {
0x20ac,0x067e,0x201a,0x0192,0x201e,0x2026,0x2020,0x2021,0x02c6,0x2030,0x0679,0x2039,0x0152,0x0686,0x0698,0x0688,
0x06af,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0x06a9,0x2122,0x0691,0x203a,0x0153,0x200c,0x200d,0x06ba,
0x00a0,0x060c,0x00a2,0x00a3,0x00a4,0x00a5,0x00a6,0x00a7,0x00a8,0x00a9,0x06be,0x00ab,0x00ac,0x00ad,0x00ae,0x00af,
0x00b0,0x00b1,0x00b2,0x00b3,0x00b4,0x00b5,0x00b6,0x00b7,0x00b8,0x00b9,0x061b,0x00bb,0x00bc,0x00bd,0x00be,0x061f,
0x06c1,0x0621,0x0622,0x0623,0x0624,0x0625,0x0626,0x0627,0x0628,0x0629,0x062a,0x062b,0x062c,0x062d,0x062e,0x062f,
0x0630,0x0631,0x0632,0x0633,0x0634,0x0635,0x0636,0x00d7,0x0637,0x0638,0x0639,0x063a,0x0640,0x0641,0x0642,0x0643,
0x00e0,0x0644,0x00e2,0x0645,0x0646,0x0647,0x0648,0x00e7,0x00e8,0x00e9,0x00ea,0x00eb,0x0649,0x064a,0x00ee,0x00ef,
0x064b,0x064c,0x064d,0x064e,0x00f4,0x064f,0x0650,0x00f7,0x0651,0x00f9,0x0652,0x00fb,0x00fc,0x200e,0x200f,0x06d2
};

static const uint8_t codepage_1256_r[128] = {
0x20,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,
0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3b,0x3c,0x3d,0x3e,0x57,0x60,0x62,0x67,
0x68,0x69,0x6a,0x6b,0x6e,0x6f,0x74,0x77,0x79,0x7b,0x7c,0x0c,0x1c,0x03,0x08,0x21,
0x3a,0x3f,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,
0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x61,0x63,0x64,0x65,0x66,0x6c,0x6d,0x70,0x71,0x72,0x73,0x75,0x76,0x78,0x7a,0x0a,
0x01,0x0d,0x0f,0x1a,0x0e,0x18,0x10,0x1f,0x2a,0x40,0x7f,0x1d,0x1e,0x7d,0x7e,0x16,
0x17,0x11,0x12,0x02,0x13,0x14,0x04,0x06,0x07,0x15,0x05,0x09,0x0b,0x1b,0x00,0x19
};

static const uint16_t codepage_1257[128] = {
0x20ac,0x0081,0x201a,0x0083,0x201e,0x2026,0x2020,0x2021,0x0088,0x2030,0x008a,0x2039,0x008c,0x00a8,0x02c7,0x00b8,
0x0090,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0x0098,0x2122,0x009a,0x203a,0x009c,0x00af,0x02db,0x009f,
0x00a0,0xf8fc,0x00a2,0x00a3,0x00a4,0xf8fd,0x00a6,0x00a7,0x00d8,0x00a9,0x0156,0x00ab,0x00ac,0x00ad,0x00ae,0x00c6,
0x00b0,0x00b1,0x00b2,0x00b3,0x00b4,0x00b5,0x00b6,0x00b7,0x00f8,0x00b9,0x0157,0x00bb,0x00bc,0x00bd,0x00be,0x00e6,
0x0104,0x012e,0x0100,0x0106,0x00c4,0x00c5,0x0118,0x0112,0x010c,0x00c9,0x0179,0x0116,0x0122,0x0136,0x012a,0x013b,
0x0160,0x0143,0x0145,0x00d3,0x014c,0x00d5,0x00d6,0x00d7,0x0172,0x0141,0x015a,0x016a,0x00dc,0x017b,0x017d,0x00df,
0x0105,0x012f,0x0101,0x0107,0x00e4,0x00e5,0x0119,0x0113,0x010d,0x00e9,0x017a,0x0117,0x0123,0x0137,0x012b,0x013c,
0x0161,0x0144,0x0146,0x00f3,0x014d,0x00f5,0x00f6,0x00f7,0x0173,0x0142,0x015b,0x016b,0x00fc,0x017c,0x017e,0x02d9
};

static const uint8_t codepage_1257_r[128] = {
0x01,0x03,0x08,0x0a,0x0c,0x10,0x18,0x1a,0x1c,0x1f,0x20,0x22,0x23,0x24,0x26,0x27,
0x0d,0x29,0x2b,0x2c,0x2d,0x2e,0x1d,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x0f,
0x39,0x3b,0x3c,0x3d,0x3e,0x44,0x45,0x2f,0x49,0x53,0x55,0x56,0x57,0x28,0x5c,0x5f,
0x64,0x65,0x3f,0x69,0x73,0x75,0x76,0x77,0x38,0x7c,0x42,0x62,0x40,0x60,0x43,0x63,
0x48,0x68,0x47,0x67,0x4b,0x6b,0x46,0x66,0x4c,0x6c,0x4e,0x6e,0x41,0x61,0x4d,0x6d,
0x4f,0x6f,0x59,0x79,0x51,0x71,0x52,0x72,0x54,0x74,0x2a,0x3a,0x5a,0x7a,0x50,0x70,
0x5b,0x7b,0x58,0x78,0x4a,0x6a,0x5d,0x7d,0x5e,0x7e,0x0e,0x7f,0x1e,0x16,0x17,0x11,
0x12,0x02,0x13,0x14,0x04,0x06,0x07,0x15,0x05,0x09,0x0b,0x1b,0x00,0x19,0x21,0x25
};

typedef struct codepage codepage;
typedef uint_fast32_t (readcodepage)(const codepage* p, const char** In);
typedef size_t (writecodepage)(const codepage* p, char** Out, size_t OutLen, uint_fast32_t ch);

struct codepage
{
    int CodePage;
    const tchar_t* Name;
    const uint16_t* UTF;
    const uint8_t* Index;
    readcodepage* Read;
    writecodepage* Write;
};

struct charconv
{
    const codepage* From;
    const codepage* To;
};

static uint_fast32_t Read_Simple(const codepage* p, const char** InPtr)
{
    uint_fast32_t ch;
    assert(p->UTF);
    ch=*(const uint8_t*)(*InPtr);
    ++(*InPtr);
    if (ch & 0x80)
        ch = p->UTF[ch-128];
    return ch;
}

static uint_fast32_t Read_UTF8(const codepage* p, const char** InPtr)
{
    uint_fast32_t ch;
    const uint8_t* In = (const uint8_t*)*InPtr;

    if ((In[0]&0x80)==0) // most of the time
    {
        ++(*InPtr);
        return In[0];
    }

    if ((In[0]&0xe0)==0xc0 && (In[1]&0xc0)==0x80)
    {
        ch=((In[0]&0x1f)<<6)+(In[1]&0x3f);
        In+=2;
    }
    else
    if ((In[0]&0xf0)==0xe0 && (In[1]&0xc0)==0x80 && (In[2]&0xc0)==0x80)
    {
        ch=((In[0]&0x0f)<<12)+((In[1]&0x3f)<<6)+(In[2]&0x3f);
        In+=3;
    }
    else
    if ((In[0]&0xf8)==0xf0 && (In[1]&0xc0)==0x80 && (In[2]&0xc0)==0x80 && (In[3]&0xc0)==0x80)
    {
        ch=((In[0]&0x07)<<18)+((In[1]&0x3f)<<12)+((In[2]&0x3f)<<6)+(In[3]&0x3f);
        In+=4;
    }
    else
    {
        ch=In[0];
        ++In;
    }
    *InPtr = (const char*)In;
    return ch;
}

static size_t Write_Simple(const codepage* p, char** OutPtr, size_t OutLen, uint_fast32_t ch)
{
    assert(p->UTF && p->Index);
    if (OutLen>1)
    {
        if (ch>=128 && (ch>=256 || p->UTF[ch-128]!=ch))
        {
	        intptr_t Mid;
	        intptr_t Lower = 0;
	        intptr_t Upper = 127;
	        while (Upper >= Lower)
	        {
		        Mid = (Upper + Lower) >> 1;
		        if (p->UTF[p->Index[Mid]]>ch)
			        Upper = Mid-1;
		        else if (p->UTF[p->Index[Mid]]<ch)
			        Lower = Mid+1;
		        else
                {
                    ch=p->Index[Mid]+128;
                    goto found;
                }
            }
            ch='?';
        }
found:  *((*OutPtr)++) = (char)ch;
        --OutLen;
    }
    return OutLen;
}

static size_t Write_UTF8(const codepage* p, char** OutPtr, size_t OutLen, uint_fast32_t ch)
{
    uint8_t* Out=(uint8_t*)*OutPtr;
    if (ch<0x80 && OutLen>1)
    {
        *Out=(uint8_t)ch;
        ++Out;
        --OutLen;
    }
    else if (ch<0x800 && OutLen>2)
    {
        Out[0]=(uint8_t)(0xc0|((ch>>6)&0x1f));
        Out[1]=(uint8_t)(0x80|(ch&0x3f));
        Out+=2;
        OutLen-=2;
    }
    else if (ch<0x10000 && OutLen>3)
    {
        Out[0]=(uint8_t)(0xe0|((ch>>12)&0x0f));
        Out[1]=(uint8_t)(0x80|((ch>>6)&0x3f));
        Out[2]=(uint8_t)(0x80|(ch&0x3f));
        Out+=3;
        OutLen-=3;
    }
    else if (OutLen>4)
    {
        Out[0]=(uint8_t)(0xf0|((ch>>18)&0x07));
        Out[1]=(uint8_t)(0x80|((ch>>12)&0x3f));
        Out[2]=(uint8_t)(0x80|((ch>>6)&0x3f));
        Out[3]=(uint8_t)(0x80|(ch&0x3f));
        Out+=4;
        OutLen-=4;
    }
    *OutPtr=(char*)Out;
    return OutLen;
}

void CharConvSS(charconv* CC, char* Out, size_t OutLen, const char* In)
{
    if (OutLen>0)
    {
        if (CC)
        {
            const codepage* From=CC->From;
            const codepage* To=CC->To;
	        while (*In)
            {
		        uint_fast32_t ch = From->Read(From,&In);
                OutLen=To->Write(To,&Out,OutLen,ch);
            }
	        *Out = 0;
        }
        else
        {
	        size_t n = min(strlen(In),OutLen-1);
	        memcpy(Out,In,n*sizeof(char));
	        Out[n] = 0;
        }
    }
}

#if SIZEOF_WCHAR!=2
void CharConvUS(charconv* CC, utf16_t* Out, size_t OutLen, const char* In)
{
	if (OutLen>0)
	{
        const codepage* From=CC->From;
	    while (*In)
        {
		    uint_fast32_t ch = From->Read(From,&In);
            if (ch>=0x10000 && OutLen>2)
            {
                Out[0]=(uint16_t)((((ch-0x10000)>>10)&0x3ff)|0xd800);
                Out[1]=(uint16_t)(((ch-0x10000)&0x3ff)|0xdc00);
                Out+=2;
                OutLen-=2;
            }
            else
            if (OutLen>1)
            {
                *Out=(uint16_t)ch;
                ++Out;
                --OutLen;
            }
        }
	    *Out = 0;
    }
}

void CharConvSU(charconv* CC, char* Out, size_t OutLen, const utf16_t* In)
{
	if (OutLen>0)
	{
        const codepage* To=CC->To;
	    while (*In)
        {
		    uint_fast32_t ch;
            if ((In[0]&~0x3ff)==0xd800 && (In[1]&~0x3ff)==0xdc00)
            {
                ch=(((In[0]&0x3ff)<<10)|(In[1]&0x3ff))+0x10000;
                In+=2;
            }
            else
            {
                ch=*(const uint16_t*)In;
                ++In;
            }
            OutLen=To->Write(To,&Out,OutLen,ch);
        }
	    *Out = 0;
    }
}
#endif

void CharConvWS(charconv* CC, wchar_t* Out, size_t OutLen, const char* In)
{
	if (OutLen>0)
	{
        const codepage* From=CC->From;
	    while (*In && OutLen>1)
        {
		    *Out = (wchar_t)From->Read(From,&In);
            ++Out;
            --OutLen;
        }
	    *Out = 0;
    }
}

void CharConvSW(charconv* CC, char* Out, size_t OutLen, const wchar_t* In)
{
	if (OutLen>0)
	{
        const codepage* To=CC->To;
	    while (*In)
        {
		    uint_fast32_t ch = *(In++);
            OutLen=To->Write(To,&Out,OutLen,ch);
        }
	    *Out = 0;
    }
}

static const codepage CodePage[] =
{
    {1252, T("ISO-8859-1"), codepage_1252,codepage_1252_r, Read_Simple, Write_Simple},
    {1250, T("ISO-8859-2"), codepage_1250,codepage_1250_r, Read_Simple, Write_Simple},
    {1254, T("ISO-8859-3"), codepage_1254,codepage_1254_r, Read_Simple, Write_Simple},
    {1257, T("ISO-8859-4"), codepage_1257,codepage_1257_r, Read_Simple, Write_Simple},
    {1251, T("ISO-8859-5"), codepage_1251,codepage_1251_r, Read_Simple, Write_Simple},
    {1256, T("ISO-8859-6"), codepage_1256,codepage_1256_r, Read_Simple, Write_Simple},
    {1253, T("ISO-8859-7"), codepage_1253,codepage_1253_r, Read_Simple, Write_Simple},
    {1255, T("ISO-8859-8"), codepage_1255,codepage_1255_r, Read_Simple, Write_Simple},
    {65001,T("UTF-8"),      NULL,NULL, Read_UTF8, Write_UTF8},
    {-1,   T("UTF-16"),     NULL,NULL},
    {-1,   T("UTF-32"),     NULL,NULL},
    {0,    NULL},
};

static NOINLINE const codepage* GetCodePage(const tchar_t* Name)
{
    const codepage* i;
    int CP;

    if (!Name)
        Name = "UTF-8";

	if (stscanf(Name,T("CP%d"),&CP)<1 &&
        stscanf(Name,T("windows-%d"),&CP)<1)
        CP=0;

    for (i=CodePage;i->Name;++i)
	    if (tcsisame_ascii(Name,i->Name) || i->CodePage==CP)
            return i;

	return CodePage; //defaults to 1252
}

charconv* CharConvOpen(const tchar_t* From, const tchar_t* To)
{
    charconv* CC = NULL;
    const codepage* FromCP = GetCodePage(From);
    const codepage* ToCP = GetCodePage(To);
    if (FromCP!=ToCP)
    {
        CC = malloc(sizeof(charconv));
        if (CC)
        {
            CC->From = FromCP;
            CC->To = ToCP;
        }
    }
    return CC;
}

void CharConvClose(charconv* p)
{
    free(p);
}

void CharConvDefault(tchar_t* Out, size_t OutLen)
{
    tcscpy_s(Out,OutLen,GetCodePage(T(""))->Name);
}
