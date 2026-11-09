#pragma once

#include "rv/types.h"

// CPU modes
typedef enum { U = 0b00, S = 0b01, M = 0b11 } priv_t;

typedef struct cpu {
    u64 pc;
    u64 regs[32];
    u64 csrs[4096];
    u64 hartid;
    u64 cycle;
    u64 instret;
    priv_t priv;
    u64  res_addr;
    bool res_valid;
    bool trapped;
} cpu_t;

void cpu_reset(cpu_t *cpu, u64 reset_pc);
void cpu_step(cpu_t *cpu);
void cpu_run(cpu_t *cpu, u64 count);

static inline u64 cpu_get_reg(const cpu_t *cpu, unsigned reg)
{
    return cpu->regs[reg];
}

static inline void cpu_set_reg(cpu_t *cpu, unsigned reg, u64 val)
{
    if (reg != 0)
        cpu->regs[reg] = val;
}
