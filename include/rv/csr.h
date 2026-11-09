#pragma once

#include "rv/bits.h"
#include "rv/types.h"
#include "rv/cpu.h"

enum {
    C_CYCLE     = 0xC00,
    C_TIME      = 0xC01,
    C_INSTRET   = 0xC02,

    C_SSTATUS   = 0x100,
    C_SIE       = 0x104,
    C_STVEC     = 0x105,
    C_SSCRATCH  = 0x140,
    C_SEPC      = 0x141,
    C_SCAUSE    = 0x142,
    C_STVAL     = 0x143,
    C_SIP       = 0x144,
    C_SATP      = 0x180,

    C_MSTATUS   = 0x300,
    C_MEDELEG   = 0x302,
    C_MIDELEG   = 0x303,
    C_MIE       = 0x304,
    C_MTVEC     = 0x305,
    C_MSCRATCH  = 0x340,
    C_MEPC      = 0x341,
    C_MCAUSE    = 0x342,
    C_MTVAL     = 0x343,
    C_MIP       = 0x344,
    C_MHARTID   = 0xF14,
};

#define SSTATUS_MASK \
    ((1ULL << 1) | (1ULL << 5) | (1ULL << 8) | (3ULL << 13) | \
     (3ULL << 15) | (1ULL << 18) | (1ULL << 19))

#define SIE_MASK ((1ULL << 1) | (1ULL << 5) | (1ULL << 9))
#define SIP_MASK SIE_MASK

static inline u16 csr_alias(u16 csr, u64 *mask)
{
    switch (csr) {
    case C_SSTATUS: *mask = SSTATUS_MASK; return C_MSTATUS;
    case C_SIE:     *mask = SIE_MASK;     return C_MIE;
    case C_SIP:     *mask = SIP_MASK;     return C_MIP;
    default:        *mask = ~0ULL;        return csr;
    }
}

static inline u64 csr_read(const cpu_t *cpu, u16 csr)
{
    u64 mask;
    u16 target = csr_alias(csr, &mask);

    if (csr == C_CYCLE)     return cpu->cycle;
    if (csr == C_TIME)      return cpu->cycle;
    if (csr == C_INSTRET)   return cpu->instret;
    if (csr == C_MHARTID)   return cpu->hartid;

    return cpu->csrs[target] & mask;
}

static inline void csr_write(cpu_t *cpu, u16 csr, u64 val)
{
    u64 mask;
    u16 target = csr_alias(csr, &mask);
    cpu->csrs[target] = (cpu->csrs[target] & ~mask) | (val & mask);
}

// Field in SSTATUS / MSTATUS
#define F_SIE       1, 1
#define F_MIE       3, 3
#define F_SPIE      5, 5
#define F_MPIE      7, 7
#define F_SPP       8, 8
#define F_MPP      12, 11

#define CSR_PRIV(c)     ((priv_t)BITS((c), 9, 8))
#define CSR_RO(c)       (BITS((c), 11, 10) == 3)

#define CSR_R(cpu, c)       csr_read(cpu, (c))
#define CSR_W(cpu, c, v)    csr_write(cpu, (c), (u64)(v))

bool csr_op(cpu_t *cpu, u16 csr, u64 *rval, u64 wval, u8 op);