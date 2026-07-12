#include <fynaos/types.h>
#include <fynaos/kernel.h>
#include <fynaos/cpu.h>
#include <fynaos/mm.h>
#include <multiboot2.h>
#include <fynaos/string.h>
#include <fynaos/kd.h>

char bootloader_info[4096];
struct multiboot2_info *multiboot2_info = (struct multiboot2_info*)bootloader_info;
struct multiboot2_mmap *multiboot2_mmap = NULL;

uint64_t timer_tick;

static void parse_loader_info(void)
{
    for (struct multiboot2_tag_header *info = (struct multiboot2_tag_header *)(multiboot2_info + 1);
         info->type != 0;
         info = (struct multiboot2_tag_header *)(_align_up((uintptr_t)info + info->size, 8)))
    {
        if (info->type == MULTIBOOT2_TAG_MEMORY_MAP)
        {
            multiboot2_mmap = (struct multiboot2_mmap*)info;
            break;
        }
    }
}

static __noreturn void child_routine()
{
    for (int i = 1; i <= 10; i++)
    {
        kprintf("%d\n", i);
        sleep_task(100);
    }
    exit_task(0);
}

static __noreturn void task_routine()
{
    struct task *task = create_kernel_task(child_routine, 0);
    int exit_code;
    ready_task(task);
    exit_code = wait_for_task(task);
    kprintf("The task was terminated with exit code %d.\n", exit_code);
    delete_task(task);
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

    struct task *task = create_kernel_task(task_routine, 0);
    ready_task(task);

    enable_interrupts();

    for (;;)
    {
        halt_cpu();
    }

    halt_cpu_forever();
}
