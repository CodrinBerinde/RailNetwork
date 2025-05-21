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

#define BOARD_HEIGHT 18 // 18 rows of cells
#define BOARD_WIDTH 28 // 12 columns of cells

uint8_t board[BOARD_HEIGHT * BOARD WIDTH];

//returns 0 if the game is won, nonzero value otherwise
uint8_t play();

#endif