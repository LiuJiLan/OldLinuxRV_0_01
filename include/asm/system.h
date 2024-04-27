#ifndef _SYSTEM_H
#define _SYSTEM_H


// rv64 only!!!

// copy from linux/arch/riscv/include/asm/csr.h

// bits in sstatus
#define SR_SIE		    0x00000002UL /* Supervisor Interrupt Enable */
#define SR_SPIE		    0x00000020UL /* Previous Supervisor IE */
#define SR_SPP		    0x00000100UL /* Previously Supervisor */
#define SR_SUM		    0x00040000UL /* Supervisor User Memory Access */

#define SR_FS		    0x00006000UL /* Floating-point Status */
#define SR_FS_OFF	    0x00000000UL
#define SR_FS_INITIAL	0x00002000UL
#define SR_FS_CLEAN	    0x00004000UL
#define SR_FS_DIRTY	    0x00006000UL

#define SR_VS		    0x00000600UL /* Vector Status */
#define SR_VS_OFF	    0x00000000UL
#define SR_VS_INITIAL	0x00000200UL
#define SR_VS_CLEAN	    0x00000400UL
#define SR_VS_DIRTY	    0x00000600UL

#define SR_FS_VS	    (SR_FS | SR_VS) /* Vector and Floating-Point Unit */

// only support sv39 now
#define SATP_MODE_39	0x8000000000000000UL

#define CAUSE_IRQ_FLAG	0x8000000000000000UL

// Interrupt causes in scause
#define IRQ_S_SOFT		1
#define IRQ_S_TIMER		5
#define IRQ_S_EXT		9

// Exception causes in scause
#define EXC_INST_MISALIGNED	        0
#define EXC_INST_ACCESS		        1
#define EXC_INST_ILLEGAL	        2
#define EXC_BREAKPOINT		        3
#define EXC_LOAD_MISALIGNED	        4
#define EXC_LOAD_ACCESS		        5
#define EXC_STORE_MISALIGNED	    6
#define EXC_STORE_ACCESS	        7
#define EXC_SYSCALL		            8
#define EXC_HYPERVISOR_SYSCALL	    9
#define EXC_SUPERVISOR_SYSCALL	    10
#define EXC_INST_PAGE_FAULT	        12
#define EXC_LOAD_PAGE_FAULT	        13
#define EXC_STORE_PAGE_FAULT	    15

// for timer
#define CSR_TIME		0xc01

// 用别名其实也可以
#define CSR_SSTATUS		0x100
#define CSR_SIE			0x104
#define CSR_STVEC		0x105
#define CSR_SSCRATCH	0x140
#define CSR_SEPC		0x141
#define CSR_SCAUSE		0x142
#define CSR_STVAL		0x143
#define CSR_SIP			0x144
#define CSR_SATP		0x180


// Linux在这里实现了M和S的解耦, 我们暂时不管这点

# define CSR_STATUS	    CSR_SSTATUS
# define CSR_IE		    CSR_SIE
# define CSR_TVEC	    CSR_STVEC
# define CSR_SCRATCH	CSR_SSCRATCH
# define CSR_EPC	    CSR_SEPC
# define CSR_CAUSE	    CSR_SCAUSE
# define CSR_TVAL	    CSR_STVAL
# define CSR_IP		    CSR_SIP

# define SR_IE		    SR_SIE
# define SR_PIE		    SR_SPIE
# define SR_PP		    SR_SPP

# define RV_IRQ_SOFT	IRQ_S_SOFT
# define RV_IRQ_TIMER	IRQ_S_TIMER
# define RV_IRQ_EXT		IRQ_S_EXT

#define IE_SIE		0x00000002UL
#define IE_TIE		0x00000020UL
#define IE_EIE		0x00000200UL

#define csr_read(csr)                       \
({                                          \
	register unsigned long __v;             \
	__asm__ __volatile__ ("csrr %0, " #csr  \
			      : "=r" (__v) :            \
			      : "memory");			    \
	__v;							        \
})

#define csr_write(csr, val)					    \
({								                \
	unsigned long __v = (unsigned long)(val);	\
	__asm__ __volatile__ ("csrw " #csr ", %0"   \
			      : : "rK" (__v)			    \
			      : "memory");			        \
})


#endif
