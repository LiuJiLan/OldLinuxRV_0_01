#ifndef _PTRACE_H
#define _PTRACE_H

#include <sys/types.h>
#include <asm/system.h>

struct pt_regs {
    unsigned long epc;
    unsigned long ra;
    unsigned long sp;
    unsigned long gp;
    unsigned long tp;
    unsigned long t0;
    unsigned long t1;
    unsigned long t2;
    unsigned long s0;
    unsigned long s1;
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;
    unsigned long s2;
    unsigned long s3;
    unsigned long s4;
    unsigned long s5;
    unsigned long s6;
    unsigned long s7;
    unsigned long s8;
    unsigned long s9;
    unsigned long s10;
    unsigned long s11;
    unsigned long t3;
    unsigned long t4;
    unsigned long t5;
    unsigned long t6;
    /* Supervisor/Machine CSRs */
    unsigned long status;
    unsigned long badaddr;
    unsigned long cause;
    /* a0 value before the syscall */
    unsigned long orig_a0;
};

#define PT_REGS_SIZE sizeof(struct pt_regs)

#define user_mode(regs) (((regs)->status & SR_PP) == 0)

struct riscv_f_ext_state {
    u32 f[32];
    u32 fcsr;
};

// d扩展包含了f扩展的内容, 所以thread_struct中仅需要存一个
struct riscv_d_ext_state {
    u64 f[32];
    u32 fcsr;
};

// 暂不打算支持V扩展



#endif
