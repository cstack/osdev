#define FB_COLS 80
#define FB_ROWS 25
#define FB_CELLS FB_COLS * FB_ROWS

#define FB_BLACK 0
#define FB_BLUE 1
#define FB_GREEN 2
#define FB_CYAN 3
#define FB_RED 4
#define FB_MAGENTA 5
#define FB_BROWN 6
#define FB_LIGHT_GREY 7
#define FB_DARK_GREY 8
#define FB_LIGHT_BLUE 9
#define FB_LIGHT_GREEN 10
#define FB_LIGHT_CYAN 11
#define FB_LIGHT_RED 12
#define FB_LIGHT_MAGENTA 13
#define FB_LIGHT_BROWN 14
#define FB_WHITE 15

// Start of memory that maps to the fram buffer
char *fb = (char *) 0x000B8000;

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
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

void fb_clear_screen()
{
  for (int i = 0; i < FB_CELLS; i++) {
    fb_write_cell(i, ' ', FB_BLACK, FB_BLACK);
  }
}

void kmain() {
  fb_clear_screen();
  fb_write_cell(0, 'H', FB_GREEN, FB_DARK_GREY);
  fb_write_cell(1, 'E', FB_GREEN, FB_DARK_GREY);
  fb_write_cell(2, 'L', FB_GREEN, FB_DARK_GREY);
  fb_write_cell(3, 'L', FB_GREEN, FB_DARK_GREY);
  fb_write_cell(4, 'O', FB_GREEN, FB_DARK_GREY);

  while (1) {}
}