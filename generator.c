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
#include "generator.h"

typedef struct PREDEF_BOARD { //structure needed to define the predefined, manually-inputted boards
  int initial_points, cities;
  int *i, *j, *pop;
} predef_board_t;

predef_board_t boards[5] = { //define some boards
    { .initial_points = 6, .cities = 4, .i = (int[]){1, 7, 8, 10}, .j = (int[]){16, 7, 17, 4}, .pop = (int[]){4, 3, 5, 5} },
    { .initial_points = 8, .cities = 5, .i = (int[]){2,3,4,8,11}, .j = (int[]){12,8,5,4,17}, .pop = (int[]){8,1,2,8,10} },
    { .initial_points = 5, .cities = 6, .i = (int[]){1,2,3,9,11,11}, .j = (int[]){4,14,1,13,0,9}, .pop = (int[]){1,9,5,6,8,2} },
    { .initial_points = 4, .cities = 5, .i = (int[]){1,7,8,10,11}, .j = (int[]){10,16,8,0,16}, .pop = (int[]){11,6,4,1,3} },
    { .initial_points = 3, .cities = 6, .i = (int[]){0,3,4,5,8,9}, .j = (int[]){13,4,2,12,4,7}, .pop = (int[]){9,3,2,3,1,4} }
};

int generate(board_t *board, int *cities) {
  int choice = rand() % 5; //choose randomly a board between 0 and 4
  int index = 0;
  
  //generate board
  for(int i = 0; i < BOARD_HEIGHT; i++) {
    for(int j = 0; j < BOARD_WIDTH; j++) {
      board->data[i * BOARD_WIDTH + j] = 0;
      board->parents[i * BOARD_WIDTH + j] = index++; //initialize parent of all cells with a different value (at the beginning, each cell is a different subnetwork)
    }
  }
  for(int k = 0; k < boards[choice].cities; k++) {
    board->data[boards[choice].i[k] * BOARD_WIDTH + boards[choice].j[k]] = boards[choice].pop[k]; //put the popualtions of the cities of th chosen board on the board
  }
  
  (*cities) = boards[choice].cities; //and store the total number of cities on the board

  return boards[choice].initial_points; //return the minimal number of initial points with which a game can be finished
}