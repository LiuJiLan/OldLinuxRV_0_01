#ifndef _SYS_H
#define _SYS_H

#include <linux/sched.h>

//extern int sys_setup(struct pt_regs *);
//extern int sys_exit(struct pt_regs *);
//extern int sys_fork(struct pt_regs *);
//extern int sys_read(struct pt_regs *);
//extern int sys_write(struct pt_regs *);
//extern int sys_open(struct pt_regs *);
//extern int sys_close(struct pt_regs *);
//extern int sys_waitpid(struct pt_regs *);
//extern int sys_creat(struct pt_regs *);
//extern int sys_link(struct pt_regs *);
//extern int sys_unlink(struct pt_regs *);
//extern int sys_execve(struct pt_regs *);
//extern int sys_chdir(struct pt_regs *);
//extern int sys_time(struct pt_regs *);
//extern int sys_mknod(struct pt_regs *);
//extern int sys_chmod(struct pt_regs *);
//extern int sys_chown(struct pt_regs *);
//extern int sys_break(struct pt_regs *);
//extern int sys_stat(struct pt_regs *);
//extern int sys_lseek(struct pt_regs *);
//extern int sys_getpid(struct pt_regs *);
//extern int sys_mount(struct pt_regs *);
//extern int sys_umount(struct pt_regs *);
//extern int sys_setuid(struct pt_regs *);
//extern int sys_getuid(struct pt_regs *);
//extern int sys_stime(struct pt_regs *);
//extern int sys_ptrace(struct pt_regs *);
//extern int sys_alarm(struct pt_regs *);
//extern int sys_fstat(struct pt_regs *);
//extern int sys_pause(struct pt_regs *);
//extern int sys_utime(struct pt_regs *);
//extern int sys_stty(struct pt_regs *);
//extern int sys_gtty(struct pt_regs *);
//extern int sys_access(struct pt_regs *);
//extern int sys_nice(struct pt_regs *);
//extern int sys_ftime(struct pt_regs *);
//extern int sys_sync(struct pt_regs *);
//extern int sys_kill(struct pt_regs *);
//extern int sys_rename(struct pt_regs *);
//extern int sys_mkdir(struct pt_regs *);
//extern int sys_rmdir(struct pt_regs *);
//extern int sys_dup(struct pt_regs *);
//extern int sys_pipe(struct pt_regs *);
//extern int sys_times(struct pt_regs *);
//extern int sys_prof(struct pt_regs *);
//extern int sys_brk(struct pt_regs *);
//extern int sys_setgid(struct pt_regs *);
//extern int sys_getgid(struct pt_regs *);
//extern int sys_signal(struct pt_regs *);
//extern int sys_geteuid(struct pt_regs *);
//extern int sys_getegid(struct pt_regs *);
//extern int sys_acct(struct pt_regs *);
//extern int sys_phys(struct pt_regs *);
//extern int sys_lock(struct pt_regs *);
//extern int sys_ioctl(struct pt_regs *);
//extern int sys_fcntl(struct pt_regs *);
//extern int sys_mpx(struct pt_regs *);
//extern int sys_setpgid(struct pt_regs *);
//extern int sys_ulimit(struct pt_regs *);
//extern int sys_uname(struct pt_regs *);
//extern int sys_umask(struct pt_regs *);
//extern int sys_chroot(struct pt_regs *);
//extern int sys_ustat(struct pt_regs *);
//extern int sys_dup2(struct pt_regs *);
//extern int sys_getppid(struct pt_regs *);
//extern int sys_getpgrp(struct pt_regs *);
//extern int sys_setsid(struct pt_regs *);
//
//fn_ptr sys_call_table[] = { sys_setup, sys_exit, sys_fork, sys_read,
//                            sys_write, sys_open, sys_close, sys_waitpid, sys_creat, sys_link,
//                            sys_unlink, sys_execve, sys_chdir, sys_time, sys_mknod, sys_chmod,
//                            sys_chown, sys_break, sys_stat, sys_lseek, sys_getpid, sys_mount,
//                            sys_umount, sys_setuid, sys_getuid, sys_stime, sys_ptrace, sys_alarm,
//                            sys_fstat, sys_pause, sys_utime, sys_stty, sys_gtty, sys_access,
//                            sys_nice, sys_ftime, sys_sync, sys_kill, sys_rename, sys_mkdir,
//                            sys_rmdir, sys_dup, sys_pipe, sys_times, sys_prof, sys_brk, sys_setgid,
//                            sys_getgid, sys_signal, sys_geteuid, sys_getegid, sys_acct, sys_phys,
//                            sys_lock, sys_ioctl, sys_fcntl, sys_mpx, sys_setpgid, sys_ulimit,
//                            sys_uname, sys_umask, sys_chroot, sys_ustat, sys_dup2, sys_getppid,
//                            sys_getpgrp,sys_setsid};

extern int sys_fork(struct pt_regs *);
fn_ptr sys_call_table[] = {sys_fork};

#define nr_system_calls (sizeof(sys_call_table)/sizeof(fn_ptr))

#endif