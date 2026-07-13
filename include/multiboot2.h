#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <fynaos/types.h>

#define MULTIBOOT2_TAG_END                              0
#define MULTIBOOT2_TAG_BOOT_COMMANDLINE                 1
#define MULTIBOOT2_TAG_BOOT_LOADER_NAME                 2
#define MULTIBOOT2_TAG_MODULES                          3
#define MULTIBOOT2_TAG_BASIC_MEMORY_INFORMATION         4
#define MULTIBOOT2_TAG_BIOS_BOOT_DEVICE                 5
#define MULTIBOOT2_TAG_MEMORY_MAP                       6
#define MULTIBOOT2_TAG_VBE_INFORMATION                  7
#define MULTIBOOT2_TAG_FRMAEBUFFER_INFORMATION          8
#define MULTIBOOT2_TAG_ELF_SYMBOLS                      9
#define MULTIBOOT2_TAG_APM_TABLE                        10
#define MULTIBOOT2_TAG_EFI_SYSTEM_TABLE_32              11
#define MULTIBOOT2_TAG_EFI_SYSTEM_TABLE_64              12
#define MULTIBOOT2_TAG_SMBIOS_TABLES                    13
#define MULTIBOOT2_TAG_ACPI_RSDP_V1                     14
#define MULTIBOOT2_TAG_ACPI_RSDP_V2PLUS                 15
#define MULTIBOOT2_TAG_NETWORKING_INFORMATION           16
#define MULTIBOOT2_TAG_EFI_MEMORY_MAP                   17
#define MULTIBOOT2_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED 18
#define MULTIBOOT2_TAG_EFI_32_IMAGE_HANDLE              19
#define MULTIBOOT2_TAG_EFI_64_IMAGE_HANDLE              20
#define MULTIBOOT2_TAG_IMAGE_LOAD_BASE_ADDRESS          21

struct multiboot2_info
{
    uint32_t totalsize;
    uint32_t reserved;
};

struct multiboot2_tag_header
{
    uint32_t type;
    uint32_t size;
};

struct multiboot2_mmap_entry
{
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t reserved;
};

struct multiboot2_mmap
{
    struct multiboot2_tag_header header;
    uint32_t                     entry_size;
    uint32_t                     entry_version;
    struct multiboot2_mmap_entry entries[];
} __attribute__((packed));

#define MULTIBOOT2_NEXT_TAG(p) \
        (struct multiboot2_tag_header*)_align_up(((uintptr_t)(p) + (uintptr_t)(p->size)), 8)

#endif
