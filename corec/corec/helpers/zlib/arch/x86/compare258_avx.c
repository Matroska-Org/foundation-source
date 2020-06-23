/* compare258_avx.c -- AVX2 version of compare258
 * Copyright Mika T. Lindqvist  <postmaster@raasu.org>
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#include "../../zbuild.h"
#include "../../zutil.h"

#include "fallback_builtins.h"

#if defined(X86_AVX2) && defined(HAVE_BUILTIN_CTZ)

#include <immintrin.h>
#ifdef _MSC_VER
#  include <nmmintrin.h>
#endif

/* UNALIGNED_OK, AVX2 intrinsic comparison */
static inline int32_t compare256_unaligned_avx2_static(const unsigned char *src0, const unsigned char *src1) {
    int32_t len = 0;

    do {
        __m256i ymm_src0, ymm_src1, ymm_cmp;
        ymm_src0 = _mm256_loadu_si256((__m256i*)src0);
        ymm_src1 = _mm256_loadu_si256((__m256i*)src1);
        ymm_cmp = _mm256_cmpeq_epi8(ymm_src0, ymm_src1); /* non-identical bytes = 00, identical bytes = FF */
        int mask = _mm256_movemask_epi8(ymm_cmp);
        if ((unsigned int)mask != 0xFFFFFFFF) {
            int match_byte = __builtin_ctz(~mask); /* Invert bits so identical = 0 */
            return (int32_t)(len + match_byte);
        }

        src0 += 32, src1 += 32, len += 32;

        ymm_src0 = _mm256_loadu_si256((__m256i*)src0);
        ymm_src1 = _mm256_loadu_si256((__m256i*)src1);
        ymm_cmp = _mm256_cmpeq_epi8(ymm_src0, ymm_src1);
        mask = _mm256_movemask_epi8(ymm_cmp);
        if ((unsigned int)mask != 0xFFFFFFFF) {
            int match_byte = __builtin_ctz(~mask);
            return (int32_t)(len + match_byte);
        }

        src0 += 32, src1 += 32, len += 32;
    } while (len < 256);

    return 256;
}

static inline int32_t compare258_unaligned_avx2_static(const unsigned char *src0, const unsigned char *src1) {
    if (*(uint16_t *)src0 != *(uint16_t *)src1)
        return (*src0 == *src1);

    return compare256_unaligned_avx2_static(src0+2, src1+2) + 2;
}

int32_t compare258_unaligned_avx2(const unsigned char *src0, const unsigned char *src1) {
    return compare258_unaligned_avx2_static(src0, src1);
}

#define LONGEST_MATCH   longest_match_unaligned_avx2
#define COMPARE256      compare256_unaligned_avx2_static
#define COMPARE258      compare258_unaligned_avx2_static

#include "match_tpl.h"

#endif
