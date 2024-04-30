#include <linux/kernel.h>
#include <asm/sbi.h>

unsigned long boot_cpu_hartid;

static inline void ebreak() {
    asm volatile("ebreak");
}

extern void paging_init(void);
extern void trap_init(void);
extern void irq_init(void);
int sbi_printf(const char *fmt, ...);

void start_kernel(void){
    paging_init();
    trap_init();
    irq_init();
    sbi_printf("We are here at %ld\n", boot_cpu_hartid);
    ebreak();

    while (1) {

    }

}

void print_debug(char * str) {
    return;
}

// sbi_console_putchar() is expected to be deprecated.
// Just for debug.
static char sbi_printbuf[1024];
int sbi_printf(const char *fmt, ...) {
    va_list args;
    int i;
    char * c = sbi_printbuf;

    va_start(args, fmt);
    i = vsprintf(sbi_printbuf, fmt, args);
    va_end(args);
    while (i--) {
        sbi_console_putchar(*(c++));
    }
    return i;
}

// 留给未来的接口
unsigned long smp_processor_id(void) {
    return boot_cpu_hartid;
}