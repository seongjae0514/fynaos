#!/bin/bash

mkdir -p .isodir/boot/grub
cp $1 .isodir/fxoskrnl.elf
cp grub.cfg .isodir/boot/grub/grub.cfg
grub-mkrescue -o $2 .isodir
rm -rf .isodir
