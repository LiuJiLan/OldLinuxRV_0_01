#ifndef _SBI_H
#define _SBI_H

#include <sys/types.h>

struct sbiret {
    long error;
    long value;
};

struct sbiret sbi_ecall(int ext, int fid, unsigned long arg0,
                        unsigned long arg1, unsigned long arg2,
                        unsigned long arg3, unsigned long arg4,
                        unsigned long arg5);

void sbi_console_putchar(int ch);
void sbi_shutdown(void);
void sbi_set_timer(uint64 stime_value);

#endif