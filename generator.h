/*******************************************************************
  generation.h    - contains the functions which relate to the
                    generation of the initial board

  Berinde Codrin
  berinde.gh.codrin@student.utcluj.ro
  Technical University of Cluj-Napoca, Romania

  developed at the Czech Technical University in Prague
  Prague, Czech Republic - may 2025

 *******************************************************************/

#ifndef GENERATOR_H
#define GENERATOR_H

#include "play.h"

int generate(board_t *board, int *cities);

void put_city(board_t *board, int i, int j, int pop, int *index);

#endif