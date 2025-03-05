NASM = nasm
LD = ld
CC = gcc

SRC_DIR = src
BUILD_DIR = build
BOOT_DIR = boot
ISO_DIR  = iso

NASM_FLAGS = -f elf32
LDFLAGS = -m elf_i386 -T $(SRC_DIR)/linker.ld
GCC_FLAGS = -masm=intel -m32 -fno-builtin -g -ffreestanding -O2 -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -c -I.

NAME = kfs
ISO  = $(NAME).iso

SRCS_C =	$(wildcard *.c) \
			$(wildcard */*.c) \
			$(wildcard */*/*.c)

SRCS_ASM =	$(wildcard *.s) \
			$(wildcard */*.s) \
			$(wildcard */*/*.s) \

C_OBJS 		= $(SRCS_C:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
ASM_OBJS 	= $(SRCS_ASM:$(SRC_DIR)/%.s=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/$(NAME)

$(BUILD_DIR)/$(NAME): $(C_OBJS) $(ASM_OBJS)
	@$(LD) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(GCC_FLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(NASM) $(NASM_FLAGS) $< -o $@ 

run: $(BUILD_DIR)/$(NAME)
	qemu-system-i386 -kernel $(BUILD_DIR)/$(NAME)

clean:
	@rm -rf $(BUILD_DIR) $(ISO) $(ISO_DIR)

re: clean all

iso: all
	mkdir -p iso/boot/grub
	cp build/kfs iso/boot
	cp src/grub.cfg iso/boot/grub
	grub-file --is-x86-multiboot iso/boot/$(NAME)
	@grub-mkrescue -o $(NAME).iso --compress=xz iso

debug: all
	qemu-system-i386 -kernel $(BUILD_DIR)/$(NAME) -s & \
	sleep 2 && \
	gdb -ex "target remote :1234" $(BUILD_DIR)/$(NAME)

.PHONY: all clean run re