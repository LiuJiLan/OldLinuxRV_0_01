#include <linux/trap.h>
#include <asm/ptrace.h>
#include <asm/system.h>
#include <sys/types.h>
#include <linux/kernel.h>

extern void print_debug(char* str);

static void die(char * str, struct pt_regs * regs)
{
    int i;
    printk("%s: %02x\n\r", str, regs->cause & 0x3fUL);
    printk("EPC:\t%p\nRA:\t%p\nSP:\t%p\nSTATUS:\t%p\nBADADDR:\t%p\n",
           regs->epc, regs->ra, regs->sp, regs->status, regs->badaddr);
    printk("Pid: %d\n\r", current->pid);
    printk("\n\r");
    do_exit(11);		/* play segment exception */
}

void do_reserved(struct pt_regs * regs) {
    die("Reserved Exception", regs);
    regs->epc += 4;
}

void do_insn_illegal(struct pt_regs * regs) {
    die("Reserved Exception", regs);
    regs->epc += 4;
}

static int show_reg[] = {0, 1, 2, 4, 32, 33, 34};
static const char * break_from_user[2] = {"User", "Kernel"};
static const char * pt_regs_name[36];
void do_break(struct pt_regs * regs) {

    printk("Breakpoint From %s Mode.\n", break_from_user[user_mode(regs)]);
    printk("Register in pt_regs:\n");
    int i, n = (sizeof(show_reg) / sizeof(int));
    for (i = 0; i < n; i++) {
        printk("%s:\t\t0x%p\n", pt_regs_name[show_reg[i]], ((size_t*)regs)[show_reg[i]]);
    }
    regs->epc += 4;
}

extern int sbi_printf(const char *fmt, ...);
void do_early_trap(struct pt_regs * regs) {
    sbi_printf("Early Trap caused by %d\nRegister in pt_regs:\n", regs->cause);
    int i, n = (sizeof(show_reg) / sizeof(int));
    for (i = 0; i < n; i++) {
        sbi_printf("%s:\t\t0x%p\n", pt_regs_name[show_reg[i]], ((size_t*)regs)[show_reg[i]]);
    }
    print_debug("do_early_trap");
    regs->epc += 4;
}


void do_misaligned(struct pt_regs * regs) {
    die("Misaligned Exception", regs);
    regs->epc += 4;
}

void do_access_fault(struct pt_regs * regs) {
    die("Access Fault Exception", regs);
    regs->epc += 4;
}

void do_impossible_ecall(struct pt_regs * regs) {
    die("Impossible Ecall Exception", regs);
    regs->epc += 4;
}

extern void system_call(struct pt_regs * regs);

// In mm/memory.c
extern void do_page_fault(struct pt_regs * regs);

void do_trap_unknown(struct pt_regs * regs) {
    die("Unknown Exception", regs);
    regs->epc += 4;
}

// 为早期debug, 先用SBI输出
void trap_init(void) {
    set_excp_vect(0,&do_misaligned);
    set_excp_vect(1,&do_access_fault);
    set_excp_vect(2,&do_insn_illegal);
    set_excp_vect(3,&do_break);
    set_excp_vect(4,&do_misaligned);
    set_excp_vect(5,&do_access_fault);
    set_excp_vect(6,&do_misaligned);
    set_excp_vect(7,&do_access_fault);
    set_excp_vect(8,&system_call);
    set_excp_vect(9,&do_impossible_ecall);
    set_excp_vect(10,&do_reserved);
    set_excp_vect(11,&do_impossible_ecall);
    set_excp_vect(12,&do_page_fault);
    set_excp_vect(13,&do_page_fault);
    set_excp_vect(14,&do_reserved);
    set_excp_vect(15,&do_page_fault);
}

static const char * pt_regs_name[36] = {
        "epc",
        "ra",
        "sp",
        "gp",
        "tp",
        "t0",
        "t1",
        "t2",
        "s0",
        "s1",
        "a0",
        "a1",
        "a2",
        "a3",
        "a4",
        "a5",
        "a6",
        "a7",
        "s2",
        "s3",
        "s4",
        "s5",
        "s6",
        "s7",
        "s8",
        "s9",
        "s10",
        "s11",
        "t3",
        "t4",
        "t5",
        "t6",
        "status",
        "badaddr",
        "cause",
        "orig_a0"
};