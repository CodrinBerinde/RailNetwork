/*******************************************************************
  play.c    - contains the main function play() that runs over the
              entire gameplay, which also contains the main loop

  Berinde Codrin
  berinde.gh.codrin@student.utcluj.ro
  Technical University of Cluj-Napoca, Romania

  developed at the Czech Technical University in Prague
  Prague, Czech Republic - may 2025

 *******************************************************************/

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "play.h"
#include "events.h"
#include "rendering.h"
#include "generator.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "mzapo_parlcd.h"

int play() {
  uint8_t board_data[BOARD_HEIGHT * BOARD_WIDTH];
  board_t board = {board_data};
  cell_t selected = {0, 0}, under_constr = {-1, 0};
  init_rendering_constants();
  init_reading_constants();
  buf_t *buf = init_buffer();

  generate(&board);
  refresh_board(&board, buf, &selected, &under_constr);
  put_buffer(buf);

  int event = 0;
  while (event != 6) {
    int event = listen_event();
    if(event != 0) {
      switch(event) {
        case 0: break;
        case 1:
          if(selected.j < BOARD_WIDTH - 1)
            selected.j++;
          break;
        case 2:
          if(selected.i < BOARD_HEIGHT - 1)
            selected.i++;
          break;
        case 3:
          if(selected.j > 0)
            selected.j--;
          break;
        case 4:
          if(selected.i > 0)
            selected.i--;
          break;
        case 6:
          break;
      }

      refresh_board(&board, buf, &selected, &under_constr);
      //clear_buf(buf, 0xffff);
      put_buffer(buf);
    }
    usleep(10000);
  }

  clear_mem_buffer(buf);
}