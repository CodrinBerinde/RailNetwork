/*******************************************************************
  rendering.h    - contains the functions which relate to the output peripherals
                of the board: LCD display and the 32 LEDs

  Berinde Codrin
  berinde.gh.codrin@student.utcluj.ro
  Technical University of Cluj-Napoca, Romania

  developed at the Czech Technical University in Prague
  Prague, Czech Republic - may 2025

 *******************************************************************/

#ifndef RENDERING_H
#define RENDERING_H

#include <stdlib.h>
#include "font_types.h"

#define PARLCD_WIDTH 480
#define PARLCD_HEIGHT 320
#define BW 1 //border width - in pixels
#define CW 16 // cell width - in pixels
#define CH 16 // cell height - in pixels
#define UB 6 // upper border - in pixels
#define LB 6 // left border - in pixels

typedef struct BUF {
  int width;
  int height;
  uint16_t *data;
} buf;

uint16_t buf0_data[PARLCD_WIDTH * PARLCD_HEIGHT];

buf buf0 = {
  .width = PARLCD_WIDTH,
  .height = PARLCD_HEIGHT,
  .data = buf0_data,
};

void fb_draw(buf *fb, unsigned char *parlcd_base);

void fb_clear(buf *fb, int color);

void fb_pixel(buf *fb, int x, int y, int color);

void fb_rectangle(buf *fb, int x0, int y0, int x1, int y1, int color);

int fb_char(buf *fb, int x0, int y0, font_descriptor_t *font, int size, int color, char ch);

#endif