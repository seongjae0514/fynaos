#ifndef FXOS_MM_MMTYPES_H
#define FXOS_MM_MMTYPES_H

#include <fxos/types.h>

typedef UINTPTR PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;
typedef UINTPTR VIRTUAL_ADDRESS,  *PVIRTUAL_ADDRESS;

#define INVALID_PHYSICAL_ADDRESS ((PHYSICAL_ADDRESS)-1)

typedef UINTPTR PAGE_INDEX, *PPAGE_INDEX;

typedef struct _MM_ADDRESS_REGION {
    UINTPTR BaseAddress;
    SIZE    Length;
} MM_ADDRESS_REGION, *PMM_ADDRESS_REGION;

#define PAGE_ALIGN_UP(A) \
        ALIGN_UP(A, 0x1000)

#define PAGE_ALIGN_DOWN(A) \
        ALIGN_DOWN(A, 0x1000)

#define BYTE_COUNT_TO_PAGE_COUNT(C) \
        (PAGE_ALIGN_UP(C) >> 12)

#define PAGE_COUNT_TO_BYTE_COUNT(C) \
        ((C) << 12)

#define BYTE_INDEX_TO_PAGE_INDEX(I) \
        ((I) >> 12)

#define ADDRESS_TO_PAGE_OFFSET(A) \
        ((A) & 0xFFF)

#endif
