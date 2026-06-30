#include <fxos/types.h>
#include <fxos/mm/mmtypes.h>
#include <fxos/mm/bootalloc.h>

PHYSICAL_ADDRESS MiBootAllocatorBegin;
PHYSICAL_ADDRESS MiBootAllocatorCurrent;
PHYSICAL_ADDRESS MiBootAllocatorLimit;

KRESULT
MmInitializeBootAllocator(
    PHYSICAL_ADDRESS PhysicalAddress,
    SIZE             MaxLength
    )
/*++

Routine Description:
    
    Initializes the boot allocator.

    This routine is called during the early boot phase before the
    physical and virtual memory managers have been initialized.

Arguments:

    PhysicalAddress - Specifies the physical address at which the
                      boot allocator begins.

    MaxLength       - Specifies the maximum size, in bytes, of the
                      boot allocator region.

--*/
{
    MiBootAllocatorBegin   = PhysicalAddress;
    MiBootAllocatorCurrent = PhysicalAddress;
    MiBootAllocatorLimit   = PhysicalAddress + MaxLength;

    return KRESULT_SUCCESS;
}

PVOID
MmAllocateBootMemory(
    SIZE Length,
    UINT Align
    )
/*++

Routine Description:
    
    This function allocates memory from the boot allocator before the
    memory manager has been fully initialized.

Arguments:

    Length - Specifies the number of bytes to allocate.

    Align  - Specifies the required alignment, in bytes, of the
             returned memory block.

--*/
{
    PHYSICAL_ADDRESS PhysicalAddress;
    VIRTUAL_ADDRESS  VirtualAddress;
    
    MiBootAllocatorCurrent = ALIGN_UP(MiBootAllocatorCurrent, Align);
    PhysicalAddress        = MiBootAllocatorCurrent;
    MiBootAllocatorCurrent += Length;

    if (MiBootAllocatorCurrent > MiBootAllocatorLimit)
    {
        return NULL;
    }

    VirtualAddress = PhysicalAddress + 0xFFFF800000000000;

    return (PVOID)VirtualAddress;
}

VOID
MmQueryBootAllocatorInformation(
    PPHYSICAL_ADDRESS Address,
    PSIZE             Length
    )
{
    *Address = MiBootAllocatorBegin;
    *Length  = MiBootAllocatorCurrent - MiBootAllocatorBegin;
}
