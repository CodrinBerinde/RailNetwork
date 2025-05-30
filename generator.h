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

//function that generates a board to be played on; it returns through parameters the generated board, the number of cities, and through return value, the minimal
//  number of initial points with which the game has soution
int generate(board_t *board, int *cities);

#endif