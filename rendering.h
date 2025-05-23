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
#define CW 24 // cell width - in pixels
#define CH 24 // cell height - in pixels
#define UB 6 // upper border - in pixels
#define LB 6 // left border - in pixels

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define UCA (0.25 * MIN(CW, CH))
#define UCB (0.8 * UCA)
#define UCAB (UCA - UCB)
#define UCW 3
#define UC_COLOR 0x07e0//under construction color

#define BORDER_COLOR 0xffff
#define SELECTED_COLOR 0xf908
#define CITY_SIZE_COLOR 0xffff

typedef struct BUF {
  int width;
  int height;
  uint16_t *data;
} buf_t;

void put_buffer(buf_t *buf);

void init_rendering_constants();

buf_t *init_buffer();

void clear_buf(buf_t *buf, uint16_t color);

void rectangle_buf(buf_t *buf, int x0, int y0, int x1, int y1, uint16_t color);

void char_buf(buf_t *buf, int i, int j, font_descriptor_t *font, int size, uint16_t color, char ch);

void draw_initial_board(buf_t *buf);

void refresh_board(board_t *board, buf_t *buf, cell_t *selected, cell_t *under_constr);

void clear_mem_buffer(buf_t *buf);

#endif