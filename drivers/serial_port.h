#define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

void serial_init(unsigned short com);
int serial_write(unsigned short com, const char * s);