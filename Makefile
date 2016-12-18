# This file makes use of automatic variables
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html#Automatic-Variables

# https://en.wikipedia.org/wiki/Netwide_Assembler
NASM=nasm

# https://en.wikipedia.org/wiki/GNU_linker
LD=~/.local/binutils/bin/i386-unknown-linux-gnu-ld

# https://en.wikipedia.org/wiki/QEMU
# Virtual machine
QEMU=qemu-system-i386

GCC=gcc

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -Wall -Wextra -Werror -c --target=x86_64-elf

ASFLAGS = -f elf

OBJECTS = loader.o \
asm_interrupts.o \
assembly_interface.o \
data_structures/global_descriptor_table.o \
data_structures/interrupt_descriptor_table.o \
data_structures/page_table.o \
data_structures/symbol_table.o \
drivers/frame_buffer.o \
drivers/keyboard.o \
drivers/pic.o \
drivers/serial_port.o \
interrupts.o \
kernel_filesystem.o \
kernel_stdio.o \
kernel_syscalls.o \
kmain.o \
multiboot_utils.o \
process.o \
stdlib.o

STDLIB = stdlib/stdio.o \
stdlib/string.o \
stdlib/syscalls.o

all: os.iso

%.o: %.c
	# Compile c files with gcc
	$(GCC) $(CFLAGS) $< -o $@

%.o: %.s
	# assemble s files with nasm
	$(NASM) $(ASFLAGS) $< -o $@

kernel.elf: $(OBJECTS) $(STDLIB)
	$(LD) -T link.ld -melf_i386 $^ -o kernel.elf # Link to make an executable for the kernel.

start_user_program.o: start_user_program.s
	$(NASM) $(ASFLAGS) $< -o $@

shell.o: shell.c
	$(GCC) $(CFLAGS) $< -o $@

shell.bin: shell.o start_user_program.o $(STDLIB)
	$(LD) -T link_user_program.ld -melf_i386 $^ -o $@

built_file_system: file_system_root/* shell.bin
	script/build_file_system

os.iso: kernel.elf shell.bin menu.lst built_file_system
	mkdir -p iso/boot/grub              # create the folder structure
	mkdir -p iso/modules
	cp stage2_eltorito iso/boot/grub/   # copy the bootloader
	cp kernel.elf iso/boot/             # copy the kernel
	cp built_file_system iso/modules	  # copy the 'user' program
	cp menu.lst iso/boot/grub           # copy the grub configuration file
	mkisofs -R                              \
          -b boot/grub/stage2_eltorito    \
          -no-emul-boot                   \
          -boot-load-size 4               \
          -A os                           \
          -input-charset utf8             \
          -quiet                          \
          -boot-info-table                \
          -o os.iso                       \
          iso

run: os.iso
	# view contents of register with `p $$eax`
	$(QEMU) -monitor stdio -cdrom $< -serial file:log/log.txt

clean:
	rm -f *.iso
	rm -f $(OBJECTS)
	rm -f *.elf # elf executables
	rm -f *.bin # flat binary executables
	rm -f *.out
	rm -rf iso/
	rm -f built_file_system
