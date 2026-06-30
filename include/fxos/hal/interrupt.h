#ifndef FXOS_HAL_INTERRUPT_H
#define FXOS_HAL_INTERRUPT_H

#include <fxos/types.h>

#define IDT_ATTRIBUTE_PRESENT                0x80

#define IDT_ATTRIBUTE_PRIVILEGE_KERNELONLY   0x00
#define IDT_ATTRIBUTE_PRIVILEGE_USERCALLABLE 0x60

#define IDT_ATTRIBUTE_TYPE_INTERRUPT_GATE    0x0E
#define IDT_ATTRIBUTE_TYPE_TRAP_GATE         0x0F
#define IDT_ATTRIBUTE_TYPE_TASK_GATE         0x05

VOID
HalDisableInterrupts(
    VOID
    );

VOID
HalEnableInterrupts(
    VOID
    );

VOID
HalHaltSystemUntilInterrupt(
    VOID
    );

VOID
NORETURN
HalHaltSystem(
    VOID
    );

VOID
HalSetIdtEntry(
    UINT  Index,
    PVOID Routine,
    UINT8 Attributes
    );

VOID
HalInitializeIdt(
    PVOID DefaultRoutine,
    UINT8 DefaultAttributes
    );

#endif
