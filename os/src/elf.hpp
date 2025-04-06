#pragma once

#include "types.hpp"

// elf header structure
struct elf_header_t
{
    uint32 magic;
    uint8 elf[12];
    uint16 type;      /* Object file type */
    uint16 machine;   /* Architecture */
    uint32 version;   /* Object file version */
    uint64 entry;     /* Entry point virtual address */
    uint64 phoff;     /* Program header table file offset */
    uint64 shoff;     /* Section header table file offset */
    uint32 flags;     /* Processor-specific flags */
    uint16 ehsize;    /* ELF header size in bytes */
    uint16 phentsize; /* Program header table entry size */
    uint16 phnum;     /* Program header table entry count */
    uint16 shentsize; /* Section header table entry size */
    uint16 shnum;     /* Section header table entry count */
    uint16 shstrndx;  /* Section header string table index */
};

// Program segment header.
typedef struct elf_prog_header_t
{
    uint32 type;   /* Segment type */
    uint32 flags;  /* Segment flags */
    uint64 off;    /* Segment file offset */
    uint64 vaddr;  /* Segment virtual address */
    uint64 paddr;  /* Segment physical address */
    uint64 filesz; /* Segment size in file */
    uint64 memsz;  /* Segment size in memory */
    uint64 align;  /* Segment alignment */
} elf_prog_header;

// segment types, attributes of elf_prog_header_t.flags
#define SEGMENT_READABLE 0x4
#define SEGMENT_EXECUTABLE 0x1
#define SEGMENT_WRITABLE 0x2
#define ELF_MAGIC 0x464C457FU // "\x7FELF" in little endian
#define ELF_PROG_LOAD 1