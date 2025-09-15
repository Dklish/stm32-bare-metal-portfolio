MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

ENTRY(Reset_Handler)

SECTIONS
{
    .isr_vector : { *(.isr_vector) } > FLASH
    .text : { *(.text*) } > FLASH
    .rodata : { *(.rodata*) } > FLASH
}
