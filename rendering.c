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
#include <time.h>
#include <unistd.h>

#include "play.h"
#include "rendering.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "mzapo_parlcd.h"
#include "font_types.h"

unsigned char *parlcd_base;
extern void *spiled_base;
font_descriptor_t *city_size_font, *menu_font, *fm_font;

void init_rendering_constants() {
  parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE , 0);
  spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE , 0);
  city_size_font = &font_rom8x16; //the font with which the sity population is written
  menu_font = &font_rom8x16; //the font with which the main menu text is written
  fm_font = &font_rom8x16; //the font with which the final message text is written
}

buf_t *init_buffer() {
  uint16_t *buf_data = (uint16_t *)malloc(PARLCD_WIDTH * PARLCD_HEIGHT * 2); //alocates memory for the buffer data
  buf_t *buf = (buf_t *)malloc(sizeof(buf)); //and for the buffer itself
  buf->width = PARLCD_WIDTH;
  buf->height = PARLCD_HEIGHT;
  buf->data = buf_data;
  return buf;
}

void show_menu(int menu_row, buf_t *buf) {
  clear_buf(buf, 0); //first clears the buffer and puts black
  //we draw the four borders of the main menu
  rectangle_buf(buf, MENU_CORNER_I, MENU_CORNER_J, MENU_CORNER_I, MENU_CORNER_J + MENU_WIDTH, 0xffff);
  rectangle_buf(buf, MENU_CORNER_I, MENU_CORNER_J, MENU_CORNER_I + MENU_HEIGHT, MENU_CORNER_J, 0xffff);
  rectangle_buf(buf, MENU_CORNER_I + MENU_HEIGHT, MENU_CORNER_J, MENU_CORNER_I + MENU_HEIGHT, MENU_CORNER_J + MENU_WIDTH, 0xffff);
  rectangle_buf(buf, MENU_CORNER_I, MENU_CORNER_J + MENU_WIDTH, MENU_CORNER_I + MENU_HEIGHT, MENU_CORNER_J + MENU_WIDTH, 0xffff);

  //we draw the lines between the rows of the menu
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

  //we write the title and author in the top-left corner
  char title[20] = "RailNetwork game", author[50] = "designed and developed by Codrin Berinde", place_date[30] = "Prague, may 2025",
        copyright[70] = "student, Technical University of Cluj-Napoca, Romania";
  str_buf(buf, 0, 0, menu_font, title, 1, 0xffff);
  str_buf(buf, menu_font->height, 0, menu_font, author, 1, 0xffff);
  str_buf(buf, 2 * menu_font->height, 0, menu_font, copyright, 1, 0xffff);
  str_buf(buf, PARLCD_HEIGHT - menu_font->height - 1, PARLCD_WIDTH - string_width(menu_font, place_date, 1) - 1, menu_font, place_date, 1, 0xffff);
}

void write_final_message(buf_t *buf, int state) {
  //first we draw the coloured rectangle on which the text will be written
  rectangle_buf(buf, (PARLCD_HEIGHT - FM_HEIGHT) / 2, (PARLCD_WIDTH - FM_WIDTH) / 2,
                     (PARLCD_HEIGHT + FM_HEIGHT) / 2, (PARLCD_WIDTH + FM_WIDTH) / 2, FM_COLOR);
  char msg[2][20] = {"You won!", "You lost!"}; //defining the messages to display
  int i, j, index;
  i = (PARLCD_HEIGHT - FM_TEXT_SIZE * fm_font->height) / 2; //calculate the i coordinate from which th text must begin
  if(state == 0) //choose which of the texts to display
    index = 0;
  else
    index = 1;
  j = (PARLCD_WIDTH - string_width(fm_font, msg[index], FM_TEXT_SIZE)) / 2; //calculate the j coordinate from which the text must begin
  str_buf(buf, i, j, fm_font, msg[index], FM_TEXT_SIZE, FM_TEXT_COLOR); //write the string on the buffer
  put_buffer(buf); //put the buffer on the lcd display
  usleep(2000000); //and wait for 2 seconds
  clear_buf(buf, 0);
  put_buffer(buf);
}

void put_buffer(buf_t *buf)
{
  uint16_t* p = buf->data;

  parlcd_write_cmd(parlcd_base, 0x2c);

  for (int i = 0; i < buf->height * buf->width; i++)
    parlcd_write_data(parlcd_base, *(p++));//write pixel by pixel the data on the lcd display
}

void clear_buf(buf_t *buf, uint16_t color) {
  for(int i = 0; i < PARLCD_HEIGHT; i++) {
    for(int j = 0; j < PARLCD_WIDTH; j++) {
      buf->data[i * PARLCD_WIDTH + j] = color; //apply color on every pixel of the buffer
    }
  }
}

void rectangle_buf(buf_t *buf, int i0, int j0, int i1, int j1, uint16_t color) {
  //check whether the rectangle fits within the screen first
  if(i0 < 0 || i1 < 0 || j0 < 0 || j1 < 0 || i0 >= PARLCD_HEIGHT || i1 >= PARLCD_HEIGHT || j0 >= PARLCD_WIDTH || j1 >= PARLCD_WIDTH)
    return;
  for(int i = i0; i <= i1; i++) {
    for(int j = j0; j <= j1; j++) {
      buf->data[i * PARLCD_WIDTH + j] = color; //if so, apply color on every pixel between the desired coordinates
    }
  }
}

void str_buf(buf_t *buf, int i, int j, font_descriptor_t *font, char *str, int size, uint16_t color) {
  char *p = str;
  while((*p) != '\0') { //while the string to display still has characters in it
    char_buf(buf, i, j, font, size, color, (*p)); //put the character on the buffer
    if(font->width == 0)//if the font is monospace
      j += size * font->maxwidth; //advance on the screen with the width of one character times the size
    else
      j += size * font->width[(int)(*p)]; //advance on the screen with the width of the last character we printed times the size
    p++;//advance to the next character into the string
  }
}

void char_buf(buf_t *buf, int i, int j, font_descriptor_t *font, int size, uint16_t color, char ch) {
  int width;
  if(font->width == 0)//if the font is monospace
    width = font->maxwidth;//the width of the character is constant
  else
    width = font->width[(int)ch];//otherwise get the width of this character
  for(unsigned int line = 0; line < font->height; line++) {//for each line of the defined character
    for(int pixel = 0; pixel < width; pixel++) {//for each pixel in a row of a character
      if(((font->bits[font->height * ch + line]) >> (15 - pixel)) & 1) { //if that pixel is 1 (we must print data on the buffer)
        for(int repeat_col = 0; repeat_col < size; repeat_col++) { //repeat the printing as many times as size
          for(int repeat_line = 0; repeat_line < size; repeat_line++) //also repeat the line on the buffer as many times as size
            buf->data[(i + size * line + repeat_line) * PARLCD_WIDTH + pixel * size + repeat_col + j] = color; //print on the buffer the color of the text
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
  if((i0 - i1) * (i0 - i1) + (j0 - j1) * (j0 - j1) <= result * result) { //if the distance is smaller
    if(smaller) //if smaller was also requires
      return 1;
    return 0;
  } //but if the distance is bigger
  if(smaller) //and smaller was required
    return 0;
  return 1;
}

int string_width(font_descriptor_t *font, char *str, int size) {
  int length = 0, res = 0;
  char *p = str; //go to the first character into the string
  while((*p) != '\0') p++, length++; //go through all the characters to calculate the length of the string
  if(font->width == 0) //if the font is monospace
    return length * font->maxwidth * size; //just return the length of the string times the standard width of the font times the size
  for(int i = 0; i < length; i++) { //otherwise run through all the characters
    res += font->width[(int)str[i]] * size; //and add to the sum the width of the current character times the size
  }
  return res;
}

void draw_rail(buf_t *buf, int i, int j, int dir, int is_city) {
  int temp; //temporary variable needed to store some final/initial coordinate, because it depends on whether in a cell there is city or not
  switch(dir) {
    case 0: //if the direction of the rail is to the right
      //if there is city in this cell, then the rail segment only starts more from the left (such that the segment does not overlap with the text)
      temp = is_city ? (LB + BW + (BW + CW) * j + CW / 2 + CITY_r) : (LB + BW + (BW + CW) * j + (CW - RAIL_WIDTH) / 2);
      rectangle_buf(buf, UB + BW + (BW + CH) * i + (CH - RAIL_WIDTH) / 2, temp,
                          UB + BW + (BW + CH) * i + (CH + RAIL_WIDTH) / 2 - 1, LB + (BW + CW) * (j + 1) - 1, RAIL_COLOR); //draw the rectangle for the segment
      break;
    case 1: //direction down
      temp = is_city ? (UB + BW + (BW + CH) * i + CH / 2 + CITY_r) : (UB + BW + (BW + CH) * i + (CH - RAIL_WIDTH) / 2);
      rectangle_buf(buf, temp, LB + BW + (BW + CW) * j + (CW - RAIL_WIDTH) / 2,
                          UB + (BW + CH) * (i + 1) - 1, LB + BW + (BW + CW) * j + (CW + RAIL_WIDTH) / 2, RAIL_COLOR);
      break;
    case 2: //direction left
      temp = is_city ? (LB + BW + (BW + CW) * j + CW / 2 - CITY_r) : (LB + BW + (BW + CW) * j + (CW + RAIL_WIDTH) / 2);
      rectangle_buf(buf, UB + BW + (BW + CH) * i + (CH - RAIL_WIDTH) / 2, LB + BW + (BW + CW) * j,
                          UB + BW + (BW + CH) * i + (CH + RAIL_WIDTH) / 2 - 1, temp - 1, RAIL_COLOR);
      break;
    case 3: //direction up
      temp = is_city ? (UB + BW + (BW + CH) * i + CH / 2 - CITY_r) : (UB + BW + (BW + CH) * i + (CH + RAIL_WIDTH) / 2);
      rectangle_buf(buf, UB + BW + (BW + CH) * i, LB + BW + (BW + CW) * j + (CW - RAIL_WIDTH) / 2,
                          temp - 1, LB + BW + (BW + CW) * j + (CW + RAIL_WIDTH) / 2, RAIL_COLOR);
      break;
  }
  if(is_city) { //if there is a city in this cell, we draw the ring around the city, by testing each pixel inside the cell
    int i_middle = UB + BW + (BW + CH) * i + CH / 2 - 1, j_middle = LB + BW + (BW + CW) * j + CW / 2 - 1; //the middle of the cell
    for(int ii = UB + BW + (BW + CH) * i; ii < UB + BW + (BW + CH) * i + CH; ii++) { //each pixel i coordinate in the cell
      for(int jj = LB + BW + (BW + CW) * j; jj < LB + BW + (BW + CW) * j + CW; jj++) { //each pixel y coordinate in the cell
        if(distance(ii, jj, i_middle, j_middle, CITY_r, 0) && distance(ii, jj, i_middle, j_middle, CITY_R, 1)) //if the distance to the middle is less than the big radius and more than the small one
          buf->data[ii * PARLCD_WIDTH + jj] = RAIL_COLOR; //we draw the ring color on the buffer for that pixel
      }
    }
  }
}

void refresh_board(board_t *board, buf_t *buf, cell_t *selected, cell_t *under_constr, int points) {
  clear_buf(buf, 0); //clear the entire buffer

  draw_initial_board(buf); //draw the cells borders

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
      if(((board->data[i * BOARD_WIDTH + j]) & 15) != 0) { //if there is city in this cell (nonzero population)
        int i_char, j_char; //the stating point of the printing of character
        int city_size = ((board->data[i * BOARD_WIDTH + j]) & 15); //compute the city size
        i_char = UB + BW + i * (CH + BW) + (CH - city_size_font->height) / 2; //compute the i coordinate of the text that writes the population
        if(city_size < 10) { //then there is a single character to be printed
          j_char = LB + BW + j * (CW + BW) + (CW - city_size_font->maxwidth) / 2; //compute the j coordinate of the text
          char_buf(buf, i_char, j_char, city_size_font, 1, CITY_SIZE_COLOR, city_size + '0'); //put the char corresponding to the city size
        } else { //then there are two characters to print
          j_char = LB + BW + j * (CW + BW) + (CW - 2 * city_size_font->maxwidth) / 2; //compute the j coordinate of the text
          char_buf(buf, i_char, j_char, city_size_font, 1, CITY_SIZE_COLOR, city_size/10 + '0');
          j_char += city_size_font->maxwidth; //advance with the j coordinate
          char_buf(buf, i_char, j_char, city_size_font, 1, CITY_SIZE_COLOR, city_size%10 + '0');
        }
      }
    }
  }

  //draw the under construction cell (by computing the coordinates of all 8 rectangles that form the 4 "L" shapes and then draw each one)
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
  int cap_points = MIN(points, 32); //cannot display more than 32 points on the led strip
  int led_display = (1 << cap_points) - 1; //display as many leds on the strip (1s in binary representation) as many points the player has
  *(uint32_t *)(spiled_base + SPILED_REG_LED_LINE_o) = led_display; //sen

  //show the status on the RGB led
  int red_component = RLEDMAX * MAX(0, MIN(1, (16.0 - points) / 13.0)); //linear formula such that for 3 or below 3 points there is full red, and above 16 points no red
  int green_component = GLEDMAX * MAX(0, MIN(1, (points - 3.0) / 13.0)); //linear formula s.t. for 16 or above 16 points it is full green, and below 3 there is no green
  int color_rgb_led = (red_component << 16) | (green_component << 8); //form the color number, accounting for both green and red components
  *(uint32_t *)(spiled_base + SPILED_REG_LED_RGB2_o) = color_rgb_led; //send the variable to the peripheral corresponding to the RGB led
}

void clear_display_data(buf_t *buf) {
  clear_buf(buf, 0);
  put_buffer(buf);
  *(uint32_t *)(spiled_base + SPILED_REG_LED_LINE_o) = 0;
  *(uint32_t *)(spiled_base + SPILED_REG_LED_RGB2_o) = 0;
}

void clear_mem_buffer(buf_t *buf) {
  free(buf);
}