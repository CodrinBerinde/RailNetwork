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

int are_neighbours(cell_t *a, cell_t *b) {
  if(a->i == -1)
    return -1;
  if(a->i == b->i) {
    if(a->j == b->j - 1)
      return 0;
    if(a->j == b->j + 1)
      return 2;
  }
  if(a->j == b->j) {
    if(a->i == b->i - 1)
      return 1;
    if(a->i == b->i + 1)
      return 3;
  }
  return -1;
}

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
    event = listen_event();
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
        case 5: //it means that the current cell was pressed
          if(under_constr == selected) {
            under_constr->i = -1;
            break;
          }
          int link = are_neighbours(&under_constr, &selected); //we test if the two cells are neighbouring
          if(link == -1)
            break;
          board->data[under_constr->i * BOARD_WIDTH + under_constr->j] |= (1 << (4 + link));
          board->data[selected->i * BOARD_WIDTH + selected->j] |= (1 << (4 + (link + 2)%4));
          under_constr.i = selected.i;
          under_constr.j = selected.j;
        case 6:
          break;
      }

      if(event == 6)
        clear_buf(buf, 0);
      else
        refresh_board(&board, buf, &selected, &under_constr);
      put_buffer(buf);
      
    }
    usleep(10000);
  }

  clear_mem_buffer(buf);
}