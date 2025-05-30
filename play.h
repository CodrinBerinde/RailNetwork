/*******************************************************************
  play.h    - contains the main function play() that runs over the
              entire gameplay, which also contains the main loop

  Berinde Codrin
  berinde.gh.codrin@student.utcluj.ro
  Technical University of Cluj-Napoca, Romania

  developed at the Czech Technical University in Prague
  Prague, Czech Republic - may 2025

 *******************************************************************/

#ifndef PLAY_H
#define PLAY_H

#include <stdint.h>

#define BOARD_HEIGHT 12 // number of rows of the board
#define BOARD_WIDTH 18 // number of columns of the board

typedef struct BOARD { //stucture that stores the content of a board (cities, rail network and the subtrees)
  uint8_t *data;
  int *parents;
} board_t;

typedef struct CELL { //structure that can hold information about the i and j coordinates of a cell
  int i, j;
} cell_t;

typedef struct BUF { //structure needed for the buffer
  int width;
  int height;
  uint16_t *data;
} buf_t;

//returns 0 if the game is won, -1 if the game was aborted and 1 if lost
int game(int points, buf_t *buf);

//the main function that runs while playing the game; it is called from main.c
void play();

//function that checks whether two cells a and b are neigbours; returns -1 if not neighbours
//  and 0..3 if they are, where 0 means that cell b is to the right of a, 1 - down, 2 - left, 3 - up
int are_neighbours(cell_t *a, cell_t *b);

//function that makes the necessary updates in the board when two cells are linked by a rail segment
void unify(board_t *board, int i0, int j0, int i1, int j1, int link, int *points, int *trees);

#endif