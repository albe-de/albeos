#!/bin/bash
set -e

# Define variables
KERNEL_SRC="kernel.c"
KERNEL_TOOLS_DIR="dependencies"
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
    if [[ -f "$item" && "$(basename "$item")" != "kernel_malloc_old.c" ]]; then  # Only include files, not directories
        kernel_deps+=" $item"
    fi
done

# Add the assembly allocator explicitly
kernel_deps+=" dependencies/including/kernel_malloc.s"

# Compile the kernel and tool files

### **** UNCOMMENT THE NEXT LINE IF YOURE USING UBUNTU****
#./$KERNEL_TOOLS_DIR/i686-elf-tools-linux/bin/i686-elf-gcc -ffreestanding -nostdlib -T "linker.ld" "$KERNEL_SRC" $kernel_deps -o "kernel.elf" -lgcc

### **** COMMENT THE NEXT 8 LINE(s) IF YOURE USING UBUNTU****
gcc -m32 -ffreestanding -nostdlib \
-fno-pie -no-pie \
-fno-stack-protector \
-fno-asynchronous-unwind-tables \
-Wl,-z,norelro \
-T linker.ld \
"$KERNEL_SRC" $kernel_deps \
-o kernel.elf -lgcc

echo "Built kernel files"
mv "kernel.elf" "$BOOT_DIR"
grub-mkrescue -o AlbeOS.iso iso

echo "Build completed. You can now boot the ISO image using QEMU:"
echo "qemu-system-x86_64 -cdrom AlbeOS.iso -vga std"
