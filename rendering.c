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
#include <time.h>

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
extern void *spiled_base;
font_descriptor_t *city_size_font, *menu_font, *fm_font;

void init_rendering_constants() {
  parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE , 0);
  spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE , 0);
  city_size_font = &font_rom8x16;
  menu_font = &font_rom8x16;
  fm_font = &font_rom8x16;
}

buf_t *init_buffer() {
  uint16_t *buf_data = (uint16_t *)malloc(PARLCD_WIDTH * PARLCD_HEIGHT * 2);
  buf_t *buf = (buf_t *)malloc(sizeof(buf));
  buf->width = PARLCD_WIDTH;
  buf->height = PARLCD_HEIGHT;
  buf->data = buf_data;
  return buf;
}

void show_menu(int menu_row, buf_t *buf) {
  clear_buf(buf, 0);
  rectangle_buf(buf, MENU_CORNER_I, MENU_CORNER_J, MENU_CORNER_I, MENU_CORNER_J + MENU_WIDTH, 0xffff);
  rectangle_buf(buf, MENU_CORNER_I, MENU_CORNER_J, MENU_CORNER_I + MENU_HEIGHT, MENU_CORNER_J, 0xffff);
  rectangle_buf(buf, MENU_CORNER_I + MENU_HEIGHT, MENU_CORNER_J, MENU_CORNER_I + MENU_HEIGHT, MENU_CORNER_J + MENU_WIDTH, 0xffff);
  rectangle_buf(buf, MENU_CORNER_I, MENU_CORNER_J + MENU_WIDTH, MENU_CORNER_I + MENU_HEIGHT, MENU_CORNER_J + MENU_WIDTH, 0xffff);

  rectangle_buf(buf, MENU_CORNER_I + MENU_ROW_HEIGHT, MENU_CORNER_J, MENU_CORNER_I + MENU_ROW_HEIGHT, MENU_CORNER_J + MENU_WIDTH, 0xffff);
  rectangle_buf(buf, MENU_CORNER_I + 2*MENU_ROW_HEIGHT, MENU_CORNER_J, MENU_CORNER_I + 2*MENU_ROW_HEIGHT, MENU_CORNER_J + MENU_WIDTH, 0xffff);
  rectangle_buf(buf, MENU_CORNER_I + 3*MENU_ROW_HEIGHT, MENU_CORNER_J, MENU_CORNER_I + 3*MENU_ROW_HEIGHT, MENU_CORNER_J + MENU_WIDTH, 0xffff);

  //draw the selected row
  rectangle_buf(buf, MENU_CORNER_I + menu_row * MENU_ROW_HEIGHT + 1, MENU_CORNER_J + 1, 
                MENU_CORNER_I + (menu_row + 1) * MENU_ROW_HEIGHT - 1, MENU_CORNER_J + MENU_WIDTH - 1, SELECTED_COLOR);

  //now we write the text on the menu
  char menu_options[4][20] = {"Difficulty 1", "Difficulty 2", "Difficulty 3", "Exit"};
  int i, j;
  for(int k = 0; k < 4; k++) {
    j = (PARLCD_WIDTH - (string_width(menu_font, menu_options[k], 2))) / 2;
    i = MENU_CORNER_I + k * MENU_ROW_HEIGHT + (MENU_ROW_HEIGHT - MENU_TEXT_SIZE * menu_font->height) / 2;
    str_buf(buf, i, j, menu_font, menu_options[k], MENU_TEXT_SIZE, MENU_TEXT_COLOR);
  }
}

void write_final_message(buf_t *buf, int state) {
  rectangle_buf(buf, (PARLCD_HEIGHT - FM_HEIGHT) / 2, (PARLCD_WIDTH - FM_WIDTH) / 2,
                     (PARLCD_HEIGHT + FM_HEIGHT) / 2, (PARLCD_WIDTH + FM_WIDTH) / 2, FM_COLOR);
  char msg[2][20] = {"You won!", "You lost!"};
  int i, j, index;
  i = (PARLCD_HEIGHT - FM_TEXT_SIZE * fm_font->height) / 2;
  if(state == 0)
    index = 0;
  else
    index = 1;
  j = (PARLCD_WIDTH - string_width(fm_font, msg[index], FM_TEXT_SIZE)) / 2;
  str_buf(buf, i, j, fm_font, msg[index], FM_TEXT_SIZE, FM_TEXT_COLOR);
  put_buffer(buf);
  usleep(2000000);
  clear_buf(buf, 0);
  put_buffer(buf);
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

void str_buf(buf_t *buf, int i, int j, font_descriptor_t *font, char *str, int size, uint16_t color) {
  char *p = str;
  while((*p) != '\0') {
    char_buf(buf, i, j, font, size, color, (*p));
    if(font->width == 0)
      j += size * font->maxwidth;
    else
      j += size * font->width[(*p)];
    p++;
  }
}

void char_buf(buf_t *buf, int i, int j, font_descriptor_t *font, int size, uint16_t color, char ch) {
  int width;
  if(font->width == 0)
    width = font->maxwidth;
  else
    width = font->width[ch];
  for(unsigned int line = 0; line < font->height; line++) {
    for(int pixel = 0; pixel < width; pixel++) {
      if(((font->bits[font->height * ch + line]) >> (15 - pixel)) & 1) {
        for(int repeat_col = 0; repeat_col < size; repeat_col++) {
          for(int repeat_line = 0; repeat_line < size; repeat_line++)
            buf->data[(i + size * line + repeat_line) * PARLCD_WIDTH + pixel * size + repeat_col + j] = color;
        }
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

int distance(int i0, int j0, int i1, int j1, int result, int smaller) {
  if((i0 - i1) * (i0 - i1) + (j0 - j1) * (j0 - j1) <= result * result) {
    if(smaller)
      return 1;
    return 0;
  }
  if(smaller)
    return 0;
  return 1;
}

int string_width(font_descriptor_t *font, char *str, int size) {
  int length = 0, res = 0;
  char *p = str;
  while((*p) != '\0') p++, length++;
  if(font->width == 0)
    return length * font->maxwidth * size;
  for(int i = 0; i < length; i++) {
    res += font->width[str[i]] * size;
  }
  return res;
}

void draw_rail(buf_t *buf, int i, int j, int dir, int is_city) {
  int temp;
  switch(dir) {
    case 0:
      temp = is_city ? (LB + BW + (BW + CW) * j + CW / 2 + CITY_r) : (LB + BW + (BW + CW) * j + (CW - RAIL_WIDTH) / 2);
      rectangle_buf(buf, UB + BW + (BW + CH) * i + (CH - RAIL_WIDTH) / 2, temp,
                          UB + BW + (BW + CH) * i + (CH + RAIL_WIDTH) / 2 - 1, LB + (BW + CW) * (j + 1) - 1, RAIL_COLOR);
      break;
    case 1:
      temp = is_city ? (UB + BW + (BW + CH) * i + CH / 2 + CITY_r) : (UB + BW + (BW + CH) * i + (CH - RAIL_WIDTH) / 2);
      rectangle_buf(buf, temp, LB + BW + (BW + CW) * j + (CW - RAIL_WIDTH) / 2,
                          UB + (BW + CH) * (i + 1) - 1, LB + BW + (BW + CW) * j + (CW + RAIL_WIDTH) / 2, RAIL_COLOR);
      break;
    case 2:
      temp = is_city ? (LB + BW + (BW + CW) * j + CW / 2 - CITY_r) : (LB + BW + (BW + CW) * j + (CW + RAIL_WIDTH) / 2);
      rectangle_buf(buf, UB + BW + (BW + CH) * i + (CH - RAIL_WIDTH) / 2, LB + BW + (BW + CW) * j,
                          UB + BW + (BW + CH) * i + (CH + RAIL_WIDTH) / 2 - 1, temp - 1, RAIL_COLOR);
      break;
    case 3:
      temp = is_city ? (UB + BW + (BW + CH) * i + CH / 2 - CITY_r) : (UB + BW + (BW + CH) * i + (CH + RAIL_WIDTH) / 2);
      rectangle_buf(buf, UB + BW + (BW + CH) * i, LB + BW + (BW + CW) * j + (CW - RAIL_WIDTH) / 2,
                          temp - 1, LB + BW + (BW + CW) * j + (CW + RAIL_WIDTH) / 2, RAIL_COLOR);
      break;
  }
  if(is_city) { //we draw the circle around the city, by testing each pixel inside the cell
    int i_middle = UB + BW + (BW + CH) * i + CH / 2 - 1, j_middle = LB + BW + (BW + CW) * j + CW / 2 - 1;
    for(int ii = UB + BW + (BW + CH) * i; ii < UB + BW + (BW + CH) * i + CH; ii++) {
      for(int jj = LB + BW + (BW + CW) * j; jj < LB + BW + (BW + CW) * j + CW; jj++) {
        if(distance(ii, jj, i_middle, j_middle, CITY_r, 0) && distance(ii, jj, i_middle, j_middle, CITY_R, 1))
          buf->data[ii * PARLCD_WIDTH + jj] = RAIL_COLOR;
      }
    }
  }
}

void refresh_board(board_t *board, buf_t *buf, cell_t *selected, cell_t *under_constr, int points) {
  clear_buf(buf, 0); //clear the entire buffer

  draw_initial_board(buf); //draw the cells

  rectangle_buf(buf, UB + BW + (CH + BW) * (selected->i), LB + BW + (CW + BW) * (selected->j),
                UB + BW + (CH + BW) * (selected->i) + CH - 1,
                LB + BW + (CW + BW) * (selected->j) + CW - 1, SELECTED_COLOR); //draw the selected cell

  //draw the rails
  for(int i = 0; i < BOARD_HEIGHT; i++) {
    for(int j = 0; j < BOARD_WIDTH; j++) {
      for(int dir = 0; dir < 4; dir++) {
        if((board->data[i * BOARD_WIDTH + j]) & (1 << (dir + 4)))
          draw_rail(buf, i, j, dir, (((board->data[i * BOARD_WIDTH + j]) & 15) != 0));
      }
    }
  }

  //draw the cities
  for(int i = 0; i < BOARD_HEIGHT; i++) {
    for(int j = 0; j < BOARD_WIDTH; j++) {
      if(((board->data[i * BOARD_WIDTH + j]) & 15) != 0) {
        int i_char, j_char; //the stating point of the printing of character
        int city_size = ((board->data[i * BOARD_WIDTH + j]) & 15);
        i_char = UB + BW + i * (CH + BW) + (CH - city_size_font->height) / 2;
        if(city_size < 10) { //then there is a single character to be printed
          j_char = LB + BW + j * (CW + BW) + (CW - city_size_font->maxwidth) / 2;
          char_buf(buf, i_char, j_char, city_size_font, 1, CITY_SIZE_COLOR, city_size + '0');
        } else { //then there are two characters to print
          j_char = LB + BW + j * (CW + BW) + (CW - 2 * city_size_font->maxwidth) / 2;
          char_buf(buf, i_char, j_char, city_size_font, 1, CITY_SIZE_COLOR, city_size/10 + '0');
          j_char += city_size_font->maxwidth;
          char_buf(buf, i_char, j_char, city_size_font, 1, CITY_SIZE_COLOR, city_size%10 + '0');
        }
      }
    }
  }

  //draw the under construction cell
  if(under_constr->i >= 0) {
    int i0, i1, j0, j1;
    i0 = UB + BW + under_constr->i * (CH + BW) + UCAB;
    i1 = i0 + UCB - 1;
    j0 = LB + BW + under_constr->j * (CW + BW) + UCA - UCW;
    j1 = j0 + UCW - 1;
    rectangle_buf(buf, i0, j0, i1, j1, UC_COLOR);

    i0 = i0 - UCAB + CH - UCA;
    i1 = i0 + UCB - 1;
    rectangle_buf(buf, i0, j0, i1, j1, UC_COLOR);

    j0 = j0 + UCW - UCA + CW - UCA;
    j1 = j0 + UCW - 1;
    rectangle_buf(buf, i0, j0, i1, j1, UC_COLOR);

    i0 = i0 + UCAB - CH + UCA;
    i1 = i0 + UCB - 1;
    rectangle_buf(buf, i0, j0, i1, j1, UC_COLOR);

    i0 = UB + BW + under_constr->i * (CH + BW) + UCA - UCW;
    i1 = i0 + UCW - 1;
    j0 = LB + BW + under_constr->j * (CW + BW) + UCAB;
    j1 = j0 + UCB - 1;
    rectangle_buf(buf, i0, j0, i1, j1, UC_COLOR);

    i0 = i0 + UCW - UCA + CH - UCA;
    i1 = i0 + UCW - 1;
    rectangle_buf(buf, i0, j0, i1, j1, UC_COLOR);

    j0 = j0 - UCAB + CW - UCA;
    j1 = j0 + UCB - 1;
    rectangle_buf(buf, i0, j0, i1, j1, UC_COLOR);

    i0 = i0 + UCA - CH + UCA - UCW;
    i1 = i0 + UCW - 1;
    rectangle_buf(buf, i0, j0, i1, j1, UC_COLOR);
    
  }

  //show the number of points on the LEDS
  int led_display = (1 << points) - 1;
  *(uint32_t *)(spiled_base + SPILED_REG_LED_LINE_o) = led_display;

  //show the status on the RGB led
  int red_component = RLEDMAX * MAX(0, (8 - points)) / 8;
  int green_component = GLEDMAX * MIN(8, points) / 8;
  int color_rgb_led = (red_component << 16) | (green_component << 8);
  *(uint32_t *)(spiled_base + SPILED_REG_LED_RGB1_o) = color_rgb_led;
}

void clear_mem_buffer(buf_t *buf) {
  free(buf);
}