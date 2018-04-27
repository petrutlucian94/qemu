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

typedef enum WHPX_CPU_EXIT_LEAFS {
    feat_none               = 0U,            // 0
    feat_fpu                = 1U << 0,       // 0x1
    feat_vme                = 1U << 1,       // 0x2
    feat_de                 = 1U << 2,       // 0x4
    feat_pse                = 1U << 3,       // 0x8
    feat_tsc                = 1U << 4,       // 0x10
    feat_msr                = 1U << 5,       // 0x20
    feat_pae                = 1U << 6,       // 0x40
    feat_mce                = 1U << 7,       // 0x80
    feat_cx8                = 1U << 8,       // 0x100
    feat_apic               = 1U << 9,       // 0x200
    feat_sep                = 1U << 11,      // 0x800
    feat_mtrr               = 1U << 12,      // 0x1000
    feat_pge                = 1U << 13,      // 0x2000
    feat_mca                = 1U << 14,      // 0x4000
    feat_cmov               = 1U << 15,      // 0x8000
    feat_pat                = 1U << 16,      // 0x10000
    feat_pse36              = 1U << 17,      // 0x20000
    feat_psn                = 1U << 18,      // 0x40000
    feat_clfsh              = 1U << 19,      // 0x80000
    feat_ds                 = 1U << 21,      // 0x200000
    feat_acpi               = 1U << 22,      // 0x400000
    feat_mmx                = 1U << 23,      // 0x800000
    feat_fxsr               = 1U << 24,      // 0x1000000
    feat_sse                = 1U << 25,      // 0x2000000
    feat_sse2               = 1U << 26,      // 0x4000000
    feat_ss                 = 1U << 27,      // 0x8000000
    feat_htt                = 1U << 28,      // 0x10000000
    feat_tm                 = 1U << 29,      // 0x20000000
    feat_pbe                = 1U << 31,      // 0x80000000

    feat_sse3               = 1U << 0,       // 0x1
    feat_pclmulqdq          = 1U << 1,       // 0x2
    feat_dtes64             = 1U << 2,       // 0x4
    feat_monitor            = 1U << 3,       // 0x8
    feat_dscpl              = 1U << 4,       // 0x10
    feat_vmx                = 1U << 5,       // 0x20
    feat_smx                = 1U << 6,       // 0x40
    feat_est                = 1U << 7,       // 0x80
    feat_tm2                = 1U << 8,       // 0x100
    feat_ssse3              = 1U << 9,       // 0x200
    feat_cnxtid             = 1U << 10,      // 0x400
    feat_cmpxchg16b         = 1U << 13,      // 0x2000
    feat_xtpr_update        = 1U << 14,      // 0x4000
    feat_pdcm               = 1U << 15,      // 0x8000
    feat_dca                = 1U << 18,      // 0x40000
    feat_sse41              = 1U << 19,      // 0x80000
    feat_sse42              = 1U << 20,      // 0x100000
    feat_x2apic             = 1U << 21,      // 0x200000
    feat_movbe              = 1U << 22,      // 0x400000
    feat_popcnt             = 1U << 23,      // 0x800000
    feat_aes                = 1U << 25,      // 0x2000000
    feat_xsave              = 1U << 26,      // 0x4000000
    feat_osxsave            = 1U << 27,      // 0x8000000
    feat_hypervisor         = 1U << 31,      // 0x80000000

    feat_lahf               = 1U << 0,       // 0x1

    feat_syscall            = 1U << 11,      // 0x800
    feat_execute_disable    = 1U << 20,      // 0x10000
    feat_em64t              = 1U << 29       // 0x20000000
} WHPX_CPU_EXIT_LEAFS;

int whpx_init_vcpu(CPUState *cpu);
int whpx_vcpu_exec(CPUState *cpu);
void whpx_destroy_vcpu(CPUState *cpu);
void whpx_vcpu_kick(CPUState *cpu);


void whpx_cpu_synchronize_state(CPUState *cpu);
void whpx_cpu_synchronize_post_reset(CPUState *cpu);
void whpx_cpu_synchronize_post_init(CPUState *cpu);
void whpx_cpu_synchronize_pre_loadvm(CPUState *cpu);

#ifdef CONFIG_WHPX

int whpx_enabled(void);

#else /* CONFIG_WHPX */

#define whpx_enabled() (0)

#endif /* CONFIG_WHPX */

#endif /* QEMU_WHPX_H */
