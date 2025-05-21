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
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "mzapo_parlcd.h"

void fb_draw(buf_t *fb, unsigned char *parlcd_base)
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

void fb_clear(buf_t *fb, int color) {

}

void fb_pixel(buf_t *fb, int x, int y, int color) {

}

void fb_rectangle(buf_t *fb, int x0, int y0, int w, int h, int color) {

}

int fb_char(buf_t *fb, int x0, int y0, font_descriptor_t *font, int size, int color, char ch) {

}

void fb_line(buf_t *fb, int dir, int x, int y1, int y2, uint16_t color) {
  if(dir == 0) { //the line is horizontal
    if(x < PARLCD_HEIGHT && y1 < PARLCD_WIDTH && y2 < PARLCD_WIDTH) {
      for(int i = x * PARLCD_WIDTH + y1; i <= x * PARLCD_WIDTH + y2; i++)
        fb->data[i] = color;
    }
  } else { //the line is vertical
    if(x < PARLCD_WIDTH && y1 < PARLCD_HEIGHT && y2 < PARLCD_HEIGHT) {
      for(int i = y1 * PARLCD_WIDTH + x; i <= y2 * PARLCD_WIDTH + x; i += PARLCD_WIDTH)
        fb->data[i] = color;
    }
  }
}

void fb_draw_empty_board(buf_t *fb) {
  //first we draw the horizontal lines
  for(int i = 0; i <= BOARD_HEIGHT; i++) {
    fb_line(fb, 0, UB + i * (CH + BW), LB, LB + BOARD_WIDTH * (CW + BW), 63);
  }

  //then we draw the vertical lines
  for(int i = 0; i <= BOARD_WIDTH; i++) {
    fb_line(fb, 1, LB + i * (CW + BW), UB, UB + BOARD_HEIGHT * (CH + BW), 63);
  }
}