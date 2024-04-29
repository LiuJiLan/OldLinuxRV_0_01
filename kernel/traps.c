#include <linux/trap.h>
#include <asm/ptrace.h>
#include <asm/system.h>
#include <sys/types.h>
#include <linux/kernel.h>

extern void print_debug(char* str);

static void die(char * str, struct pt_regs * regs)
{
    // this just for debug
    print_debug(str);
//    long * esp = (long *) esp_ptr;
//    int i;
//
//    printk("%s: %04x\n\r",str,nr&0xffff);
//    printk("EIP:\t%04x:%p\nEFLAGS:\t%p\nESP:\t%04x:%p\n",
//           esp[1],esp[0],esp[2],esp[4],esp[3]);
//    printk("fs: %04x\n",_fs());
//    printk("base: %p, limit: %p\n",get_base(current->ldt[1]),get_limit(0x17));
//    if (esp[4] == 0x17) {
//        printk("Stack: ");
//        for (i=0;i<4;i++)
//            printk("%p ",get_seg_long(0x17,i+(long *)esp[3]));
//        printk("\n");
//    }
//    str(i);
//    printk("Pid: %d, process nr: %d\n\r",current->pid,0xffff & i);
//    for(i=0;i<10;i++)
//        printk("%02x ",0xff & get_seg_byte(esp[1],(i+(char *)esp[0])));
//    printk("\n\r");
//    do_exit(11);		/* play segment exception */
}

void do_reserved(struct pt_regs * regs) {
    die("Reserved Exception", regs);
    regs->epc += 4;
}

void do_insn_illegal(struct pt_regs * regs) {
    die("Reserved Exception", regs);
    regs->epc += 4;
}

static const char * break_from_user[2] = {"User", "Kernel"};
static const char * pt_regs_name[36];
void do_break(struct pt_regs * regs) {

    printk("Breakpoint From %s Mode.\n", break_from_user[user_mode(regs)]);
    printk("Register in pt_regs:\n");
    for (int i = 0; i < 36; i++) {
        printk("%s:\t\t0x%p\n", pt_regs_name[i], ((size_t*)regs)[i]);
    }
    regs->epc += 4;
}

extern int sbi_printf(const char *fmt, ...);
void do_early_trap(struct pt_regs * regs) {
    sbi_printf("Early Trap caused by %d\nRegister in pt_regs:\n", regs->cause);
    for (int i = 0; i < 36; i++) {
        sbi_printf("%s:\t\t0x%p\n", pt_regs_name[i], ((size_t*)regs)[i]);
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

void system_call(struct pt_regs * regs) {
    regs->epc += 4;
}

// In mm/memory.c
extern void do_page_fault(struct pt_regs * regs);

void do_trap_unknown(struct pt_regs * regs) {
    die("Unknown Exception", regs);
    regs->epc += 4;
}

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