#ifndef _FYNAOS_STRING_H
#define _FYNAOS_STRING_H

#include <fynaos/types.h>

void *memset(void *, int, size_t);
void *memcpy(void *dst, void *src, size_t);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *buf, const char *s);
int strncmp(const char *s1, const char *s2, size_t len);
int otoi(const char *s);

#endif
