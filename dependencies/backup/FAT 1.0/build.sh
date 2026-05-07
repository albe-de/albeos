#!/bin/bash
set -e

# Define variables
KERNEL_SRC="kernel.c"
KERNEL_TOOLS_DIR="dependencies"
ISO_DIR="iso"
BOOT_DIR="$ISO_DIR/boot"
GRUB_DIR="$BOOT_DIR/grub"
GRUB_CFG="grub.cfg"

# Disk image
DISK_IMG="disk.img"
DISK_SIZE_SECTORS=2048   # 1MB (2048 * 512)

# Create disk image if it doesn't exist
if [ ! -f disk.img ]; then
    echo "[+] Creating disk image (disk.img)..."
    dd if=/dev/zero of=disk.img bs=1M count=32

    echo "[+] Partitioning disk..."
    printf "o\nn\np\n1\n\n\nw\n" | fdisk disk.img

    echo "[+] Creating FAT32 filesystem..."

    if ! command -v mkfs.fat &> /dev/null; then
        echo "[!] mkfs.fat not found. Install dosfstools."
        exit 1
    fi

    LOOP=$(losetup -Pf --show disk.img)

    mkfs.fat -F 32 ${LOOP}p1

    losetup -d $LOOP

    echo "[+] Disk fully initialized (MBR + FAT)"
fi

sudo chown $SUDO_USER:$SUDO_USER disk.img 2>/dev/null || true

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
boot.s "$KERNEL_SRC" $kernel_deps \
-o kernel.elf -lgcc

echo "Built kernel files"
mv "kernel.elf" "$BOOT_DIR"
grub-mkrescue -o AlbeOS.iso iso

echo "Build completed. Boot with:"
echo "FOR UBUNTU:"
echo "   qemu-system-x86_64 -cdrom AlbeOS.iso -drive file=disk.img,format=raw -vga std"
echo "FOR ARCH:"
echo "   qemu-system-x86_64 -cdrom AlbeOS.iso -drive file=disk.img,format=raw -vga std -vnc 127.0.0.1:0"

# echo "qemu-system-x86_64 -cdrom AlbeOS.iso -vga std -vnc 127.0.0.1:0"