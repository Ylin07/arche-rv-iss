#include "rv/loader.h"
#include "rv/cpu.h"
#include "rv/mem.h"
#include <stdio.h>
#include <stdlib.h>
#define TOHOST 0x80001000ULL
int main(int argc, char **argv)
{
    cpu_t cpu;
    u64 steps = argc > 2 ? strtoull(argv[2], NULL, 0) : 1000000;
    if (argc < 2) return 1;
    mem_init();
    if (!bin_load(argv[1], DRAM_BASE)) return 1;
    cpu_reset(&cpu, DRAM_BASE);
    while (steps--) {
        cpu_step(&cpu);
        u64 result = mem_read64(TOHOST);
        if (result == 1) return 0;
        if (result != 0) {
            fprintf(stderr, "test failed: tohost=%llu\n",
                    (unsigned long long)result);
            return 2;
        }
    }
    fprintf(stderr, "timeout pc=0x%016llx\\n", (unsigned long long)cpu.pc);
    return 3;
}
