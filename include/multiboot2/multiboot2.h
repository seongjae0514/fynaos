#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <fxos/types.h>

typedef struct _MULTIBOOT2_INFORMATION {
    UINT32 TotalSize;
    UINT32 Reserved;
} MULTIBOOT2_INFORMATION, *PMULTIBOOT2_INFORMATION;

typedef struct _MULTIBOOT2_TAG_HEADER {
    UINT32 Type;
    UINT32 Size;
} MULTIBOOT2_TAG_HEADER, *PMULTIBOOT2_TAG_HEADER;

typedef struct _MULTIBOOT2_MMAP_ENTRY {
    UINT64                Address;
    UINT64                Length;
    UINT32                Type;
    UINT32                Reserved;
} MULTIBOOT2_MMAP_ENTRY, *PMULTIBOOT2_MMAP_ENTRY;

typedef struct _MULTIBOOT2_TAG_MMAP {
    MULTIBOOT2_TAG_HEADER Header;
    UINT32                EntrySize;
    UINT32                EntryVersion;
    MULTIBOOT2_MMAP_ENTRY Entries[];
} MULTIBOOT2_TAG_MMAP, *PMULTIBOOT2_TAG_MMAP;

#define MULTIBOOT2_TAG_TYPE_END                              0
#define MULTIBOOT2_TAG_TYPE_BOOT_COMMANDLINE                 1
#define MULTIBOOT2_TAG_TYPE_BOOT_LOADER_NAME                 2
#define MULTIBOOT2_TAG_TYPE_MODULES                          3
#define MULTIBOOT2_TAG_TYPE_BASIC_MEMORY_INFORMATION         4
#define MULTIBOOT2_TAG_TYPE_BIOS_BOOT_DEVICE                 5
#define MULTIBOOT2_TAG_TYPE_MEMORY_MAP                       6
#define MULTIBOOT2_TAG_TYPE_VBE_INFORMATION                  7
#define MULTIBOOT2_TAG_TYPE_FRMAEBUFFER_INFORMATION          8
#define MULTIBOOT2_TAG_TYPE_ELF_SYMBOLS                      9
#define MULTIBOOT2_TAG_TYPE_APM_TABLE                        10
#define MULTIBOOT2_TAG_TYPE_EFI_SYSTEM_TABLE_32              11
#define MULTIBOOT2_TAG_TYPE_EFI_SYSTEM_TABLE_64              12
#define MULTIBOOT2_TAG_TYPE_SMBIOS_TABLES                    13
#define MULTIBOOT2_TAG_TYPE_ACPI_RSDP_V1                     14
#define MULTIBOOT2_TAG_TYPE_ACPI_RSDP_V2PLUS                 15
#define MULTIBOOT2_TAG_TYPE_NETWORKING_INFORMATION           16
#define MULTIBOOT2_TAG_TYPE_EFI_MEMORY_MAP                   17
#define MULTIBOOT2_TAG_TYPE_EFI_BOOT_SERVICES_NOT_TERMINATED 18
#define MULTIBOOT2_TAG_TYPE_EFI_32_IMAGE_HANDLE              19
#define MULTIBOOT2_TAG_TYPE_EFI_64_IMAGE_HANDLE              20
#define MULTIBOOT2_TAG_TYPE_IMAGE_LOAD_BASE_ADDRESS          21

#endif
