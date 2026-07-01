#ifndef FXOS_RTL_PRINTF_H
#define FXOS_RTL_PRINTF_H

#include <fxos/types.h>
#include <stdarg.h>

PSTR
RtlVStringPrintf(
    PSTR    Buffer,
    SIZE    Length,
    PCSTR   Format,
    va_list Args
    );

PSTR
RtlStringPrintf(
    PSTR  Buffer,
    SIZE  Length,
    PCSTR Format,
    ...
    );

#endif
