#include <fxos/types.h>
#include <fxos/ke/interrupt.h>
#include <fxos/hal/interrupt.h>

#define EXTERN_SPECIFIC_INTERRUPT_STUB(N)              \
        NORETURN VOID KiSpecificInterruptStub##N(VOID)

#define SET_IDT_ENTRY(N)                               \
        HalSetIdtEntry(                                \
            N,                                         \
            KiSpecificInterruptStub##N,                \
            IDT_ATTRIBUTE_PRESENT              |       \
            IDT_ATTRIBUTE_PRIVILEGE_KERNELONLY |       \
            IDT_ATTRIBUTE_TYPE_INTERRUPT_GATE          \
            )

EXTERN_SPECIFIC_INTERRUPT_STUB(0);  // Divide error
EXTERN_SPECIFIC_INTERRUPT_STUB(1);  // Debug exception
EXTERN_SPECIFIC_INTERRUPT_STUB(2);  // NMI interrupt
EXTERN_SPECIFIC_INTERRUPT_STUB(3);  // Breakpoint
EXTERN_SPECIFIC_INTERRUPT_STUB(4);  // Overflow
EXTERN_SPECIFIC_INTERRUPT_STUB(5);  // BOUND range exeeded
EXTERN_SPECIFIC_INTERRUPT_STUB(6);  // Invalid opcode
EXTERN_SPECIFIC_INTERRUPT_STUB(7);  // Device not available
EXTERN_SPECIFIC_INTERRUPT_STUB(8);  // Double fault
EXTERN_SPECIFIC_INTERRUPT_STUB(9);  // Coprocessor segment overrun
EXTERN_SPECIFIC_INTERRUPT_STUB(10); // Invalid TSS
EXTERN_SPECIFIC_INTERRUPT_STUB(11); // Segment not present
EXTERN_SPECIFIC_INTERRUPT_STUB(12); // Stack-Segment fault
EXTERN_SPECIFIC_INTERRUPT_STUB(13); // General protection
EXTERN_SPECIFIC_INTERRUPT_STUB(14); // Page fault
EXTERN_SPECIFIC_INTERRUPT_STUB(16); // x87 error
EXTERN_SPECIFIC_INTERRUPT_STUB(17); // Alignment check
EXTERN_SPECIFIC_INTERRUPT_STUB(18); // Machine check
EXTERN_SPECIFIC_INTERRUPT_STUB(19); // SIMD Floating-point exception
EXTERN_SPECIFIC_INTERRUPT_STUB(20); // Virtualization exception
EXTERN_SPECIFIC_INTERRUPT_STUB(21); // Control protection exception
EXTERN_SPECIFIC_INTERRUPT_STUB(28); // Hypervisor injection
EXTERN_SPECIFIC_INTERRUPT_STUB(29); // VMM communication
EXTERN_SPECIFIC_INTERRUPT_STUB(30); // Security

NORETURN
VOID
KiDefaultInterruptStub(
    VOID
    );

KRESULT
KeInitializeInterrupts(
    VOID
    )
{
    HalInitializeIdt(
        KiDefaultInterruptStub,
        IDT_ATTRIBUTE_PRESENT              |
        IDT_ATTRIBUTE_PRIVILEGE_KERNELONLY |
        IDT_ATTRIBUTE_TYPE_INTERRUPT_GATE
        );

    SET_IDT_ENTRY(0);
    SET_IDT_ENTRY(1);
    SET_IDT_ENTRY(2);
    SET_IDT_ENTRY(3);
    SET_IDT_ENTRY(4);
    SET_IDT_ENTRY(5);
    SET_IDT_ENTRY(6);
    SET_IDT_ENTRY(7);
    SET_IDT_ENTRY(8);
    SET_IDT_ENTRY(9);
    SET_IDT_ENTRY(10);
    SET_IDT_ENTRY(11);
    SET_IDT_ENTRY(12);
    SET_IDT_ENTRY(13);
    SET_IDT_ENTRY(14);
    SET_IDT_ENTRY(16);
    SET_IDT_ENTRY(17);
    SET_IDT_ENTRY(18);
    SET_IDT_ENTRY(19);
    SET_IDT_ENTRY(21);
    SET_IDT_ENTRY(20);
    SET_IDT_ENTRY(28);
    SET_IDT_ENTRY(29);
    SET_IDT_ENTRY(30);

    return KRESULT_SUCCESS;
}
