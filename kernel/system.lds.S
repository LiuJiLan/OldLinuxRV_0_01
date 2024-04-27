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
        *(.text)
        . = ALIGN(4096);
        PROVIDE(etext = .);
    }

    .rodata : {
        . = ALIGN(16);
         *(.rodata)
    }

    .data : {
        . = ALIGN(16);
        boot/head.o(.data)
        *(.data)
    }

    .bss :{
        . = ALIGN(4096);
        PROVIDE(bss_start = .);
        *(.bss)
        . = ALIGN(4096);
        PROVIDE(bss_stop = .);
    }

    PROVIDE(end = .);
}
