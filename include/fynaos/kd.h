/*
 * include/fynaos/kd.h
 * FYNAOS Kernel Debugger
 * 
 * This module manages kernel debugging.
 */

#ifndef FYNAOS_KD_H
#define FYNAOS_KD_H

#include <fynaos/types.h>

/*
 * NOTE: ENTERPROC(aka kd_enterproc) and LEAVEPROC(aka kd_leaveproc)
 *       must not be called in serial printing helper functions. (ex. kprintf)
 *       Because kd_enterproc and kd_leaveproc call kprintf.
 *       It will cause stack overflow!
 */

void kd_assert(boolean_t cond, const char *exp,
               const char *msg,
               const char *file, int line, const char *function);

#ifdef DEBUG
#define ASSERT(cond, msg) \
        kd_assert(cond, #cond, msg, __FILE__, __LINE__, __FUNCTION__)
#define DPRINT(fmt, ...) \
        kprintf(fmt, ##__VA_ARGS__)
#else
#define ASSERT(cond, msg)
#define DPRINT(fmt, ...)
#endif /* DEBUG */

#endif /* FYNAOS_KD_H */
