section .data
    global isr_vector_table

align 8
isr_vector_table:
%assign i 0
%rep 256
    dq isr%+i
    %assign i i+1
%endrep

section .text
    extern dispatch_interrupt
    global exit_interrupt

%assign i 0
%rep 256
    global isr%+i
isr%+i:
    %if i <> 8 && !(i >= 10 && i <= 14) && i <> 17 && i <> 21 && i <> 29 && i <> 30
        push qword 0
    %endif
    
    push qword i
    jmp isr_common
    
%assign i i+1
%endrep

isr_common:
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov  rdi, rsp
    jmp  dispatch_interrupt

exit_interrupt:
    mov rsp, rdi
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp

    add rsp, 16

    iretq
