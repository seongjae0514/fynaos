#ifndef FXOS_SNPRINTF_H
#define FXOS_SNPRINTF_H

#include <fynaos/types.h>
#include <stdarg.h>

size_t vsnprintf(char *buffer, size_t length, const char *format, va_list args);
size_t snprintf(char *buffer, size_t len, const char *format, ...);

#endif
