#ifndef _MM_H
#define _MM_H

#include <sys/types.h>

// rv64有效
typedef uint64              pte_t;
#define NPTE                512

// ########## from xv6-riscv
#define PTE_V (1UL << 0) // valid
#define PTE_R (1UL << 1)
#define PTE_W (1UL << 2)
#define PTE_X (1UL << 3)
#define PTE_U (1UL << 4) // 1 -> user can access
#define PTE_G (1UL << 5)
#define PTE_A (1UL << 6) // 是否访问过
#define PTE_D (1UL << 7) // 是否被写过

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK          0x1FF // 9 bits
#define PXSHIFT(level)  (PGSHIFT+(9*(level)))
#define PX(level, va) ((((uint64) (va)) >> PXSHIFT(level)) & PXMASK)

// ##################################

extern pte_t swapper_pg_dir[];

#endif
