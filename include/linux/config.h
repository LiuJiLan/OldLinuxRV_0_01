#ifndef _CONFIG_H
#define _CONFIG_H

//############################################
//先放这里再说

#if __riscv_xlen != 64
#error "Now, codes only support RISC-V 64."
#endif

#define THREAD_SIZE         (PAGE_SIZE << 1)

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
#define PLIC_SOURCE_NR 54
#define PLIC_BASE_PA 0x0c000000UL
#define PLIC_BASE    P2V_WO(PLIC_BASE_PA)
#define PLIC_PRIORITY(id) (PLIC_BASE + (id) * 4)
#define PLIC_PENDING(id) (PLIC_BASE + 0x1000 + ((id) / 32) * 4)
#define PLIC_SENABLE(hart, id) (PLIC_BASE + 0x2080 + (hart) * 0x100 + ((id) / 32) * 4)
#define PLIC_STHRESHOLD(hart) (PLIC_BASE + 0x201000 + (hart) * 0x2000)
#define PLIC_SCLAIM(hart) (PLIC_BASE + 0x201004 + (hart) * 0x2000)
#define PLIC_SCOMPLETE(hart) (PLIC_BASE + 0x201004 + (hart) * 0x2000)


// for VF2
//#define PLIC_SOURCE_NR 136
//#define PLIC_BASE_PA 0x0c000000UL
//#define PLIC_BASE    P2V_WO(PLIC_BASE_PA)
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



/* #define LASU_HD */
#define LINUS_HD

/* Root device at bootup. */
#if	defined(LINUS_HD)
#define ROOT_DEV 0x306
#elif	defined(LASU_HD)
#define ROOT_DEV 0x302
#else
#error "must define HD"
#endif

/*
 * HD type. If 2, put 2 structures with a comma. If just 1, put
 * only 1 struct. The structs are { HEAD, SECTOR, TRACKS, WPCOM, LZONE, CTL }
 *
 * NOTE. CTL is supposed to be 0 for drives with less than 8 heads, and
 * 8 if heads >= 8. Don't know why, and I haven't tested it on a drive with
 * more than 8 heads, but that is what the bios-listings seem to imply. I
 * just love not having a manual.
 */
#if	defined(LASU_HD)
#define HD_TYPE { 7,35,915,65536,920,0 }
#elif	defined(LINUS_HD)
#define HD_TYPE { 5,17,980,300,980,0 },{ 5,17,980,300,980,0 }
#else
#error "must define a hard-disk type"
#endif

#endif
