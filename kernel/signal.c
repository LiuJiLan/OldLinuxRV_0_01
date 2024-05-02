// 原Linux 0.01没有signal.c和do_signal函数
// 名字借用自Linux 0.12

#include <signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>

// 丑陋的嵌套if, 之后慢慢优化吧
void do_signal(struct pt_regs * regs) {
    if (current == FIRST_TASK) {
        // task[0] cannot have signals
        return;
    }

    if (!user_mode(regs)) {
        // 来自内核态的话直接返回, 早期Linux内核态不能被抢断
        // 不需要分别判断code segment和stack segment是否是内核态
        // 这是我们和最早版本Linux的不同
        return;
    }

    if (current->signal == 0) {
        return;
    }

    for (int nr = 0; nr < 32; nr++) {
        unsigned long mask = 1UL << nr;
        if (current->signal & mask) {
            // clear it
            current->signal &= ~mask;

            fn_ptr fn = current->sig_fn[nr];
            if (fn == 0) {
                // 0 is default signal handler - exit
                // 我们没有实现default_signal,
                // 对于Linux 0.01来说, 它实在是太短了, 没必要单独作为一个函数

                if (nr + 1 == SIGCHLD) {
                    continue;
                } else {
                    // do_exit(nr + 1)
                }
                // signal中的位数和自定义函数的下标是对应的
                // 但和数字的编号差了1

            } else if (fn == 1) {
                // 1 is ignore - find next signal
                continue;
            } else {
                panic("No implement of customized signal.");
            }
        }
    }
}
