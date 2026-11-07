#include "rv/mem.h"
#include <stdlib.h>
#include <string.h>

static u8 *dram;

void mem_init(void) {
    dram = (u8 *)calloc(1, DRAM_SIZE);
}

void mem_load(u64 paddr, const void *src, u64 size) {
    if (paddr >= DRAM_BASE && (paddr - DRAM_BASE + size) <= DRAM_SIZE) {
        memcpy(dram + (paddr - DRAM_BASE), src, size);
    }
}

static inline void *pa2va(u64 paddr, u64 len) {
    u64 offset = paddr - DRAM_BASE;
    return (offset <= DRAM_SIZE - len) ? (dram + offset) : NULL;
}

u8  mem_read8 (u64 paddr) { void *p = pa2va(paddr, 1); return p ? *(u8  *)p : 0; }
u16 mem_read16(u64 paddr) { void *p = pa2va(paddr, 2); return p ? *(u16 *)p : 0; }
u32 mem_read32(u64 paddr) { void *p = pa2va(paddr, 4); return p ? *(u32 *)p : 0; }
u64 mem_read64(u64 paddr) { void *p = pa2va(paddr, 8); return p ? *(u64 *)p : 0; }

void mem_write8 (u64 paddr, u8  val) { void *p = pa2va(paddr, 1); if (p) *(u8  *)p = val; }
void mem_write16(u64 paddr, u16 val) { void *p = pa2va(paddr, 2); if (p) *(u16 *)p = val; }
void mem_write32(u64 paddr, u32 val) { void *p = pa2va(paddr, 4); if (p) *(u32 *)p = val; }
void mem_write64(u64 paddr, u64 val) { void *p = pa2va(paddr, 8); if (p) *(u64 *)p = val; }

