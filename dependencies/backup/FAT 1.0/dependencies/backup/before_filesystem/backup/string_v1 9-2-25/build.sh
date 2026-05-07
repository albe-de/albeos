#!/bin/bash
set -e

# Define variables
KERNEL_SRC="kernel.c"
KERNEL_TOOLS_DIR="kernel_tools"
ISO_DIR="iso"
BOOT_DIR="$ISO_DIR/boot"
GRUB_DIR="$BOOT_DIR/grub"
GRUB_CFG="grub.cfg"

# Remove existing files
rm -f "AlbeOS.iso"
rm -f "kernel.elf"
rm -f "$BOOT_DIR/kernel.elf"

# Collect all Kernel dependencies
kernel_deps=""
for item in "$KERNEL_TOOLS_DIR"/*; do
    if [[ -f "$item" ]]; then  # Only include files, not directories
        kernel_deps+=" $item"
    fi
done

# Compile the kernel and tool files
./i686-elf-tools-linux/bin/i686-elf-gcc -ffreestanding -nostdlib -T "linker.ld" "$KERNEL_SRC" $kernel_deps -o "kernel.elf"

echo "Built kernel files"
mv "kernel.elf" "$BOOT_DIR"
grub-mkrescue -o AlbeOS.iso iso

echo "Build completed. You can now boot the ISO image using QEMU:"
echo "qemu-system-x86_64 -cdrom AlbeOS.iso"
