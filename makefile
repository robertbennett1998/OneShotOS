SOURCE_DIR = src
_SOURCE_DIR = src
INTIMIDIATE_DIR = intimidiate
_INTIMIDIATE_DIR = ..
BIN32_DIR = bin32
_BIN32_DIR = ..
i686_ELF_BIN_DIR = $(BIN32_DIR)/i686-elf
_i686_ELF_BIN_DIR = $(_BIN32_DIR)/..
OUT_FILE_NAME = OneShotOS

all : make_dirs build-32

build-32 : i686-elf-link-32

i686-elf-compile-32 :
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

i686-elf-link-32 : i686-elf-compile-32
	$(eval o_files = $(shell find $(INTIMIDIATE_DIR)/ -maxdepth 1 -type f -regex ".*\.o"))
	$(eval o_files = $(addprefix $(_i686_ELF_BIN_DIR)/, $(o_files)))
	cd $(i686_ELF_BIN_DIR);\
	i686-elf-gcc -T $(_i686_ELF_BIN_DIR)/linker.ld -o $(OUT_FILE_NAME)-i686-elf.bin -ffreestanding -O2 -nostdlib $(o_files) -lgcc
	rm -rf $(INTIMIDIATE_DIR)

clean :
	rm -rf $(BIN32_DIR)
	rm -rf $(INTIMIDIATE_DIR)

make_dirs :
	mkdir -p $(i686_ELF_BIN_DIR)
	mkdir -p $(INTIMIDIATE_DIR)
