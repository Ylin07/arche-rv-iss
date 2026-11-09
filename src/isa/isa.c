#include "internal.h"
#include "rv/types.h"
#include "rv/trap.h"

u64 isa_exec(cpu_t *cpu, u32 inst)
{
    u64 next_pc = cpu->pc + 4;

    switch (BITS(inst, 6, 0)) {
    // LUI
    MATCH(0x37, SET_RD(IMM_U));                                        
    // AUIPC
    MATCH(0x17, SET_RD(cpu->pc + IMM_U));                              
    // JAL
    MATCH(0x6F, (SET_RD(next_pc), next_pc = cpu->pc + IMM_J));         
    // JALR
    MATCH(0x67, {
        u64 target = (RS1 + IMM_I) & ~1ULL;
        SET_RD(next_pc);
        next_pc = target;
    });
    // BRANCH
    MATCH(0x63, next_pc = exec_branch(cpu, inst, next_pc));            
    // LOAD
    MATCH(0x03, exec_load(cpu, inst));                                 
    // STORE
    MATCH(0x23, exec_store(cpu, inst));                                
    // OP-IMM 
    MATCH(0x13, exec_op_imm(cpu, inst));                               
    // OP-IMM32
    MATCH(0x1B, exec_op_imm32(cpu, inst));                             
    // OP
    MATCH(0x33, (F7 == 0x01) ? exec_op_m(cpu, inst) : exec_op_i(cpu, inst));
    // OP-32
    MATCH(0x3B, (F7 == 0x01) ? exec_op_m32(cpu, inst) : exec_op_i32(cpu, inst));
    // SYSTEM / CSR
    MATCH(0x73, next_pc = exec_system(cpu, inst, next_pc));
    // AMO
    MATCH(0x2F, exec_op_a(cpu, inst));
    // FENCE / FENCE.I
    MATCH(0x0F, (void)0);
    // ILLEGAL INST
    DEFAULT({
        cpu_trap(cpu, ILLEGAL_INSN, inst);
        next_pc = cpu->pc;
    });
    }

    return next_pc;
}

