#ifndef FXOS_BOOTALLOC_H
#define FXOS_BOOTALLOC_H

#include <fxos/types.h>
#include <fxos/mm/mmtypes.h>

KRESULT
MmInitializeBootAllocator(
    PHYSICAL_ADDRESS PhysicalAddress,
    SIZE             MaxLength
    );

PVOID
MmAllocateBootMemory(
    SIZE Length,
    UINT Align
    );

VOID
MmQueryBootAllocatorInformation(
    PPHYSICAL_ADDRESS Address,
    PSIZE             Length
    );

#endif
