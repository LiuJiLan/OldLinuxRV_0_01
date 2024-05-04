#include <stdarg.h>

/*
 * 'kernel.h' contains some often-used function prototypes etc
 */
//void verify_area(void * addr,int count);
volatile void panic(const char * str);
//int printf(const char * fmt, ...);
int printk(const char * fmt, ...);
//int tty_write(unsigned ch,char * buf,int count);

int vsprintf(char *buf, const char *fmt, va_list args);


// 暂时不考虑多核的话, 这样就够了
// 尽量使用接口, 而不是boot_cpu_hartid
// extern unsigned long boot_cpu_hartid;
extern unsigned long smp_processor_id(void);

// for debug
extern void print_debug(char* str);
extern int sbi_printf(const char *fmt, ...);