#ifndef FXOS_HAL_X64_PORT_H
#define FXOS_HAL_X64_PORT_H

#include <fxos/types.h>

VOID
HalWritePort8(
    UINT16 Port,
    UINT8  Value
    );

UINT8
HalReadPort8(
    UINT16 Port
    );

VOID
HalWritePort16(
    UINT16 Port,
    UINT16 Value
    );

UINT16
HalReadPort16(
    UINT16 Port
    );

#endif
