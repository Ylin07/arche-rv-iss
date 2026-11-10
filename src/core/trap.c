#include "rv/csr.h"
#include "rv/trap.h"

#define TRAP_ENTER(P) do {                                                     \
    CSR_W(cpu, C_##P##EPC,   cpu->pc);                                         \
    CSR_W(cpu, C_##P##CAUSE, cause);                                           \
    CSR_W(cpu, C_##P##TVAL,  tval);                                            \
    u64 s = CSR_R(cpu, C_##P##STATUS);                                         \
    s = BITS_SET(s, F_##P##PIE, BITS(s, F_##P##IE));                           \
    s = BITS_SET(s, F_##P##IE,  0);                                            \
    s = BITS_SET(s, F_##P##PP,  cpu->priv);                                    \
    CSR_W(cpu, C_##P##STATUS, s);                                              \
    cpu->priv = P;                                                             \
    u64 vec = CSR_R(cpu, C_##P##TVEC);                                         \
    cpu->pc = vec & ~3ULL;                                                     \
    if ((vec & 3) == 1 && (cause & INTERRUPT_BIT))                             \
        cpu->pc += 4 * (cause & ~INTERRUPT_BIT);                               \
} while (0)

#define TRAP_RETURN(P) do {                                                    \
    u64 s = CSR_R(cpu, C_##P##STATUS);                                         \
    cpu->priv = (priv_t)BITS(s, F_##P##PP);                                    \
    s = BITS_SET(s, F_##P##IE,  BITS(s, F_##P##PIE));                          \
    s = BITS_SET(s, F_##P##PIE, 1);                                            \
    s = BITS_SET(s, F_##P##PP,  U);                                            \
    CSR_W(cpu, C_##P##STATUS, s);                                              \
    cpu->pc = CSR_R(cpu, C_##P##EPC);                                          \
} while (0)


static inline priv_t trap_target(const cpu_t *cpu, u64 cause)
{
    bool interrupt = cause & INTERRUPT_BIT;
    u64 code = cause & ~INTERRUPT_BIT;
    u64 deleg = interrupt ? CSR_R(cpu, C_MIDELEG) : CSR_R(cpu, C_MEDELEG);

    return (cpu->priv < M && code < 64 && ((deleg >> code) & 1)) ? S : M;
}

bool cpu_check_interrupt(cpu_t *cpu)
{
    u64 ready = CSR_R(cpu, C_MIP) & CSR_R(cpu, C_MIE);
    if (!ready) return false;

    u64 s = CSR_R(cpu, C_MSTATUS);
    u64 deleg = CSR_R(cpu, C_MIDELEG);

    u64 m_mask = (cpu->priv < M || (cpu->priv == M && BITS(s, F_MIE))) ? ~deleg : 0;
    u64 s_mask = (cpu->priv < S || (cpu->priv == S && BITS(s, F_SIE))) ?  deleg : 0;
    u64 pending = ready & (m_mask | s_mask);

    for (unsigned i = 0; i < sizeof(IRQ_PRIO); i++) {
        u8 code = IRQ_PRIO[i];
        if (pending & (1ULL << code)) {
            cpu_trap(cpu, INTERRUPT_BIT | code, 0);
            return true;
        }
    }
    return false;
}

void cpu_trap(cpu_t *cpu, u64 cause, u64 tval)
{
    cpu->trapped = true;
    if (trap_target(cpu, cause) == S) {
        TRAP_ENTER(S);
    } else {
        TRAP_ENTER(M);
    }
}

void cpu_xret(cpu_t *cpu, priv_t from)
{
    if (cpu->priv < from) {
        cpu_trap(cpu, ILLEGAL_INSN, 0);
        return;
    }

    if (from == S) {
        TRAP_RETURN(S);
    } else {
        TRAP_RETURN(M);
    }
}
