#include <fxos/types.h>

#define IDT_ENTRIES_COUNT 256

struct _IDT_ENTRY {
    UINT16 IsrLow;
    UINT16 KernelCodeSegmentSelector;
    UINT8  IstOffset;
    UINT8  Attributes;
    UINT16 IsrMiddle;
    UINT32 IsrHigh;
    UINT32 Reserved;
} __attribute__((packed));

typedef struct _IDT_ENTRY IDT_ENTRY, *PIDT_ENTRY;

IDT_ENTRY Idt[IDT_ENTRIES_COUNT];

VOID
HalDisableInterrupts(
    VOID
    )
{
    asm volatile ("cli");
}

VOID
HalEnableInterrupts(
    VOID
    )
{
    asm volatile ("sti");
}

VOID
HalHaltSystemUntilInterrupt(
    VOID
    )
{
    asm volatile ("hlt");
}

VOID
NORETURN
HalHaltSystem(
    VOID
    )
{
    HalDisableInterrupts();

    for (;;)
    {
        HalHaltSystemUntilInterrupt();
    }
}

static
inline
VOID
SetSingleIdtEntry(
    PIDT_ENTRY Entry,
    PVOID      Routine,
    UINT8      Attributes
    )
{
    UINTPTR RoutineAddress = (UINTPTR)Routine;

    Entry->Attributes                = Attributes;
    Entry->KernelCodeSegmentSelector = 0x08;
    Entry->Reserved                  = 0;
    Entry->IsrLow                    = (UINT16)(RoutineAddress & 0xFFFF);
    Entry->IsrMiddle                 = (UINT16)((RoutineAddress >> 16) & 0xFFFF);
    Entry->IsrHigh                   = (UINT32)((RoutineAddress >> 32) & 0xFFFFFFFF);
    Entry->IstOffset                 = 0;
}

VOID
HalSetIdtEntry(
    UINT  Index,
    PVOID Routine,
    UINT8 Attributes
    )
{
    SetSingleIdtEntry(
        &Idt[Index],
        Routine,
        Attributes
    );
}

VOID
HalInitializeIdt(
    PVOID DefaultRoutine,
    UINT8 DefaultAttributes
    )
{
    for (UINT i = 0; i < IDT_ENTRIES_COUNT; i++)
    {
        SetSingleIdtEntry(
            &Idt[i],
            DefaultRoutine,
            DefaultAttributes
        );
    }
}
