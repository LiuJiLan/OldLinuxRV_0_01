#include <linux/config.h>

OUTPUT_ARCH( "riscv" )

ENTRY(_start)

PHYSICAL_BASE = LD_PA_KERNEL;
VIRTUAL_BASE = LD_VA_KERNEL;


SECTIONS
{
    . = VIRTUAL_BASE;

    .text :AT(PHYSICAL_BASE) {
        boot/head.o(.text)
        *(.text .text.*)
        . = ALIGN(4096);
        PROVIDE(etext = .);
    }

    .rodata : {
        . = ALIGN(16);
        *(.srodata .srodata.*)
        . = ALIGN(16);
        *(.rodata .rodata.*)
    }

    .data : {
        . = ALIGN(16);
        *(.sdata .sdata.*)
        . = ALIGN(16);
        *(.data .data.*)
    }

    .bss :{
        . = ALIGN(16);
        PROVIDE(bss_start = .);
        *(.sbss .sbss.*)
        . = ALIGN(16);
        *(.bss .bss.*)
        . = ALIGN(16);
        PROVIDE(bss_stop = .);
    }
    . = ALIGN(4096);
    PROVIDE(end = .);
}
