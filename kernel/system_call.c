// 在Linux 0.12中, 这个文件叫做sys_call

#include <asm/ptrace.h>
#include <linux/trap.h>
#include <linux/sys.h>
#include <signal.h>

// 为了解决一些神奇的报错
fn_ptr sys_call_table[] = { sys_setup, sys_exit, sys_fork, sys_read,
                            sys_write, sys_open, sys_close, sys_waitpid, sys_creat, sys_link,
                            sys_unlink, sys_execve, sys_chdir, sys_time, sys_mknod, sys_chmod,
                            sys_chown, sys_break, sys_stat, sys_lseek, sys_getpid, sys_mount,
                            sys_umount, sys_setuid, sys_getuid, sys_stime, sys_ptrace, sys_alarm,
                            sys_fstat, sys_pause, sys_utime, sys_stty, sys_gtty, sys_access,
                            sys_nice, sys_ftime, sys_sync, sys_kill, sys_rename, sys_mkdir,
                            sys_rmdir, sys_dup, sys_pipe, sys_times, sys_prof, sys_brk, sys_setgid,
                            sys_getgid, sys_signal, sys_geteuid, sys_getegid, sys_acct, sys_phys,
                            sys_lock, sys_ioctl, sys_fcntl, sys_mpx, sys_setpgid, sys_ulimit,
                            sys_uname, sys_umask, sys_chroot, sys_ustat, sys_dup2, sys_getppid,
                            sys_getpgrp,sys_setsid};

int sys_execve(struct pt_regs * regs) {

}

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

    // 原汇编后续处理
    do_signal(regs);

    local_irq_disable(); // 关中断
}



