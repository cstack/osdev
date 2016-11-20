#ifndef INCLUDE_ASSEMBLY_INTERFACE_H
#define INCLUDE_ASSEMBLY_INTERFACE_H

#include "types.h"

/** outb:
 *  Sends the given data to the given I/O port. Defined in io.s
 *
 *  @param port The I/O port to send the data to
 *  @param data The data to send to the I/O port
 */
void outb(unsigned short port, unsigned char data);

/** inb:
 *  Read a byte from an I/O port.
 *
 *  @param  port The address of the I/O port
 *  @return      The read byte
 */
unsigned char inb(unsigned short port);

/** asm_lgdt:
 *  Loads the global descriptor table
 *
 *  @param gdt The address of a gdt description structure
 *  http://wiki.osdev.org/Global_Descriptor_Table#Structure
 */
void asm_lgdt(void * gdt);

void tss_flush();

/** load_idt:
 *  Loads the interrupt descriptor table
 *
 *  @param idt The address of the first entry in the IDT
 */
void load_idt(void * idt);

/** interrupt:
 *  Generates a software interrupt
 */
void interrupt();
void interrupt_out_of_memory();

/** enable_hardware_interrupts:
 *  Sets the Interrupt bit in the FLAGS
 *  register, enabling maskable
 *  hardware interrupts
 */
void enable_hardware_interrupts();

void disable_hardware_interrupts();

/** set_page_directory:
 *  Sets control register 3 to the physical address of a page directory
 */
void set_page_directory();

/** enable_paging:
 *  Sets the paging bit on control register 0
 */
void enable_paging();

void* current_stack_pointer();

void enter_user_mode();

#endif /* INCLUDE_ASSEMBLY_INTERFACE_H */