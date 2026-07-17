#ifndef FYNAOS_ELF_H
#define FYNAOS_ELF_H

#include <fynaos/types.h>

#define ELF_MAGIC 0x464C457F

#define PT_LOAD 1

struct elf64_ehdr
{
    uint8_t  magic[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phdr_offset;
    uint64_t shdr_offset;
    uint32_t flags;
    uint16_t ehdr_size;
    uint16_t phdr_entry_size;
    uint16_t phdr_count;
    uint16_t shdr_entry_size;
    uint16_t shdr_count;
    uint16_t shdr_str_index;
} __attribute__((packed));

struct elf64_phdr
{
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesize;
    uint64_t memsize;
    uint64_t align;
} __attribute__((packed));

#endif
