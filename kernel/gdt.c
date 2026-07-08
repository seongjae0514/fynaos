/*
 * fynaos/kernel/gdt.c
 *
 * Kernel Global Descriptor Table Management
 */

#include <fynaos/kernel.h>

#define STACK_SIZE   4096
#define TSS_SELECTOR 0x28

struct tss {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed));

static __align(16) uint8_t special_stack[STACK_SIZE] = { 0 };
static __align(16) struct tss tss = {
    0,
    0,
    0,
    0,
    0,
    (uint64_t)(special_stack + sizeof(special_stack)),
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0xFFFF
};

static __align(16) uint64_t gdt[] = {
    0x0000000000000000, // null
    0x00209A0000000000, // kernel code
    0x0000920000000000, // kernel data
    0x0000F20000000000, // user data
    0x0020FA0000000000, // user code
    0x0000000000000000, // tss
    0x0000000000000000  // tss
};

void init_gdt(void)
{
    uint64_t low, high;
    uint64_t base, limit;

    base = (uint64_t)&tss;
    limit = sizeof(struct tss) - 1;

    low =
         ( limit & 0xFFFFULL)
       | ((base & 0xFFFFULL) << 16)
       | (((base >> 16) & 0xFFULL) << 32)
       | ((uint64_t)0x89 << 40)
       | (((limit >> 16) & 0xFULL) << 48)
       | (((base >> 24) & 0xFFULL) << 56);
 
    high = ((base >> 32) & 0xFFFFFFFFULL);

    gdt[5] = low;
    gdt[6] = high;

    /*
     * During GDT initialization the CPU is still running only in
     * kernel mode, so RSP0 is not used yet.
     * It will be updated by swap_rsp0() before entering user mode.
     */

    tss.rsp0 = 0;

    struct gdtr gdtr = { sizeof(gdt) - 1, (uint64_t)gdt  };

    asm volatile (
        "lgdt %0\n\t"
        "pushq $0x08\n\t"
        "leaq 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%ss\n\t"
        "mov $0x28, %%ax\n\t"
        "ltr %%ax\n\t"
        ::"m"(gdtr)
        : "rax", "memory"
    );
}

uint64_t swap_rsp0(uint64_t s)
{
    uint64_t old = tss.rsp0;
    tss.rsp0 = s;
    return old;
}
