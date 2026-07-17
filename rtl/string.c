#include <fynaos/types.h>
#include <fynaos/kernel.h>
#include <fynaos/kd.h>
#include <fynaos/string.h>

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

int strcmp(const char *s1, const char *s2)
{
    while (*s1 || *s2)
    {
        if (*s1++ != *s2++) return *s1 - *s2;
    }
    return 0;
}

char *strcpy(char *buf, const char *s)
{
    char *p = buf;
    while (*s)
    {
        *p++ = *s;
    }
    return buf;
}

int otoi(const char *s)
{
    int v = 0;

    while (*s == ' ') s++;

    while (*s >= '0' && *s <= '7')
    {
        v = (v << 3) + (*s++ - '0');
    }

    return v;
}

int strncmp(const char *s1, const char *s2, size_t len)
{
    while (len-- && (*s1 || *s2))
    {
        if (*s1 != *s2) return *s1 - *s2;
    }
    return 0;
}
