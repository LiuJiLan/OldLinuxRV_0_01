#ifndef _ASM_H
#define _ASM_H

#include <asm/asm_offsets.h>

#define PAGE_SIZE       4096
#define PAGE_SHIFT      12

#define REGBYTES        8

#define REG_S           sd
#define REG_L           ld
#define REG_SC		    sc.d
#define REG_AMOSWAP_AQ	amoswap.d.aq
#define REG_ASM		    .dword

#define RISCV_PTR		.dword
#define RISCV_SZPTR		8
#define RISCV_LGPTR		3

#endif