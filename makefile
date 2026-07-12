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
           -MMD -MP

LDFLAGS := -T linker.ld -nostdlib

ASFLAGS := -f elf64 -Iinclude -g

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
               mm/pmm.c           \
               mm/vmm.c           \
               mm/boot.c          \
               rtl/string.c       \
               kd/kd.c            \
               mm/pool.c          \
               kernel/sched.c     \
               kernel/pic.c       \
               kernel/timer.c     \
               kernel/gdt.c

ASM_SOURCES := boot/boot.asm             \
               kernel/interrupt_asm.asm  \
               kernel/switch_context.asm

OBJECTS     := $(addprefix bin/, $(C_SOURCES:.c=.o)) $(addprefix bin/, $(ASM_SOURCES:.asm=.o))

#
# Verbose
#

V ?= 0
ifeq ($(V), 0)
Q := @
else
Q := 
endif

#---------------------------------------
# Build
#---------------------------------------

.PHONY: all clean iso bindirs

all: bin/fxoskrnl.elf

#---------------------------------------
# Link
#---------------------------------------

bin/fxoskrnl.elf: $(OBJECTS)
	@printf "LD  %s\n" "$@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^

#---------------------------------------
# Assemble/Compile
#---------------------------------------

-include $(OBJECTS:.o=.d)

bin/%.o: %.c | bindirs
	@printf "CC  %s\n" "$@"
	$(Q)$(CC) $(CCFLAGS) $< -o $@

bin/%.o: %.asm | bindirs
	@printf "AS  %s\n" "$@"
	$(Q)$(AS) $(ASFLAGS) $< -o $@

#---------------------------------------
# Clean
#---------------------------------------

clean:
	$(Q)rm -rf bin/

#---------------------------------------
# ISO
#---------------------------------------

iso: bin/os.iso

bin/os.iso: bin/fxoskrnl.elf
	@printf "ISO %s\n" "$@"
	$(Q)./make-iso.sh $< $@

#---------------------------------------
# Bin
#---------------------------------------

bindirs:
	$(Q)mkdir -p $(sort $(dir $(OBJECTS)))
