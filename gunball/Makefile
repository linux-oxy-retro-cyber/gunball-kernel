CC = gcc
CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdlib -nodefaultlibs -Wall -Iinclude
AS = nasm
ASFLAGS = -f elf32
LD = ld
LDFLAGS = -m elf_i386 -T linker.ld

BUILD_DIR = build
SRC_DIR = src

OBJS = $(BUILD_DIR)/boot.o \
       $(BUILD_DIR)/vga.o \
       $(BUILD_DIR)/kernel.o

OUTPUT = $(BUILD_DIR)/kernel.bin

all: setup $(OUTPUT)

setup:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot/boot.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/vga.o: $(SRC_DIR)/drivers/vga.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(OUTPUT)

run: all
	qemu-system-i386 -kernel $(OUTPUT)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all setup run clean
