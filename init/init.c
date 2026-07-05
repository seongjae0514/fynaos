#include <fynaos/types.h>
#include <fynaos/kernel.h>
#include <fynaos/cpu.h>
#include <fynaos/mm.h>
#include <multiboot2.h>
#include <fynaos/string.h>
#include <fynaos/kd.h>

const char *multiboot2_tags[] = {
    "END",
    "Boot command line",
    "Boot loader name",
    "Modules",
    "Basic memory information",
    "BIOS Boot device",
    "Memory map",
    "VBE Information",
    "Framebuffer information",
    "ELF Symbols",
    "APM Table",
    "EFI 32-bit system table pointer",
    "EFI 64-bit system table pointer",
    "SMBIOS tables",
    "ACPI RSDP v1",
    "ACPI RSDP v2+",
    "Networking information",
    "EFI memory map",
    "EFI boot services not terminated",
    "EFI 32-bit image handle",
    "EFI 64-bit image handle",
    "Image load base address"
};

char multiboot2_mmap_buffer[4096];
struct multiboot2_mmap *multiboot2_mmap = NULL;

extern phys_addr_t initial_pml4;
extern phys_addr_t initial_pdpt;
extern phys_addr_t initial_pd;

static void parse_loader_info(struct multiboot2_info *info)
{
    ENTERPROC();

    struct multiboot2_tag_header *header = (struct multiboot2_tag_header*)(info + 1);

    while (header->type)
    {
        if (header->type == MULTIBOOT2_TAG_MEMORY_MAP)
        {
            multiboot2_mmap = (struct multiboot2_mmap*)header;
            break;
        }
        header = MULTIBOOT2_NEXT_TAG(header);
    }

    LEAVEPROC();
}

__noreturn void kmain(uintptr_t info)
{
    ENTERPROC();

    (void)info;
    init_serial();
    init_interrupts();

    /* Check if the multiboot information is valid */
    if (!info || info & 07)
    {
        kernel_panic("boot failed: invalid multiboot information", 0);
    }

    /* Parse multiboot information */
    parse_loader_info((struct multiboot2_info*)info);

    memcpy(multiboot2_mmap_buffer, multiboot2_mmap, multiboot2_mmap->header.size);

    /* Initialize mm with mmap information */
    init_memory(
        ((struct multiboot2_mmap*)multiboot2_mmap_buffer)->entries,
        (unsigned int)((multiboot2_mmap->header.size - sizeof(struct multiboot2_tag_header))
        / multiboot2_mmap->entry_size));

    halt_cpu_forever();
    LEAVEPROC();
}
