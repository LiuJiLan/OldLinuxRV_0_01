// 在Linux 0.12中, 这个文件叫做sys_call

#include <asm/ptrace.h>
#include <linux/trap.h>
#include <linux/sys.h>

void system_call(struct pt_regs * regs) {
    long syscall = regs->a7;    // 当前任务号

    regs->epc += 4;
    regs->orig_a0 = regs->a0;

    local_irq_enable(); // 开中断

    // 即使访问的系统调用越界, 也应该能响应中断

    if (syscall < 0 || syscall >= nr_system_calls) {
        // 这个版本还不是$-ENOSYS
        regs->a0 = -1;
        return;
    }
    regs->a0 = sys_call_table[syscall](regs);



    // 后续处理

    // do_signal(struct pt_regs * regs)

    local_irq_disable(); // 关中断
}

int sys_fork(struct pt_regs * regs) {
    return 0;
}

//call _find_empty_process
//testl %eax,%eax
//        js 1f
//push %gs
//        pushl %esi
//        pushl %edi
//        pushl %ebp
//        pushl %eax
//        call _copy_process
//        addl $20,%esp
//1:	ret