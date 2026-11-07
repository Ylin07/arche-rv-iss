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
        "li t0, 0x80006000\n"
        "sd t1, 0(t0)\n"
        "csrr t0, mepc\n"
        "addi t0, t0, 4\n"
        "csrw mepc, t0\n"
        "mret\n"
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
ASM_R(mulh, "mulh %0, %1, %2")
ASM_R(mulhsu, "mulhsu %0, %1, %2")
ASM_R(mulhu, "mulhu %0, %1, %2")
ASM_R(div_, "div %0, %1, %2")
ASM_R(divu, "divu %0, %1, %2")
ASM_R(rem_, "rem %0, %1, %2")
ASM_R(remu, "remu %0, %1, %2")
ASM_R(addw, "addw %0, %1, %2")
ASM_R(subw, "subw %0, %1, %2")
ASM_R(sllw, "sllw %0, %1, %2")
ASM_R(srlw, "srlw %0, %1, %2")
ASM_R(sraw, "sraw %0, %1, %2")
ASM_R(mulw, "mulw %0, %1, %2")
ASM_R(divw, "divw %0, %1, %2")
ASM_R(divuw, "divuw %0, %1, %2")
ASM_R(remw, "remw %0, %1, %2")
ASM_R(remuw, "remuw %0, %1, %2")

static u64 addi(u64 a, i64 b) { u64 r; __asm__ volatile ("addi %0, %1, %2" : "=r"(r) : "r"(a), "I"(b)); return r; }
static u64 slli(u64 a) { u64 r; __asm__ volatile ("slli %0, %1, 17" : "=r"(r) : "r"(a)); return r; }
static u64 srli(u64 a) { u64 r; __asm__ volatile ("srli %0, %1, 17" : "=r"(r) : "r"(a)); return r; }
static u64 srai(u64 a) { u64 r; __asm__ volatile ("srai %0, %1, 17" : "=r"(r) : "r"(a)); return r; }
static u64 slti(u64 a, i64 b) { u64 r; __asm__ volatile ("slti %0, %1, %2" : "=r"(r) : "r"(a), "I"(b)); return r; }
static u64 sltiu(u64 a, i64 b) { u64 r; __asm__ volatile ("sltiu %0, %1, %2" : "=r"(r) : "r"(a), "I"(b)); return r; }
static u64 xori(u64 a, i64 b) { u64 r; __asm__ volatile ("xori %0, %1, %2" : "=r"(r) : "r"(a), "I"(b)); return r; }
static u64 ori(u64 a, i64 b) { u64 r; __asm__ volatile ("ori %0, %1, %2" : "=r"(r) : "r"(a), "I"(b)); return r; }
static u64 andi(u64 a, i64 b) { u64 r; __asm__ volatile ("andi %0, %1, %2" : "=r"(r) : "r"(a), "I"(b)); return r; }
static u64 addiw(u64 a, i64 b) { u64 r; __asm__ volatile ("addiw %0, %1, %2" : "=r"(r) : "r"(a), "I"(b)); return r; }
static u64 slliw(u64 a) { u64 r; __asm__ volatile ("slliw %0, %1, 7" : "=r"(r) : "r"(a)); return r; }
static u64 srliw(u64 a) { u64 r; __asm__ volatile ("srliw %0, %1, 7" : "=r"(r) : "r"(a)); return r; }
static u64 srawi(u64 a) { u64 r; __asm__ volatile ("sraiw %0, %1, 7" : "=r"(r) : "r"(a)); return r; }
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
    if (slti((u64)-1, -1) != 0 || sltiu(0, -1) != 1) return 10;
    if (xori(0xaa, 0xff) != 0x55 || ori(0xaa, 0x55) != 0xff || andi(0xaa, 0x55) != 0) return 11;
    if (addiw(0x100000000ULL, 3) != 3 || slliw(1) != 128 || srliw((u64)-1) != 0x01ffffff || srawi((u64)-1) != (u64)-1) return 12;
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

static u64 call_target(u64 value)
{
    return value * 3 + 1;
}

static int test_control(void)
{
    u64 (*target)(u64) = call_target;
    i64 signed_value = -7;
    u64 unsigned_value = 1ULL << 63;

    if (target(9) != 28) return 1;
    if (!(1 == 1) || 1 != 1) return 2;
    if (!(signed_value < 0) || signed_value >= 0) return 3;
    if (!(unsigned_value >= 1) || unsigned_value < 1) return 4;
    for (unsigned i = 0; i < 256; i++)
        if (i == 255) return 0;
    return 5;
}

static int test_store_widths(void)
{
    volatile uint8_t *p8 = (volatile uint8_t *)0x80005000ULL;
    volatile uint16_t *p16 = (volatile uint16_t *)0x80005010ULL;
    volatile uint32_t *p32 = (volatile uint32_t *)0x80005020ULL;
    volatile u64 *p64 = (volatile u64 *)0x80005030ULL;
    u64 value = 0x1122334455667788ULL;

    __asm__ volatile ("sb %1, 0(%0)" :: "r"(p8), "r"(value));
    __asm__ volatile ("sh %1, 0(%0)" :: "r"(p16), "r"(value));
    __asm__ volatile ("sw %1, 0(%0)" :: "r"(p32), "r"(value));
    __asm__ volatile ("sd %1, 0(%0)" :: "r"(p64), "r"(value));
    if (*p8 != 0x88 || *p16 != 0x7788 || *p32 != 0x55667788)
        return 1;
    if (*p64 != value)
        return 2;
    return 0;
}

static int test_m(void)
{
    if (mul(1234567, 7654321) != 9449772114007ULL) return 1;
    if (div_(-123456789, 37) != (u64)-3336669) return 2;
    if (rem_(-123456789, 37) != (u64)-36) return 3;
    if (divu(100, 0) != UINT64_MAX || remu(100, 0) != 100) return 4;
    if (div_((u64)INT64_MIN, (u64)-1) != (u64)INT64_MIN) return 5;
    if (mulh((u64)-3, 7) != (u64)-1) return 6;
    if (mulhsu((u64)-3, 7) != (u64)-1) return 7;
    if (mulhu((u64)-1, 7) != 6) return 8;
    if (mulw(0x100000002ULL, 3) != 6 || divw((u64)-7, 2) != (u64)-3 || divuw(0xffffffffULL, 2) != 0x7fffffff) return 9;
    if (remw((u64)-7, 2) != (u64)-1 || remuw(0xffffffffULL, 2) != 1) return 10;
    return 0;
}

static int test_load_sign(void)
{
    volatile uint8_t *p8 = (volatile uint8_t *)0x80004000ULL;
    volatile uint16_t *p16 = (volatile uint16_t *)0x80004010ULL;
    volatile uint32_t *p32 = (volatile uint32_t *)0x80004020ULL;
    i64 b, h, w; u64 ub, uh, uw;

    *p8 = 0x80; *p16 = 0x8001; *p32 = 0x80000001;
    __asm__ volatile ("lb %0, 0(%1)" : "=r"(b) : "r"(p8));
    __asm__ volatile ("lh %0, 0(%1)" : "=r"(h) : "r"(p16));
    __asm__ volatile ("lw %0, 0(%1)" : "=r"(w) : "r"(p32));
    __asm__ volatile ("lbu %0, 0(%1)" : "=r"(ub) : "r"(p8));
    __asm__ volatile ("lhu %0, 0(%1)" : "=r"(uh) : "r"(p16));
    __asm__ volatile ("lwu %0, 0(%1)" : "=r"(uw) : "r"(p32));
    if (b != -128 || h != -32767 || w != (i64)0xffffffff80000001ULL) return 1;
    if (ub != 0x80 || uh != 0x8001 || uw != 0x80000001) return 2;
    return 0;
}

static int test_remaining_i(void)
{
    volatile u64 *p = (volatile u64 *)0x80004030ULL;
    u64 v = 0x1122334455667788ULL, got;
    *p = v;
    __asm__ volatile ("ld %0, 0(%1)" : "=r"(got) : "r"(p));
    if (got != v) return 1;
    __asm__ volatile ("fence rw, rw\nfence.i\nsfence.vma" ::: "memory");
    return 0;
}

static int branch_result(const char *op, u64 a, u64 b)
{
    int result;
    if (op[0] == 'e')
        __asm__ volatile ("beq %1, %2, 1f\n li %0, 0\n j 2f\n1: li %0, 1\n2:" : "=r"(result) : "r"(a), "r"(b));
    else if (op[0] == 'n')
        __asm__ volatile ("bne %1, %2, 1f\n li %0, 0\n j 2f\n1: li %0, 1\n2:" : "=r"(result) : "r"(a), "r"(b));
    else if (op[0] == 'l')
        __asm__ volatile ("blt %1, %2, 1f\n li %0, 0\n j 2f\n1: li %0, 1\n2:" : "=r"(result) : "r"(a), "r"(b));
    else if (op[0] == 'g')
        __asm__ volatile ("bge %1, %2, 1f\n li %0, 0\n j 2f\n1: li %0, 1\n2:" : "=r"(result) : "r"(a), "r"(b));
    else if (op[0] == 'u')
        __asm__ volatile ("bltu %1, %2, 1f\n li %0, 0\n j 2f\n1: li %0, 1\n2:" : "=r"(result) : "r"(a), "r"(b));
    else
        __asm__ volatile ("bgeu %1, %2, 1f\n li %0, 0\n j 2f\n1: li %0, 1\n2:" : "=r"(result) : "r"(a), "r"(b));
    return result;
}

static int test_all_branches(void)
{
    if (!branch_result("e", 3, 3) || branch_result("n", 3, 3)) return 1;
    if (!branch_result("l", (u64)-1, 1) || !branch_result("g", 1, (u64)-1)) return 2;
    if (!branch_result("u", 1, 2) || !branch_result("x", 2, 1)) return 3;
    return 0;
}

static int test_branches(void)
{
    volatile int taken = 0; i64 neg = -1; u64 high = 1ULL << 63;
    if (1 != 2) taken++;
    if (neg < 0) taken++;
    if (high >= 1) taken++;
    for (unsigned i = 0; i < 100; i++) if (i == 99) taken++;
    return taken == 4 ? 0 : 1;
}

static int test_csr_alias(void)
{
    u64 value;
    __asm__ volatile ("li t0, 2\ncsrw sstatus, t0");
    __asm__ volatile ("csrr %0, mstatus" : "=r"(value));
    __asm__ volatile ("csrw mstatus, zero");
    return (value & 2) ? 0 : 1;
}

static int test_csr_ops(void)
{
    u64 old, value;
    __asm__ volatile ("csrw mstatus, zero\ncsrrs %0, mstatus, %1" : "=r"(old) : "r"(2));
    __asm__ volatile ("csrr %0, mstatus" : "=r"(value));
    if (old != 0 || (value & 2) == 0) return 1;
    __asm__ volatile ("csrrc %0, mstatus, %1" : "=r"(old) : "r"(2));
    if ((old & 2) == 0) return 2;
    __asm__ volatile ("csrwi mstatus, 3\ncsrr %0, mstatus" : "=r"(value));
    if ((value & 3) != 3) return 3;
    __asm__ volatile ("csrrsi %0, mstatus, 4" : "=r"(old));
    __asm__ volatile ("csrrci %0, mstatus, 4" : "=r"(old));
    return 0;
}

static int test_trap(void)
{
    volatile u64 *cause = (volatile u64 *)0x80006000ULL;

    *cause = 0;
    __asm__ volatile ("ecall");
    if (*cause != 11)
        return 1;

    *cause = 0;
    __asm__ volatile ("ebreak");
    if (*cause != 3)
        return 2;
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
    int (*tests[])(void) = { test_alu, test_word, test_memory, test_control,
                             test_store_widths, test_load_sign, test_branches,
                             test_m, test_remaining_i, test_all_branches,
                             test_csr, test_csr_alias, test_csr_ops, test_trap };
    for (unsigned i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
        if (tests[i]()) return (int)i + 1;
    return 0;
}
