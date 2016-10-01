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
drivers/frame_buffer.o \
drivers/keyboard.o \
drivers/pic.o \
drivers/serial_port.o \
interrupts.o \
kmain.o \
stdio.o

all: os.iso

%.o: %.c
	# Compile c files with gcc
	$(GCC) $(CFLAGS)  $< -o $@

%.o: %.s
	# assemble s files with nasm
	$(NASM) $(ASFLAGS) $< -o $@

kernel.elf: $(OBJECTS)
	$(LD) -T link.ld -melf_i386 $(OBJECTS) -o kernel.elf # Link to make an executable for the kernel.

os.iso: kernel.elf
	mkdir -p iso/boot/grub              # create the folder structure
	cp stage2_eltorito iso/boot/grub/   # copy the bootloader
	cp kernel.elf iso/boot/             # copy the kernel
	cp menu.lst iso/boot/grub           # copy the configuration file
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
	rm -f *.elf
	rm -f *.out
	rm -rf iso/
