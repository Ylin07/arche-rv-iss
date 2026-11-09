#pragma once

#include "rv/cpu.h"

#define INTERRUPT_BIT (1ULL << 63)

// Trap causes
enum {
    INST_ADDR_MISALIGNED  = 0,
    INST_ACCESS_FAULT     = 1,
    ILLEGAL_INSN          = 2,
    BREAKPOINT            = 3,

    LOAD_ADDR_MISALIGNED  = 4,
    LOAD_ACCESS_FAULT     = 5,
    STORE_ADDR_MISALIGNED = 6,
    STORE_ACCESS_FAULT    = 7,

    ECALL_BASE = 8,

    INST_PAGE_FAULT  = 12,
    LOAD_PAGE_FAULT  = 13,
    STORE_PAGE_FAULT = 15,
};

void cpu_trap(cpu_t *cpu, u64 cause, u64 tval);
void cpu_xret(cpu_t *cpu, priv_t from);
