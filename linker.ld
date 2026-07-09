ENTRY(_start)
OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
    . = 0x100000;
    _kernel_phys_start = .;
    _kernel_virt_start  = . + 0xFFFF800000000000;

    /*
     *  Lower-half address starts from here -- `.` points physical address.
     *  This address is used to initialize page table.
     */

    /* Init section ************************/
    . = ALIGN(0x1000);
    _init_phys_start = .;
    _init_virt_start  = . + 0xFFFF800000000000;
    .init : {
        *(.init*)
    }
    _init_virt_end  = . + 0xFFFF800000000000;
    _init_phys_end = .;

    /*
     *  Higher-half address starts from here -- `.` points virtual address.
     *  `. - 0xFFFF800000000000` points physical address.
     */

    . += 0xFFFF800000000000;

    /* Text section ************************/
    . = ALIGN(4096);
    _text_phys_start = . - 0xFFFF800000000000;
    _text_virt_start = .;
    .text : AT(ADDR(.text) - 0xFFFF800000000000) {
        *(.text)
    }
    _text_virt_end = .;
    _text_phys_end = . - 0xFFFF800000000000;

    /* Read-only data section **************/
    . = ALIGN(4096);
    _rodata_phys_start = . - 0xFFFF800000000000;
    _rodata_virt_start  = .;
    .rodata : AT(ADDR(.rodata) - 0xFFFF800000000000) {
        *(.rodata*)
    }
    _rodata_virt_end  = .;
    _rodata_phys_end = . - 0xFFFF800000000000;

    /* Data section ************************/
    . = ALIGN(4096);
    _data_phys_start = . - 0xFFFF800000000000;
    _data_virt_start  = .;
    .data : AT(ADDR(.data) - 0xFFFF800000000000) {
        *(.data*)
    }
    _data_virt_end  = .;
    _data_phys_end = . - 0xFFFF800000000000;

    /* BSS section *************************/
    . = ALIGN(4096);
    _bss_phys_start = . - 0xFFFF800000000000;
    _bss_virt_start  = .;
    .bss : AT(ADDR(.bss) - 0xFFFF800000000000) {
        *(.bss*)
    }
    _bss_virt_end  = .;
    _bss_phys_end = . - 0xFFFF800000000000;

    _kernel_virt_end = .;
    _kernel_phys_end = . - 0xFFFF800000000000;
}
