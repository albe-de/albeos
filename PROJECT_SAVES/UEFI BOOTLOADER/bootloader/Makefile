# "A makefile is a text file used by the make utility to automate the process of building software projects"
# Goodbye, build.sh--- you werent long for this world

# Fixed UEFI Makefile
TARGET = BOOTX64.EFI
EFI_DIR = esp/EFI/BOOT
BUILD_DIR = build

# Use clang for UEFI (mingw has issues)
CC = clang
CFLAGS = -target x86_64-pc-win32-coff -fno-stack-protector -fshort-wchar -mno-red-zone -Wall
LDFLAGS = -fuse-ld=lld-link -Wl,/subsystem:efi_application -Wl,/entry:efi_main -Wl,/nodefaultlib -Wl,/dll

all: $(TARGET) image

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(EFI_DIR):
	mkdir -p $(EFI_DIR)

$(TARGET): boot.c | $(BUILD_DIR) $(EFI_DIR)
	$(CC) $(CFLAGS) -c boot.c -o $(BUILD_DIR)/boot.o
	lld-link /subsystem:efi_application /entry:efi_main /out:$(BUILD_DIR)/$(TARGET) /nodefaultlib /dll $(BUILD_DIR)/boot.o
	cp $(BUILD_DIR)/$(TARGET) $(EFI_DIR)/

image: $(TARGET)
	dd if=/dev/zero of=efiboot.img bs=1M count=16
	mkfs.fat -F32 efiboot.img
	mkdir -p /tmp/efimount
	sudo mount -o loop efiboot.img /tmp/efimount
	sudo cp -r esp/EFI /tmp/efimount/
	sudo umount /tmp/efimount

clean:
	rm -rf $(BUILD_DIR) esp efiboot.img

.PHONY: all image clean
