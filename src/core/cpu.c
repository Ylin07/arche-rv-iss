#include "rv/cpu.h"
#include "rv/isa.h"
#include "rv/mem.h"
#include "rv/trap.h"
#include <stdbool.h>
#include <string.h>

void cpu_reset(cpu_t *cpu, u64 reset_pc)
{
    memset(cpu, 0, sizeof(*cpu));
    cpu->pc = reset_pc;
    cpu->priv = M;
}

void cpu_step(cpu_t *cpu)
{
    cpu->trapped = false;
    cpu->cycle++;

    if (cpu_check_interrupt(cpu))
        return;

    u32 inst = mem_read32(cpu->pc);
    u64 next_pc = isa_exec(cpu, inst);

    if (!cpu->trapped) {
        cpu->instret++;
        cpu->pc = next_pc;
    }
}

void cpu_run(cpu_t *cpu, u64 count)
{
    while (count--)
        cpu_step(cpu);
}
