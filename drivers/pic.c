#include "pic.h"
#include "../assembly_interface.h"
#include "../stdio.h"

#define PIC1    0x20    /* IO base address for master PIC */
#define PIC2    0xA0    /* IO base address for slave PIC */
#define PIC1_COMMAND  PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA (PIC2+1)
 
#define ICW1_ICW4 0x01    /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL  0x08    /* Level triggered (edge) mode */
#define ICW1_INIT 0x10    /* Initialization - required! */
 
#define ICW4_8086 0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02    /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C    /* Buffered mode/master */
#define ICW4_SFNM 0x10    /* Special fully nested (not) */

#define PIC_EOI   0x20    /* End-of-interrupt command code */
 
/*
arguments:
  offset1 - vector offset for master PIC
    vectors on the master become offset1..offset1+7
  offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
  unsigned char a1, a2;
 
  a1 = inb(PIC1_DATA);                        // save masks
  a2 = inb(PIC2_DATA);
 
  outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
  outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
  outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
  outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
  outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
  outb(PIC1_DATA, ICW4_8086);
  outb(PIC2_DATA, ICW4_8086);
 
  outb(PIC1_DATA, a1);   // restore saved masks.
  outb(PIC2_DATA, a2);
}

void IRQ_set_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void IRQ_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

void pic_init() {
  // PIC_remap(0x20, 0x28);
  // IRQ_set_mask(0); // disable timer
  // // IRQ_set_mask(1); // don't disable keyboard
  // IRQ_set_mask(3); // disable com2
  // IRQ_set_mask(4); // disable com1
  // IRQ_set_mask(5); // disable lpt2
  // IRQ_set_mask(6); // disable floppy disk
  // IRQ_set_mask(7); // disable lpt1
  // IRQ_set_mask(8); // disable real time clock
  // IRQ_set_mask(9); // disable gernal i/o
  // IRQ_set_mask(10); // disable gernal i/o
  // IRQ_set_mask(11); // disable gernal i/o
  // IRQ_set_mask(12); // disable gernal i/o
  // IRQ_set_mask(13); // disable coprocessor
  // IRQ_set_mask(14); // disable ide bus
  // IRQ_set_mask(15); // disable ide bus
  outb(PIC1_DATA, 0xFD); // Only enable keyboard
  outb(PIC2_DATA, 0xFF); // Don't enable any interrupts on slave pic
  enable_hardware_interrupts();
}

/** pic_acknowledge:
 *  Acknowledges an interrupt from either PIC 1 or PIC 2.
 *
 *  @param num The number of the interrupt
 */
void pic_acknowledge()
{
  log("pic_acknowledge()\n");
  outb(PIC1_DATA, PIC_EOI);
  outb(PIC2_DATA, PIC_EOI);
  log("ack sent\n");
}