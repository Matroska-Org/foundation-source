/*
 * Copyright (c) 2008-2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __LIBEBML_CRC_H
#define __LIBEBML_CRC_H

extern bool_t EBML_CRCMatches(ebml_crc *CRC, const uint8_t *Buf, size_t Size);
extern void EBML_CRCAddBuffer(ebml_crc *CRC, const uint8_t *Buf, size_t Size);
extern void EBML_CRCFinalize(ebml_crc *CRC);

#endif /* __LIBEBML_CRC_H */
