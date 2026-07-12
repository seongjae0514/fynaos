/*
 * fynaos/mm/vmm.c
 * Virtual Memory Manager
 */

#include <fynaos/mm.h>
#include <fynaos/string.h>
#include <fynaos/cpu.h>
#include <multiboot2.h>

struct mm  kernel_mm;
struct mm *current_mm = NULL;

void *phys_to_virt(phys_addr_t phys)
{
    return (void*)(phys + KERNEL_ADDRESS_BASE);
}

/*
 * This function converts virtual address to physical address.
 * if 'mm' is NULL, this function assume that the address space
 * is kernel's.
 * 
 * This function returns converted address.
 * If the virtual address is not presented for the address space,
 * returns INVALID_PHYSICAL_ADDRESS.
 */
phys_addr_t virt_to_phys(struct mm *mm, virt_addr_t virt)
{
    if (!mm)
    {
        if (virt < KERNEL_ADDRESS_BASE)
        {
            return INVALID_PHYSICAL_ADDRESS;
        }

        return virt - KERNEL_ADDRESS_BASE;
    }

    page_index_t pml4_index = PML4_INDEX(virt);
    page_index_t pdpt_index = PDPT_INDEX(virt);
    page_index_t pd_index   = PD_INDEX(virt);
    page_index_t pt_index   = PT_INDEX(virt);
    uintptr_t    offset     = PAGE_OFFSET(virt);

    uintptr_t *pml4  = phys_to_virt(mm->pml4);
    uintptr_t  pml4e = pml4[pml4_index];

    if (!(pml4e & PAGE_PRESENT))
    {
        return INVALID_PHYSICAL_ADDRESS;
    }

    uintptr_t *pdpt  = phys_to_virt(pml4e & ~0xFFFULL);
    uintptr_t  pdpte = pdpt[pdpt_index];
    
    if (!(pdpte & PAGE_PRESENT))
    {
        return INVALID_PHYSICAL_ADDRESS;
    }

    uintptr_t *pd  = phys_to_virt(pdpte & ~0xFFFULL);
    uintptr_t  pde = pd[pd_index];
    
    if (!(pde & PAGE_PRESENT))
    {
        return INVALID_PHYSICAL_ADDRESS;
    }

    uintptr_t *pt  = phys_to_virt(pde & ~0xFFFULL);
    uintptr_t  pte = pt[pt_index];

    if (!(pte & PAGE_PRESENT))
    {
        return INVALID_PHYSICAL_ADDRESS;
    }

    phys_addr_t phys = (pte & ~0xFFFULL) + offset;
    
    return phys;
}

/*
 * Initializes kernel address space(0xFFFF800000000000~).
 * this function makes higher-half direct mapping(virt=phys+0xFFFF800000000000).
 * 
 * Basically, this function had to use 4 level paging,
 * but this function uses 1GB huge page.
 * So, all of the kernel address space is readable, writable, and executable.
 * I know that this is very dangerous, but
 * I don't want to fix it because my head is full. :-(
 */
void init_kernel_mm(struct multiboot2_mmap_entry *entries, size_t count)
{
    kernel_mm.pml4 = virt_to_phys(NULL, (virt_addr_t)alloc_boot_memory(4096, 4096));

    phys_addr_t *ppml4e = phys_to_virt(kernel_mm.pml4 + 8 * PML4_INDEX(KERNEL_ADDRESS_BASE));
    *ppml4e = virt_to_phys(NULL, (virt_addr_t)alloc_boot_memory(4096, 4096)) | PAGE_PRESENT | PAGE_WRITABLE;

    phys_addr_t  phys_pdpt = *ppml4e;
    phys_addr_t *ppdpt     = phys_to_virt(phys_pdpt & ~0xFFFULL);

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].type == 1 && entries[i].addr >= 0x100000)
        {
            page_index_t pdpt_begin = PDPT_INDEX(entries[i].addr);
            page_index_t pdpt_end   = PDPT_INDEX(_align_up(entries[i].addr + entries[i].len, 0x40000000));

            for (page_index_t pdpt_index = pdpt_begin; pdpt_index < pdpt_end; pdpt_index++)
            {
                uintptr_t phys_addr = (uintptr_t)pdpt_index * 0x40000000;
                ppdpt[pdpt_index] = phys_addr | PAGE_HUGE | PAGE_PRESENT | PAGE_WRITABLE;
            }
        }
    }

    /* Register kernel pml4 */
    write_cr3(kernel_mm.pml4);
}

boolean_t map_page(struct mm *mm, phys_addr_t frame, virt_addr_t page, uint32_t attr)
{
    page_index_t pml4_index = PML4_INDEX(page);
    page_index_t pdpt_index = PDPT_INDEX(page);
    page_index_t pd_index   = PD_INDEX(page);
    page_index_t pt_index   = PT_INDEX(page);

    phys_addr_t *ppml4 = phys_to_virt(mm->pml4);

    if (!(ppml4[pml4_index] & PAGE_PRESENT))
    {
        zero_page(ppml4[pml4_index] = alloc_frames(0));
        ppml4[pml4_index] |= PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    phys_addr_t *ppdpt = phys_to_virt(ppml4[pml4_index] & ~0xFFFULL);

    if (!(ppdpt[pdpt_index] & PAGE_PRESENT))
    {
        zero_page(ppdpt[pdpt_index] = alloc_frames(0));
        ppdpt[pdpt_index] |= PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    phys_addr_t *ppd = phys_to_virt(ppdpt[pdpt_index] & ~0xFFFULL);

    if (!(ppd[pd_index] & PAGE_PRESENT))
    {
        zero_page(ppd[pd_index] = alloc_frames(0));
        ppd[pd_index] |= PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    phys_addr_t *ppt = phys_to_virt(ppd[pd_index] & ~0xFFFULL);

    if (ppt[pt_index] & PAGE_PRESENT)
    {
        return FALSE;
    }

    ppt[pt_index] = frame | attr;

    return TRUE;
}

phys_addr_t unmap_page(struct mm *mm, virt_addr_t page)
{
    page_index_t pml4_index = PML4_INDEX(page);
    page_index_t pdpt_index = PDPT_INDEX(page);
    page_index_t pd_index   = PD_INDEX(page);
    page_index_t pt_index   = PT_INDEX(page);

    phys_addr_t *ppml4 = phys_to_virt(mm->pml4);

    if (!(ppml4[pml4_index] & PAGE_PRESENT))
    {
        return INVALID_PHYSICAL_ADDRESS;
    }

    phys_addr_t *ppdpt = phys_to_virt(ppml4[pml4_index] & ~0xFFFULL);

    if (!(ppdpt[pdpt_index] & PAGE_PRESENT))
    {
        return INVALID_PHYSICAL_ADDRESS;
    }

    phys_addr_t *ppd = phys_to_virt(ppdpt[pdpt_index] & ~0xFFFULL);

    if (!(ppd[pd_index] & PAGE_PRESENT))
    {
        return INVALID_PHYSICAL_ADDRESS;
    }

    phys_addr_t *ppt = phys_to_virt(ppd[pd_index] & ~0xFFFULL);

    if (!(ppt[pt_index] & PAGE_PRESENT))
    {
        return INVALID_PHYSICAL_ADDRESS;
    }

    phys_addr_t phys = ppt[pt_index] & ~0xFFFULL;

    ppt[pt_index] = 0ULL;

    return phys;
}

void map_kernel_for_user_mm(struct mm *mm)
{
    phys_addr_t *user_pml4   = phys_to_virt(mm->pml4);
    phys_addr_t *kernel_pml4 = phys_to_virt(kernel_mm.pml4);

    user_pml4[PML4_INDEX(KERNEL_ADDRESS_BASE)] =
        kernel_pml4[PML4_INDEX(KERNEL_ADDRESS_BASE)];
}

struct mm *create_mm(void)
{
    struct mm   *mm;
    phys_addr_t  pml4_page;

    if ((pml4_page = alloc_frames(0)) == INVALID_PHYSICAL_ADDRESS)
    {
        return NULL;
    }

    if (!(mm = kmalloc(sizeof(struct mm))))
    {
        free_frame(pml4_page);
        return NULL;
    }

    zero_page(pml4_page);

    mm->pml4 = pml4_page;

    return mm;
}

void swap_mm(struct mm *mm)
{
    if (!current_mm || current_mm == mm) return;
    current_mm = mm;
    write_cr3(mm->pml4);
}

static void free_page_table(phys_addr_t table, int level)
{
    phys_addr_t *pt = phys_to_virt(table);
    for (int i = 0; i < 512; i++)
    {
        if (!(pt[i] & PAGE_PRESENT)) continue;
        phys_addr_t child = pt[i] & ~PAGE_MASK;
        if (level > 0) free_page_table(child, level - 1);
        else free_frame(child);
    }
    free_frame(table);
}

void delete_mm(struct mm *mm)
{
    free_page_table(mm->pml4, 3);
    kfree(mm);
}
