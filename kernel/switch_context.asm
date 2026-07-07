;
; fynaos/kernel/switch_context.asm
;
; Software context switching routines
;

section .text
    global switch_context

;
; struct context *switch_context(struct context *prev, struct context *next)
;
; Saves current context to <prev> and switches current context to <next>
; <prev> and <next> must be container of `struct task`.
; Returns the previous context before this context executed, which is may not <prev>.
;
switch_context:
    ;
    ; rdi:        struct context *prev
    ; rsi:        struct context *next
    ; [rdi]:      uint64_t rsp
    ; [rdi + 8]:  uint64_t rbx
    ; [rdi + 16]: uint64_t rbp
    ; [rdi + 24]: uint64_t r12
    ; [rdi + 32]: uint64_t r13
    ; [rdi + 40]: uint64_t r14
    ; [rdi + 48]: uint64_t r15
    ;

    ; save current context
    mov [rdi],      rsp
    mov [rdi + 8],  rbx
    mov [rdi + 16], rbp
    mov [rdi + 24], r12
    mov [rdi + 32], r13
    mov [rdi + 40], r14
    mov [rdi + 48], r15

    ; change current context
    mov rsp, [rsi]
    mov rbx, [rsi + 8]
    mov rbp, [rsi + 16]
    mov r12, [rsi + 24]
    mov r13, [rsi + 32]
    mov r14, [rsi + 40]
    mov r15, [rsi + 48]

    mov rax, rdi

    ;
    ; Note:
    ;     Reason why I stored RDI to RAX is to deliver the previous context pointer
    ;     to the next context.
    ;     By doing so, at the time of returning of this function,
    ;     threr is previous context pointer in RAX, which indicates return value.
    ;

    ret
