ENTRY(_start)
OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
    . = 0x100000;
    _KernelImagePhysicalStart = .;
    _KernelImageVirtualStart  = . + 0xFFFF800000000000;

    /*
     *  Lower-half address starts from here -- `.` points physical address.
     *  This address is used to initialize page table.
     */

    /* Init section ************************/
    . = ALIGN(0x1000);
    _InitialSectionPhysicalStart = .;
    _InitialSectionVirtualStart  = . + 0xFFFF800000000000;
    .init : {
        *(.init*)
    }
    _InitialSectionVirtualEnd  = . + 0xFFFF800000000000;
    _InitialSectionPhysicalEnd = .;

    /*
     *  Higher-half address starts from here -- `.` points virtual address.
     *  `. - 0xFFFF800000000000` points physical address.
     */

    . += 0xFFFF800000000000;

    /* Text section ************************/
    . = ALIGN(4096);
    _TextSectionPhysicalStart = . - 0xFFFF800000000000;
    _TextSectionVirtualStart = .;
    .text : AT(ADDR(.text) - 0xFFFF800000000000) {
        *(.text)
    }
    _TextSectionVirtualEnd = .;
    _TextSectionPhysicalEnd = . - 0xFFFF800000000000;

    /* Read-only data section **************/
    . = ALIGN(4096);
    _ReadOnlyDataSectionPhysicalStart = . - 0xFFFF800000000000;
    _ReadOnlyDataSectionVirtualStart  = .;
    .rodata : AT(ADDR(.rodata) - 0xFFFF800000000000) {
        *(.rodata*)
    }
    _ReadOnlyDataSectionVirtualEnd  = .;
    _ReadOnlyDataSectionPhysicalEnd = . - 0xFFFF800000000000;

    /* Data section ************************/
    . = ALIGN(4096);
    _DataSectionPhysicalStart = . - 0xFFFF800000000000;
    _DataSectionVirtualStart  = .;
    .data : AT(ADDR(.data) - 0xFFFF800000000000) {
        *(.data*)
    }
    _DataSectionVirtualEnd  = .;
    _DataSectionPhysicalEnd = . - 0xFFFF800000000000;

    /* BSS section *************************/
    . = ALIGN(4096);
    _BssSectionPhysicalStart = . - 0xFFFF800000000000;
    _BssSectionVirtualStart  = .;
    .bss : AT(ADDR(.bss) - 0xFFFF800000000000) {
        *(.bss*)
    }
    _BssSectionVirtualEnd  = .;
    _BssSectionPhysicalEnd = . - 0xFFFF800000000000;

    _KernelImageVirtualEnd = .;
    _KernelImagePhysicalEnd = . - 0xFFFF800000000000;
}
