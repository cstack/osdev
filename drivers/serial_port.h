#define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

void serial_init(unsigned short com);
int serial_write(unsigned short com, const char * s);
void serial_write_byte(unsigned short com, char c);
void serial_write_bytes(unsigned short com, char * c, int n);