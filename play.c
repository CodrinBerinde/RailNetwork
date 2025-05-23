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

  //fb_draw_empty_board();
  //fb_draw();
  while(1) {
    
    //here we attempt to read from SPILED knobs


    int event = listen_event();
    if(event != 0) {
      refresh_board(&board, buf, &selected, &under_constr);
      put_buffer(buf);
    }
    //sleep(1);
  }

  clear_mem_buffer(buf);
}