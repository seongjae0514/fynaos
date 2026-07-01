%macro ISR_NO_ERRORCODE 1
    global KiSpecificInterruptStub%1
KiSpecificInterruptStub%1:
    push 0
    push %1
    jmp  KiCommonInterruptStub
%endmacro

%macro ISR_ERRORCODE 1
    global KiSpecificInterruptStub%1
KiSpecificInterruptStub%1:
    push %1
    jmp  KiCommonInterruptStub
%endmacro

section .text
    global KiExitInterruptService
    global KiCommonInterruptStub
    global KiDefaultInterruptStub
    extern KiDispatchInterrupt

;
; NORETURN
; VOID
; KiExitInterruptService(
;     PKINTERRUPT_FRAME InterruptFrame
; )
;
KiExitInterruptService:
;++
;
; Routine Description:
;
;     Exits interrupt service routine and
;     returns to original execution flow.
;
; Arguments:
;
;     InterruptFrame - An interrupt frame of current interrupt
;
;--
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

KiCommonInterruptStub:
;++
;
; Routine Description:
;
;     Pushes registers to save context and calls
;     c interrupt service routine.
;
; Arguments:
;
;     None.
;
;--
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
    jmp  KiDispatchInterrupt

KiDefaultInterruptStub:
;++
;
; Routine Description:
;
;     Dispatches interrupt which is not dispatched by
;     specific stubs.
;
;     Pushes vector number -1(0xFFFFFFFFFFFFFFFF).
;
; Arguments:
;
;     None.
;
;--
    push 0
    push -1
    jmp  KiCommonInterruptStub

ISR_NO_ERRORCODE 0
ISR_NO_ERRORCODE 1
ISR_NO_ERRORCODE 2
ISR_NO_ERRORCODE 3
ISR_NO_ERRORCODE 4
ISR_NO_ERRORCODE 5
ISR_NO_ERRORCODE 6
ISR_NO_ERRORCODE 7
ISR_ERRORCODE    8
ISR_NO_ERRORCODE 9
ISR_ERRORCODE    10
ISR_ERRORCODE    11
ISR_ERRORCODE    12
ISR_ERRORCODE    13
ISR_ERRORCODE    14
ISR_NO_ERRORCODE 16
ISR_ERRORCODE    17
ISR_NO_ERRORCODE 18
ISR_NO_ERRORCODE 19
ISR_NO_ERRORCODE 20
ISR_ERRORCODE    21
ISR_NO_ERRORCODE 28
ISR_ERRORCODE    29
ISR_ERRORCODE    30
