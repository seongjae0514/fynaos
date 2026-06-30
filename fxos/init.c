#include <fxos/types.h>
#include <fxos/ke/init.h>
#include <fxos/mm/bootalloc.h>

NORETURN
VOID
KiSystemStartup(
    PVOID LoaderInformation
    )
{
    UNREFERENCED_PARAMETER(LoaderInformation);

    PSTR String = "Hello, world!";

    for (PUINT16 i = (PUINT16)0xB8000; *String != '\0'; i++, String++)
    {
        *i = (UINT16)(*String | 0x0700);
    }

    for (;;)
    {
        asm volatile (
            "cli\n"
            "hlt\n"
        );
    }
}
