#include <stdarg.h>

#include <linux/kernel.h>

static char buf[1024];

extern int vsprintf(char *buf, const char *fmt, va_list args);

int printk_sbi_write(unsigned ch, char * buf, int count) {
    while (count--) {
        sbi_console_putchar(*(buf++));
    }
}

// sys_setup的时候换一下函数指针
// 因为使用tty_write会导致中断提前被打开
int (*printk_write)(unsigned, char*, int) = printk_sbi_write;

int printk(const char *fmt, ...) {
    va_list args;
    int i;

    va_start(args, fmt);
    i=vsprintf(buf,fmt,args);
    va_end(args);
    // tty_write(0, buf, i);
    printk_write(0, buf, i);
    return i;
}

void printk_init(void) {
    printk_write = tty_write;
}
