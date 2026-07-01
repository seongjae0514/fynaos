#include <fxos/types.h>
#include <fxos/init.h>
#include <fxos/mm/bootalloc.h>
#include <fxos/hal/interrupt.h>
#include <fxos/ke/interrupt.h>

NORETURN
VOID
KiSystemStartup(
    PVOID LoaderInformation
    )
{
    UNREFERENCED_PARAMETER(LoaderInformation);

    KeInitializeInterrupts();

    asm volatile ("int3");
    asm volatile ("int3");
    asm volatile ("int3");
    asm volatile ("int3");
    asm volatile ("int3");
    asm volatile ("int3");
    asm volatile ("int3");
    asm volatile ("int3");

    PSTR String = "Hello, world!";

    for (PUINT16 i = (PUINT16)0xB8000; *String != '\0'; i++, String++)
    {
        *i = (UINT16)(*String | 0x0700);
    }

    HalHaltSystem();
}
