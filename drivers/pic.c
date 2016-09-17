#include "pic.h"
#include "../assembly_interface.h"

#define PIC1_PORT_A 0x20
#define PIC2_PORT_A 0xA0

/* The PIC interrupts have been remapped */
#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT   PIC2_START_INTERRUPT + 7

#define PIC_ACK     0x20

/** pic_acknowledge:
 *  Acknowledges an interrupt from either PIC 1 or PIC 2.
 *
 *  @param num The number of the interrupt
 */
void pic_acknowledge(uint32_t interrupt)
{
  if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT) {
    return;
  }

  if (interrupt < PIC2_START_INTERRUPT) {
    outb(PIC1_PORT_A, PIC_ACK);
  } else {
    outb(PIC2_PORT_A, PIC_ACK);
  }
}