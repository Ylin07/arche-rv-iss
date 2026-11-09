#include "rv/csr.h"
#include "rv/trap.h"

static inline void trap_push_s(cpu_t *cpu)
{
    u64 s = CSR_R(cpu, C_MSTATUS);
    s = BITS_SET(s, F_SPIE, BITS(s, F_SIE));
    s = BITS_SET(s, F_SIE,  0);
    s = BITS_SET(s, F_SPP,  cpu->priv);
    CSR_W(cpu, C_MSTATUS, s);
}

static inline void trap_push_m(cpu_t *cpu)
{
    u64 s = CSR_R(cpu, C_MSTATUS);
    s = BITS_SET(s, F_MPIE, BITS(s, F_MIE));
    s = BITS_SET(s, F_MIE,  0);
    s = BITS_SET(s, F_MPP,  cpu->priv);
    CSR_W(cpu, C_MSTATUS, s);
}

static inline void trap_pop_s(cpu_t *cpu)
{
    u64 s = CSR_R(cpu, C_MSTATUS);
    cpu->priv = (priv_t)BITS(s, F_SPP);

    s = BITS_SET(s, F_SIE,  BITS(s, F_SPIE));
    s = BITS_SET(s, F_SPIE, 1);
    s = BITS_SET(s, F_SPP,  U);
    CSR_W(cpu, C_MSTATUS, s);
}

static inline void trap_pop_m(cpu_t *cpu)
{
    u64 s = CSR_R(cpu, C_MSTATUS);
    cpu->priv = (priv_t)BITS(s, F_MPP);

    s = BITS_SET(s, F_MIE,  BITS(s, F_MPIE));
    s = BITS_SET(s, F_MPIE, 1);
    s = BITS_SET(s, F_MPP,  U);
    CSR_W(cpu, C_MSTATUS, s);
}

void cpu_trap(cpu_t *cpu, u64 cause, u64 tval)
{
    u64 deleg = (i64)cause < 0 ? CSR_R(cpu, C_MIDELEG) : CSR_R(cpu, C_MEDELEG);
    bool to_s = (cpu->priv < M) && ((deleg >> (cause & 63)) & 1);

    if (to_s) {
        CSR_W(cpu, C_SEPC,   cpu->pc);
        CSR_W(cpu, C_SCAUSE, cause);
        CSR_W(cpu, C_STVAL,  tval);

        trap_push_s(cpu);
        cpu->priv = S;
        cpu->pc   = CSR_R(cpu, C_STVEC) & ~3ULL;
    } else {
        CSR_W(cpu, C_MEPC,   cpu->pc);
        CSR_W(cpu, C_MCAUSE, cause);
        CSR_W(cpu, C_MTVAL,  tval);

        trap_push_m(cpu);
        cpu->priv = M;
        cpu->pc   = CSR_R(cpu, C_MTVEC) & ~3ULL;
    }
}

void cpu_xret(cpu_t *cpu, priv_t from)
{
    if (cpu->priv < from) {
        cpu_trap(cpu, ILLEGAL_INSN, 0);
        return;
    }

    if (from == S) {
        trap_pop_s(cpu);
        cpu->pc = CSR_R(cpu, C_SEPC);
    } else {
        trap_pop_m(cpu);
        cpu->pc = CSR_R(cpu, C_MEPC);
    }
}
