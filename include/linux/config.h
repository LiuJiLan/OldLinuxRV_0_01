#ifndef _CONFIG_H
#define _CONFIG_H

//############################################
//先放这里再说

#if __riscv_xlen != 64
#error "Now, codes only support RISC-V 64."
#endif

#define THREAD_SIZE         (PAGE_SIZE << 1)

// #define VA_BITS 39
// #define VA_KERNEL (1UL << (VA_BITS - 1))

// In sched.h
// #define TASK_SIZE	(~1UL << (VA_BITS - 1))

#define VA_KERNEL       0xFFFFFFC000000000U
#define LD_VA_KERNEL    0xFFFFFFC000000000 // 为了链接脚本
#define PA_KERNEL       (RAM_START + SBI_SIZE)
#define LD_PA_KERNEL    (LD_RAM_START + LD_SBI_SIZE)
#define V_P_DIFF        (VA_KERNEL - PA_KERNEL)

// 还是沿用xv6-riscv的宏
#define V2P(a) (((unsigned long) (a)) - V_P_DIFF)   //  64位下改了数据类型
#define P2V(a) ((void *)(((unsigned char *) (a)) + V_P_DIFF))

//  纯数值的转换
#define V2P_WO(x) ((x) - V_P_DIFF)    // same as V2P, but without casts //  不带类型转换
#define P2V_WO(x) ((x) + V_P_DIFF)    // same as P2V, but without casts

// 临时给设备用, vma之后再调
#define D_VA_START      0xffffffff80000000UL // -2GiB
#define D_PA_START      0x0UL
#define D_V_P_DIFF        (D_VA_START - D_PA_START)
#define D_V2P_WO(x) ((x) - D_V_P_DIFF)
#define D_P2V_WO(x) ((x) + D_V_P_DIFF)

//  ### //
//  这块放置不同平台相关的数值
#define RAM_START       0x80000000U    // QEMU的内存起点
#define LD_RAM_START    0x80000000
#define RAM_SIZE        0x08000000U    // 128MiB 比赛要求的QEMU大小为128M
#define SBI_SIZE        0x00200000U    // rv64的2MiB的SBI
#define LD_SBI_SIZE     0x00200000

//  注意给SBI的时钟中断间隔应该为
//  TIMEBASE_FREQUENCY / HZ
#define TIMEBASE_FREQUENCY 10000000UL
//#define TIMEBASE_FREQUENCY 4000000UL



// for QEMU
#define PLIC_SOURCE_NR          54
#define PLIC_BASE_PA            0x0c000000UL
#define PLIC_BASE               D_P2V_WO(PLIC_BASE_PA)
#define PLIC_PRIORITY(id)       (PLIC_BASE + (id) * 4)
#define PLIC_PENDING(id)        (PLIC_BASE + 0x1000 + ((id) / 32) * 4)
#define PLIC_SENABLE(hart, id)  (PLIC_BASE + 0x2080 + (hart) * 0x100 + ((id) / 32) * 4)
#define PLIC_STHRESHOLD(hart)   (PLIC_BASE + 0x201000 + (hart) * 0x2000)
#define PLIC_SCLAIM(hart)       (PLIC_BASE + 0x201004 + (hart) * 0x2000)
#define PLIC_SCOMPLETE(hart)    (PLIC_BASE + 0x201004 + (hart) * 0x2000)


// for VF2
//#define PLIC_SOURCE_NR 136
//#define PLIC_BASE_PA 0x0c000000UL
//#define PLIC_BASE    D_P2V_WO(PLIC_BASE_PA)
//#define PLIC_PRIORITY(id) (PLIC_BASE + (id) * 4)
//#define PLIC_PENDING(id) (PLIC_BASE + 0x1000 + ((id) / 32) * 4)
//#define PLIC_SENABLE(hart, id) (PLIC_BASE + 0x2000 + (hart) * 0x100 + ((id) / 32) * 4)
//#define PLIC_STHRESHOLD(hart) (PLIC_BASE + 0x200000 + (hart) * 0x2000)
//#define PLIC_SCLAIM(hart) (PLIC_BASE + 0x200004 + (hart) * 0x2000)
//#define PLIC_SCOMPLETE(hart) (PLIC_BASE + 0x200004 + (hart) * 0x2000)




//############################################

#define HIGH_MEMORY (RAM_START + RAM_SIZE)

#define BUFFER_SIZE 0x400000UL // 先留4MiB
#define BUFFER_END (RAM_START + SBI_SIZE + BUFFER_SIZE)


/* Root device at bootup. */
#define ROOT_DEV 0x0301 //第一个硬盘第一个分区

#endif