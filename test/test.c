#include <stdint.h>

typedef uint64_t u64;
typedef int64_t i64;

int test_main(void);

__attribute__((naked, noreturn, section(".text.init")))
void _start(void)
{
    __asm__ volatile (
        "la sp, stack_top\n"
        "la t0, trap_entry\n"
        "csrw mtvec, t0\n"
        "call test_main\n"
        "li t0, 0x80001000\n"
        "mv t1, a0\n"
        "bnez t1, 1f\n"
        "li t1, 1\n"
        "1: sd t1, 0(t0)\n"
        "2: j 2b\n"
        ".align 2\n"
        "trap_entry:\n"
        "csrr t1, mcause\n"
        "li t0, 0x80001000\n"
        "sd t1, 0(t0)\n"
        "3: j 3b\n"
    );
}

#define ASM_R(name, insn) \
static u64 name(u64 a, u64 b) { u64 r; __asm__ volatile (insn : "=r"(r) : "r"(a), "r"(b)); return r; }
#define ASM_I(name, insn) \
static u64 name(u64 a, u64 b) { u64 r; __asm__ volatile (insn : "=r"(r) : "r"(a), "r"(b)); return r; }

ASM_R(add, "add %0, %1, %2")
ASM_R(sub, "sub %0, %1, %2")
ASM_R(sll, "sll %0, %1, %2")
ASM_R(srl, "srl %0, %1, %2")
ASM_R(sra, "sra %0, %1, %2")
ASM_R(slt, "slt %0, %1, %2")
ASM_R(sltu, "sltu %0, %1, %2")
ASM_R(xor_, "xor %0, %1, %2")
ASM_R(or_, "or %0, %1, %2")
ASM_R(and_, "and %0, %1, %2")
ASM_R(mul, "mul %0, %1, %2")
ASM_R(div_, "div %0, %1, %2")
ASM_R(divu, "divu %0, %1, %2")
ASM_R(rem_, "rem %0, %1, %2")
ASM_R(remu, "remu %0, %1, %2")
ASM_R(addw, "addw %0, %1, %2")
ASM_R(subw, "subw %0, %1, %2")
ASM_R(sllw, "sllw %0, %1, %2")
ASM_R(srlw, "srlw %0, %1, %2")
ASM_R(sraw, "sraw %0, %1, %2")

static u64 addi(u64 a, i64 b) { u64 r; __asm__ volatile ("addi %0, %1, %2" : "=r"(r) : "r"(a), "I"(b)); return r; }
static u64 slli(u64 a) { u64 r; __asm__ volatile ("slli %0, %1, 17" : "=r"(r) : "r"(a)); return r; }
static u64 srli(u64 a) { u64 r; __asm__ volatile ("srli %0, %1, 17" : "=r"(r) : "r"(a)); return r; }
static u64 srai(u64 a) { u64 r; __asm__ volatile ("srai %0, %1, 17" : "=r"(r) : "r"(a)); return r; }
static int test_alu(void)
{
    u64 a = 0x8000000000000000ULL, b = 17;
    if (add(5, 3) != 8 || sub(5, 8) != (u64)-3) return 1;
    if (sll(1, 63) != (u64)1 << 63) return 2;
    if (srl((u64)-1, 63) != 1 || sra((u64)-1, 63) != (u64)-1) return 3;
    if (slt(a, b) != 1 || sltu(a, b) != 0) return 4;
    if (xor_(0xaa, 0xff) != 0x55 || or_(0xaa, 0x55) != 0xff) return 5;
    if (and_(0xaa, 0x55) != 0) return 6;
    if (addi(9, -12) != (u64)-3) return 7;
    if (slli(1) != (u64)1 << 17 || srli((u64)1 << 17) != 1) return 8;
    if (srai((u64)-1) != (u64)-1) return 9;
    return 0;
}

static int test_word(void)
{
    u64 high = 0x0000000100000000ULL;
    if (addw(high, 3) != 3) return 1;
    if (subw(1, 2) != (u64)-1) return 2;
    if (sllw(1, 31) != (u64)0xffffffff80000000ULL) return 3;
    if (srlw((u64)-1, 1) != 0x7fffffff) return 4;
    if (sraw((u64)-1, 1) != (u64)-1) return 5;
    return 0;
}

static int test_memory(void)
{
    volatile uint8_t *p8 = (volatile uint8_t *)0x80002000ULL;
    volatile uint16_t *p16 = (volatile uint16_t *)0x80002010ULL;
    volatile uint32_t *p32 = (volatile uint32_t *)0x80002020ULL;
    volatile u64 *p64 = (volatile u64 *)0x80002030ULL;

    *p8 = 0x80; *p16 = 0x8001; *p32 = 0x80000001; *p64 = 0x1122334455667788ULL;
    if (*p8 != 0x80 || *p16 != 0x8001 || *p32 != 0x80000001) return 1;
    if (*p64 != 0x1122334455667788ULL) return 2;
    for (unsigned i = 0; i < 128; i++)
        ((volatile u64 *)0x80003000ULL)[i] = (u64)i * i + 3;
    for (unsigned i = 0; i < 128; i++)
        if (((volatile u64 *)0x80003000ULL)[i] != (u64)i * i + 3) return 3;
    return 0;
}

static int test_m(void)
{
    if (mul(1234567, 7654321) != 9449772114007ULL) return 1;
    if (div_(-123456789, 37) != (u64)-3336669) return 2;
    if (rem_(-123456789, 37) != (u64)-36) return 3;
    if (divu(100, 0) != UINT64_MAX || remu(100, 0) != 100) return 4;
    if (div_((u64)INT64_MIN, (u64)-1) != (u64)INT64_MIN) return 5;
    return 0;
}

static int test_csr(void)
{
    u64 cycle, time, instret, hartid, mstatus;
    __asm__ volatile ("csrr %0, cycle" : "=r"(cycle));
    __asm__ volatile ("csrr %0, time" : "=r"(time));
    __asm__ volatile ("csrr %0, instret" : "=r"(instret));
    __asm__ volatile ("csrr %0, mhartid" : "=r"(hartid));
    __asm__ volatile ("csrr %0, mstatus" : "=r"(mstatus));
    if (hartid != 0 || time < cycle || instret == 0) return 1;
    __asm__ volatile ("csrw mstatus, zero");
    __asm__ volatile ("csrr %0, mstatus" : "=r"(mstatus));
    if (mstatus != 0) return 2;
    return 0;
}

int test_main(void)
{
    int (*tests[])(void) = { test_alu, test_word, test_memory, test_m, test_csr };
    for (unsigned i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
        if (tests[i]()) return (int)i + 1;
    return 0;
}
