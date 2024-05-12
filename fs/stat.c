#include <errno.h>
#include <sys/stat.h>

#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kernel.h>

static int cp_stat(struct m_inode * inode, struct stat * statbuf)
{
	struct stat tmp;
	int i;

	verify_area(statbuf,sizeof (* statbuf));
	tmp.st_dev = inode->i_dev;
	tmp.st_ino = inode->i_num;
	tmp.st_mode = inode->i_mode;
	tmp.st_nlink = inode->i_nlinks;
	tmp.st_uid = inode->i_uid;
	tmp.st_gid = inode->i_gid;
	tmp.st_rdev = inode->i_zone[0];
	tmp.st_size = inode->i_size;
	tmp.st_atime = inode->i_atime;
	tmp.st_mtime = inode->i_mtime;
	tmp.st_ctime = inode->i_ctime;
	for (i=0 ; i<sizeof (tmp) ; i++)
        ((char *) statbuf)[i] = ((char *) &tmp)[i];
	return (0);
}

int sys_stat(struct pt_regs * regs)
{
    char * filename = regs->a0;
    struct stat * statbuf = regs->a1;

	int i;
	struct m_inode * inode;

	if (!(inode=namei(filename)))
		return -ENOENT;
	i=cp_stat(inode,statbuf);
	iput(inode);
	return i;
}

int sys_fstat(struct pt_regs * regs)
{
    unsigned int fd = regs->a0;
    struct stat * statbuf = regs->a1;

	struct file * f;
	struct m_inode * inode;

	if (fd >= NR_OPEN || !(f=current->filp[fd]) || !(inode=f->f_inode))
		return -ENOENT;
	return cp_stat(inode,statbuf);
}
