NASM = nasm
LD = ld
SRC_DIR = src
BUILD_DIR = build
NASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T $(SRC_DIR)/linker.ld
NAME = kfs.bin

all:
	mkdir -p build
	nasm -f bin -o build/boot.bin src/boot.s
	qemu-system-i386 -fda build/boot.bin

clean:
	rm -rf $(BUILD_DIR) src/*.o

.PHONY: clean