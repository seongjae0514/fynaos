#include <fxos/types.h>
#include <fxos/init.h>
#include <fxos/ke/interrupt.h>
#include <fxos/mm/bootalloc.h>
#include <fxos/hal/interrupt.h>
#include <fxos/hal/serial.h>
#include <fxos/rtl/printf.h>

static
VOID
PrintLine(
    PCSTR s
    )
{
    while (*s)
    {
        HalPutCharacterToSerial(*s++);
    }
}

NORETURN
VOID
KiSystemStartup(
    PVOID LoaderInformation
    )
{
    UNREFERENCED_PARAMETER(LoaderInformation);

    KeInitializeInterrupts();
    HalInitializeSerial();

    CHAR Buffer[256];

    RtlStringPrintf(
        Buffer,
        sizeof(Buffer),
        "Loader information is at 0x%016llX.\n",
        LoaderInformation
        );

    PrintLine(Buffer);

    HalHaltSystem();
}
