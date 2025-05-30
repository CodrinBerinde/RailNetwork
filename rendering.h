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
#include "mzapo_parlcd.h"
#include "font_types.h"
#include "play.h"

#define PARLCD_WIDTH 480
#define PARLCD_HEIGHT 320

#define BW 1 //border width - in pixels
#define CW 24 // cell width - in pixels
#define CH 24 // cell height - in pixels
#define UB 6 // upper border - in pixels
#define LB 6 // left border - in pixels
#define RAIL_WIDTH 6

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CITY_R ((int)(0.5 * MIN(CW, CH)))
#define CITY_r ((int)(0.35 * MIN(CW, CH)))

#define UCA ((int)(0.35 * MIN(CW, CH)))
#define UCB ((int)(0.75 * UCA))
#define UCAB (UCA - UCB)
#define UCW 2
#define UC_COLOR 0x07e0//under construction color
#define RAIL_COLOR 0x001f

#define BORDER_COLOR 0xffff
#define SELECTED_COLOR 0xf908
#define CITY_SIZE_COLOR 0xffff
#define MENU_TEXT_COLOR 0xffff
#define MENU_TEXT_SIZE 2

#define FM_WIDTH 250
#define FM_HEIGHT 100
#define FM_COLOR 0xffe0
#define FM_TEXT_SIZE 3
#define FM_TEXT_COLOR 0x001f

#define MENU_WIDTH 250
#define MENU_ROW_HEIGHT 40
#define MENU_HEIGHT (4 * MENU_ROW_HEIGHT)
#define MENU_CORNER_J ((int)((PARLCD_WIDTH - MENU_WIDTH) / 2))
#define MENU_CORNER_I ((int)((PARLCD_HEIGHT - MENU_HEIGHT) / 2))

#define RLEDMAX 255 //maximum value for red component for the RGB led
#define GLEDMAX 255 //maximum value for green component for the RGB led

void put_buffer(buf_t *buf);

void init_rendering_constants();

buf_t *init_buffer();

void clear_buf(buf_t *buf, uint16_t color);

void write_final_message(buf_t *buf, int state);

void rectangle_buf(buf_t *buf, int x0, int y0, int x1, int y1, uint16_t color);

void char_buf(buf_t *buf, int i, int j, font_descriptor_t *font, int size, uint16_t color, char ch);

void draw_initial_board(buf_t *buf);

int distance(int i0, int j0, int i1, int j1, int result, int smaller);

void refresh_board(board_t *board, buf_t *buf, cell_t *selected, cell_t *under_constr, int points);

void show_menu(int menu_row, buf_t *buf);

void clear_mem_buffer(buf_t *buf);

int string_width(font_descriptor_t *font, char *str, int size);

void str_buf(buf_t *buf, int i, int j, font_descriptor_t *font, char *str, int size, uint16_t color);

void clear_display_data(buf_t *buf);

#endif