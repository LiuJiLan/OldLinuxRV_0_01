#include <linux/sched.h>
#include <linux/trap.h>
#include <signal.h>



union task_union {
    struct task_struct task;
    struct thread_info thread_info;
    char stack[THREAD_SIZE];
};

__attribute__((__aligned__(THREAD_SIZE)))
union task_union init_task_union = {{
        .thread_info	= INIT_THREAD_INFO(init_task),

        .state          = 0,
        .stack          = (void *)&init_task_union,
        .counter        = 15,
        .priority       = 15,

        /* signals */
        .signal         = 0,
        .sig_restorer   = NULL,
        .sig_fn         = {(fn_ptr) 0,},

        /* pid etc.. */
        // 其他以默认的方式变为0
        .father         = -1,

        .thread         = INIT_THREAD
    },
};

long volatile jiffies=0;
long startup_time=0;
struct task_struct *last_task_used_math = NULL;
struct task_struct * task[NR_TASKS] = {&(init_task_union.task), };

// 之后考虑动态分配
// long user_stack [PAGE_SIZE >> 2] ;

// release原型在exit.c里
extern void release(struct task_struct * p);
void finish_task_switch(struct task_struct *prev) {
    long prev_state = prev->state;
    // 在这回收进程描述符的空间!!!
    if (prev_state == TASK_DEAD) {
        release(prev);
    }
}

void schedule_tail(struct task_struct *prev) {
    // 很短, 但是为了日后扩展, 留着
    finish_task_switch(prev);
}

// 替代switch_to, 相较Linux的有简化
// switch_to包括页表和上下文都在TSS中
// 而TSS的交换是原子的, 所以这两个步骤前后关开中断
void
context_switch(struct task_struct *prev,
               struct task_struct *next) {
    local_irq_disable();

    // 相当于enter_lazy_tlb
    // 我们没有实现页表的惰性切换
    // 这是可选的改进点
    load_root_page_table(SATP_MODE_39, next->pgd);
    barrier();

    // switch_to是一个宏, 所以prev会被更改
    // (如果是C函数则是值的深拷贝被更改)
    switch_to(prev, next, prev);
    barrier();

    local_irq_enable();

    //finish_task_switch(prev);
}

/*
 *  'schedule()' is the scheduler function. This is GOOD CODE! There
 * probably won't be any reason to change this, as it should work well
 * in all circumstances (ie gives IO-bound processes good response etc).
 * The one thing you might take a look at is the signal-handler code here.
 *
 *   NOTE!!  Task 0 is the 'idle' task, which gets called when no other
 * tasks can run. It can not be killed, and it cannot sleep. The 'state'
 * information in task[0] is never used.
 */
void schedule(void)
{
    int i,next,c;
    struct task_struct ** p;

/* check alarm, wake up any interruptible tasks that have got a signal */

    for(p = &LAST_TASK ; p > &FIRST_TASK ; --p)
        if (*p) {
            if ((*p)->alarm && (*p)->alarm < jiffies) {
                (*p)->signal |= (1<<(SIGALRM-1));
                (*p)->alarm = 0;
            }
            if ((*p)->signal && (*p)->state==TASK_INTERRUPTIBLE)
                (*p)->state=TASK_RUNNING;
        }

/* this is the scheduler proper: */

    while (1) {
        c = -1;
        next = 0;
        i = NR_TASKS;
        p = &task[NR_TASKS];
        while (--i) {
            if (!*--p)
                continue;
            if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
                c = (*p)->counter, next = i;
        }
        if (c) break;
        for(p = &LAST_TASK ; p > &FIRST_TASK ; --p)
            if (*p)
                (*p)->counter = ((*p)->counter >> 1) +
                                (*p)->priority;
    }
    // switch_to(next);
    context_switch(current, next);
}

int sys_pause(void)
{
    current->state = TASK_INTERRUPTIBLE;
    schedule();
    return 0;
}

void sleep_on(struct task_struct **p)
{
    struct task_struct *tmp;

    if (!p)
        return;
    if (current == init_task)
        panic("task[0] trying to sleep");
    tmp = *p;
    *p = current;
    current->state = TASK_UNINTERRUPTIBLE;
    schedule();
    if (tmp)
        tmp->state = TASK_RUNNING;
}

void interruptible_sleep_on(struct task_struct **p)
{
    struct task_struct *tmp;

    if (!p)
        return;
    if (current == init_task)
        panic("task[0] trying to sleep");
    tmp=*p;
    *p=current;
    repeat:	current->state = TASK_INTERRUPTIBLE;
    schedule();
    if (*p && *p != current) {
        (**p).state = TASK_RUNNING; // 唤醒后面来的, 重新休眠自己
        goto repeat;
    }
    *p=NULL;
    if (tmp)
        tmp->state = TASK_RUNNING;
}

void wake_up(struct task_struct **p)
{
    if (p && *p) {
        (**p).state = TASK_RUNNING;
        *p=NULL;
    }
}

void do_timer(struct pt_regs * regs) {
    if (user_mode(regs)) {
        current->utime++;
    } else {
        current->stime++;
    }

    if ((--current->counter) > 0) {
        return;
    }
    current->counter = 0;
    if (!user_mode(regs)) {
        return;
    }
    schedule();

    // 原汇编后续处理
    do_signal(regs);
}


void sched_init(void) {;
    for (int i = 1; i < NR_TASKS; i++) {
        task[i] = NULL;
    }
}

