%include "multiboot2/multiboot2.inc"

[bits 32]

global _start
extern KiSystemStartup

;-------------------------------------
; Definitions
;-------------------------------------

%define INITIAL_STACK_SIZE 2048
%define CODE_SELECTOR      0x08
%define DATA_SELECTOR      0x10
%define KERNEL_STACK_SIZE  8192

;-------------------------------------
; Multiboot2 headers
;-------------------------------------

section .init.multiboot

MultibootHeaderBegin:
    dd MULTIBOOT2_HEADER_MAGIC
    dd MULTIBOOT2_ARCHITECTURE_I386
    dd MultibootHeaderEnd - MultibootHeaderBegin
    dd -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE_I386 + (MultibootHeaderEnd - MultibootHeaderBegin))
MultibootHeaderEnd:

align 8

MultibootEndHeaderBegin:
    dd MULTIBOOT2_HEADER_TAG_END
    dw 0
    dd MultibootEndHeaderEnd - MultibootEndHeaderBegin
MultibootEndHeaderEnd:

;-------------------------------------
; Globals
;-------------------------------------

section .init.data

align 4096

InitialPML4:
    times 512 dq 0

InitialPDPT:
    times 512 dq 0

InitialPD:
    times 512 dq 0

InitialStack:
    times INITIAL_STACK_SIZE db 0

InitialGDT:
    dq 0x0000000000000000 ; null descriptor
    dq 0x00af9a000000ffff ; kernel code segment
    dq 0x00af92000000ffff ; kernel data segment
InitialGDTEnd:

InitialGDTR:
    dw InitialGDTEnd - InitialGDT - 1
    dd InitialGDT

align 8

MultibootInformation:
    dq 0

;-------------------------------------
; i386 entry point
;-------------------------------------

section .init.text

_panic_hang:
    cli
    hlt
    jmp _panic_hang

_start:
    ;
    ; Initialize the stack pointer
    ;

    mov esp, InitialStack + INITIAL_STACK_SIZE

    ;
    ; Store the multiboot informations
    ;

    mov [MultibootInformation], ebx

    ;
    ; Check if the multiboot magic is valid
    ;

    cmp eax, MULTIBOOT2_BOOTLOADER_MAGIC
    jne _panic_hang

    ;
    ; Initialize the initial page tables
    ;

    mov eax,           InitialPDPT
    or  eax,           0x03
    mov [InitialPML4], eax
    mov [InitialPML4 + 256 * 8], eax

    mov eax,           InitialPD
    or  eax,           0x03
    mov [InitialPDPT], eax

    ;
    ; Initialize PD as huge page (2MiB)
    ;

    xor  ecx, ecx
    xor  edx, edx

.A: mov  eax, ecx
    shl  eax, 21
    or   eax, 0x83
    mov  [InitialPD + ecx * 8],     eax
    mov  [InitialPD + ecx * 8 + 4], edx

    add  ecx, 1
    cmp  ecx, 512
    jl   .A

    ;
    ; Register PML4
    ;

    mov eax, InitialPML4
    mov cr3, eax

    ;
    ; Enable PAE
    ;

    mov eax, cr4
    or  eax, 0x20
    mov cr4, eax

    ;
    ; Enable LME
    ;

    mov ecx, 0xC0000080
    rdmsr
    or  eax, 0x100
    wrmsr

    ;
    ; Turn on the paging
    ;

    mov eax, cr0
    or  eax, 0x80000000
    mov cr0, eax 

    ;
    ; Load initial GDT
    ;

    lgdt [InitialGDTR]

    mov  ax, DATA_SELECTOR
    mov  ds, ax
    mov  es, ax
    mov  ss, ax
    mov  fs, ax
    mov  gs, ax

    jmp  CODE_SELECTOR:_start_longmode

;-------------------------------------
; x64 entry point
;-------------------------------------

[bits 64]

_start_longmode:
    mov  rax, _start_higherhalf
    jmp  rax

;-------------------------------------
; Higher half entry
;-------------------------------------

section .bss

KernelStack:
    resb 8192

section .text

_start_higherhalf:
    ;
    ; Initialize the stack pointer
    ;

    lea rsp, [rel KernelStack + KERNEL_STACK_SIZE]

    ;
    ; Jump to the C kernel source
    ;

    mov rdi, [MultibootInformation]
    jmp KiSystemStartup

