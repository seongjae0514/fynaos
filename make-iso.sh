#!/bin/bash

mkdir -p .isodir/boot/grub
cp fxoskrnl.elf .isodir/fxoskrnl.elf
cp grub.cfg .isodir/boot/grub/grub.cfg
grub-mkrescue -o os.iso .isodir
rm -rf .isodir