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

#include "play.h"
#include "events.h"
#include "rendering.h"
#include "generator.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "mzapo_parlcd.h"

void *spiled_base;
unsigned char *parlcd_base;

void init_rendering_constants() {
  spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE , 0);
  parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE , 0);
}

buf_t *init_buffer() {
  uint16_t *buf_data = (uint16_t *)malloc(PARLCD_WIDTH * PARLCD_HEIGHT * 2);
  buf_t *buf = (buf_t *)malloc(sizeof(buf));
  buf->width = PARLCD_WIDTH;
  buf->height = PARLCD_HEIGHT;
  buf->data = buf_data;
  return buf;
}

void put_buffer(buf_t *buf)
{
  uint16_t* p = buf->data;

  parlcd_write_cmd(parlcd_base, 0x2c);

  for (int i = 0; i < buf->height * buf->width; i++)
    parlcd_write_data(parlcd_base, *(p++));
}

void fb_clear(int color) {

}

void fb_pixel(int x, int y, int color) {

}

void fb_rectangle(int x0, int y0, int w, int h, int color) {

}

int fb_char(int x0, int y0, font_descriptor_t *font, int size, int color, char ch) {

}

void fb_line(buf_t *buf, int dir, int x, int y1, int y2, uint16_t color) {
  if(dir == 0) { //the line is horizontal
    if(x < PARLCD_HEIGHT && y1 < PARLCD_WIDTH && y2 < PARLCD_WIDTH) {
      for(int i = x * PARLCD_WIDTH + y1; i <= x * PARLCD_WIDTH + y2; i++)
        buf->data[i] = color;
    }
  } else { //the line is vertical
    if(x < PARLCD_WIDTH && y1 < PARLCD_HEIGHT && y2 < PARLCD_HEIGHT) {
      for(int i = y1 * PARLCD_WIDTH + x; i <= y2 * PARLCD_WIDTH + x; i += PARLCD_WIDTH)
        buf->data[i] = color;
    }
  }
}

void draw(board_t *board, buf_t *buf) {
  //first we draw the horizontal lines
  for(int i = 0; i <= BOARD_HEIGHT; i++) {
    //fb_line(0, UB + i * (CH + BW), LB, 100, BORDER_COLOR);
    fb_line(buf, 0, UB + i * (CH + BW), LB, LB + BOARD_WIDTH * (CW + BW), BORDER_COLOR);
  }
  //fb_line(0, 0, 0, PARLCD_WIDTH - 1, BORDER_COLOR);

  //then we draw the vertical lines
  for(int i = 0; i <= BOARD_WIDTH; i++) {
    fb_line(buf, 1, LB + i * (CW + BW), UB, UB + BOARD_HEIGHT * (CH + BW), BORDER_COLOR);
  }

  put_buffer(buf);
}