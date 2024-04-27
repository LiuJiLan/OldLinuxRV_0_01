#include <linux/sched.h>

__attribute__((__aligned__(THREAD_SIZE)))
struct task_struct init_task = {
        .thread_info	= INIT_THREAD_INFO(init_task),
        .state          = 0,
        .stack          = init_task,


        .thread         = INIT_THREAD
};