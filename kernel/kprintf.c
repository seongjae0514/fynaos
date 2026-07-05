#include <fynaos/kernel.h>
#include <fynaos/snprintf.h>
#include <stdarg.h>
#include <fynaos/kd.h>

static void put_string(const char *s)
{
    while (*s)
    {
        write_serial(*s++);
    }
}

size_t kprintf(const char *fmt, ...)
{
    char    buffer[512];
    va_list args;
    size_t  len;
    va_start(args, fmt);

    len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    put_string(buffer);

    va_end(args);
    return len;
}
