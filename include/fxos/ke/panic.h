#ifndef FXOS_KE_PANIC_H
#define FXOS_KE_PANIC_H

#include <fxos/types.h>

NORETURN
VOID
KeKernelPanic(
    PCSTR Message
    );

#endif
