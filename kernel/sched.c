#include <linux/sched.h>


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


        .thread         = INIT_THREAD
    },
};


void do_timer(struct pt_regs * regs) {

}
