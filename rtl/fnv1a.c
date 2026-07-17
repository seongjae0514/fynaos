#include <fynaos/hash.h>

uint64_t fnv1a(const char *s)
{
    uint64_t h = 14695981039346656037ULL;

    while (*s) {
        h ^= (unsigned char)*s++;
        h *= 1099511628211ULL;
    }

    return h;
}
