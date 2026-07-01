#include <fxos/types.h>
#include <fxos/init.h>
#include <fxos/ke/interrupt.h>
#include <fxos/mm/bootalloc.h>
#include <fxos/hal/interrupt.h>
#include <fxos/hal/serial.h>

NORETURN
VOID
KiSystemStartup(
    PVOID LoaderInformation
    )
{
    UNREFERENCED_PARAMETER(LoaderInformation);

    KeInitializeInterrupts();
    
    HalInitializeSerial();
    HalPutCharacterToSerial('!');

    HalHaltSystem();
}
