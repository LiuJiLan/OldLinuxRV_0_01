#include <asm/sbi.h>

struct sbiret sbi_ecall(int ext, int fid, unsigned long arg0,
                        unsigned long arg1, unsigned long arg2,
                        unsigned long arg3, unsigned long arg4,
                        unsigned long arg5)
{
    struct sbiret ret;

    register uptr_t a0 asm ("a0") = (uptr_t)(arg0);
    register uptr_t a1 asm ("a1") = (uptr_t)(arg1);
    register uptr_t a2 asm ("a2") = (uptr_t)(arg2);
    register uptr_t a3 asm ("a3") = (uptr_t)(arg3);
    register uptr_t a4 asm ("a4") = (uptr_t)(arg4);
    register uptr_t a5 asm ("a5") = (uptr_t)(arg5);
    register uptr_t a6 asm ("a6") = (uptr_t)(fid);
    register uptr_t a7 asm ("a7") = (uptr_t)(ext);
    asm volatile ("ecall"
            : "+r" (a0), "+r" (a1)
            : "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6), "r" (a7)
            : "memory");
    ret.error = a0;
    ret.value = a1;

    return ret;
}

#define SBI_EXT_0_1_CONSOLE_PUTCHAR 0x1
#define SBI_EXT_0_1_SHUTDOWN 0x8

#define SBI_EXT_TIME 0x54494D45
#define SBI_EXT_TIME_SET_TIMER 0

void sbi_console_putchar(int ch)
{
    sbi_ecall(SBI_EXT_0_1_CONSOLE_PUTCHAR, 0, ch, 0, 0, 0, 0, 0);
}

void sbi_shutdown(void)
{
    sbi_ecall(SBI_EXT_0_1_SHUTDOWN, 0, 0, 0, 0, 0, 0, 0);
}

void sbi_set_timer(uint64 stime_value)
{
    sbi_ecall(SBI_EXT_TIME, SBI_EXT_TIME_SET_TIMER, stime_value, 0,
              0, 0, 0, 0);
}