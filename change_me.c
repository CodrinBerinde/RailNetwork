/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  change_me.c      - main file

  Berinde Codrin
  berinde.gh.codrin@student.utcluj.ro
  Technical University of Cluj-Napoca, Romania

  developed at the Czech Technical University in Prague
  Prague, Czech Republic - may 2025

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define BOARD_HEIGHT 18 // 18 rows of cells
#define BOARD_WIDTH 28 // 12 columns of cells

#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "mzapo_parlcd.h"
#include "font_types.h"

#include "play.h"
#include "events.h"
#include "rendering.h"
#include "generator.h"

uint16_t board_data[BOARD_HEIGHT * BOARD_WIDTH];

board_t board = {board_data};

int main(int argc, char *argv[])
{

  /* Serialize execution of applications */

  /* Try to acquire lock the first */
  if (serialize_lock(1) <= 0) {
    printf("System is occupied\n");

    if (1) {
      printf("Waiting\n");
      /* Wait till application holding lock releases it or exits */
      serialize_lock(0);
    }
  }

  printf("Hello world\n");

  play_game();

  sleep(1);

  printf("Goodbye world\n");

  /* Release the lock */
  serialize_unlock();

  return 0;
}
