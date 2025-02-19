NASM = nasm
LD = ld
CC = gcc

SRC_DIR = src
BUILD_DIR = build
BOOT_DIR = boot

NASM_FLAGS = -f elf32
LDFLAGS = -m elf_i386 -T $(SRC_DIR)/linker.ld
GCC_FLAGS = -masm=intel -m32 -fno-builtin -g -ffreestanding -O2 -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -c -I.

NAME = kfs

SRCS_C =	$(wildcard *.c) \
			$(wildcard */*.c) \
			$(wildcard */*/*.c)

SRCS_ASM =	$(wildcard *.s) \
			$(wildcard */*.s) \
			$(wildcard */*/*.s) \

HEADER_C =	$(wildcard *.h) \
			$(wildcard */*.h) \
			$(wildcard */*/*.h)

C_OBJS 		= $(SRCS_C:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
ASM_OBJS 	= $(SRCS_ASM:$(SRC_DIR)/%.s=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/$(NAME)

$(BUILD_DIR)/$(NAME): $(C_OBJS) $(ASM_OBJS)
	@$(LD) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_C)
	@mkdir -p $(dir $@)
	$(CC) $(GCC_FLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(NASM) $(NASM_FLAGS) $< -o $@ 

run: $(BUILD_DIR)/$(NAME)
	@qemu-system-i386 -kernel $<

clean:
	@rm -rf $(BUILD_DIR)

re: clean all

debug: all
	qemu-system-i386 -kernel build/$(NAME) -s -S & \
	sleep 1 && \
	gdb -ex "target remote :1234" build/kfs

.PHONY: all clean run re