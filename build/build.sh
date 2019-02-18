#!/bin/bash
mkdir $1
cp $2 $1/osserver.bin
mkdir $1/buildiso
mkdir $1/buildiso/boot
mkdir $1/buildiso/boot/grub
cp grub.cfg $1/buildiso/boot/grub/
mkdir $1/buildiso/boot/efi
mkdir $1/buildiso/boot/efi/EFI/
mkdir $1/buildiso/boot/efi/EFI/grub/
cp grub.cfg $1/buildiso/boot/efi/EFI/grub/
cp $2 $1/buildiso/boot/osserver.bin
cd $1
grub-mkrescue -o os.iso buildiso
rm -rf $1/buildiso/