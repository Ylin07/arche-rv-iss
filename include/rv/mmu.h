#pragma once

#include "rv/cpu.h"

typedef enum {
    MMU_FETCH,
    MMU_LOAD,
    MMU_STORE,
} mmu_access_t;

#define PTE_V (1ULL << 0)
#define PTE_R (1ULL << 1)
#define PTE_W (1ULL << 2)
#define PTE_X (1ULL << 3)
#define PTE_U (1ULL << 4)
#define PTE_G (1ULL << 5)
#define PTE_A (1ULL << 6)
#define PTE_D (1ULL << 7)

#define PA_INVALID UINT64_MAX

u64 va2pa(cpu_t *cpu, u64 va, mmu_access_t access);
