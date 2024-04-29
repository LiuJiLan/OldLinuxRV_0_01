#include <linux/kernel.h>

volatile void panic(const char * str)
{
    printk("Kernel panic: %s\n\r",str);
    for(;;);
}
