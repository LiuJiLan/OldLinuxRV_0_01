#include <asm/system.h>
#include <linux/config.h>

// 只支持-顺序-单核-启动!!!



// 暂时还没学会Linux的那个宏,
// 只能用这张方式了, 否则会报错
#define PT_REGS_SIZE        288
#define PAGE_SIZE           4096
#define PAGE_SHIFT          12
#define THREAD_SIZE         (PAGE_SIZE << 1)

.text

.global _start
_start = V2P_WO(entry)

.global for_gdb_physical
for_gdb_physical = V2P_WO(for_gdb_virtual)

.global entry

.align 2
entry:
    // 关中断, 清中断pending
    // WARL, 所以可以直接写
	csrw CSR_IE, zero
	csrw CSR_IP, zero

    // if use gp, should load here

    // 清除sstatus中扩展功能位
    li t0, SR_FS_VS
    csrc CSR_STATUS, t0

    // 清bss
    //  相对地址, 所以现在a3、a4中都是物理地址
    la a3, bss_start
    la a4, bss_stop
    ble a4, a3, clear_bss_done
    clear_bss:
    	REG_S zero, (a3)
    	add a3, a3, RISCV_SZPTR
    	blt a3, a4, clear_bss
    clear_bss_done:

    // 与早期Linux使用的x86-32不一样,
    // rv gcc中只能取变量的地址
    la a2, boot_cpu_hartid
    REG_S a0, (a2)

    // 第一次设置内核(服务)运行的环境
    la tp, init_task_union
    la sp, init_task_union + THREAD_SIZE
    addi sp, sp, -PT_REGS_SIZE

    // 准备好dtb作为参数
    mv a0, a1

    // setup_vm接收被存入a1中的dtb地址
    call setup_vm

    // 准备将内核转跳到正确的虚拟地址
    // 留有一个传递正在使用的页表的接口
    // 多核启动中, 其它的流程中a0将被赋予swapper_pg_dir
    // 所以我们现在同时在拿trampoline_pg_dir做转跳页表和临时页表
    la a0, trampoline_pg_dir
    call relocate_enable_mmu

for_gdb_virtual:
    call setup_trap_vector

    //  用于调试
    //  从这里我们进入了VA中
    // 第二次设置内核(服务)运行的环境
    la tp, init_task_union
    la sp, init_task_union + THREAD_SIZE
    addi sp, sp, -PT_REGS_SIZE
    tail start_kernel

.align 2
relocate_enable_mmu:
    // 我们的kernel的va固定
    // Linux中rv64 kernel va不固定
    li a1, V_P_DIFF
    add ra, ra, a1

	la a2, 1f
	add a2, a2, a1
	csrw CSR_TVEC, a2

    // 注意, 这一步是接口
	mv a2, a0
	srl a2, a2, PAGE_SHIFT

    // 暂时只支持sv39
	li a1, SATP_MODE_39

	// 也是接口
	or a2, a2, a1

	la a0, trampoline_pg_dir
    srl a0, a0, PAGE_SHIFT
    or a0, a0, a1
    sfence.vma
    csrw CSR_SATP, a0


    .align 2
    1:
    // 对于启动核来说, 这个步骤重复了
    // 因为这是留给SMP的接口
    csrw CSR_SATP, a2
    sfence.vma

    ret

setup_trap_vector:
    ret

#undef PT_REGS_SIZE
#undef PAGE_SIZE
#undef PAGE_SHIFT
#undef THREAD_SIZE


