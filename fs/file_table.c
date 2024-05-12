#include <linux/fs.h>

struct file file_table[NR_FILE];

// 放在外面现代GCC好像会报错
struct super_block * get_super(int dev)
{
    struct super_block * s;

    for(s = 0+super_block;s < NR_SUPER+super_block; s++)
        if (s->s_dev == dev)
            return s;
    return NULL;
}
