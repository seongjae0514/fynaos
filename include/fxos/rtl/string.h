#ifndef FXOS_RTL_STRING_H
#define FXOS_RTL_STRING_H

#include <fxos/types.h>

VOID
RtlCopyMemory(
    PSTR  Destination,
    PCSTR Source,
    SIZE  Length
    );

VOID
RtlMoveMemory(
    PSTR  Destination,
    PCSTR Source,
    SIZE  Length
    );

VOID
RtlSetMemory(
    PSTR Destination,
    CHAR Value,
    SIZE Length
    );

static
inline
__attribute__((always_inline))
VOID
RtlZeroMemory(
    PVOID Destination,
    SIZE  Size
    )
{
    RtlSetMemory(
        (PSTR)Destination,
        0,
        Size
        );
}

SIZE
RtlGetStringLength(
    PCSTR String
    );

VOID
RtlCopyString(
    PSTR  Destination,
    PCSTR Source
    );

KRESULT
RtlUnsignedIntegerToString64(
    UINT64 Value,
    PSTR   Buffer,
    CHAR   Radix
    );

KRESULT
RtlIntegerToString64(
    INT64 Value,
    PSTR  Buffer,
    CHAR  Radix
    );

#endif