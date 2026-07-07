#!/bin/bash

if [ "$1" = "debug" ]; then
    QEMU_FLAGS="-S -s"
else
    QEMU_FLAGS=""
fi

qemu-system-x86_64 -cdrom bin/os.iso -serial stdio $QEMU_FLAGS
