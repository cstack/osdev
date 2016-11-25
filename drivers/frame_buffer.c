// Documentation:
// http://wiki.osdev.org/Printing_To_Screen

#include "frame_buffer.h"
#include "../assembly_interface.h"
#include "../stdio.h"

/* The I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

// Start of memory that maps to the frame buffer
char *fb = (char *) 0xC00B8000;

uint16_t cursor_pos = 0;

uint8_t buffer[FB_CELLS*2];

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(unsigned int cell, char c, unsigned char fg, unsigned char bg)
{
    int i = cell*2;
    buffer[i] = fb[i] = c;

    buffer[i+1] = fb[i + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

void scroll()
{
  uint32_t num_cells_to_copy = FB_CELLS - FB_COLS;
  for (uint32_t cell = 0; cell < num_cells_to_copy; cell++) {
    int i = cell*2;
    buffer[i] = fb[i] = buffer[i+FB_COLS*2];
    buffer[i+1] = fb[i+1] = buffer[i+1+FB_COLS*2];
  }
  uint32_t bytes_per_row = FB_COLS*2;
  uint32_t last_byte_index = FB_CELLS*2 - 1;
  for (uint32_t i = 0; i < bytes_per_row; i++) {
    buffer[last_byte_index - i] = fb[last_byte_index - i] = 0;
  }
}

void clear_screen()
{
  for (int i = 0; i < FB_CELLS; i++) {
    fb_write_cell(i, ' ', FB_BLACK, FB_BLACK);
  }
}

/** move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void move_cursor_to_pos(unsigned short pos)
{
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  outb(FB_DATA_PORT,    pos & 0x00FF);
}

uint16_t beginning_of_line(uint16_t pos) {
  return pos / FB_COLS * FB_COLS;
}

uint16_t beginning_of_next_line(uint16_t pos) {
  return beginning_of_line(pos + FB_COLS);
}

void fb_write_byte(uint8_t b) {
  if (b == '\n') {
    cursor_pos = beginning_of_next_line(cursor_pos);
    while (cursor_pos >= FB_CELLS) {
      scroll();
      cursor_pos -= FB_COLS;
    }
    move_cursor_to_pos(cursor_pos);
  } else {
    fb_write_cell(cursor_pos, b, FB_WHITE, FB_BLACK);
    cursor_pos++;
  }
}

void fb_backspace() {
  cursor_pos--;
  fb_write_cell(cursor_pos, ' ', FB_WHITE, FB_BLACK);
  move_cursor_to_pos(cursor_pos);
}
