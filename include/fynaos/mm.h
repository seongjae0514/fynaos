#ifndef FYNAOS_MM_H
#define FYNAOS_MM_H

#include <fynaos/types.h>

#define KERNEL_ADDRESS_BASE 0xFFFF800000000000
#define PAGE_SIZE           4096

#define PFN_RESERVED        0x00000001U

#define PML4_INDEX(addr)    (((uint64_t)(addr) >> 39) & 0x1FF)
#define PDPT_INDEX(addr)    (((uint64_t)(addr) >> 30) & 0x1FF)
#define PD_INDEX(addr  )    (((uint64_t)(addr) >> 21) & 0x1FF)
#define PT_INDEX(addr)      (((uint64_t)(addr) >> 12) & 0x1FF)
#define PAGE_OFFSET(addr)   ((uint64_t)(addr) & 0xFFF)

#define PAGE_PRESENT      (1ULL << 0)   // Present
#define PAGE_WRITABLE     (1ULL << 1)   // Read/Write
#define PAGE_USER         (1ULL << 2)   // User/Supervisor
#define PAGE_PWT          (1ULL << 3)   // Page Write Through
#define PAGE_PCD          (1ULL << 4)   // Page Cache Disable
#define PAGE_ACCESSED     (1ULL << 5)   // Accessed
#define PAGE_DIRTY        (1ULL << 6)   // Dirty (PTE only)
#define PAGE_HUGE         (1ULL << 7)
#define PAGE_PAT          (1ULL << 7)   // PAT (PTE), PS (PDE/PDPTE)
#define PAGE_GLOBAL       (1ULL << 8)   // Global (PTE only)
#define PAGE_NO_EXECUTE   (1ULL << 63)  // NX

struct multiboot2_mmap_entry;

struct mm
{
    phys_addr_t pml4;
};

struct pfn
{
    struct pfn *next;
    uint32_t    flags;
    uint32_t    refcount;
};

struct boot_alloc
{
    phys_addr_t begin;
    phys_addr_t tail;
    phys_addr_t limit;
};

kresult_t init_memory(struct multiboot2_mmap_entry *entries, unsigned int count);
phys_addr_t virt_to_phys(struct mm *mm, virt_addr_t virt);
void alloc_specific_page(phys_addr_t addr);
void free_page(phys_addr_t addr);
phys_addr_t alloc_page(void);
void *phys_to_virt(phys_addr_t phys);
void zero_page(phys_addr_t page);
void kfree(void *addr);
void *kmalloc(size_t len);
boolean_t init_pool(void);
boolean_t map_page(struct mm *mm, phys_addr_t frame, virt_addr_t page, uint32_t attr);
phys_addr_t unmap_page(struct mm *mm, virt_addr_t page);
void map_kernel_for_user_mm(struct mm *mm);

#endif
