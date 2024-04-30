#ifndef _SCHED_H
#define _SCHED_H

#include <asm/ptrace.h>
#include <linux/config.h>

typedef int (*fn_ptr)(struct pt_regs *);

extern union task_union init_task_union;

// thread_info

struct thread_info {
    unsigned long   flags;      /* low level flags */

    long			kernel_sp;	/* Kernel stack pointer */
    long			user_sp;	/* User stack pointer */
    int			    cpu;
};

#define INIT_THREAD_INFO(tsk)               \
{						                    \
	.flags		= 0,			            \
}

// thread_struct


struct thread_struct {
    /* Callee-saved registers */
    unsigned long ra;
    unsigned long sp;	    /* Kernel mode stack */
    unsigned long s[12];	/* s[0]: frame pointer */
    struct riscv_d_ext_state fstate;
    unsigned long bad_cause;
};

#define INIT_THREAD {					        \
	.sp = THREAD_SIZE + (long)&init_task_union,	\
}

#define STACK_ALIGN		16

#define task_pt_regs(tsk)						                \
	((struct pt_regs *)((tsk) + THREAD_SIZE		\
    - sizeof(struct pt_regs)))


// 进程描述符

struct task_struct {
    struct thread_info          thread_info;

    /* -1 unrunnable, 0 runnable, >0 stopped */
    volatile long               state;
    void * stack;

    /*
    long counter;
    long priority;
    long signal;
    fn_ptr sig_restorer;
    fn_ptr sig_fn[32];

    int exit_code;
    unsigned long end_code,end_data,brk,start_stack;
    long pid,father,pgrp,session,leader;
    unsigned short uid,euid,suid;
    unsigned short gid,egid,sgid;
    long alarm;
    long utime,stime,cutime,cstime,start_time;
    unsigned short used_math;

    int tty;
    unsigned short umask;
    struct m_inode * pwd;
    struct m_inode * root;
    unsigned long close_on_exec;
    struct file * filp[NR_OPEN];

    struct desc_struct ldt[3];

    struct tss_struct tss;
    */

    struct thread_struct		thread;
};

static inline void *task_stack_page(const struct task_struct *task){
    return task->stack;
}

#define init_task ((struct task_struct *)&(init_task_union.task))

#endif
