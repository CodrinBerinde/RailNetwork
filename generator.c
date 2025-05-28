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

#include "play.h"
#include "events.h"
#include "rendering.h"
#include "generator.h"

void generate(board_t *board, int *cities) {
  //generate dummy board
  for(int i = 0; i < BOARD_HEIGHT; i++) {
    for(int j = 0; j < BOARD_WIDTH; j++) {
      board->data[i * BOARD_WIDTH + j] = 0;
      board->parents[i * BOARD_WIDTH + j] = 0;
    }
  }
  board->data[15] = 4;
  board->data[100] = 12;
  board->data[113] = 9;
  board->parents[15] = 1;
  board->parents[100] = 2;
  board->parents[113] = 3;
  (*cities) = 3;
}