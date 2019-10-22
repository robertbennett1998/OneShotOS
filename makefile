SOURCE_DIR = src
_SOURCE_DIR = src
INTIMIDIATE_DIR = intimidiate
_INTIMIDIATE_DIR = ..
BIN_DIR = bin
_BIN_DIR = ..
DEPLOY_DIR = deploy
_DEPLOY_DIR = ..
i686_ELF_BIN_DIR = $(BIN_DIR)/i686-elf
_i686_ELF_BIN_DIR = $(_BIN_DIR)/..
i686_ELF_DEPLOY_DIR = $(DEPLOY_DIR)/i686-elf
_i686_ELF_DEPLOY_DIR = $(_DEPLOY_DIR)/..
OUT_FILE_NAME = OneShotOS

all : make_dirs build deploy

build : i686-elf-link

deploy : i686-elf-deploy

rebuild : clean all

launch-i686 :
	qemu-system-i386 -cdrom deploy/i686-elf/OneShotOS-i686-elf.iso

i686-elf-compile :
	$(eval src_asm_files = $(shell find $(SOURCE_DIR)/ -maxdepth 1 -type f -regex ".*\.s"))
	$(eval src_asm_o_files = $(notdir $(src_asm_files)))	
	$(eval src_asm_o_files = $(src_asm_o_files:.s=.o))
	$(eval src_asm_files = $(addprefix $(_INTIMIDIATE_DIR)/, $(src_asm_files)))	
	cd $(INTIMIDIATE_DIR);\
	i686-elf-as -c $(src_asm_files) -o $(src_asm_o_files)

	$(eval src_c_files = $(shell find $(SOURCE_DIR)/ -maxdepth 1 -type f -regex ".*\.c"))
	$(eval src_c_o_files = $(notdir $(src_c_files)))	
	$(eval src_c_o_files = $(src_c_o_files:.c=.o))
	$(eval src_c_files = $(addprefix $(_INTIMIDIATE_DIR)/, $(src_c_files)))	
	cd $(INTIMIDIATE_DIR);\
	i686-elf-gcc -c $(src_c_files) -o $(src_c_o_files) -std=gnu99 -ffreestanding -O2 -Wall -Wextra

i686-elf-link : i686-elf-compile
	$(eval o_files = $(shell find $(INTIMIDIATE_DIR)/ -maxdepth 1 -type f -regex ".*\.o"))
	$(eval o_files = $(addprefix $(_i686_ELF_BIN_DIR)/, $(o_files)))
	cd $(i686_ELF_BIN_DIR);\
	i686-elf-gcc -T $(_i686_ELF_BIN_DIR)/linker.ld -o $(OUT_FILE_NAME)-i686-elf.bin -ffreestanding -O2 -nostdlib $(o_files) -lgcc
	rm -rf $(INTIMIDIATE_DIR)/*

i686-elf-deploy :
	cd $(INTIMIDIATE_DIR);\
	mkdir -p boot/grub;\
	echo "menuentry \"$(OUT_FILE_NAME)\" { multiboot /boot/$(OUT_FILE_NAME)-i686-elf.bin }" > boot/grub/grub.cfg;\
	cp $(_INTIMIDIATE_DIR)/$(i686_ELF_BIN_DIR)/$(OUT_FILE_NAME)-i686-elf.bin boot
	grub-mkrescue -o $(i686_ELF_DEPLOY_DIR)/$(OUT_FILE_NAME)-i686-elf.iso $(INTIMIDIATE_DIR)/
	rm -rf $(INTIMIDIATE_DIR)

clean :
	rm -rf $(BIN_DIR)
	rm -rf $(DEPLOY_DIR)
	rm -rf $(INTIMIDIATE_DIR)

make_dirs :
	mkdir -p $(i686_ELF_BIN_DIR)
	mkdir -p $(i686_ELF_DEPLOY_DIR)
	mkdir -p $(INTIMIDIATE_DIR)