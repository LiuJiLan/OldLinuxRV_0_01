#ifndef _SCHED_H
#define _SCHED_H

#define NR_TASKS 64
#define HZ      100
#define LATCH   (TIMEBASE_FREQUENCY / HZ)

// 从Linux 0.12中引入的宏
// 暂时设为64MiB
#define TASK_SIZE	0x04000000


#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]

#include <asm/ptrace.h>
#include <asm/system.h>
#include <linux/config.h>
#include <linux/mm.h>

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



#define TASK_RUNNING		    0
#define TASK_INTERRUPTIBLE	    1
#define TASK_UNINTERRUPTIBLE	2
#define TASK_ZOMBIE		        3
#define TASK_STOPPED		    4
// 加入这个状态的原因看论文
#define TASK_DEAD		        5

typedef int (*fn_ptr)(struct pt_regs *);


// 进程描述符
struct task_struct {
    // 应用了thread_info, 不再是需要硬编码一些东西
    struct thread_info          thread_info;

    /* -1 unrunnable, 0 runnable, >0 stopped */
    volatile long               state;
    void *                      stack; // 内核栈
    long                        counter;
    long                        priority;

    long                        signal;
    // Well, 我知道函数的类型有点问题, 但是先这样算了吧
    // 反正不支持用户态的信号, 之后再改吧
    fn_ptr                      sig_restorer;
    fn_ptr                      sig_fn[32];

    int                         exit_code;
    pte_t *                     pgd;
    unsigned long end_code,end_data,brk,start_stack;

    long pid,father,pgrp,session,leader;
    unsigned short uid,euid,suid;
    unsigned short gid,egid,sgid;

    long                        alarm;
    long utime,stime,cutime,cstime,start_time;

    unsigned short used_math;

    /* file system info */
//    int tty;		/* -1 if no tty, so it must be signed */
//    unsigned short umask;
//    struct m_inode * pwd;
//    struct m_inode * root;
//    unsigned long close_on_exec;
//    struct file * filp[NR_OPEN];

    struct thread_struct		thread;
};

static inline void *task_stack_page(const struct task_struct *task){
    return task->stack;
}

#define init_task ((struct task_struct *)&(init_task_union.task))

extern struct task_struct *task[NR_TASKS];
extern struct task_struct *last_task_used_math;
register struct task_struct *riscv_current_is_tp asm("tp");
static inline struct task_struct * get_current(void) {
    return riscv_current_is_tp;
}
#define current get_current()
extern long volatile jiffies;
extern long startup_time;

#define CURRENT_TIME (startup_time+jiffies/HZ)

//extern void sleep_on(struct task_struct ** p);
//extern void interruptible_sleep_on(struct task_struct ** p);
//extern void wake_up(struct task_struct ** p);

// 在汇编中
extern struct task_struct *__switch_to(struct task_struct *,
                                       struct task_struct *);

//#define switch_to(prev, next, last)			\
//do {							            \
//	struct task_struct *__prev = (prev);	\
//	struct task_struct *__next = (next);	\
//	if (has_fpu())					        \
//		__switch_to_fpu(__prev, __next);	\
//	if (has_vector())					    \
//		__switch_to_vector(__prev, __next);	\
//	((last) = __switch_to(__prev, __next));	\
//} while (0)

#define switch_to(prev, next, last)			\
do {							            \
	struct task_struct *__prev = (prev);	\
	struct task_struct *__next = (next);	\
	((last) = __switch_to(__prev, __next));	\
} while (0)

#endif
