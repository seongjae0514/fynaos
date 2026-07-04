#include <fynaos/types.h>
#include <fynaos/kernel.h>

void *memset(void *dst, int val, size_t len)
{
    char *a = dst;

    while (len--)
    {
        *a++ = (char)val;
    }

    return dst;
}

void *memcpy(void *dst, void *src, size_t len)
{
    char *d = dst, *s = src;

    while (len--)
    {
        *d++ = *s++;
    }

    return dst;
}
