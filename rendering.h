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

#define PARLCD_WIDTH 480 //screen width (in pixels)
#define PARLCD_HEIGHT 320 //screen height (in pixels)

#define BW 1 //border width - in pixels
#define CW 24 // cell width - in pixels
#define CH 24 // cell height - in pixels
#define UB 6 // upper border - in pixels
#define LB 6 // left border - in pixels
#define RAIL_WIDTH 6 //the width of a rail segment - in pixels

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CITY_R ((int)(0.5 * MIN(CW, CH))) //the big radius of the ring that surounds a city connected by rail
#define CITY_r ((int)(0.35 * MIN(CW, CH))) //the small radius of the ring that surrounds a city connected by rail

#define UCA ((int)(0.35 * MIN(CW, CH))) //horizontal and vertical distance from the inner corner of a green "L" that is drawn into an "under construction" cell, to the edges of the cell
#define UCB ((int)(0.75 * UCA)) // size of one arm of one green "L" shape drawn into an "under construction" cell
#define UCAB (UCA - UCB)
#define UCW 2 //the width of one arm of that "L" shape
#define UC_COLOR 0x07e0 //under construction color of the "L" shapes
#define RAIL_COLOR 0x001f //color of the railway segments

#define BORDER_COLOR 0xffff //color of the borders of cells
#define SELECTED_COLOR 0xf908 //color of the selected cell
#define CITY_SIZE_COLOR 0xffff //color with which a city population is written on the lcd display
#define MENU_TEXT_COLOR 0xffff //color of the text in the menu
#define MENU_TEXT_SIZE 2 //size of the text in the menu

#define FM_WIDTH 250 //the final message (written after a game: "You won!"/"You lost!") width
#define FM_HEIGHT 100 //the final message height
#define FM_COLOR 0xffe0 //the color of the rectangle on which the final message is written
#define FM_TEXT_SIZE 3 //the text size of the final message on the screen
#define FM_TEXT_COLOR 0x001f //the text color of the final message

#define MENU_WIDTH 250 //the width of the main menu
#define MENU_ROW_HEIGHT 40 //the height of one row in the main menu
#define MENU_HEIGHT (4 * MENU_ROW_HEIGHT) //the total height of the main menu
#define MENU_CORNER_J ((int)((PARLCD_WIDTH - MENU_WIDTH) / 2)) //computation of the top-left corner i cordinate of the main menu
#define MENU_CORNER_I ((int)((PARLCD_HEIGHT - MENU_HEIGHT) / 2)) //computation of the top-left corner j cordinate of the main menu

#define RLEDMAX 255 //maximum value for red component for the RGB led
#define GLEDMAX 255 //maximum value for green component for the RGB led

//fuction that puts the buffer on the lcd display
void put_buffer(buf_t *buf);

//initialization of teh constants related to the display
void init_rendering_constants();

//initializes the buffer
buf_t *init_buffer();

//clears the buffer buf, applying on it the color color everywhere
void clear_buf(buf_t *buf, uint16_t color);

//function that writes the final message after a game is completed: "You won!" if state is 0, "You lost" - otherwise
void write_final_message(buf_t *buf, int state);

//function that displays a rectangle of color color in the buffer, defined by the corners (i0, j0), (i1, j1)
void rectangle_buf(buf_t *buf, int x0, int y0, int x1, int y1, uint16_t color);

//function that writes on the buffer buf, beginning with the top-left coordinate (i ,j), the character ch, with color color, having the font font and size size
void char_buf(buf_t *buf, int i, int j, font_descriptor_t *font, int size, uint16_t color, char ch);

//draws the empty board (only the borders of cells)
void draw_initial_board(buf_t *buf);

//returns 0 if the euclidean distance between points (i0, j0), (i1, j1) is smaller than result and "smaller" is 1, or if the distance is bigger but "smaller" is 0
//  and 0 otherwise
int distance(int i0, int j0, int i1, int j1, int result, int smaller);

//redraws on the buffer the appropriate information, according to the board
void refresh_board(board_t *board, buf_t *buf, cell_t *selected, cell_t *under_constr, int points);

//displays the main menu
void show_menu(int menu_row, buf_t *buf);

//clears the memory allocated for the buffer
void clear_mem_buffer(buf_t *buf);

//calculates the width in pixels, of a string str of size size, written with the font font
int string_width(font_descriptor_t *font, char *str, int size);

//displays the entire string str on the buffer buf, beginning with the top-left corner (i, j), with font font, size size and color color
void str_buf(buf_t *buf, int i, int j, font_descriptor_t *font, char *str, int size, uint16_t color);

//clears all the displayed data (lcd, leds - both strip and RGB)
void clear_display_data(buf_t *buf);

#endif