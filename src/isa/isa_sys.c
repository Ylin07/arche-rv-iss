#include "internal.h"
#include "rv/trap.h"
#include "rv/csr.h"

u64 exec_system(cpu_t *cpu, u32 inst, u64 next_pc)
{
    bool trapped = false;

    switch (F3) {
    // PRIVILEGED / SYSTEM TRAPS
    MATCH(0x0, {
        switch (inst) {
        // ECALL
        MATCH(0x00000073, cpu_trap(cpu, ECALL_BASE + cpu->priv, 0));
        // EBREAK
        MATCH(0x00100073, cpu_trap(cpu, BREAKPOINT, 0));
        // SRET
        MATCH(0x10200073, cpu_xret(cpu, S));
        // MRET
        MATCH(0x30200073, cpu_xret(cpu, M));
        // WFI
        MATCH(0x10500073, (void)0);
        // SFENCE.VMA
        MATCH(0x12000073, (void)0);
        DEFAULT(cpu_trap(cpu, ILLEGAL_INSN, inst));
        }
        trapped = (inst != 0x12000073 && inst != 0x10500073);
    });

    // RESERVED
    MATCH(0x4, {
        cpu_trap(cpu, ILLEGAL_INSN, inst);
        trapped = true;
    });

    // CSR ACCESS (CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI)
    DEFAULT({
        u16 csr  = BITS(inst, 31, 20);
        u8  op   = F3 & 0x3;
        u64 wval = (F3 & 0x4) ? RS1_I : RS1;
        u64 old  = 0;

        u8 real_op = (op == 1 || wval != 0) ? op : 0;

        if (!csr_op(cpu, csr, &old, wval, real_op)) {
            cpu_trap(cpu, ILLEGAL_INSN, inst);
            trapped = true;
        } else {
            SET_RD(old);
        }
    });
    }

    return trapped ? cpu->pc : next_pc;
}