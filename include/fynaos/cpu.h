#ifndef _FYNAOS_CPU_H
#define _FYNAOS_CPU_H

#include <fynaos/types.h>

enum cpu_mode
{
    CPU_MODE_NONE,
    CPU_MODE_KERNEL,
    CPU_MODE_USER
};

static inline __forceinline void disable_interrupts(void)
{
    asm volatile ("cli");
}

static inline __forceinline void enable_interrupts(void)
{
    asm volatile ("sti");
}

static inline __forceinline void halt_cpu(void)
{
    asm volatile ("hlt");
}

static inline __forceinline __noreturn void halt_cpu_forever(void)
{
    for (;;)
    {
        disable_interrupts();
        halt_cpu();
    }
}

static inline __forceinline uint8_t inb(uint16_t port)
{
    uint8_t result;
    asm volatile (
        "inb %1, %0"
        : "=a"(result)
        : "Nd"(port)
    );
    return result;
}

static inline __forceinline uint16_t inw(uint16_t port)
{
    uint16_t result;
    asm volatile (
        "inw %1, %0"
        : "=a"(result)
        : "Nd"(port)
    );
    return result;
}

static inline __forceinline void outb(uint16_t port, uint8_t val)
{
    asm volatile (
        "outb %1, %0"
        :: "Nd"(port), "a"(val)
    );
}

static inline __forceinline void outw(uint16_t port, uint16_t val)
{
    asm volatile (
        "outw %1, %0"
        :: "Nd"(port), "a"(val)
    );
}

static inline __forceinline unsigned long save_and_disable_interrupts(void)
{
    unsigned long flags;

    asm volatile (
        "pushfq\n\t"
        "pop %0\n\t"
        "cli\n\t"
        :"=r"(flags)
    );

    return flags;
}

static inline __forceinline void restore_interrupts(unsigned long flags)
{
    asm volatile (
        "push %0\n\t"
        "popfq\n\t"
        ::"r"(flags)
    );
}

static inline __forceinline void write_cr3(uintptr_t pml4)
{
    asm volatile ("mov %0, %%cr3"::"r"(pml4));
}

static inline __forceinline void cpuid(uint32_t leaf, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
    asm volatile(
    "cpuid"
    : "=a"(*a),
      "=b"(*b),
      "=c"(*c),
      "=d"(*d)
    : "a" (leaf)
    );
}

#endif
