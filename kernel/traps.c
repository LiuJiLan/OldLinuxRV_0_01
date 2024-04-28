#include <linux/trap.h>
#include <asm/ptrace.h>

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

void do_trap_unknown(struct pt_regs * regs) {
    die("Unknown Exception", regs);
    regs->epc += 4;
}