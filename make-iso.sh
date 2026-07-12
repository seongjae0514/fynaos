#!/bin/bash

set +x

mkdir -p .isodir/boot/grub
cp $1 .isodir/fxoskrnl.elf
cp grub.cfg .isodir/boot/grub/grub.cfg
grub-mkrescue -o $2 .isodir 2>/dev/null
rm -rf .isodir
