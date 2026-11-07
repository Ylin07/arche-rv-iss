#include "rv/loader.h"
#include "rv/mem.h"
#include <stdio.h>
#include <stdlib.h>

u64 bin_load(const char *path, u64 paddr)
{
    FILE *f = fopen(path, "rb"); long n; void *p;
    if (!f) return 0;
    fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
    if (n <= 0) { fclose(f); return 0; }
    p = malloc((size_t)n);
    if (!p || fread(p, 1, (size_t)n, f) != (size_t)n) { free(p); fclose(f); return 0; }
    fclose(f); mem_load(paddr, p, (u64)n); free(p); return (u64)n;
}
