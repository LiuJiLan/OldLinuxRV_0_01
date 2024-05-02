#include <linux/kernel.h>
#include <asm/sbi.h>
#include <linux/sched.h>

unsigned long boot_cpu_hartid;
unsigned long OFFSET_TASK_THREAD;

static inline void ebreak() {
    asm volatile("ebreak");
}

extern void paging_init(void);
extern void trap_init(void);
extern void irq_init(void);
int sbi_printf(const char *fmt, ...);

void start_kernel(void){
    // 暂时没什么好办法, 这个偏移值不太能硬编码
    OFFSET_TASK_THREAD = offsetof(struct task_struct, thread);

    paging_init();
    trap_init();
    irq_init();
    // tty_init();
    // sched_init();
    // buffer_init();
    // hd_init();

    // sti();
    // 我们不需要在内核里开中断, 因为我们可以用RISC-V的xstatus来一次性开启中断
    // move_to_user_mode();
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