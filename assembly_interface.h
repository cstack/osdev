#ifndef INCLUDE_ASSEMBLY_INTERFACE_H
#define INCLUDE_ASSEMBLY_INTERFACE_H

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

/** lgdt:
 *  Loads the global descriptor table
 *
 *  @param gdt The address of a gdt description structure
 *  http://wiki.osdev.org/Global_Descriptor_Table#Structure
 */
void lgdt(void * gdt);

/** load_idt:
 *  Loads the interrupt descriptor table
 *
 *  @param idt The address of the first entry in the IDT
 */
void load_idt(void * idt);

#endif /* INCLUDE_ASSEMBLY_INTERFACE_H */