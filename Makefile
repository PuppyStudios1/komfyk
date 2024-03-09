ifeq ($(K),1)
x86_64_asm_source_files := $(shell find src/impl/x86_64 -name *.asm)
x86_64_asm_object_files := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%.o,  $(x86_64_asm_source_files))

kernel_source_files := $(shell find src/impl/kernel -name *.c)
kernel_object_files := $(patsubst src/impl/kernel/%.c, build/x86_64/%.o,  $(kernel_source_files))

$(x86_64_asm_object_files): build/x86_64/%.o : src/impl/x86_64/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/x86_64/%.o, src/impl/x86_64/%.asm, $@) -o $@

.PHONY: build-x86_64
build-x86_64:  $(x86_64_asm_object_files)
	mkdir -p dist/x86_64 && \
	x86_64-elf-ld -n -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(x86_64_asm_object_files) && \
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso

	$(info iso image is ready!)
endif

# Makefile that includes a menuconfig target

# Path to the Kconfig file
KCONFIG := Kconfig

# Command to run menuconfig
MENUCONFIG := mconf

# Check for the existence of the Kconfig file
ifeq ("$(wildcard $(KCONFIG))","")
$(error No Kconfig file found)
endif

# Default target
all:
	@echo "Run 'make menuconfig' to configure."

# menuconfig target
menuconfig:
	@$(MENUCONFIG) $(KCONFIG)

# Include the generated .config file if it exists
-include .config

