#include <fynaos/fs/tarfs.h>
#include <fynaos/string.h>

#define BLOCK_SIZE 512

static void *tarfs_root = NULL;

static boolean_t check_tar_end(void *addr)
{
    uint64_t *a = addr;
    for (int i = 0; i < (int)(BLOCK_SIZE * 2 / sizeof(uint64_t)); i++)
    {
        if (a[i] != 0) return FALSE;
    }
    return TRUE;
}

void tarfs_init(void *addr)
{
    tarfs_root = addr;
}

int tarfs_find(const char *name, void **addr)
{
    char *cur = tarfs_root;

    for (;;)
    {
        struct ustar_header *header = (struct ustar_header*)cur;
        int size = otoi(header->size);

        if (header->name[0] == 0 && check_tar_end(cur))
        {
            return -1;
        }

        if (strncmp(header->name, name, sizeof(header->name)) == 0)
        {
            *addr = cur + BLOCK_SIZE;
            return size;
        }

        cur += _align_up(size, BLOCK_SIZE) + BLOCK_SIZE;
    }
}

int tarfs_read(void *buffer, size_t len, const char *name)
{
    void *addr;
    int tar_len = tarfs_find(name, &addr);
    if (len > (size_t)tar_len) len = (size_t)tar_len;
    memcpy(buffer, addr, len);
    return tar_len;
}
