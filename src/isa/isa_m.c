#include "internal.h"
#include "rv/types.h"

void exec_op_m(cpu_t *cpu, u32 inst)
{
    switch (F3) {
    // MUL
    MATCH(0x0, SET_RD(RS1 * RS2));
    // MULH
    MATCH(0x1, SET_RD(((i128)(i64)RS1 * (i128)(i64)RS2) >> 64));
    // MULHSU
    MATCH(0x2, SET_RD(((i128)(i64)RS1 * (u128)RS2) >> 64));
    // MULHU
    MATCH(0x3, SET_RD(((u128)RS1 * (u128)RS2) >> 64));
    // DIV
    MATCH(0x4, SET_RD(RS2 == 0 ? UINT64_MAX : 
               ((i64)RS1 == INT64_MIN && (i64)RS2 == -1) ? RS1 : (u64)((i64)RS1 / (i64)RS2)));
    // DIVU
    MATCH(0x5, SET_RD(RS2 == 0 ? UINT64_MAX : RS1 / RS2));
    // REM
    MATCH(0x6, SET_RD(RS2 == 0 ? RS1 : 
               ((i64)RS1 == INT64_MIN && (i64)RS2 == -1) ? 0 : (u64)((i64)RS1 % (i64)RS2)));
    // REMU
    MATCH(0x7, SET_RD(RS2 == 0 ? RS1 : RS1 % RS2));
    DEFAULT((void)0);
    }
}

void exec_op_m32(cpu_t *cpu, u32 inst)
{
    i32 s1 = (i32)RS1, s2 = (i32)RS2;
    u32 u1 = (u32)RS1, u2 = (u32)RS2;

    switch (F3) {
    // MULW
    MATCH(0x0, SET_RD((i32)(s1 * s2)));
    // DIVW
    MATCH(0x4, SET_RD(s2 == 0 ? -1 : 
               (s1 == INT32_MIN && s2 == -1) ? (i32)INT32_MIN : (i32)(s1 / s2)));
    // DIVUW
    MATCH(0x5, SET_RD(u2 == 0 ? -1 : (i32)(u1 / u2)));
    // REMW
    MATCH(0x6, SET_RD(s2 == 0 ? (i32)s1 : 
               (s1 == INT32_MIN && s2 == -1) ? 0 : (i32)(s1 % s2)));
    // REMUW
    MATCH(0x7, SET_RD(u2 == 0 ? (i32)u1 : (i32)(u1 % u2)));
    DEFAULT((void)0);
    }
}