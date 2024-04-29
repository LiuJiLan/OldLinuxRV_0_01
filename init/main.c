#include <linux/kernel.h>

unsigned long boot_cpu_hartid;

static inline void ebreak() {
    asm volatile("ebreak");
}

extern void trap_init(void);


void start_kernel(void){
    trap_init();
    printk("We are here at %ld\n", boot_cpu_hartid);
    ebreak();




    while (1) {

    }

}

void print_debug(char* str) {
    return;
}