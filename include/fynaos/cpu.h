#ifndef _FYNAOS_CPU_H
#define _FYNAOS_CPU_H

#include <fynaos/types.h>

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

static inline __forceinline void outw(int16_t port, int16_t val)
{
    asm volatile (
        "outw %1, %0"
        :: "Nd"(port), "a"(val)
    );
}

#endif
