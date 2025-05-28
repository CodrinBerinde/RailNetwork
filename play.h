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

#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "mzapo_parlcd.h"
//#include "rendering.h"

#define BOARD_HEIGHT 12 // 12 rows of cells
#define BOARD_WIDTH 18 // 18 columns of cells

extern BUF buf_t;

typedef struct BOARD {
  uint8_t *data;
  int *parents;
} board_t;

typedef struct CELL {
  int i, j;
} cell_t;

//returns 0 if the game is won, nonzero value otherwise
int game(int points, buf_t *buf);

void play();

int are_neighbours(cell_t *a, cell_t *b);

void unify(board_t *board, int i0, int j0, int i1, int j1, int link, int *points, int *trees);

#endif