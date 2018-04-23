#include <windows.h>
#include <stdbool.h>
#include <stdint.h>

#include "exec/hwaddr.h"

#ifndef TRACE_CPU_EXIT_H
#define TRACE_CPU_EXIT_H

typedef struct _EXIT_STATS {
        int exit_type;
        int call_count;
        LONGLONG min_duration;
        LONGLONG max_duration;
        LONGLONG total_duration;
} EXIT_STATS;

typedef struct _DEV_ACCESS_STATS {
        char* dev_name;
        int write_count;
        int call_count;
        LONGLONG min_duration;
        LONGLONG max_duration;
        LONGLONG total_duration;
} DEV_ACCESS_STATS;

void trace_cpu_init(int vcpu_count);
void trace_cpu_push_event(int cpu_id, int exit_type, LONGLONG elapsedMicrosec);
void trace_cpu_print_cpu_stats(int cpu_id);
void trace_cpu_print_stats(void);

void trace_mmio_event(int cpu_id, hwaddr addr, int len,
                      bool is_write, LONGLONG elapsedMicrosec);
void trace_ioport_event(int cpu_id, hwaddr addr, int len,
                        bool is_write, LONGLONG elapsedMicrosec);
#endif // TRACE_CPU_EXIT_H
