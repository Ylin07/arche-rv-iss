#pragma once

#define MASK64(len) \
    ((len) >= 64 ? UINT64_MAX : ((UINT64_C(1) << (len)) - 1))

#define BITS(v, hi, lo) ({                          \
    u64 _v  = (u64)(v);                             \
    unsigned _hi = (unsigned)(hi);                  \
    unsigned _lo = (unsigned)(lo);                  \
    (_v >> _lo) & MASK64(_hi - _lo + 1);            \
})

#define SEXT(v, len) ({                             \
    u64 _v = (u64)(v);                              \
    unsigned _len = (unsigned)(len);                \
    unsigned _shift = 64 - _len;                    \
    (i64)(_v << _shift) >> _shift;                  \
})
