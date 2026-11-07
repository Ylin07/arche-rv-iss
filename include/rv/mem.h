#pragma once

#include "rv/types.h"

#define DRAM_BASE 0x80000000ULL
#define DRAM_SIZE (128ULL * 1024 * 1024)

void mem_init(void);
void mem_load(u64 paddr, const void *src, u64 size);

u8  mem_read8 (u64 paddr);
u16 mem_read16(u64 paddr);
u32 mem_read32(u64 paddr);
u64 mem_read64(u64 paddr);

void mem_write8 (u64 paddr, u8  val);
void mem_write16(u64 paddr, u16 val);
void mem_write32(u64 paddr, u32 val);
void mem_write64(u64 paddr, u64 val);
