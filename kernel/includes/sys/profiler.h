#ifndef PROFILER_H
#define PROFILER_H

#define PROFILER_COL 60
#define PROFILER_ROW 10
#define PROFILER_BLOCKS PROFILER_COL * PROFILER_ROW
#define PROFILER_C_YES   0xC
#define PROFILER_C_NO    0x2
#define PROFILER_C_KERNEL 0x8
#define PROFILER_C_DEBUG 0x7


void profiler_memory();

#endif
