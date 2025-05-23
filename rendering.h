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
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "mzapo_parlcd.h"
#include "font_types.h"

#define PARLCD_WIDTH 480
#define PARLCD_HEIGHT 320
#define BW 1 //border width - in pixels
#define CW 16 // cell width - in pixels
#define CH 16 // cell height - in pixels
#define UB 6 // upper border - in pixels
#define LB 6 // left border - in pixels

#define BORDER_COLOR 0xffff
#define SELECTED_COLOR 0xffe0

typedef struct BUF {
  int width;
  int height;
  uint16_t *data;
} buf_t;

void put_buffer(buf_t *buf);

void init_rendering_constants();

buf_t *init_buffer();

void clear_buf(buf_t *buf, uint16_t color);

void fb_pixel(int x, int y, int color);

void rectangle_buf(buf_t *buf, int x0, int y0, int x1, int y1, uint16_t color);

int fb_char(buf_t *buf, int x0, int y0, font_descriptor_t *font, int size, int color, char ch);

void fb_line(buf_t *buf, int dir, int x, int y1, int y2, uint16_t color);

void draw_initial_board(buf_t *buf);

void refresh_board(board_t *board, buf_t *buf, cell_t *selected, cell_t *under_constr);

void clear_mem_buffer(buf_t *buf);

#endif