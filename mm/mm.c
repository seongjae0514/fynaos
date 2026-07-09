/*
 * mm/mm.c
 * FYNAOS memory manager
 * 
 * This module manages all of the kernel memory.
 */

#include <fynaos/types.h>
#include <fynaos/kernel.h>
#include <fynaos/mm.h>
#include <fynaos/string.h>
#include <fynaos/symbols.h>
#include <multiboot2.h>
#include <fynaos/kd.h>

static struct pfn        *pfn_database     = NULL;
static size_t             pfn_database_len = 0;
static struct pfn        *free_list_head   = NULL;

struct mm                 kernel_mm        = { 0 };

static struct boot_alloc  boot_allocator   = { 0 };

static void init_boot_allocator(phys_addr_t base, size_t len)
{
    boot_allocator.begin = base;
    boot_allocator.tail  = base;
    boot_allocator.limit = base + len;
}

static virt_addr_t alloc_boot_memory(size_t len, size_t align)
{
    phys_addr_t addr = _align_up(boot_allocator.tail, align);

    if (addr + len > boot_allocator.limit)
    {
        return INVALID_PHYSICAL_ADDRESS;
    }

    boot_allocator.tail = addr + len;

    memset((void*)(addr + KERNEL_ADDRESS_BASE), 0, len);

    return addr + KERNEL_ADDRESS_BASE;
}

static void alloc_pfn_database(phys_addr_t limit)
{
    pfn_database_len = (limit >> 12);
    pfn_database = (struct pfn*)alloc_boot_memory(pfn_database_len * sizeof(struct pfn), 8);

    if ((virt_addr_t)pfn_database == INVALID_VIRTUAL_ADDRESS)
    {
        kernel_panic("Failed to boot: memory not enough", 0);
    }

    for (size_t i = 0; i < pfn_database_len; i++)
    {
        pfn_database[i].flags    = PFN_RESERVED;
        pfn_database[i].next     = NULL;
        pfn_database[i].refcount = 0;
    }
}

static void unreserve_pfn_region(phys_addr_t addr, phys_addr_t limit)
{
    page_index_t begin = addr >> 12;
    page_index_t end   = limit >> 12;

    for (; begin < end; begin++)
    {
        if (begin >= pfn_database_len)
        {
            break;
        }

        pfn_database[begin].flags &= ~PFN_RESERVED;
    }
}

static void init_free_list(void)
{
    struct pfn *head = NULL;
    struct pfn *tail = NULL;

    for (page_index_t i = 0; i < pfn_database_len; i++)
    {
        if (!(pfn_database[i].flags & PFN_RESERVED) && pfn_database[i].refcount == 0)
        {
            if (head == NULL)
            {
                head = &pfn_database[i];
                tail = head;
            }
            else
            {
                tail->next = &pfn_database[i];
                tail = tail->next;
            }
        }
    }

    if (tail != NULL)
    {
        tail->next = NULL;
    }

    free_list_head = head;
}

/*
 * This routine initializes the pfn database.
 * This routine only allocates memory for db,
 * initializes free list, and flag reserved.
 * This routine doesn't mark used regions.
 */
static void init_phys(struct multiboot2_mmap_entry *entries, size_t count)
{
    /*
     * Initialize boot allocator
     */

    phys_addr_t boot_begin;
    phys_addr_t boot_limit;

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].addr >= 0x100000 && entries[i].type == 1)
        {
            boot_begin = entries[i].addr;
            boot_limit = boot_begin + entries[i].len;
            break;
        }
    }

    if (boot_begin < (phys_addr_t)&_kernel_phys_end)
    {
        boot_begin = _align_up((uintptr_t)&_kernel_phys_end, 4096);
    }

    init_boot_allocator(boot_begin, boot_limit - boot_begin);

    /*
     * Get a last usable memory region
     * and allocate pfn
     */

    phys_addr_t phys_limit = 0;

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].type != 1)
        {
            continue;
        }

        phys_addr_t current_limit = entries[i].addr + entries[i].len;

        if (current_limit > phys_limit)
        {
            phys_limit = current_limit;
        }
    }

    alloc_pfn_database(phys_limit);

    /*
     * Mark usable memory regions
     */

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].type == 1)
        {
            unreserve_pfn_region(entries[i].addr, entries[i].addr + entries[i].len);
        }
    }

    /*
     * Initialize free list
     */

    init_free_list();
}

phys_addr_t alloc_page(void)
{
    if (!free_list_head)
    {
        return INVALID_PHYSICAL_ADDRESS;
    }

    struct pfn *pfn = free_list_head;
    free_list_head = free_list_head->next;

    pfn->refcount++;
    pfn->next = NULL;

    return (phys_addr_t)(pfn - pfn_database) << 12;
}

void zero_page(phys_addr_t page)
{
    memset((void*)(page + KERNEL_ADDRESS_BASE), 0, 0x1000);
}

void free_page(phys_addr_t addr)
{
    page_index_t i = addr >> 12;

    if (i >= pfn_database_len)
    {
        kernel_panic("Invalid address passed to 'free_page'", 0);
    }

    struct pfn *pfn = &pfn_database[i];

    if (pfn->refcount == 0)
    {
        kernel_panic("double-free detected.", 0);
    }

    pfn->refcount--;

    if (pfn->refcount == 0)
    {
        pfn->next = free_list_head;
        free_list_head = pfn;
    }
}

void alloc_specific_page(phys_addr_t addr)
{
    struct pfn *pfn = &pfn_database[addr >> 12];

    if (pfn->refcount == 0)
    {
        /* Remove it from free list */

        struct pfn *current = free_list_head;
        struct pfn *prev    = NULL;

        while (current)
        {
            if (current == pfn)
            {
                if (prev)
                {
                    prev->next = current->next;
                }
                else
                {
                    free_list_head = current->next;
                }
                break;
            }

            prev = current;
            current = current->next;
        }

        if (!current)
        {
            kernel_panic("mm: free list corruption detected.", 0);
        }
    }

    /* Increase refcount */

    pfn->refcount++;
}

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
static void init_kernel_mm(struct multiboot2_mmap_entry *entries, size_t count)
{
    kernel_mm.pml4 = virt_to_phys(NULL, alloc_boot_memory(4096, 4096));

    phys_addr_t *ppml4e = phys_to_virt(kernel_mm.pml4 + 8 * PML4_INDEX(KERNEL_ADDRESS_BASE));
    *ppml4e = virt_to_phys(NULL, alloc_boot_memory(4096, 4096)) | PAGE_PRESENT | PAGE_WRITABLE;

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
    asm volatile (
        "mov %0, %%cr3"
        ::"r"(kernel_mm.pml4)
        : "memory"
    );
}

/*
 * After calling of this function,
 * the boot allocator must not be used.
 */
static void alloc_pages_used_by_kernel(void)
{
    phys_addr_t  begin = (phys_addr_t)&_kernel_phys_start;
    phys_addr_t  end   = _align_up(boot_allocator.tail, 0x1000);

    for (phys_addr_t i = begin; i < end; i += 0x1000)
    {
        alloc_specific_page(i);
    }
}

kresult_t init_memory(struct multiboot2_mmap_entry *entries, unsigned int count)
{
    init_phys(entries, count);
    init_kernel_mm(entries, count);
    alloc_pages_used_by_kernel();
    init_pool();
    return 0;
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
        zero_page(ppml4[pml4_index] = alloc_page());
        ppml4[pml4_index] |= PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    phys_addr_t *ppdpt = phys_to_virt(ppml4[pml4_index] & ~0xFFFULL);

    if (!(ppdpt[pdpt_index] & PAGE_PRESENT))
    {
        zero_page(ppdpt[pdpt_index] = alloc_page());
        ppdpt[pdpt_index] |= PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    phys_addr_t *ppd = phys_to_virt(ppdpt[pdpt_index] & ~0xFFFULL);

    if (!(ppd[pd_index] & PAGE_PRESENT))
    {
        zero_page(ppd[pd_index] = alloc_page());
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
