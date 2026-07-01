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

CCFLAGS := -ffreestanding -fno-pie -no-pie -fno-stack-protector -fno-plt  \
           -fno-builtin -fno-asynchronous-unwind-tables -fno-exceptions   \
           -fno-omit-frame-pointer                                        \
           -Wall -Wextra  -Wshadow -Wconversion -Wsign-conversion  \
           -Wundef -Wcast-align -Wstrict-prototypes                       \
           -g -m64 -mcmodel=large -c -Iinclude/ -mno-red-zone -DDEBUG=1

LDFLAGS := -T linker.ld -nostdlib

ASFLAGS := -f elf64 -Iinclude

#
# Sources
#

C_SOURCES   := fxos/init.c          \
               fxos/ke/interrupt.c  \
               fxos/mm/bootalloc.c  \
               fxos/hal/interrupt.c \
               fxos/hal/serial.c    \
               fxos/hal/port.c

ASM_SOURCES := fxos/start.asm  \
               fxos/ke/isr.asm

OBJECTS     := $(addprefix bin/, $(C_SOURCES:.c=.o)) $(addprefix bin/, $(ASM_SOURCES:.asm=.o))

#---------------------------------------
# Build
#---------------------------------------

.PHONY: all clean iso bindirs

all: fxoskrnl.elf

#---------------------------------------
# Link
#---------------------------------------

fxoskrnl.elf: $(OBJECTS)
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

iso: fxoskrnl.elf
	./make-iso.sh

#---------------------------------------
# Bin
#---------------------------------------

bindirs:
	mkdir -p $(sort $(dir $(OBJECTS)))
