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
char bootloader_info[4096];
struct multiboot2_info *multiboot2_info = (struct multiboot2_info*)bootloader_info;
struct multiboot2_mmap *multiboot2_mmap = NULL;

uint64_t timer_tick;

void parse_loader_info()
{
    for (struct multiboot2_tag_header *info = (struct multiboot2_tag_header *)(multiboot2_info + 1);
         info->type != 0;
         info = (struct multiboot2_tag_header *)(_align_up((uintptr_t*)info + info->size, 8)))
    {
        if (info->type == MULTIBOOT2_TAG_MEMORY_MAP)
        {
            multiboot2_mmap = (struct multiboot2_mmap*)info;
            break;
        }
    }
}

static struct task *task1 = NULL;
static struct task *task2 = NULL;

__noreturn void task1_proc(void)
{
    for (int i = 0; i < 10; i++)
    {
        kprintf("task 1\n");
        sleep_task(1000);
    }
    exit_task(0);
}

__noreturn void task2_proc(void)
{
    kprintf("task1 terminated with exit code %d.\n", wait_for_task(task1));
    exit_task(0);
}

__noreturn void kmain(struct multiboot2_info *info)
{
    disable_interrupts();
    init_serial();
    init_interrupts();

    kprintf("FYNAOS is initializing..\n");

    /* Check if the multiboot information is valid */

    if ((uintptr_t)info & 0x7 || !info)
    {
        kprintf("error: invalid bootloader information\n");
        kernel_panic("boot failed.", PANIC_FLAG_SILENCE);
    }

    /* Copy the information to the safe buffer */

    if (info->totalsize >= sizeof(bootloader_info))
    {
        kprintf("error: bootloader information is too big\n");
        kernel_panic("boot failed.", PANIC_FLAG_SILENCE);
    }

    memcpy(multiboot2_info, info, info->totalsize);

    /* Parse the multiboot2 information */

    parse_loader_info();

    /* Initialize GDT */

    init_gdt();

    /* init mm */

    init_memory(multiboot2_mmap->entries,
                (unsigned int)(multiboot2_mmap->header.size - sizeof(struct multiboot2_mmap)) / multiboot2_mmap->entry_size);

    /* Initialize IRQ and timer */

    init_pic();
    init_pit(100);

    mask_pic_irq(0, TRUE);

    /* Initialize scheduler */
    
    init_sched();

    /* Start first kernel task */

    ready_task(task1 = create_kernel_task(task1_proc, 1));
    ready_task(task2 = create_kernel_task(task2_proc, 1));

    enable_interrupts();

    for (;;)
    {
        halt_cpu();
    }

    halt_cpu_forever();
}
