ENTRY(kernel_main64)
SECTIONS {
    . = 0x1000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss) }
}
