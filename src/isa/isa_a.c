#include "internal.h"
#include "rv/mem.h"
#include "rv/trap.h"

static u64 amo_calc(u64 old, u64 src, bool is_signed, u32 op)
{
    switch (op) {
    // AMOSWAP
    MATCH(0, return src);
    // AMOADD
    MATCH(1, return old + src);
    // AMOXOR
    MATCH(2, return old ^ src);
    // AMOAND
    MATCH(3, return old & src);
    // AMOOR
    MATCH(4, return old | src);
    // AMOMIN / AMOMINU
    MATCH(5, return is_signed
                    ? ((i64)old < (i64)src ? old : src)
                    : (old < src ? old : src));
    // AMOMAX / AMOMAXU
    MATCH(6, return is_signed
                    ? ((i64)old > (i64)src ? old : src)
                    : (old > src ? old : src));
    DEFAULT(return old);
    }
}

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
        MATCH(0x01, res = amo_calc(old, src, true, 0));
        // AMOADD.W 
        MATCH(0x00, res = amo_calc(old, src, true, 1));
        // AMOXOR.W 
        MATCH(0x04, res = amo_calc(old, src, true, 2));
        // AMOAND.W 
        MATCH(0x0c, res = amo_calc(old, src, true, 3));
        // AMOOR.W 
        MATCH(0x08, res = amo_calc(old, src, true, 4));
        // AMOMIN.W 
        MATCH(0x10, res = amo_calc((i32)old, (i32)src, true, 5));
        // AMOMAX.W 
        MATCH(0x14, res = amo_calc((i32)old, (i32)src, true, 6));
        // AMOMINU.W 
        MATCH(0x18, res = amo_calc(old, src, false, 5));
        // AMOMAXU.W        
        MATCH(0x1c, res = amo_calc(old, src, false, 6));
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
        MATCH(0x01, res = amo_calc(old, src, true, 0));
        // AMOADD.D
        MATCH(0x00, res = amo_calc(old, src, true, 1));
        // AMOXOR.D
        MATCH(0x04, res = amo_calc(old, src, true, 2));
        // AMOAND.D
        MATCH(0x0c, res = amo_calc(old, src, true, 3));
        // AMOOR.D
        MATCH(0x08, res = amo_calc(old, src, true, 4));
        // AMOMIN.D
        MATCH(0x10, res = amo_calc(old, src, true, 5));
        // AMOMAX.D
        MATCH(0x14, res = amo_calc(old, src, true, 6));
        // AMOMINU.D
        MATCH(0x18, res = amo_calc(old, src, false, 5));
        // AMOMAXU.D 
        MATCH(0x1c, res = amo_calc(old, src, false, 6));
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
