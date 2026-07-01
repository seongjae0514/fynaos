#ifndef FXOS_HAL_SERIAL_H
#define FXOS_HAL_SERIAL_H

#include <fxos/types.h>

KRESULT
HalInitializeSerial(
    VOID
    );

KRESULT
HalPutCharacterToSerial(
    CHAR Character
    );

#endif
