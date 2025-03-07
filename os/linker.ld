ENTRY(_entry)

SECTIONS {
    . = 0x80200000; /* 内核起始地址 */

    .text : {
        _text_begin = .;
        *(.text.entry)
        *(.text .text.*)
        . = ALIGN(4096);
        _text_end = .;
    }

    .rodata : {
        _rodata_begin = .;
        *(.rodata .rodata.*)
        . = ALIGN(4096);
        _rodata_end = .;
    }

    .data : {
        _data_begin = .;
        *(.data .data.*)
        . = ALIGN(4096);
        _data_end = .;
    }

    .bss : {
        _bss_begin = .;
        *(.bss .bss.*)
        . = ALIGN(4096);
        _bss_end = .;
    }

    /* 定义内核结束符号 */
    . = ALIGN(4096); /* 对齐到一页 */
    _kernel_end = .;
}