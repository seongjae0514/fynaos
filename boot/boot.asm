;
; boot/boot.asm
;
; Initializes cpu environment and invoke main kernel source.
; Starts long mode and set initial pages.
;

;----------------------------------------------
; externs and globals
;----------------------------------------------

global _start
extern kmain

;----------------------------------------------
; definitions
;----------------------------------------------

%define MULTIBOOT2_HEADER_MAGIC      0xE85250D6

%define MULTIBOOT2_HEADER_TAG_END    0
%define MULTIBOOT2_ARCHITECTURE_I386 0
%define MULTIBOOT2_BOOTLOADER_MAGIC  0x36D76289

%define INITIAL_STACK_SIZE           2048

%define INITIAL_STACK_SIZE 2048
%define CODE_SELECTOR      0x08
%define DATA_SELECTOR      0x10
%define KERNEL_STACK_SIZE  8192

;----------------------------------------------
; multiboot2 information
;----------------------------------------------

section .init.multiboot

multiboot_header_start:
    dd MULTIBOOT2_HEADER_MAGIC
    dd MULTIBOOT2_ARCHITECTURE_I386
    dd multiboot_header_end - multiboot_header_start
    dd -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header_start))
multiboot_header_end:

align 8

multiboot_endheader_start:
    dd MULTIBOOT2_HEADER_TAG_END
    dw 0
    dd multiboot_endheader_end - multiboot_endheader_start
multiboot_endheader_end:

;----------------------------------------------
; initial globals
;----------------------------------------------

[bits 32]

section .init.data
    global initial_pml4
    global initial_pdpt
    global initial_pd

align 4096

initial_pml4:
    times 512 dq 0

initial_pdpt:
    times 512 dq 0

initial_pd:
    times 512 dq 0

initial_gdt:
    dq 0x0000000000000000 ; null descriptor
    dq 0x00af9a000000ffff ; kernel code segment
    dq 0x00af92000000ffff ; kernel data segment
initial_gdt_end:

initial_gdtr:
    dw initial_gdt_end - initial_gdt - 1
    dd initial_gdt

align 4096

initial_stack:
    times INITIAL_STACK_SIZE db 0

multiboot_info:
    dq 0

;----------------------------------------------
; entry point
;----------------------------------------------

section .init.text

_hang:
    cli
    hlt
    jmp _hang

_start:
    ; Ininitialize the stack pointer
    mov  esp, initial_stack + INITIAL_STACK_SIZE

    ; Check if the bootloader magic is valid
    mov  [multiboot_info], ebx
    cmp  eax, MULTIBOOT2_BOOTLOADER_MAGIC
    jnz  _hang

    ; Initialize initial PML4
    mov  eax, initial_pdpt
    or   eax, 0x03
    mov  [initial_pml4], eax
    mov  [initial_pml4 + 256 * 8], eax

    ; Initialize initial PDPT
    mov  eax, initial_pd
    or   eax, 0x03
    mov  [initial_pdpt], eax

    ; Initialize PD as huge page
    xor  ecx, ecx
    xor  edx, edx
.A: mov  eax, ecx
    shl  eax, 21
    or   eax, 0x83
    mov  [initial_pd + ecx * 8], eax
    mov  [initial_pd + ecx * 8 + 4], edx
    add  ecx, 1
    cmp  ecx, 512
    jl   .A

    ; Register PML4
    mov  eax, initial_pml4
    mov  cr3, eax

    ; Enable PAE
    mov  eax, cr4
    or   eax, 0x20
    mov  cr4, eax
    
    ; Enable LME
    mov  ecx, 0xC0000080
    rdmsr
    or   eax, 0x100
    wrmsr

    ; Turn on paging
    mov eax, cr0
    or  eax, 0x80000000
    mov cr0, eax 

    ; Load initial GDT
    lgdt [initial_gdtr]
    mov  ax, DATA_SELECTOR
    mov  ds, ax
    mov  es, ax
    mov  ss, ax
    mov  fs, ax
    mov  gs, ax

    ; Jump to long mode code
    jmp  CODE_SELECTOR:_start_longmode

;----------------------------------------------
; long mode entry
;----------------------------------------------

[bits 64]

_start_longmode:
    mov  rax, _start_higherhalf
    jmp  rax

;----------------------------------------------
; higher-half entry
;----------------------------------------------

section .bss
    global kernel_stack

kernel_stack:
    resb 8192

section .text

_start_higherhalf:
    lea rsp, [rel kernel_stack  + KERNEL_STACK_SIZE]
    mov rdi, [abs multiboot_info]
    lea rax, [rel kmain]
    jmp kmain
