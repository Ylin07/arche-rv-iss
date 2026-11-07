#pragma once

#include "rv/bits.h"
#include "rv/cpu.h"
#include "rv/types.h"

#define RD_I       BITS(inst, 11, 7)
#define RS1_I      BITS(inst, 19, 15)
#define RS2_I      BITS(inst, 24, 20)
#define F3         BITS(inst, 14, 12)
#define F7         BITS(inst, 31, 25)

#define RS1        cpu_get_reg(cpu, RS1_I)
#define RS2        cpu_get_reg(cpu, RS2_I)
#define SET_RD(v)  cpu_set_reg(cpu, RD_I, (v))

#define IMM_I      SEXT(BITS(inst, 31, 20), 12)
#define IMM_S      SEXT((BITS(inst, 31, 25) << 5) | BITS(inst, 11, 7), 12)
#define IMM_B      SEXT((BITS(inst, 31, 31) << 12) | (BITS(inst, 7, 7) << 11) | \
                        (BITS(inst, 30, 25) << 5)  | (BITS(inst, 11, 8) << 1), 13)
#define IMM_U      SEXT(BITS(inst, 31, 12) << 12, 32)
#define IMM_J      SEXT((BITS(inst, 31, 31) << 20) | (BITS(inst, 19, 12) << 12) | \
                        (BITS(inst, 20, 20) << 11) | (BITS(inst, 30, 21) << 1), 21)

#define PA_I       (RS1 + IMM_I)
#define PA_S       (RS1 + IMM_S)

#define MATCH(pattern, expr)    case (pattern): expr; break
#define DEFAULT(expr)           default: expr; break


u64  exec_branch  (cpu_t *cpu, u32 inst, u64 next_pc);
void exec_load    (cpu_t *cpu, u32 inst);
void exec_store   (cpu_t *cpu, u32 inst);
void exec_op_imm  (cpu_t *cpu, u32 inst);
void exec_op_imm32(cpu_t *cpu, u32 inst);
void exec_op_i    (cpu_t *cpu, u32 inst);
void exec_op_i32  (cpu_t *cpu, u32 inst);
void exec_op_m    (cpu_t *cpu, u32 inst);
void exec_op_m32  (cpu_t *cpu, u32 inst);
u64  exec_system  (cpu_t *cpu, u32 inst, u64 next_pc);
