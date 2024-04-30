// 名字来自0.97之后开始的irq.h

#include <linux/trap.h>
#include <linux/sys.h>
#include <linux/kernel.h>
#include <sys/types.h>

#define CLINT_IRQ (RV_IRQ_SOFT | RV_IRQ_TIMER | RV_IRQ_EXT)

void do_irq_unknown(struct pt_regs * regs);
void do_irq_software(struct pt_regs * regs); // This is IPI !!!
void do_irq_timer(struct pt_regs * regs);
void do_irq_external(struct pt_regs * regs);

extern void do_timer(struct pt_regs * regs);

void do_irq(struct pt_regs * regs) {
    // 仅处理clint的三个中断
    unsigned long intr_type = regs->cause & CLINT_IRQ;
    switch (intr_type) {
        case RV_IRQ_SOFT:
        case RV_IRQ_TIMER:
        case RV_IRQ_EXT:
        default:
            do_irq_unknown(regs);
    }
}

// 仅包含对CLINT的未知中断处理, 例如可能来自CLIC
void do_irq_unknown(struct pt_regs * regs) {
    // 直接panic, 因为没有手段去清除
    // 进panic死循环是避免中断风暴的唯一手段
    panic("Unknown IRQ from CLINT");
}

// 按照Linux内核的说法这个应该是IPI中断
void do_irq_software(struct pt_regs * regs) {
    printk("IPI or so called RISC-V Software Interrupt.\n");
    csr_clear(sip, RV_IRQ_SOFT);
    // 仅清除中断即可
}

void do_irq_timer(struct pt_regs * regs) {
    do_timer(regs);
}

void (*plic_intr_table[PLIC_SOURCE_NR])(struct pt_regs *) = {0};

void do_irq_external(struct pt_regs * regs) {
    int cpu = smp_processor_id();
    int irq = *(uint32_t*)PLIC_SCLAIM(cpu);
    regs->orig_a0 = irq;
    plic_intr_table[irq](regs);
    *(uint32_t*)PLIC_SCOMPLETE(cpu) = regs->orig_a0;
}

void irq_software_init(void) {
    csr_set(sie, RV_IRQ_SOFT);
}

void irq_timer_init(void) {
    csr_set(sie, RV_IRQ_TIMER);
}

void irq_external_init(void) {
    int cpu = smp_processor_id();
    // 获取第一个中断使能寄存器的位置
    uint32_t * p = (uint32_t*)PLIC_SENABLE(cpu, 0);
    // 清除可能的外部中断使能
    for (int i = 0; i < (PLIC_SOURCE_NR / 32 + 1); i++) {
        *(p+i) = 0x0;
    }
    *(uint32_t*)PLIC_STHRESHOLD(cpu) = 0;
    csr_set(sie, RV_IRQ_EXT);
}

// 要在页表初始化之后进行
void irq_init(void) {
    irq_software_init();
    irq_timer_init();
    irq_external_init();
}

void set_intr_init(int n, void (*addr)(struct pt_regs *)) {
    int cpu = smp_processor_id();
    *(uint32_t*)PLIC_PRIORITY(n) = 7;
    set_intr_vect(n, addr);
    *(uint32_t*)PLIC_SENABLE(cpu, n) |= 0x1 << n % 32;
}