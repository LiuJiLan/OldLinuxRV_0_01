#ifndef _TRAP_H
#define _TRAP_H

#include <asm/ptrace.h>
#include <asm/system.h>

extern void (*excp_vect_table[16])(struct pt_regs *);

#define set_excp_vect(n,addr) (excp_vect_table[n] = addr)

static inline void local_irq_enable(void) {
    csr_set(CSR_SSTATUS, SR_SIE);
}

static inline void local_irq_disable(void) {
    csr_clear(CSR_SSTATUS, SR_SIE);
}

#endif