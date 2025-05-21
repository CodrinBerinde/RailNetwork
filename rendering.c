/*******************************************************************
  rendering.c    - contains the functions which relate to the output peripherals
                of the board: LCD display and the 32 LEDs

  Berinde Codrin
  berinde.gh.codrin@student.utcluj.ro
  Technical University of Cluj-Napoca, Romania

  developed at the Czech Technical University in Prague
  Prague, Czech Republic - may 2025

 *******************************************************************/

#include <stdlib.h>

#include "rendering.h"

void fb_draw(fbuf_t *fb, unsigned char *parlcd_base)
{
  uint16_t* p = fb->data;
#ifndef QTRVSIM
  parlcd_write_cmd(parlcd_base, 0x2c);

  for (int i = 0; i < fb->height * fb->width; i++)
    parlcd_write_data(parlcd_base, *(p++));
#else
  memcpy(parlcd_base, fb->data, fb->height * fb->width * 2);
#endif
}

void fb_clear(fbuf_t *fb, int color) {

}

void fb_pixel(fbuf_t *fb, int x, int y, int color) {

}

void fb_rectangle(fbuf_t *fb, int x0, int y0, int w, int h, int color) {

}

int fb_char(fbuf_t *fb, int x0, int y0, font_descriptor_t *font, int size, int color, char ch) {

}