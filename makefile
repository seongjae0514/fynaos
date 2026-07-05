#---------------------------------------
# Variables
#---------------------------------------

#
# Build tools
#

CC := gcc
LD := ld
AS := nasm

#
# Build options
#

CCFLAGS := -ffreestanding                  \
           -fno-pie                        \
           -no-pie                         \
           -fno-stack-protector            \
           -fno-plt                        \
           -fno-builtin                    \
           -fno-asynchronous-unwind-tables \
           -fno-exceptions                 \
           -fno-omit-frame-pointer         \
           -Wall                           \
           -Wextra                         \
           -Werror                         \
           -Wshadow                        \
           -Wconversion                    \
           -Wsign-conversion               \
           -Wundef                         \
           -Wcast-align                    \
           -Wstrict-prototypes             \
           -m64                            \
           -mcmodel=large                  \
           -mno-red-zone                   \
           -c                              \
		   -Iinclude                       \
		   -DDEBUG=1                       \
		   -g                              \

LDFLAGS := -T linker.ld -nostdlib

ASFLAGS := -f elf64 -Iinclude

#
# Sources
#

C_SOURCES   := init/init.c        \
               kernel/serial.c    \
               rtl/snprintf.c     \
               kernel/kprintf.c   \
               kernel/interrupt.c \
               kernel/panic.c     \
               mm/mm.c            \
               rtl/string.c       \
               kd/kd.c            \
               mm/pool.c

ASM_SOURCES := boot/boot.asm            \
               kernel/interrupt_asm.asm

OBJECTS     := $(addprefix bin/, $(C_SOURCES:.c=.o)) $(addprefix bin/, $(ASM_SOURCES:.asm=.o))

#---------------------------------------
# Build
#---------------------------------------

.PHONY: all clean iso bindirs

all: bin/fxoskrnl.elf

#---------------------------------------
# Link
#---------------------------------------

bin/fxoskrnl.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

#---------------------------------------
# Assemble/Compile
#---------------------------------------

bin/%.o: %.c | bindirs
	$(CC) $(CCFLAGS) $< -o $@

bin/%.o: %.asm | bindirs
	$(AS) $(ASFLAGS) $< -o $@

#---------------------------------------
# Clean
#---------------------------------------

clean:
	rm -rf $(OBJECTS) fxoskrnl.elf

#---------------------------------------
# ISO
#---------------------------------------

iso: bin/fxoskrnl.elf
	./make-iso.sh $< bin/os.iso

#---------------------------------------
# Bin
#---------------------------------------

bindirs:
	mkdir -p $(sort $(dir $(OBJECTS)))
