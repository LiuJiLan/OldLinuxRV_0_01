#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utime.h>
#include <sys/stat.h>

#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>

#include <asm/ptrace.h>

int sys_utime(struct pt_regs * regs)
{
    char * filename = regs->a0;
    struct utimbuf * times = regs->a1;

	struct m_inode * inode;
	long actime,modtime;

	if (!(inode=namei(filename)))
		return -ENOENT;
	if (times) {
		actime = *((unsigned long *) &times->actime);
		modtime = *((unsigned long *) &times->modtime);
	} else
		actime = modtime = CURRENT_TIME;
	inode->i_atime = actime;
	inode->i_mtime = modtime;
	inode->i_dirt = 1;
	iput(inode);
	return 0;
}

int sys_access(struct pt_regs * regs)
{
    const char * filename = regs->a0;
    int mode = regs->a1;

	struct m_inode * inode;
	int res;

	mode &= 0007;
	if (!(inode=namei(filename)))
		return -EACCES;
	res = inode->i_mode & 0777;
	iput(inode);
	if (!(current->euid && current->uid))
		if (res & 0111)
			res = 0777;
		else
			res = 0666;
	if (current->euid == inode->i_uid)
		res >>= 6;
	else if (current->egid == inode->i_gid)
		res >>= 6;
	if ((res & 0007 & mode) == mode)
		return 0;
	return -EACCES;
}

int sys_chdir(struct pt_regs * regs)
{
    const char * filename = regs->a0;

	struct m_inode * inode;

	if (!(inode = namei(filename)))
		return -ENOENT;
	if (!S_ISDIR(inode->i_mode)) {
		iput(inode);
		return -ENOTDIR;
	}
	iput(current->pwd);
	current->pwd = inode;
	return (0);
}

int sys_chroot(struct pt_regs * regs)
{
    const char * filename regs->a0;

	struct m_inode * inode;

	if (!(inode=namei(filename)))
		return -ENOENT;
	if (!S_ISDIR(inode->i_mode)) {
		iput(inode);
		return -ENOTDIR;
	}
	iput(current->root);
	current->root = inode;
	return (0);
}

int sys_chmod(struct pt_regs * regs)
{
    const char * filename = regs->a0;
    int mode = regs->a1;

	struct m_inode * inode;

	if (!(inode=namei(filename)))
		return -ENOENT;
	if (current->uid && current->euid)
		if (current->uid!=inode->i_uid && current->euid!=inode->i_uid) {
			iput(inode);
			return -EACCES;
		} else 
			mode = (mode & 0777) | (inode->i_mode & 07000);
	inode->i_mode = (mode & 07777) | (inode->i_mode & ~07777);
	inode->i_dirt = 1;
	iput(inode);
	return 0;
}

int sys_chown(struct pt_regs * regs)
{
    const char * filename = regs->a0;
    int uid = regs->a1;
    int gid = regs->a2;

	struct m_inode * inode;

	if (!(inode=namei(filename)))
		return -ENOENT;
	if (current->uid && current->euid) {
		iput(inode);
		return -EACCES;
	}
	inode->i_uid=uid;
	inode->i_gid=gid;
	inode->i_dirt=1;
	iput(inode);
	return 0;
}

int do_open(const char * filename,int flag,int mode) {
    struct m_inode * inode;
    struct file * f;
    int i,fd;

    mode &= 0777 & ~current->umask;
    for(fd=0 ; fd<NR_OPEN ; fd++)
        if (!current->filp[fd])
            break;
    if (fd>=NR_OPEN)
        return -EINVAL;
    current->close_on_exec &= ~(1<<fd);
    f=0+file_table;
    for (i=0 ; i<NR_FILE ; i++,f++)
        if (!f->f_count) break;
    if (i>=NR_FILE)
        return -EINVAL;
    (current->filp[fd]=f)->f_count++;
    if ((i=open_namei(filename,flag,mode,&inode))<0) {
        current->filp[fd]=NULL;
        f->f_count=0;
        return i;
    }
/* ttys are somewhat special (ttyxx major==4, tty major==5) */
    if (S_ISCHR(inode->i_mode))
        if (MAJOR(inode->i_zone[0])==4) {
            if (current->leader && current->tty<0) {
                current->tty = MINOR(inode->i_zone[0]);
                tty_table[current->tty].pgrp = current->pgrp;
            }
        } else if (MAJOR(inode->i_zone[0])==5)
            if (current->tty<0) {
                iput(inode);
                current->filp[fd]=NULL;
                f->f_count=0;
                return -EPERM;
            }
    f->f_mode = inode->i_mode;
    f->f_flags = flag;
    f->f_count = 1;
    f->f_inode = inode;
    f->f_pos = 0;
    return (fd);
}

int sys_open(struct pt_regs * regs)
{
    const char * filename = regs->a0;
    int flag = regs->a1;
    int mode = regs->a2;

    do_open(filename, flag, mode);
}

int sys_creat(struct pt_regs * regs)
{
    const char * pathname = regs->a0;
    int mode = regs->a1;

	return do_open(pathname, O_CREAT | O_TRUNC, mode);
}

int do_close(unsigned int fd)
{
    struct file * filp;

    if (fd >= NR_OPEN)
        return -EINVAL;
    current->close_on_exec &= ~(1<<fd);
    if (!(filp = current->filp[fd]))
        return -EINVAL;
    current->filp[fd] = NULL;
    if (filp->f_count == 0)
        panic("Close: file count is 0");
    if (--filp->f_count)
        return (0);
    iput(filp->f_inode);
    return (0);
}

int sys_close(struct pt_regs * regs)
{
    unsigned int fd = regs->a0;
	return do_close(fd);
}
