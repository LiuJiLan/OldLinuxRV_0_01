#ifndef _TRAP_H
#define _TRAP_H

#include <asm/ptrace.h>

extern void (*excp_vect_table[16])(struct pt_regs *);

#define set_excp_vect(n,addr) (excp_vect_table[n] = addr)

#endif