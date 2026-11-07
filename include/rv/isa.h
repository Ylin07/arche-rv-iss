#pragma once

#include "rv/cpu.h"
#include "rv/types.h"

u64 isa_exec(cpu_t *cpu, u32 inst);
