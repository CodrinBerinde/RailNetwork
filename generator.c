/*******************************************************************
  generation.h    - contains the functions which relate to the
                    generation of the initial board

  Berinde Codrin
  berinde.gh.codrin@student.utcluj.ro
  Technical University of Cluj-Napoca, Romania

  developed at the Czech Technical University in Prague
  Prague, Czech Republic - may 2025

 *******************************************************************/

#include <stdlib.h>
#include <time.h>

#include "play.h"
#include "events.h"
#include "rendering.h"
#include "generator.h"

typedef struct PREDEF_BOARD {
  int initial_points, cities;
  int *i, *j, *pop;
} predef_board_t;

predef_board_t boards[15] = {
    { .initial_points = 14, cities = 5, .i = (int[]){1, 3, 5, 7, 9}, .j = (int[]){2, 6, 10, 14, 16}, .pop = (int[]){3, 6, 2, 5, 4} },
    { .initial_points = 17, cities = 6, .i = (int[]){0, 4, 6, 8, 10, 11}, .j = (int[]){1, 3, 5, 7, 13, 16}, .pop = (int[]){4, 3, 6, 2, 5, 3} },
    { .initial_points = 19, cities = 7, .i = (int[]){1, 2, 4, 6, 7, 10, 11}, .j = (int[]){3, 8, 5, 9, 12, 14, 17}, .pop = (int[]){2, 4, 3, 6, 3, 2, 5} },
    { .initial_points = 13, cities = 5, .i = (int[]){0, 2, 6, 8, 10}, .j = (int[]){0, 6, 8, 12, 16}, .pop = (int[]){3, 5, 2, 4, 6} },
    { .initial_points = 15, cities = 6, .i = (int[]){1, 3, 5, 7, 9, 11}, .j = (int[]){2, 4, 7, 10, 13, 17}, .pop = (int[]){2, 6, 3, 5, 4, 2} },
    { .initial_points = 21, cities = 7, .i = (int[]){0, 1, 3, 4, 6, 7, 9}, .j = (int[]){1, 3, 5, 8, 10, 13, 16}, .pop = (int[]){4, 2, 5, 3, 2, 6, 1} },
    { .initial_points = 18, cities = 6, .i = (int[]){2, 4, 6, 8, 10, 11}, .j = (int[]){0, 3, 6, 9, 12, 15}, .pop = (int[]){3, 5, 4, 2, 3, 6} },
    { .initial_points = 22, cities = 7, .i = (int[]){0, 2, 3, 5, 6, 8, 9, 11}, .j = (int[]){0, 2, 5, 7, 10, 12, 14, 17}, .pop = (int[]){1, 2, 4, 3, 5, 4, 2, 6} },
    { .initial_points = 20, cities = 7, .i = (int[]){1, 3, 5, 7, 9, 11, 10}, .j = (int[]){1, 3, 5, 7, 9, 11, 15}, .pop = (int[]){3, 3, 2, 5, 6, 2, 4} },
    { .initial_points = 16, cities = 6, .i = (int[]){0, 2, 4, 6, 8, 10}, .j = (int[]){1, 4, 7, 10, 13, 16}, .pop = (int[]){2, 4, 3, 5, 2, 6} },
    { .initial_points = 24, cities = 9, .i = (int[]){0, 2, 4, 6, 8, 10, 11, 3, 5}, .j = (int[]){0, 2, 4, 6, 8, 10, 12, 14, 16}, .pop = (int[]){3, 5, 2, 3, 6, 2, 4, 1, 3} },
    { .initial_points = 19, cities = 7, .i = (int[]){1, 3, 4, 6, 7, 9, 10}, .j = (int[]){1, 3, 5, 7, 9, 11, 13}, .pop = (int[]){2, 4, 3, 5, 2, 4, 3} },
    { .initial_points = 23, cities = 9, .i = (int[]){0, 2, 4, 6, 8, 10, 11, 5, 7}, .j = (int[]){0, 3, 6, 9, 12, 15, 17, 14, 4}, .pop = (int[]){2, 5, 4, 3, 2, 6, 1, 3, 2} },
    { .initial_points = 20, cities = 8, .i = (int[]){1, 3, 5, 7, 9, 11, 4, 6}, .j = (int[]){2, 4, 6, 8, 10, 12, 15, 3}, .pop = (int[]){3, 5, 4, 2, 6, 2, 3, 2} },
    { .initial_points = 18, cities = 7, .i = (int[]){0, 2, 4, 6, 8, 10, 5}, .j = (int[]){1, 3, 5, 7, 9, 11, 13}, .pop = (int[]){2, 3, 5, 4, 6, 1, 2} }
};


void put_city(board_t *board, int i, int j, int pop, int *index) {
  if(board->parents[i * BOARD_WIDTH + j] == 0) {
    pop &= 15; //truncate population at 15
    board->data[i * BOARD_WIDTH + j] = pop;
    (*index)++;
    board->parents[i * BOARD_WIDTH + j] = (*index);
  }
}

int generate(board_t *board, int *cities) {
  int choice = rand() % 15;
  int index = 1;
  
  //generate board
  for(int i = 0; i < BOARD_HEIGHT; i++) {
    for(int j = 0; j < BOARD_WIDTH; j++) {
      board->data[i * BOARD_WIDTH + j] = 0;
      board->parents[i * BOARD_WIDTH + j] = 0;
    }
  }
  for(int k = 0; k < boards[choice].cities; k++) {
    board->data[boards[choice].i[k] * BOARD_WIDTH + boards[choice].j[k]] = boards[choice].pop;
    board->parents[boards[choice].i[k] * BOARD_WIDTH + boards[choice].j[k]] = index++;
  }
  
  (*cities) = boards[choice].cities;

  return boards[choice].initial_points;
}