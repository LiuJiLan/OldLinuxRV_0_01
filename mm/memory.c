#include <sys/types.h>
#include <linux/mm.h>
#include <linux/config.h>

#define PMD_SIZE     (PAGE_SIZE << 9)

pte_t trampoline_pg_dir[];
pte_t trampoline_pg_l2pte[];
uptr_t dtb_va = 0;

void setup_vm(uptr_t dtb_pa) {
    dtb_va = dtb_pa + V_P_DIFF;

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
    for (int i = PX(1, va_start); i < NPTE; i++){
        if (pa_start < pa_end) {
            trampoline_pg_l2pte[i] = PA2PTE(pa_start) | perm;
        } else {
            break;
        }
        pa_start += PMD_SIZE;
    }
}

__attribute__((__aligned__(PGSIZE)))
pte_t swapper_pg_dir[NPTE]={0};  // 参考页表 / 进程0的页表

__attribute__((__aligned__(PGSIZE)))
pte_t trampoline_pg_dir[NPTE]={0};  // level 1

__attribute__((__aligned__(PGSIZE)))
pte_t trampoline_pg_l2pte[NPTE]={0};  // level 2