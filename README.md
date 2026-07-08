# FYNAOS

A Hobby OS written from scratch.  
FYNAOS is a learning-focused x86-64 operating system project.

## Features 

- [x] Boot the OS with Multiboot2
- [x] Basic kernel entry point
- [x] Serial printing
- [x] Basic CPU exception handling
- [x] Physical memory manager
- [x] Virtual memory manager
- [x] Kernel heap allocator
- [x] Address space management
- [x] Kernel scheduler
- [ ] Handling IRQ with APIC
- [ ] User mode

## Build

### Requirements

- GCC
- GNU Binutils
- GRUB Tools
- NASM
- GNU Make

### Build with MAKE

#### Building kernel

```bash
make
```

#### Creating ISO file

```bash
make iso
```

## Run

Run with QEMU:

```bash
./runvm.sh
```

Debug with QEMU and GDB:

```bash
./runvm.sh debug
```



