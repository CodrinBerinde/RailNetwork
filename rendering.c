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
#include <stdio.h>

#include "play.h"
#include "events.h"
#include "rendering.h"
#include "generator.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "mzapo_parlcd.h"
#include "font_types.h"

unsigned char *parlcd_base;
font_descriptor_t *city_size_font;

void init_rendering_constants() {
  parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE , 0);
  city_size_font = &font_rom8x16;
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

void clear_buf(buf_t *buf, uint16_t color) {
  for(int i = 0; i < PARLCD_HEIGHT; i++) {
    for(int j = 0; j < PARLCD_WIDTH; j++) {
      buf->data[i * PARLCD_WIDTH + j] = color;
    }
  }
}

void rectangle_buf(buf_t *buf, int i0, int j0, int i1, int j1, uint16_t color) {
  if(i0 < 0 || i1 < 0 || j0 < 0 || j1 < 0 || i0 >= PARLCD_HEIGHT || i1 >= PARLCD_HEIGHT || j0 >= PARLCD_WIDTH || j1 >= PARLCD_WIDTH)
    return;
  for(int i = i0; i <= i1; i++) {
    for(int j = j0; j <= j1; j++) {
      buf->data[i * PARLCD_WIDTH + j] = color;
    }
  }
}

void char_buf(buf_t *buf, int i, int j, font_descriptor_t *font, int size, uint16_t color, char ch) {
  for(unsigned int line = 0; line < font->height; line++) {
    //printf("tiparesc a %d-a valoare: %d\n", font->height * ch + line, font->bits[font->height * ch + line]);
    for(int pixel = 0; pixel < font->maxwidth; pixel++) {
      if(((font->bits[font->height * ch + line]) >> (15 - pixel)) & 1) {
        buf->data[(i + line) * PARLCD_WIDTH + pixel + j] = color;
      }
    }
  }
}

void draw_initial_board(buf_t *buf) {
  //first we draw the horizontal lines
  for(int i = 0; i <= BOARD_HEIGHT; i++) {
    rectangle_buf(buf, UB + i * (CH + BW), LB, UB + i * (CH + BW) + BW - 1, LB + BOARD_WIDTH * (CW + BW), BORDER_COLOR);
  }

  //then we draw the vertical lines
  for(int i = 0; i <= BOARD_WIDTH; i++) {
    rectangle_buf(buf, UB, LB + i * (CW + BW), UB + BOARD_HEIGHT * (CW + BW), LB + i * (CW + BW) + BW - 1, BORDER_COLOR);
  }
}

void refresh_board(board_t *board, buf_t *buf, cell_t *selected, cell_t *under_constr) {
  clear_buf(buf, 0); //clear the entire buffer

  draw_initial_board(buf); //draw the cells

  rectangle_buf(buf, UB + BW + (CH + BW) * (selected->i), LB + BW + (CW + BW) * (selected->j),
                UB + BW + (CH + BW) * (selected->i) + CH - 1,
                LB + BW + (CW + BW) * (selected->j) + CW - 1, SELECTED_COLOR); //draw the selected cell

  //draw the cities
  for(int i = 0; i < BOARD_HEIGHT; i++) {
    for(int j = 0; j < BOARD_WIDTH; j++) {
      if(((board->data[i * BOARD_WIDTH + j]) & 15) != 0) {
        int i_char, j_char; //the stating point of the printing of character
        int city_size = ((board->data[i * BOARD_WIDTH + j]) & 15);
        i_char = UB + BW + i * (CH + BW) + (CH - city_size_font->height) / 2;
        if(city_size < 10) { //then there is a single character to be printed
          j_char = LB + BW + j * (CW + BW) + (CW - city_size_font->maxwidth) / 2;
          char_buf(buf, i_char, j_char, city_size_font, 0, CITY_SIZE_COLOR, city_size + '0');
        } else { //then there are two characters to print
          j_char = LB + BW + j * (CW + BW) + (CW - 2 * city_size_font->maxwidth) / 2;
          char_buf(buf, i_char, j_char, city_size_font, 0, CITY_SIZE_COLOR, city_size/10 + '0');
          j_char += city_size_font->maxwidth;
          char_buf(buf, i_char, j_char, city_size_font, 0, CITY_SIZE_COLOR, city_size%10 + '0');
        }
      }
    }
  }

  //draw the under construction cell
  if(under_constr->i >= 0) {
    int i0, i1, j0, j1;
    i0 = UB + BW + under_constr->i * (CH + BW) + UCA - UCB;
    i1 = i0 + UCB;
    j0 = LB + BW + under_constr->j * (CW + BW) + UCA - UCW;
    j1 = j0 + UCW;
    rectangle_buf(buf, i0, j0, i1, j1, UC_COLOR);
  }
}

void clear_mem_buffer(buf_t *buf) {
  free(buf);
}