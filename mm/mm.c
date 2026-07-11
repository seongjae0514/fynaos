#include <fynaos/mm.h>
#include <fynaos/symbols.h>
#include <multiboot2.h>

boolean_t init_memory(struct multiboot2_mmap_entry *entries, size_t count)
{
    phys_addr_t boot_begin  = INVALID_PHYSICAL_ADDRESS;
    phys_addr_t boot_limit  = INVALID_PHYSICAL_ADDRESS;
    phys_addr_t frame_limit = 0;

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].addr >= 0x100000 &&
            entries[i].addr >= _kernel_phys_start &&
            entries[i].addr + entries[i].len >= _kernel_phys_end &&
            entries[i].type == 1)
        {
            boot_begin = (phys_addr_t)&_kernel_phys_end;
            boot_limit = entries[i].addr + entries[i].len;
            break;
        }
    }

    if (boot_begin == INVALID_PHYSICAL_ADDRESS)
    {
        return FALSE;
    }

    init_boot_alloc(boot_begin, boot_limit);

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].addr >= 0x100000 &&
            entries[i].type == 1 &&
            entries[i].addr + entries[i].len > frame_limit)
        {
            frame_limit = entries[i].addr + entries[i].len;
        }
    }

    init_frames(frame_limit);

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].addr >= 0x100000 &&
            entries[i].type == 1)
        {
            unreserve_frame_region(entries[i].addr >> PAGE_SHIFT,
                                   (entries[i].addr + entries[i].len) >> PAGE_SHIFT);
        }
    }

    init_kernel_mm(entries, count);

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].addr >= 0x100000 &&
            entries[i].type == 1)
        {
            phys_addr_t begin = entries[i].addr;
            phys_addr_t limit = entries[i].addr + entries[i].len;

            if (entries[i].addr <= (phys_addr_t)&_kernel_phys_start &&
                entries[i].addr + entries[i].len >= (phys_addr_t)&_kernel_phys_end)
            {
                phys_addr_t tmp;
                get_boot_memory_info(&tmp, &begin);
            }

            init_buddy_allocator(begin >> PAGE_SHIFT, limit >> PAGE_SHIFT);
        }
    }

    init_pool();

    return TRUE;
}
