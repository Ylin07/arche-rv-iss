#include "rv/cpu.h"
#include "rv/csr.h"

bool cpu_csr_op(cpu_t *cpu, u16 csr, u64 *rval, u64 wval, u8 op)
{
    if (csr >= 4096)
        return false;

    if (cpu->priv < CSR_PRIV(csr) || (CSR_RO(csr) && op != 0))
        return false;

    u64 old = CSR_R(cpu, csr);
    
    // op: 0=read, 1=write, 2=set, 3=clear
    if (op != 0) {
        u64 val = old;
        if (op == 1) val = wval;
        if (op == 2) val = old | wval;
        if (op == 3) val = old & ~wval;

        CSR_W(cpu, csr, val);
    }

    if (rval)
        *rval = old;

    return true;
}