OUTPUT_ARCH( "riscv" )

ENTRY(_start)

RAM_START = 0x80000000;
PROVIDE(LD_RAM_START = RAM_START);

V_P_DIFF = 0xFFFFFFC000000000;
PHYSICAL_BASE = RAM_START + 0x00200000;
VIRTUAL_BASE = PHYSICAL_BASE + V_P_DIFF;


SECTIONS
{
    . = VIRTUAL_BASE;
    .text :AT(PHYSICAL_BASE) {
        boot/head.o(.text)
        *(.text)
    }
    
    . = ALIGN(4096);
    PROVIDE(etext = .);

    .rodata : {
     *(.rodata)
    }
    

    .data : {
        boot/head.o(.data)
        *(.data)
    }

    .bss :{
            *(.bss)
    }
    
    PROVIDE(end = .);
}
