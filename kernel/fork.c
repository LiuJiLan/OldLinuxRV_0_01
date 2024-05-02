/*
 *  'fork.c' contains the help-routines for the 'fork' system call
 * (see also system_call.s), and some misc functions ('verify_area').
 * Fork is rather simple, once you get the hang of it, but the memory
 * management can be a bitch. See 'mm/mm.c': 'copy_page_tables()'
 */
#include <errno.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/config.h>
#include <linux/mm.h>
#include <asm/system.h>

#include <sys/types.h>

#include <string.h>

extern void write_verify(unsigned long address);

long last_pid=0;

void verify_area(void * addr,ssize_t size) {
    unsigned long start;

    start = (unsigned long) addr;
    size += start & 0xFFFUL;
    start &= ~0xFFFUL;
    while (size>0) {
        size -= 4096;
        write_verify(start);
        start += 4096;
    }
}

int find_empty_process(void) {
    int i;

    repeat:
    if ((++last_pid)<0) last_pid=1;
    for(i=0 ; i<NR_TASKS ; i++)
        if (task[i] && task[i]->pid == last_pid) goto repeat;
    for(i=1 ; i<NR_TASKS ; i++)
        if (!task[i])
            return i;
    return -EAGAIN;
}

int copy_process(struct pt_regs * regs) {
    int nr;
    struct task_struct *p;
    int i;
    // struct file *f;

    nr = find_empty_process();
    if (nr < 0) {
        return nr;
    }

    p = (struct task_struct *) get_two_free_pages();
    if (!p)
        return -EAGAIN;

    *p = *current;	/* NOTE! this doesn't copy the supervisor stack */
    p->state = TASK_RUNNING;
    p->stack = (ssize_t) p + THREAD_SIZE; // 相当于p->tss.esp0
    p->pid = last_pid;
    p->father = current->pid;
    p->counter = p->priority;
    p->signal = 0;
    p->alarm = 0;
    p->leader = 0;		/* process leadership doesn't inherit */
    p->utime = p->stime = 0;
    p->cutime = p->cstime = 0;
    p->start_time = jiffies;


//    if (last_task_used_math == current)
//        __asm__("fnsave %0"::"m" (p->tss.i387));

    // 原copy_mem逻辑
    // 里面同时设置了p->pgd
    if (copy_page_tables(p)) {
        // != 0 则失败
        free_two_pages(p);
        return -EAGAIN; // 没有返回-ENOMEM;
    }

//    for (i=0; i<NR_OPEN;i++)
//        if (f=p->filp[i])
//            f->f_count++;
//    if (current->pwd)
//        current->pwd->i_count++;
//    if (current->root)
//        current->root->i_count++;

    task[nr] = p;	/* do this last, just in case */

    // 相当于copy_thread的步骤
    struct pt_regs *childregs = task_pt_regs(p);
    memset(&p->thread.s, 0, sizeof(p->thread.s));
    *childregs = *(current_pt_regs());

//    riscv_v_vstate_off(childregs);
//    if (usp) /* User fork */
//        childregs->sp = usp;
//    if (clone_flags & CLONE_SETTLS)
//        childregs->tp = tls;
    childregs->a0 = 0; /* Return value of fork() */
    p->thread.s[0] = 0;

    return last_pid;
}



int sys_fork(struct pt_regs * regs) {
    // 本来的逻辑是先find_empty_process再copy_process
    // 因为不仅要找新的pid, 还要保留一个nr用于ldt
    // 就用last_pid全局变量来传值, 这对多核是不利的
    // 所以我们合成一个函数
    return copy_process(regs);
}