#ifndef FXOS_KD_H
#define FXOS_KD_H

#include <fxos/types.h>

KRESULT
KdPrintSerial(
    PCSTR Format,
    ...
    );

VOID
KdAssert(
    BOOLEAN Condition,
    PCSTR   Expression,
    PCSTR   Message,
    PCSTR   File,
    UINT    Line,
    PCSTR   Function
    );

#ifdef DEBUG

#define DPRINT(FMT, ...) \
        KdPrintSerial(FMT, ##__VA_ARGS__)

#define ASSERT(COND, MSG) \
        KdAssert(COND, #COND, MSG, __FILE__, __LINE__, __FUNCTION__)

#else

#define DPRINT(FMT, ...)
#define ASSERT(COND, MSG)

#endif /* DEBUG */

#endif /* FXOS_KD_H */
