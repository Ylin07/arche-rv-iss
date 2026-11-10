#include "internal.h"
#include "rv/mem.h"
#include "rv/trap.h"

u64 exec_branch(cpu_t *cpu, u32 inst, u64 next_pc)
{
    bool take = false;
    switch (F3) {
    // BEQ
    MATCH(0x0, take = (RS1 == RS2));
    // BNE
    MATCH(0x1, take = (RS1 != RS2));
    // BLT
    MATCH(0x4, take = ((i64)RS1 < (i64)RS2));
    // BGE
    MATCH(0x5, take = ((i64)RS1 >= (i64)RS2));
    // BLTU
    MATCH(0x6, take = ((u64)RS1 < (u64)RS2));
    // BGEU
    MATCH(0x7, take = ((u64)RS1 >= (u64)RS2));
    DEFAULT((void)0);
    }
    if (!take) return next_pc;

    u64 target = cpu->pc + IMM_B;
    if (target & 3) {
        cpu_trap(cpu, INST_ADDR_MISALIGNED, target);
        return cpu->pc;
    }
    return target;
}

void exec_load(cpu_t *cpu, u32 inst)
{
    switch (F3) {
    // LB
    MATCH(0x0, SET_RD((i8)mem_read8(PA_I)));
    // LH
    MATCH(0x1, SET_RD((i16)mem_read16(PA_I)));
    // LW
    MATCH(0x2, SET_RD((i32)mem_read32(PA_I)));
    // LD
    MATCH(0x3, SET_RD(mem_read64(PA_I)));
    // LBU
    MATCH(0x4, SET_RD((u64)mem_read8(PA_I)));
    // LHU
    MATCH(0x5, SET_RD((u64)mem_read16(PA_I)));
    // LWU
    MATCH(0x6, SET_RD((u64)mem_read32(PA_I)));
    DEFAULT((void)0);
    }
}

void exec_store(cpu_t *cpu, u32 inst)
{
    cpu->res_valid = false;
    switch (F3) {
    // SB
    MATCH(0x0, mem_write8(PA_S, (u8)RS2));
    // SH
    MATCH(0x1, mem_write16(PA_S, (u16)RS2));
    // SW
    MATCH(0x2, mem_write32(PA_S, (u32)RS2));
    // SD
    MATCH(0x3, mem_write64(PA_S, (u64)RS2));
    DEFAULT((void)0);
    }
}

void exec_op_imm(cpu_t *cpu, u32 inst)
{
    switch (F3) {
    // ADDI
    MATCH(0x0, SET_RD(RS1 + IMM_I));
    // SLLI
    MATCH(0x1, SET_RD(RS1 << (IMM_I & 0x3f)));
    // SLTI
    MATCH(0x2, SET_RD((i64)RS1 < IMM_I));
    // SLTIU
    MATCH(0x3, SET_RD((u64)RS1 < (u64)IMM_I));
    // XORI
    MATCH(0x4, SET_RD(RS1 ^ IMM_I));
    // SRLI / SRAI
    MATCH(0x5, SET_RD((F7 & 0x20) ? (u64)((i64)RS1 >> (IMM_I & 0x3f))
                                  : ((u64)RS1 >> (IMM_I & 0x3f))));
    // ORI
    MATCH(0x6, SET_RD(RS1 | IMM_I));
    // ANDI
    MATCH(0x7, SET_RD(RS1 & IMM_I));
    DEFAULT((void)0);
    }
}

void exec_op_imm32(cpu_t *cpu, u32 inst)
{
    switch (F3) {
    // ADDIW
    MATCH(0x0, SET_RD((i32)(RS1 + IMM_I)));
    // SLLIW
    MATCH(0x1, SET_RD((i32)((u32)RS1 << (IMM_I & 0x1f))));
    // SRLIW / SRAW
    MATCH(0x5, SET_RD((F7 & 0x20) ? ((i32)RS1 >> (IMM_I & 0x1f))
                                  : ((i32)((u32)RS1 >> (IMM_I & 0x1f)))));
    DEFAULT((void)0);
    }
}

void exec_op_i(cpu_t *cpu, u32 inst)
{
    switch (F3) {
    // ADD / SUB
    MATCH(0x0, SET_RD((F7 & 0x20) ? (RS1 - RS2) : (RS1 + RS2)));
    // SLL
    MATCH(0x1, SET_RD(RS1 << (RS2 & 0x3f)));
    // SLT
    MATCH(0x2, SET_RD((i64)RS1 < (i64)RS2));
    // SLTU
    MATCH(0x3, SET_RD((u64)RS1 < (u64)RS2));
    // XOR
    MATCH(0x4, SET_RD(RS1 ^ RS2));
    // SRL / SRA
    MATCH(0x5, SET_RD((F7 & 0x20) ? (u64)((i64)RS1 >> (RS2 & 0x3f))
                                  : ((u64)RS1 >> (RS2 & 0x3f))));
    // OR
    MATCH(0x6, SET_RD(RS1 | RS2));
    // AND
    MATCH(0x7, SET_RD(RS1 & RS2));
    DEFAULT((void)0);
    }
}

void exec_op_i32(cpu_t *cpu, u32 inst)
{
    switch (F3) {
    // ADDW / SUBW
    MATCH(0x0, SET_RD((F7 & 0x20) ? (i32)(RS1 - RS2) : (i32)(RS1 + RS2)));
    // SLLW
    MATCH(0x1, SET_RD((i32)((u32)RS1 << (RS2 & 0x1f))));
    // SRLW / SRAW
    MATCH(0x5, SET_RD((F7 & 0x20) ? ((i32)RS1 >> (RS2 & 0x1f))
                                  : ((i32)((u32)RS1 >> (RS2 & 0x1f)))));
    DEFAULT((void)0);
    }
}
