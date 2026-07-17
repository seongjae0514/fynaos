#ifndef FYNAOS_TARFS_H
#define FYNAOS_TARFS_H

#include <fynaos/types.h>

struct ustar_header
{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char pad[12];
};

void tarfs_init(void *addr);
int tarfs_find(const char *name, void **addr);
int tarfs_read(void *buffer, size_t len, const char *name);

#endif
