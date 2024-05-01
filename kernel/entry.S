// 所有的trap时的栈都遵循pt_regs
// 基本上照搬RISC-V Linux

#include <linux/config.h>
#include <asm/system.h>
#include <asm/asm.h>

/* save all GPs except x1 ~ x5 */
.macro save_from_x6_to_x31
REG_S x6,  PT_T1(sp)
REG_S x7,  PT_T2(sp)
REG_S x8,  PT_S0(sp)
REG_S x9,  PT_S1(sp)
REG_S x10, PT_A0(sp)
REG_S x11, PT_A1(sp)
REG_S x12, PT_A2(sp)
REG_S x13, PT_A3(sp)
REG_S x14, PT_A4(sp)
REG_S x15, PT_A5(sp)
REG_S x16, PT_A6(sp)
REG_S x17, PT_A7(sp)
REG_S x18, PT_S2(sp)
REG_S x19, PT_S3(sp)
REG_S x20, PT_S4(sp)
REG_S x21, PT_S5(sp)
REG_S x22, PT_S6(sp)
REG_S x23, PT_S7(sp)
REG_S x24, PT_S8(sp)
REG_S x25, PT_S9(sp)
REG_S x26, PT_S10(sp)
REG_S x27, PT_S11(sp)
REG_S x28, PT_T3(sp)
REG_S x29, PT_T4(sp)
REG_S x30, PT_T5(sp)
REG_S x31, PT_T6(sp)
.endm

/* restore all GPs except x1 ~ x5 */
.macro restore_from_x6_to_x31
REG_L x6,  PT_T1(sp)
REG_L x7,  PT_T2(sp)
REG_L x8,  PT_S0(sp)
REG_L x9,  PT_S1(sp)
REG_L x10, PT_A0(sp)
REG_L x11, PT_A1(sp)
REG_L x12, PT_A2(sp)
REG_L x13, PT_A3(sp)
REG_L x14, PT_A4(sp)
REG_L x15, PT_A5(sp)
REG_L x16, PT_A6(sp)
REG_L x17, PT_A7(sp)
REG_L x18, PT_S2(sp)
REG_L x19, PT_S3(sp)
REG_L x20, PT_S4(sp)
REG_L x21, PT_S5(sp)
REG_L x22, PT_S6(sp)
REG_L x23, PT_S7(sp)
REG_L x24, PT_S8(sp)
REG_L x25, PT_S9(sp)
REG_L x26, PT_S10(sp)
REG_L x27, PT_S11(sp)
REG_L x28, PT_T3(sp)
REG_L x29, PT_T4(sp)
REG_L x30, PT_T5(sp)
REG_L x31, PT_T6(sp)
.endm




.text
.global handle_exception

// 4 bytes 对齐
.align 2
handle_exception:
    // 如果是0则是来自内核态
    csrrw tp, CSR_SCRATCH, tp
    bnez tp, save_context

    restore_kernel_tpsp:
        // 是来自内核则保存内核栈
        csrr tp, CSR_SCRATCH
        REG_S sp, TASK_TI_KERNEL_SP(tp)

    save_context:
        // 无论是否来自内核都保存用户栈
    	REG_S sp, TASK_TI_USER_SP(tp)
    	// 改用内核栈
    	REG_L sp, TASK_TI_KERNEL_SP(tp)
    	addi sp, sp, -(PT_SIZE)
    	REG_S x1,  PT_RA(sp)
    	REG_S x3,  PT_GP(sp)
    	REG_S x5,  PT_T0(sp)
    	save_from_x6_to_x31

        li t0, SR_SUM | SR_FS_VS

        // 从thread_info中取sp到pt_regs
        REG_L s0, TASK_TI_USER_SP(tp)
        csrrc s1, CSR_STATUS, t0
        csrr s2, CSR_EPC
        csrr s3, CSR_TVAL
        csrr s4, CSR_CAUSE
        csrr s5, CSR_SCRATCH
        REG_S s0, PT_SP(sp)
        REG_S s1, PT_STATUS(sp)
        REG_S s2, PT_EPC(sp)
        REG_S s3, PT_BADADDR(sp)
        REG_S s4, PT_CAUSE(sp)
        REG_S s5, PT_TP(sp)

    // 设为0标记当前为内核态
    csrw CSR_SCRATCH, x0

    // 作为后面所有的参数
    move a0, sp /* pt_regs */
    // 装载返回点
    la ra, ret_from_exception

    // 论文中提到的Linux技巧
    bge s4, zero, 1f

    // Handle interrupts
    tail do_irq

    1:
        // Handle exceptions
        slli t0, s4, RISCV_LGPTR
        la t1, excp_vect_table
        la t2, excp_vect_table_end
        add t0, t1, t0
        // 异常越界的情况下
        bgeu t0, t2, 1f
        REG_L t0, 0(t0)
        jr t0
    1:
        tail do_trap_unknown

// 4 bytes 对齐
.align 2
// 会被handle_exception和ret_from_fork调用
ret_from_exception:
	REG_L s0, PT_STATUS(sp)
	andi s0, s0, SR_SPP
	bnez s0, 1f

	// 如果准备返回用户态则提前把还原后的栈存到thread_info里
	// 但sp本身还维持着内核栈
	addi s0, sp, (PT_SIZE)
	REG_S s0, TASK_TI_KERNEL_SP(tp)

    // 平时由sscratch存放进程描述符
	csrw CSR_SCRATCH, tp
    1:
        REG_L a0, PT_STATUS(sp)
        REG_L  a2, PT_EPC(sp)
        // Linux 用这个指令处理一些不同hart的问题
        // 其实 REG_S a2, PT_EPC(sp)应该就能在单核运行了
        REG_SC x0, a2, PT_EPC(sp)

        csrw CSR_STATUS, a0
        csrw CSR_EPC, a2

        REG_L x1,  PT_RA(sp)
        REG_L x3,  PT_GP(sp)
        REG_L x4,  PT_TP(sp)
        REG_L x5,  PT_T0(sp)
        restore_from_x6_to_x31

        // 如果是返回到用户, 这里面是用户栈
        // 如果是返回到内核, 这里面是内核栈
        REG_L x2,  PT_SP(sp)
	sret

// 4 bytes 对齐
.align 2
// fork后新进程开始执行的地方
// 暂时仅支持用户进程/线程
ret_from_fork:
    // schedule_tail紧接在switch_to后
    // 参数是switch_to放在a0里的prev上一个进程
	call schedule_tail
	move a0, sp /* pt_regs */
	la ra, ret_from_exception
	// tail syscall_exit_to_user_mode
	ret

// 4 bytes 对齐
.align 2
.global __switch_to
// a0: previous task_struct
// a1: next task_struct
// a0在整个过程中没有改变, 所以又相当于返回了a0作为prev
__switch_to:
	/* Save context into prev->thread */
	la    a4, OFFSET_TASK_THREAD
	ld    a4, 0(a4)
	add   a3, a0, a4
	add   a4, a1, a4
	REG_S ra,  THREAD_RA(a3)
	REG_S sp,  THREAD_SP(a3)
	REG_S s0,  THREAD_S0(a3)
	REG_S s1,  THREAD_S1(a3)
	REG_S s2,  THREAD_S2(a3)
	REG_S s3,  THREAD_S3(a3)
	REG_S s4,  THREAD_S4(a3)
	REG_S s5,  THREAD_S5(a3)
	REG_S s6,  THREAD_S6(a3)
	REG_S s7,  THREAD_S7(a3)
	REG_S s8,  THREAD_S8(a3)
	REG_S s9,  THREAD_S9(a3)
	REG_S s10, THREAD_S10(a3)
	REG_S s11, THREAD_S11(a3)

	/* Restore context from next->thread */
	REG_L ra,  THREAD_RA(a4)
	REG_L sp,  THREAD_SP(a4)
	REG_L s0,  THREAD_S0(a4)
	REG_L s1,  THREAD_S1(a4)
	REG_L s2,  THREAD_S2(a4)
	REG_L s3,  THREAD_S3(a4)
	REG_L s4,  THREAD_S4(a4)
	REG_L s5,  THREAD_S5(a4)
	REG_L s6,  THREAD_S6(a4)
	REG_L s7,  THREAD_S7(a4)
	REG_L s8,  THREAD_S8(a4)
	REG_L s9,  THREAD_S9(a4)
	REG_L s10, THREAD_S10(a4)
	REG_L s11, THREAD_S11(a4)
	/* The offset of thread_info in task_struct is zero. */
	move tp, a1
	ret


.data
.global excp_vect_table
.global excp_vect_table_end

.align RISCV_LGPTR // 2^3对齐
excp_vect_table:
    .rept 16 // 暂时仅支持的16个异常(包含reserved)
    // RISCV_PTR do_reserved
    RISCV_PTR do_early_trap // 开发期间调试
    .endr
excp_vect_table_end:
