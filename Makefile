NASM = nasm
LD = ld
CC = gcc

SRC_DIR = src
BUILD_DIR = build
BOOT_DIR = boot

NASM_FLAGS = -f elf32
LDFLAGS = -m elf_i386 -T $(SRC_DIR)/linker.ld
GCC_FLAGS = -masm=intel -m32 -fno-builtin -ffreestanding -O2 -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -c

NAME = kfs

SRCS_C = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/lib/*.c)
SRCS_ASM = $(wildcard $(SRC_DIR)/*.s)
OBJS = $(SRCS_C:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) $(SRCS_ASM:$(SRC_DIR)/%.s=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/$(NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(GCC_FLAGS) $< -o $@

$(BUILD_DIR)/lib/%.o: $(SRC_DIR)/lib/%.c
	@mkdir -p $(dir $@)
	$(CC) $(GCC_FLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(NASM) $(NASM_FLAGS) $< -o $@

$(BUILD_DIR)/$(NAME): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

run: $(BUILD_DIR)/$(NAME)
	qemu-system-i386 -kernel $<

clean:
	rm -rf $(BUILD_DIR)

re: clean all run

.PHONY: all clean run re
