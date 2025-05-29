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
    { .initial_points = 3, .cities = 8, .i = (int[]){1,9,7,4,2,2,2,10}, .j = (int[]){12,7,17,6,0,9,12,12}, .pop = (int[]){2,14,14,6,5,2,6,14} },
    { .initial_points = 4, .cities = 5, .i = (int[]){7,1,10,6,1}, .j = (int[]){16,8,7,17,7}, .pop = (int[]){13,5,7,5,1} },
    { .initial_points = 6, .cities = 7, .i = (int[]){3,8,5,0,11,10,1}, .j = (int[]){1,5,9,16,17,0,12}, .pop = (int[]){12,5,6,14,9,5,8} },
    { .initial_points = 7, .cities = 8, .i = (int[]){10,0,8,2,4,7,1,6}, .j = (int[]){6,17,0,7,11,14,3,8}, .pop = (int[]){9,5,8,3,11,5,4,6} },
    { .initial_points = 8, .cities = 5, .i = (int[]){2,6,4,1,3}, .j = (int[]){2,14,7,16,1}, .pop = (int[]){7,12,8,13,6} },
    { .initial_points = 6, .cities = 7, .i = (int[]){0,2,8,5,10,3,6}, .j = (int[]){1,9,14,16,4,7,11}, .pop = (int[]){6,7,11,14,8,4,5} },
    { .initial_points = 4, .cities = 8, .i = (int[]){5,3,6,9,10,1,0,11}, .j = (int[]){17,7,0,3,13,1,14,11}, .pop = (int[]){5,8,7,9,12,6,6,13} },
    { .initial_points = 5, .cities = 7, .i = (int[]){3,9,0,4,8,10,6}, .j = (int[]){11,1,15,3,6,8,17}, .pop = (int[]){6,14,8,7,5,5,9} },
    { .initial_points = 6, .cities = 5, .i = (int[]){1,11,0,5,3}, .j = (int[]){1,15,13,5,9}, .pop = (int[]){10,14,12,7,3} },
    { .initial_points = 5, .cities = 6, .i = (int[]){2,7,5,0,9,4}, .j = (int[]){17,4,9,11,2,6}, .pop = (int[]){9,5,8,7,3,6} },
    { .initial_points = 4, .cities = 8, .i = (int[]){10,2,4,0,6,11,1,7}, .j = (int[]){16,2,10,5,8,13,17,6}, .pop = (int[]){13,6,8,4,7,12,10,5} },
    { .initial_points = 5, .cities = 6, .i = (int[]){0,3,11,2,8,5}, .j = (int[]){9,5,12,15,7,1}, .pop = (int[]){9,8,10,5,6,3} },
    { .initial_points = 6, .cities = 7, .i = (int[]){4,10,1,0,3,6,5}, .j = (int[]){14,9,3,13,6,1,7}, .pop = (int[]){5,13,10,12,4,6,2} },
    { .initial_points = 6, .cities = 5, .i = (int[]){2,8,1,11,7}, .j = (int[]){12,6,1,4,13}, .pop = (int[]){6,14,7,11,8} },
    { .initial_points = 3, .cities = 7, .i = (int[]){5,2,11,4,1,6,0}, .j = (int[]){2,10,14,7,16,12,6}, .pop = (int[]){4,7,13,9,6,5,2} }
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
    board->data[boards[choice].i[k] * BOARD_WIDTH + boards[choice].j[k]] = boards[choice].pop[k];
    board->parents[boards[choice].i[k] * BOARD_WIDTH + boards[choice].j[k]] = index++;
  }
  
  (*cities) = boards[choice].cities;

  return boards[choice].initial_points;
}