#include <sys/types.h>
#include <linux/mm.h>
#include <linux/config.h>
#include <linux/kernel.h>
#include <asm/ptrace.h>
#include <asm/system.h>
#include <string.h>

#define PMD_SIZE     (PAGE_SIZE << 9)

pte_t trampoline_pg_dir[];
pte_t trampoline_pg_l2pte[];
uptr_t dtb_va = 0;
//uptr_t early_stage_va_end = 0;

#define LOW_MEM BUFFER_END

#define PAGING_MEMORY (HIGH_MEMORY - LOW_MEM)
#define PAGING_PAGES (PAGING_MEMORY/4096)
#define MAP_NR(addr) (((addr)-P2V(LOW_MEM))>>12)

// 4096 bytes -> 2 bytes
// 1MiB -> 512 bytes = 0.5 KiB
// 1GiB -> 512KiB = 0.5 MiB
static unsigned short mem_map[PAGING_PAGES] = {0,};

pte_t *walk(pagetable_t pagetable, uint64 va, int alloc);
uint64 walkaddr(pagetable_t pagetable, uint64 va);
int mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm);
void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free);
void freewalk(pagetable_t pagetable, int high_level);

// 返回内核虚拟地址
unsigned long get_free_page(void) {
    // 扫描mem_map查找空闲页
    for (int i = 0; i < PAGING_PAGES; i++) {
        if (mem_map[i] == 0) {
            mem_map[i] = 1;  // 标记为已使用
            unsigned long page_addr = P2V(LOW_MEM) + i * PAGE_SIZE;
            // 初始化页面内容为0
            memset((void *) page_addr, 0, PAGE_SIZE);
            return page_addr;
        }
    }
    return 0;  // 如果没有空闲页面，返回0
}

// 参数为虚拟地址!!! 和原版不一样!!!
void free_page(unsigned long addr) {
    if (addr < LOW_MEM + VA_KERNEL) return;
    if (addr > HIGH_MEMORY + VA_KERNEL)
        panic("trying to free nonexistent page");
    addr -= (LOW_MEM + VA_KERNEL);
    addr >>= 12;
    if (mem_map[addr]--) return;
    mem_map[addr] = 0;
    panic("trying to free free page");
}

int map_page(pte_t * page_table, size_t va, size_t pa, int perm, int top_level, int leaf_level);

// init swapper_pg_dir with direct mapping
// 没有映射设备地址
void paging_init(void) {
    uint64 ram_start = PA_KERNEL;
    uint64 ram_end = RAM_START + RAM_SIZE;
    uint64 satp = 0;
    int perm = PTE_X | PTE_W | PTE_R | PTE_V;

//    if (0 != mappages(swapper_pg_dir, P2V(ram_start), ram_end - ram_start, ram_start, perm)) {
//        panic("paging_init");
//    }
    while (ram_start < ram_end) {
        map_page(swapper_pg_dir, P2V(ram_start), ram_start, perm, 2, 0);
        ram_start += PAGE_SIZE;
    }


    // 临时措施, 未来把外设的页表移到外设初始化里面去
    // RTC
    ram_start = 0x10000UL;
    ram_end = ram_start + PAGE_SIZE;
    while (ram_start < ram_end) {
        map_page(swapper_pg_dir, P2V(ram_start), ram_start, perm, 2, 0);
        ram_start += PAGE_SIZE;
    }

    // PLIC, 有点浪费, 但是就这样了 (后面两页是UART和VIRT_IO)
    ram_start = 0xC000000UL;
    ram_end = 0x10000000UL + PAGE_SIZE * 2;
    while (ram_start < ram_end) {
        map_page(swapper_pg_dir, P2V(ram_start), ram_start, perm, 2, 0);
        ram_start += PAGE_SIZE;
    }

    satp = SATP_MODE_39 | V2P(swapper_pg_dir) >> PGSHIFT;
    csr_write(satp, satp);
    local_flush_tlb_all();
}

// int device_map_page(...) {
// }


void setup_vm(uptr_t dtb_pa) {
    dtb_va = P2V(dtb_pa);

    uptr_t pa_start = PA_KERNEL;
    uptr_t pa_end = RAM_START + RAM_SIZE;
    // 向上对齐, 可能会包括不存在的物理内存, 但是无所谓
    pa_end = (pa_end + PMD_SIZE - 1) & ~(PMD_SIZE - 1);

    uptr_t va_start = VA_KERNEL;
    // 结尾用512来约束

    pte_t perm = PTE_V;

    // 正如论文所说, 这里的编号是相反的
    trampoline_pg_dir[PX(2, va_start)] = PA2PTE(trampoline_pg_l2pte) | perm;

    perm = PTE_X | PTE_W | PTE_R | PTE_V;
    for (int i = PX(1, va_start); i < NPTE; i++) {
        if (pa_start < pa_end) {
            trampoline_pg_l2pte[i] = PA2PTE(pa_start) | perm;
        } else {
            break;
        }
        pa_start += PMD_SIZE;
    }
}

void do_page_fault(struct pt_regs *regs) {
    regs->epc += 4;
}

__attribute__((__aligned__(PGSIZE)))
pte_t swapper_pg_dir[NPTE] = {0};  // 参考页表 / 进程0的页表

__attribute__((__aligned__(PGSIZE)))
pte_t trampoline_pg_dir[NPTE] = {0};  // level 1

__attribute__((__aligned__(PGSIZE)))
pte_t trampoline_pg_l2pte[NPTE] = {0};  // level 2


/*
// 类似于put_page, 但有不同
// 仅接收已经分配好空间的root页表
// 接收的page_table为页表的虚拟地址
int map_page(pte_t * page_table, size_t va, size_t pa, int perm) {
    // 数组大小与页表级数相关, 3级页表则保留2个用于记录
    // 例如, 如果之后要支持sv48的4级页表, 则pte_index[3] = {-1, -1, -1};
    (pte_t *) pte_record[2] = {0}; // 同时记录当前函数中分配的页
    pte_t * current_table = page_table;
    pte_t * new_page = NULL;

    int index = -1;
    for (int level = 2; level > 0; level--) {
        index = PX(level, va);
        if (current_table[index] & PTE_V == 0) {
            if ((new_page = get_free_page(void)) == 0) {
                // free page(s) new allocated in this function
                for (; level < 2; level++) {
                    if (pte_record[level] != 0) {
                        // pte = pte_record[i][PX(level, va)]
                        // pa = get_pa_from(pte)
                        // va = __va(pa)
                        free_page(PTE2VA(pte_record[level-1][PX(level, va)]));
                    }
                }
                return -1;
            }
            pte_record[level-1] = new_page; // 记录这个是新分配的
            current_table[index] = VA2PTE(new_page);
        }
        // 无论是否新分配都可以由这一行获得
        // 但已经存在的则不记录, 避免被回滚
        current_table = PTE2VA(current_table[index]);
    }

    // for循环stop时, level为目标级别, 且一定存在
    current_table[PX(level, va)] = PA2PTE(pa) | perm;
    return 0;
}
 */

// 类似于put_page, 但有不同
// 接收的page_table为页表是虚拟地址, 且不做任何边界检查, 要求调用者来保证对齐
// root_level和leaf_level可以做灵活的巨页映射或者从已经映射过的页的中间开始映射
// 但是root_level的页表必须存在, 且仅接收已经分配好空间的top页表
// 例如, 将3级sv39的页表映射到4KiB的大小的页时, top_level = 3 - 1; leaf_level = 0;
// 例如, 将4级sv48的页表映射到2MiB的大小的页时, top_level = 4 - 1; leaf_level = 1;
int map_page(pte_t * page_table, size_t va, size_t pa, int perm, int top_level, int leaf_level) {
    // sv39只用2条记录, 因为top必须已经分配
    // RISC-V最多sv57有5级, 所以仅需4条记录来满足未来扩展
    pte_t * pte_record[4] = {0}; // 同时记录当前函数中分配的页
    pte_t * current_table = page_table;
    pte_t * new_page = NULL;

    int index = -1;
    int level = -1;
    for (level = top_level; level > leaf_level; level--) {
        index = PX(level, va);
        if ((current_table[index] & PTE_V) == 0) {
            if ((new_page = get_free_page()) == 0) {
                // free page(s) new allocated in this function
                for (; level < top_level; level++) {
                    if (pte_record[level] != 0) {
                        // pte = pte_record[i][PX(level, va)]
                        // pa = get_pa_from(pte)
                        // va = __va(pa)
                        free_page(PTE2VA(pte_record[level-1][PX(level, va)]));
                    }
                }
                return -1;
            }

            // 注意, 记录是记录level-1的。
            // 例如, top_level = 2新创建的本质上是属于level = 1的页表"目录"
            pte_record[level-1] = new_page; // 记录这个是新分配的
            current_table[index] = VA2PTE(new_page) | PTE_V;
            new_page = NULL;
        }
        // 无论是否新分配都可以由这一行获得
        // 但已经存在的则不记录, 避免被回滚
        current_table = PTE2VA(current_table[index]);
    }

    // for循环stop时, level为目标级别, 且一定存在
    current_table[PX(level, va)] = PA2PTE(pa) | perm;
    return 0;
}

// 如果成功, 返回正数
// 如果出错, 返回负数, 其绝对值是成功映射长度
size_t unmap_page_range(pte_t * page_table, size_t va, size_t pa, size_t size, int perm) {
    return 0;
}