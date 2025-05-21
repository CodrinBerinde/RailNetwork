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



uint16_t buf0_data[PARLCD_WIDTH * PARLCD_HEIGHT];

buf_t buf = {
  .width = PARLCD_WIDTH,
  .height = PARLCD_HEIGHT,
  .data = buf0_data,
};

int play_game() {
  void *spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE , 0);
  unsigned char *parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE , 0);

  fb_draw_empty_board(&buf);
  fb_draw(&buf, parlcd_base);
  while(1) {
    sleep(1);
  }
}