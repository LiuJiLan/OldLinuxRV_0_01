#include <sys/types.h>
#include <linux/mm.h>
#include <linux/config.h>
#include <linux/kernel.h>
#include <asm/ptrace.h>
#include <asm/system.h>
#include <string.h>
#include <asm/asm.h>
#include <linux/sched.h>
#include <errno.h>
#include <signal.h>

#define PMD_SIZE     (PAGE_SIZE << 9)

pte_t trampoline_pg_dir[];
pte_t trampoline_pg_l2pte[];
uptr_t dtb_va = 0;
//uptr_t early_stage_va_end = 0;

#define LOW_MEM BUFFER_END

#define PAGING_MEMORY (HIGH_MEMORY - LOW_MEM)
#define PAGING_PAGES (PAGING_MEMORY/4096)
//#define MAP_NR(addr) (((unsigned long)(addr)-(unsigned long)P2V(LOW_MEM))>>12)
#define MAP_NR(addr) (((unsigned long)(addr)-P2V_WO(LOW_MEM))>>12)


// 4096 bytes -> 2 bytes
// 1MiB -> 512 bytes = 0.5 KiB
// 1GiB -> 512KiB = 0.5 MiB
static unsigned short mem_map[PAGING_PAGES] = {0,};



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
    if (addr < P2V(LOW_MEM)) return;
    if (addr > P2V(HIGH_MEMORY))
        panic("trying to free nonexistent page");
    if (mem_map[MAP_NR(addr)]--) return;
    mem_map[MAP_NR(addr)] = 0;
    panic("trying to free free page");
}


// 转为进程描述符用的, 暂时这样吧
unsigned long get_two_free_pages(void) {
    // 扫描mem_map查找空闲页
    for (int i = PAGING_PAGES-2; i >= 0; i--) {
        if (mem_map[i] == 0 && mem_map[i + 1] == 0) {
            mem_map[i] = 1;  // 标记为已使用
            mem_map[i + 1] = 1;  // 标记为已使用
            unsigned long page_addr = P2V(LOW_MEM) + i * PAGE_SIZE;
            // 初始化页面内容为0
            memset((void *) page_addr, 0, PAGE_SIZE * 2);
            return page_addr;
        }
    }
    return 0;  // 如果没有空闲页面，返回0
}

// 转为进程描述符用的, 暂时这样
void free_two_pages(unsigned long addr) {
    int p1, p2;
    if (addr < P2V(LOW_MEM)) return;
    if (addr > P2V(HIGH_MEMORY))
        panic("trying to free nonexistent page");
    p1 = mem_map[MAP_NR(addr)]--;
    p2 = mem_map[MAP_NR(addr)+1]--;
    if (p1 && p2) return;
    mem_map[MAP_NR(addr)] = 0;
    mem_map[MAP_NR(addr)+1] = 0;
    panic("trying to free free page");
}

// 剩下页表相关的接口来自Linux 0.97.6

// 别问为什么命名那么离谱, blame Linux!!!

// !!!仅!!! 用于清除sv39的root页表中的表项
static void free_one_table(pte_t * pte) {
    // 注意!!! 缺乏异常处理和保护, 使用的时候小心
    if (*pte & PTE_V) {
        // 缺乏对巨页的异常处理
        // 缺乏对sv39以外的支持

        pte_t * l1_pte = PTE2VA(*pte);
        for (int i = 0; i < NPTE; i++, l1_pte++) {
            if (*l1_pte & PTE_V) {
                pte_t * l0_pte = PTE2VA(*l1_pte);
                for (int j = 0; j < NPTE; j++, l0_pte++) {
                    if (*l0_pte & PTE_V) {
                        free_page(PTE2VA(*l0_pte));
                    }
                }
                free_page(PTE2VA(*l1_pte));
            }
        }
        free_page(PTE2VA(*pte));
    }
}

/*
 * This function clears all user-level page tables of a process - this
 * is needed by execve(), so that old pages aren't in the way. Note that
 * unlike 'free_page_tables()', this function still leaves a valid
 * page-table-tree in memory: it just removes the user pages. The two
 * functions are similar, but there is a fundamental difference.
 */
void clear_page_tables(struct task_struct * tsk) {
    int i;
    pte_t * page_dir;

    if (!tsk)
        return;
    if (tsk == task[0])
        panic("task[0] (swapper) doesn't support exec() yet\n");
    page_dir = (pte_t *) tsk->pgd;
    if (page_dir == swapper_pg_dir) {
        printk("Trying to clear kernel page-directory: not good\n");
        return;
    }
    // 在内核里操作, 不会有影响
    for (i = 0 ; i < NPTE / 2 ; i++,page_dir++)
        free_one_table(page_dir);
    invalidate();
    return;
}

/*
 * This function frees up all page tables of a process when it exits.
 */
void free_page_tables(struct task_struct * tsk) {
    int i;
    pte_t * page_dir;

    if (!tsk)
        return;
    if (tsk == FIRST_TASK) {
        printk("task[0] (swapper) killed: unable to recover\n");
        panic("Trying to free up swapper memory space");
    }
    page_dir = (pte_t *) tsk->pgd;
    if (page_dir == swapper_pg_dir) {
        printk("Trying to free kernel page-directory: not good\n");
        return;
    }
    if (tsk == current)
       load_root_page_table(SATP_MODE_39, swapper_pg_dir);

    pte_t * pte = page_dir;
    // 几乎和clear_page_tables, 而不是像0.97.6中那样有768和1024的区别
    // 高端内存直接引用就行了, 反正暂时没有实现vma
    for (i = 0 ; i < NPTE / 2 ; i++,pte++)
        free_one_table(pte);
    free_page(page_dir);
    invalidate();
}

// copy_page_tables的辅助函数
// !!!仅!!! 用于设置sv39的root页表中的表项的权限为用户态仅读, 并提高引用计数
// 没有对内核态的保护, 小心使用!!!
static void reference_count_one_table(pte_t * pte) {
    // 注意!!! 缺乏异常处理和保护, 使用的时候小心
    int perm = PTE_U | PTE_X | PTE_R | PTE_V;
    if (*pte & PTE_V) {
        // 缺乏对巨页的异常处理
        // 缺乏对sv39以外的支持

        // 可能被调用free_page都要提升引用计数

        pte_t * l1_pte = PTE2VA(*pte);
        // 相当于mem_map[MAP_NR(PTE2VA(*pte))]++;
        mem_map[MAP_NR(l1_pte)]++;
        for (int i = 0; i < NPTE; i++, l1_pte++) {
            if (*l1_pte & PTE_V) {
                pte_t * l0_pte = PTE2VA(*l1_pte);
                // 相当于mem_map[MAP_NR(PTE2VA(*l1_pte))]++;
                mem_map[MAP_NR(l0_pte)]++;
                for (int j = 0; j < NPTE; j++, l0_pte++) {
                    if (*l0_pte & PTE_V) {
                        *l0_pte &= ~0x3FFUL; // 清除flags
                        *l0_pte |= perm;
                        mem_map[MAP_NR(PTE2VA(*l0_pte))]++;
                    }
                }
            }
        }
    }
}

/*
 * copy_page_tables() just copies the whole process memory range:
 * note the special handling of RESERVED (ie kernel) pages, which
 * means that they are always shared by all processes.
 */
// copy current->pgd to tsk->pgd
// 用户态页表增加引用然后改为只读, 内核态直接引用就好了
// 原版这里变量命名奇奇怪怪的
int copy_page_tables(struct task_struct * tsk) {
    int i;
    pte_t * old_pg_dir;
    pte_t * new_pg_dir;

    old_pg_dir = current->pgd;
    new_pg_dir = get_free_page();
    if (!new_pg_dir)
        return -ENOMEM;
    tsk->pgd = new_pg_dir;

    // 直接复制, 相当于引用了所有的子页表
    memcpy(new_pg_dir, old_pg_dir, PAGE_SIZE);

    pte_t * pte = old_pg_dir;
    for (i = 0 ; i < NPTE / 2 ; i++, pte++) {
        reference_count_one_table(pte);
    }
    invalidate();
    return 0;
}


// 原来版本只能适应2级页表的异常处理,
// 且仅支持内核和用户态的线性地址的起点都为0的情况
// 且仅在change_ldt和do_no_page中被调用
// 不如直接调用map_page
// unsigned long put_page(unsigned long page,unsigned long address) {}

// ####################################################################

// 接收页表的虚拟地址
void load_root_page_table(ssize_t satp_mode, pte_t * root_page_table) {
    uint64 satp = satp_mode | V2P(root_page_table) >> PGSHIFT;
    csr_write(satp, satp);
    local_flush_tlb_all();
}

int map_page(pte_t * page_table, size_t va, size_t pa, int perm, int top_level, int leaf_level);

// init swapper_pg_dir with direct mapping
// 没有映射设备地址
void paging_init(void) {
    uint64 ram_start = PA_KERNEL;
    uint64 ram_end = RAM_START + RAM_SIZE;
    int perm = PTE_X | PTE_W | PTE_R | PTE_V;

//    if (0 != mappages(swapper_pg_dir, P2V(ram_start), ram_end - ram_start, ram_start, perm)) {
//        panic("paging_init");
//    }
    // 在这里顺便把恒等映射给做了
    // 顺便一说, swapper_pg_dir本来只能做参考页表的, 在里面做恒等其实只是为了不实现内核进程
    while (ram_start < ram_end) {
        map_page(swapper_pg_dir, P2V(ram_start), ram_start, perm, 2, 0);
        map_page(swapper_pg_dir, ram_start, ram_start, PTE_U | perm, 2, 0);
        ram_start += PAGE_SIZE;
    }


    // 临时措施, 未来把外设的页表移到外设初始化里面去
    // RTC
    ram_start = 0x10000UL;
    ram_end = ram_start + PAGE_SIZE;
    while (ram_start < ram_end) {
        map_page(swapper_pg_dir, D_P2V_WO(ram_start), ram_start, perm, 2, 0);
        ram_start += PAGE_SIZE;
    }

    // PLIC, 有点浪费, 但是就这样了 (后面两页是UART和VIRT_IO)
    ram_start = 0xC000000UL;
    ram_end = 0x10000000UL + PAGE_SIZE * 2;
    while (ram_start < ram_end) {
        map_page(swapper_pg_dir, D_P2V_WO(ram_start), ram_start, perm, 2, 0);
        ram_start += PAGE_SIZE;
    }

    load_root_page_table(SATP_MODE_39, swapper_pg_dir);
}

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

// #############页相关异常处理################

// 返回对应的页表项的指针
// 如果返回NULL说明中间或最终级缺少
pte_t * get_pte(pte_t * page_table, size_t va) {
    // 拿map_page改的, 懒得优化了
    pte_t * current_table = page_table;

    int index = -1;
    for (int level = 2; level > 0; level--) {
        index = PX(level, va);
        if (current_table[index] & PTE_V == 0) {
            return NULL;
        }
        current_table = PTE2VA(current_table[index]);
    }

    return &current_table[PX(0, va)];
}

void do_wp_page(unsigned long address, pte_t * table_entry) {
    int perm = PTE_U | PTE_X | PTE_W | PTE_R | PTE_V;
    unsigned long old_page,new_page;

    old_page = PTE2VA(*table_entry);

    // 仅有一个进程在引用, 直接解除写保护
    if (old_page >= P2V(LOW_MEM) && mem_map[MAP_NR(old_page)]==1) {
        *table_entry |= PTE_W;
        return;
    }

    if (!(new_page=get_free_page())) {
        do_exit(SIGSEGV);
    }
    if (map_page(current->pgd, new_page, address, perm, 2, 0)) {
         // res == -1, 意味着失败
         do_exit(SIGSEGV);
    }

    // do_exit是对当前进程来做的, 发生了就会不会往下面走了, 相当于return

    // 之前已经检查过了, 不用再检查
    mem_map[MAP_NR(old_page)]--;
    *table_entry |= PTE_W;
    memcpy(new_page, address, PAGE_SIZE);
}

void write_verify(unsigned long address) {
    address &= ~0xFFFUL;
    if (address < PAGE_SIZE || address >= TASK_SIZE) {
        // 仅允许对用户空间做缺页操作, 至少暂时是
        printk("Write Verify on pid:%d at %p.\n", current->pid, address);
        do_exit(SIGSEGV);
    }

    pte_t * table_entry = get_pte(current->pgd, address);
    // 如果页不存在, 直接返回, 后面由do_no_page来处理
    if (table_entry != NULL && (*table_entry | PTE_W | PTE_V) == PTE_V) {
        // 存在且不可写
        do_wp_page(address, table_entry);
    }
    return;
}

void do_no_page(unsigned long address, pte_t * table_entry) {
    int perm = PTE_U | PTE_X | PTE_W | PTE_R | PTE_V;
    unsigned long tmp;
    if (tmp=get_free_page()) {
        if (!map_page(current->pgd, tmp, address, perm, 2, 0)) {
            // res != -1, 意味着成功
            return;
        }
    }
    do_exit(SIGSEGV);
}

void do_page_fault(struct pt_regs *regs) {
    ssize_t badaddr = regs->badaddr & ~0xFFFUL;
    if (badaddr < PAGE_SIZE || badaddr >= TASK_SIZE) {
        // 仅允许对用户空间做缺页操作, 至少暂时是
        printk("Page Fault on pid:%d at %p.\n", current->pid, regs->badaddr);
//        if (table_entry != NULL && (*table_entry | PTE_V)) {
//            printk("Page is Existed.\n");
//        } else {
//            printk("Page is Not Existed.\n");
//        }
        do_exit(SIGSEGV);
    }

    pte_t * table_entry = get_pte(current->pgd, badaddr);
    if (table_entry != NULL && (*table_entry | PTE_V)) {
        // 中间过程不缺页, 且当前页面存在, 说明是写时复制
//        if (*table_entry | PTE_U) {
//            do_wp_page(badaddr, table_entry);
//        } else {
//            // 是内核态页表项不许COW
//            // 现在这个实现下面有可能的就是外设了
//            printk("Kernel COW at %p.\n", regs->badaddr);
//            panic("No COW in Kernel!");
//        }
        do_wp_page(badaddr, table_entry);
    } else {
        // 否则则是缺页
//        if (!user_mode(regs)) {
//            // 不实现vma的情况下, 内核态的页已经全部被回收
//            // 现在这个实现下面有可能的就是外设了
//            printk("Kernel No Page Fault at %p.\n", regs->badaddr);
//            panic("No Page in Kernel!");
//        }
        do_no_page(badaddr, table_entry);
    }
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