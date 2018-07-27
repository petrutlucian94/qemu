/*
 * QEMU Windows Hypervisor Platform accelerator (WHPX) support
 *
 * Copyright Microsoft, Corp. 2017
 *
 * Authors:
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#ifndef QEMU_WHPX_H
#define QEMU_WHPX_H

#include "config-host.h"
#include "qemu-common.h"

int whpx_init_vcpu(CPUState *cpu);
int whpx_vcpu_exec(CPUState *cpu);
void whpx_destroy_vcpu(CPUState *cpu);
void whpx_vcpu_kick(CPUState *cpu);


void whpx_cpu_synchronize_state(CPUState *cpu);
void whpx_cpu_synchronize_post_reset(CPUState *cpu);
void whpx_cpu_synchronize_post_init(CPUState *cpu);
void whpx_cpu_synchronize_pre_loadvm(CPUState *cpu);

#ifdef CONFIG_WHPX

#include "whp-dispatch.h"

struct whpx_state {
    uint64_t mem_quota;
    WHV_PARTITION_HANDLE partition;
    bool apic_in_platform;
};

struct whpx_lapic_state
{
    struct
    {
        uint32_t data;
        uint32_t padding[3];
    } fields[256];
};

extern struct whpx_state whpx_global;

int whpx_enabled(void);
void whpx_apic_get(DeviceState *s);
#define whpx_apic_in_platform() (whpx_global.apic_in_platform)

#else /* CONFIG_WHPX */

#define whpx_enabled() (0)
#define whpx_apic_in_platform() (0)

#endif /* CONFIG_WHPX */

#endif /* QEMU_WHPX_H */
