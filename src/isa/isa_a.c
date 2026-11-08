#include "internal.h"
#include "rv/mem.h"

#define AMO_CALC(old, src, is_signed, op) ({                                \
    (op == 0) ? (src) :                                                     \
    (op == 1) ? ((old) + (src)) :                                           \
    (op == 2) ? ((old) ^ (src)) :                                           \
    (op == 3) ? ((old) & (src)) :                                           \
    (op == 4) ? ((old) | (src)) :                                           \
    (op == 5) ? ((is_signed ? (i64)(old) < (i64)(src) : (u64)(old) < (u64)(src)) ? (old) : (src)) : \
                ((is_signed ? (i64)(old) > (i64)(src) : (u64)(old) > (u64)(src)) ? (old) : (src));  \
})

void exec_op_a(cpu_t *cpu, u32 inst)
{
    u64 addr = PA_A;

    switch (F3) {
    case 0x2: {
        if (addr & 0x3) {
            cpu_trap(cpu, (F5 == 0x2) ? 4 : 6, addr); // 4: Load, 6: Store/AMO misaligned
            return;
        }
        // LR.W
        if (F5 == 0x2) {
            cpu->res_addr  = addr;
            cpu->res_valid = true;
            SET_RD((i64)(i32)mem_read32(addr));
            return;
        }
        // SC.W
        if (F5 == 0x3) {
            if (cpu->res_valid && cpu->res_addr == addr) {
                mem_write32(addr, (u32)RS2);
                SET_RD(0);
            } else {
                SET_RD(1);
            }
            cpu->res_valid = false;
            return;
        }
        // AMO.W 
        u32 old = mem_read32(addr);
        u32 src = (u32)RS2;
        u32 res;

        switch (F5) {
        // AMOSWAP.W
        MATCH(0x01, res = AMO_CALC(old, src, 1, 0));
        // AMOADD.W 
        MATCH(0x00, res = AMO_CALC(old, src, 1, 1));
        // AMOXOR.W 
        MATCH(0x04, res = AMO_CALC(old, src, 1, 2));
        // AMOAND.W 
        MATCH(0x0c, res = AMO_CALC(old, src, 1, 3));
        // AMOOR.W 
        MATCH(0x08, res = AMO_CALC(old, src, 1, 4));
        // AMOMIN.W 
        MATCH(0x10, res = AMO_CALC((i32)old, (i32)src, 1, 5));
        // AMOMAX.W 
        MATCH(0x14, res = AMO_CALC((i32)old, (i32)src, 1, 6));
        // AMOMINU.W 
        MATCH(0x18, res = AMO_CALC(old, src, 0, 5));
        // AMOMAXU.W        
        MATCH(0x1c, res = AMO_CALC(old, src, 0, 6));           
        DEFAULT(cpu_trap(cpu, ILLEGAL_INSN, inst); return);
        }
        mem_write32(addr, res);
        cpu->res_valid = false;
        SET_RD((i64)(i32)old);
        break;
    }

    case 0x3: {
        if (addr & 0x7) {
            cpu_trap(cpu, (F5 == 0x2) ? 4 : 6, addr);
            return;
        }
        // LR.D
        if (F5 == 0x2) {
            cpu->res_addr  = addr;
            cpu->res_valid = true;
            SET_RD(mem_read64(addr));
            return;
        }
        // SC.D
        if (F5 == 0x3) {
            if (cpu->res_valid && cpu->res_addr == addr) {
                mem_write64(addr, (u64)RS2);
                SET_RD(0);
            } else {
                SET_RD(1);
            }
            cpu->res_valid = false;
            return;
        }
        // AMO.D 
        u64 old = mem_read64(addr);
        u64 src = (u64)RS2;
        u64 res;

        switch (F5) {
        // AMOSWAP.D
        MATCH(0x01, res = AMO_CALC(old, src, 1, 0)); 
        // AMOADD.D
        MATCH(0x00, res = AMO_CALC(old, src, 1, 1)); 
        // AMOXOR.D
        MATCH(0x04, res = AMO_CALC(old, src, 1, 2)); 
        // AMOAND.D
        MATCH(0x0c, res = AMO_CALC(old, src, 1, 3)); 
        // AMOOR.D
        MATCH(0x08, res = AMO_CALC(old, src, 1, 4)); 
        // AMOMIN.D
        MATCH(0x10, res = AMO_CALC((i64)old, (i64)src, 1, 5)); 
        // AMOMAX.D
        MATCH(0x14, res = AMO_CALC((i64)old, (i64)src, 1, 6)); 
        // AMOMINU.D
        MATCH(0x18, res = AMO_CALC(old, src, 0, 5));          
        // AMOMAXU.D 
        MATCH(0x1c, res = AMO_CALC(old, src, 0, 6));           
        DEFAULT(cpu_trap(cpu, ILLEGAL_INSN, inst); return);
        }
        mem_write64(addr, res);
        cpu->res_valid = false;
        SET_RD(old);
        break;
    }

    DEFAULT(cpu_trap(cpu, ILLEGAL_INSN, inst));
    }
}
