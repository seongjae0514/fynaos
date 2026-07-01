#include <fxos/types.h>
#include <fxos/kd/kd.h>
#include <fxos/hal/interrupt.h>

NORETURN
VOID
KeKernelPanic(
    PCSTR Message
    )
{
    KdPrintSerial(
        "Kernel panic: %s\n"
        "System halted.\n",
        Message
        );

    HalHaltSystem();
}
