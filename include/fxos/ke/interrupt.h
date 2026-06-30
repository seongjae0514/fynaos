#ifndef FXOS_KE_INTERRUPT_H
#define FXOS_KE_INTERRUPT_H

#include <fxos/types.h>

typedef struct _KINTERRUPT_FRAME {
    /* Common stub pushes */
    UINT64 R15;
    UINT64 R14;
    UINT64 R13;
    UINT64 R12;
    UINT64 R11;
    UINT64 R10;
    UINT64 R9;
    UINT64 R8;
    UINT64 RAX;
    UINT64 RBX;
    UINT64 RCX;
    UINT64 RDX;
    UINT64 RSI;
    UINT64 RDI;
    UINT64 RBP;

    /* Specific stub pushes */
    UINT64 Vector;
    UINT64 ErrorCode;

    /* CPU Pushes */
    UINT64 RIP;
    UINT64 CS;
    UINT64 RFLAGS;
    UINT64 RSP;
    UINT64 SS;
} KINTERRUPT_FRAME;



#endif
