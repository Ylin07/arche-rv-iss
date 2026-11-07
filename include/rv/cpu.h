#pragma once

#include "rv/types.h"

typedef struct {
    u64 pc;
    u64 regs[32];
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
