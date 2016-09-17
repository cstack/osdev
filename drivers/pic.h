#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H

#include "../types.h"

/** pic_acknowledge:
 *  Acknowledges an interrupt from either PIC 1 or PIC 2.
 *
 *  @param num The number of the interrupt
 */
void pic_acknowledge(uint32_t interrupt);

#endif /* INCLUDE_PIC_H */