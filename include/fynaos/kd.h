/*
 * include/fynaos/kd.h
 * FYNAOS Kernel Debugger
 * 
 * This module manages kernel debugging.
 */

#ifndef FYNAOS_KD_H
#define FYNAOS_KD_H

#include <fynaos/types.h>
#include <fynaos/cpu.h>

void kd_assert(boolean_t cond, const char *exp,
               const char *msg,
               const char *file, int line, const char *function);

#ifdef DEBUG
#define ASSERT(cond, msg) \
        kd_assert(cond, #cond, msg, __FILE__, __LINE__, __FUNCTION__)
#define DPRINT(fmt, ...) \
        kprintf(fmt, ##__VA_ARGS__)
#define INTERRUPT_DISABLED_ASSERT(msg)                                                        \
        do {                                                                                  \
            unsigned long flags = save_and_disable_interrupts();                                        \
            kd_assert(!(flags & 0x200), "flags & 0x200", msg, __FILE__, __LINE__, __FUNCTION__); \
            restore_interrupts(flags);                                                        \
        } while (0)
#else
#define ASSERT(cond, msg)
#define DPRINT(fmt, ...)
#define INTERRUPT_DISABLED_ASSERT(msg)
#endif /* DEBUG */

#endif /* FYNAOS_KD_H */
