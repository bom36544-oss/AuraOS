ENTRY(kernel_main64)
SECTIONS {
    . = 0x10000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss) }
}