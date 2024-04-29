#include <linux/kernel.h>
#include <asm/sbi.h>

unsigned long boot_cpu_hartid;

static inline void ebreak() {
    asm volatile("ebreak");
}

extern void trap_init(void);
extern void paging_init(void);
int sbi_printf(const char *fmt, ...);

void start_kernel(void){
    ebreak();
    paging_init();
    trap_init();
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