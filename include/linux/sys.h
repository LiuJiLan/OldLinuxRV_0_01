#ifndef _SYS_H
#define _SYS_H

#include <linux/sched.h>

extern int sys_setup(struct pt_regs *);
extern int sys_exit(struct pt_regs *);
extern int sys_fork(struct pt_regs *);
extern int sys_read(struct pt_regs *);
extern int sys_write(struct pt_regs *);
extern int sys_open(struct pt_regs *);
extern int sys_close(struct pt_regs *);
extern int sys_waitpid(struct pt_regs *);
extern int sys_creat(struct pt_regs *);
extern int sys_link(struct pt_regs *);
extern int sys_unlink(struct pt_regs *);
extern int sys_execve(struct pt_regs *);
extern int sys_chdir(struct pt_regs *);
extern int sys_time(struct pt_regs *);
extern int sys_mknod(struct pt_regs *);
extern int sys_chmod(struct pt_regs *);
extern int sys_chown(struct pt_regs *);
extern int sys_break(struct pt_regs *);
extern int sys_stat(struct pt_regs *);
extern int sys_lseek(struct pt_regs *);
extern int sys_getpid(struct pt_regs *);
extern int sys_mount(struct pt_regs *);
extern int sys_umount(struct pt_regs *);
extern int sys_setuid(struct pt_regs *);
extern int sys_getuid(struct pt_regs *);
extern int sys_stime(struct pt_regs *);
extern int sys_ptrace(struct pt_regs *);
extern int sys_alarm(struct pt_regs *);
extern int sys_fstat(struct pt_regs *);
extern int sys_pause(struct pt_regs *);
extern int sys_utime(struct pt_regs *);
extern int sys_stty(struct pt_regs *);
extern int sys_gtty(struct pt_regs *);
extern int sys_access(struct pt_regs *);
extern int sys_nice(struct pt_regs *);
extern int sys_ftime(struct pt_regs *);
extern int sys_sync(struct pt_regs *);
extern int sys_kill(struct pt_regs *);
extern int sys_rename(struct pt_regs *);
extern int sys_mkdir(struct pt_regs *);
extern int sys_rmdir(struct pt_regs *);
extern int sys_dup(struct pt_regs *);
extern int sys_pipe(struct pt_regs *);
extern int sys_times(struct pt_regs *);
extern int sys_prof(struct pt_regs *);
extern int sys_brk(struct pt_regs *);
extern int sys_setgid(struct pt_regs *);
extern int sys_getgid(struct pt_regs *);
extern int sys_signal(struct pt_regs *);
extern int sys_geteuid(struct pt_regs *);
extern int sys_getegid(struct pt_regs *);
extern int sys_acct(struct pt_regs *);
extern int sys_phys(struct pt_regs *);
extern int sys_lock(struct pt_regs *);
extern int sys_ioctl(struct pt_regs *);
extern int sys_fcntl(struct pt_regs *);
extern int sys_mpx(struct pt_regs *);
extern int sys_setpgid(struct pt_regs *);
extern int sys_ulimit(struct pt_regs *);
extern int sys_uname(struct pt_regs *);
extern int sys_umask(struct pt_regs *);
extern int sys_chroot(struct pt_regs *);
extern int sys_ustat(struct pt_regs *);
extern int sys_dup2(struct pt_regs *);
extern int sys_getppid(struct pt_regs *);
extern int sys_getpgrp(struct pt_regs *);
extern int sys_setsid(struct pt_regs *);

extern fn_ptr sys_call_table[];

#define nr_system_calls (sizeof(sys_call_table)/sizeof(fn_ptr))

#endif