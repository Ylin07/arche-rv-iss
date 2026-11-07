#pragma once


#define MASK(len) \
    ((len) >= 64 ? ~0ULL : ((1ULL << (len)) - 1ULL))

#define SEXT(v, len) \
    ((i64)((u64)(v) << (64 - (len))) >> (64 - (len)))

#define _BITS(v, hi, lo) \
    (((u64)(v) >> (lo)) & MASK((hi) - (lo) + 1))

#define BITS(v, ...) \
    _BITS(v, __VA_ARGS__)

#define _BITS_SET(v, hi, lo, val) \
    (((u64)(v) & ~(MASK((hi) - (lo) + 1) << (lo))) | (((u64)(val) & MASK((hi) - (lo) + 1)) << (lo)))

#define BITS_SET(v, ...) \
    _BITS_SET(v, __VA_ARGS__)
