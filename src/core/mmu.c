#include "rv/mmu.h"
#include "rv/csr.h"
#include "rv/mem.h"
#include "rv/trap.h"

u64 va2pa(cpu_t *cpu, u64 va, mmu_access_t access)
{
    u64 status = CSR_R(cpu, C_MSTATUS);
    u64 satp = CSR_R(cpu, C_SATP);
    u64 mode = BITS(satp, F_SATP_MODE);
    priv_t priv = cpu->priv;

    if (priv == M && access != MMU_FETCH && BITS(status, F_MPRV))
        priv = (priv_t)BITS(status, F_MPP);

    if (priv == M || mode == SATP_BARE)
        return va;

    if (mode != SATP_SV39 || (u64)SEXT(va, 39) != va)
        goto page_fault;

    u64 table = BITS(satp, F_SATP_PPN) << 12;
    for (int level = 2; level >= 0; level--) {
        u64 index = (va >> (12 + 9 * level)) & 0x1ff;
        u64 addr = table + index * 8;

        if (addr < DRAM_BASE || addr - DRAM_BASE > DRAM_SIZE - 8)
            goto access_fault;
        u64 pte = mem_read64(addr);
        if (!(pte & PTE_V) || (!(pte & PTE_R) && (pte & PTE_W)) || BITS(pte, 63, 54))
            goto page_fault;

        u64 base = BITS(pte, 53, 10) << 12;
        if (!(pte & (PTE_R | PTE_X))) {
            if (pte & (PTE_U | PTE_A | PTE_D))
                goto page_fault;
            table = base;
            continue;
        }

        if (priv == U && !(pte & PTE_U))
            goto page_fault;
        if (priv == S && (pte & PTE_U) &&
            (access == MMU_FETCH || !BITS(status, F_SUM)))
            goto page_fault;

        switch (access) {
        case MMU_FETCH:
            if (!(pte & PTE_X)) goto page_fault;
            break;
        case MMU_LOAD:
            if (!(pte & PTE_R) && !(BITS(status, F_MXR) && (pte & PTE_X)))
                goto page_fault;
            break;
        case MMU_STORE:
            if (!(pte & PTE_W)) goto page_fault;
            break;
        }

        // A leaf at level 1/2 maps a 2 MiB/1 GiB aligned superpage.
        u64 offset_mask = (1ULL << (12 + 9 * level)) - 1;
        if (base & offset_mask)
            goto page_fault;

        // Single-hart hardware-managed A/D bits; update only after checks.
        u64 updated = pte | PTE_A | (access == MMU_STORE ? PTE_D : 0);
        if (updated != pte)
            mem_write64(addr, updated);
        return base | (va & offset_mask);
    }

page_fault:
    cpu_trap(cpu, access == MMU_FETCH ? INST_PAGE_FAULT :
                  access == MMU_LOAD ? LOAD_PAGE_FAULT : STORE_PAGE_FAULT, va);
    return PA_INVALID;

access_fault:
    cpu_trap(cpu, access == MMU_FETCH ? INST_ACCESS_FAULT :
                  access == MMU_LOAD ? LOAD_ACCESS_FAULT : STORE_ACCESS_FAULT, va);
    return PA_INVALID;
}
