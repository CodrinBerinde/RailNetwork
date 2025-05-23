/*******************************************************************
  events.c    - contains the functions which relate to the input peripherals
                of the board: knobs (both rotating and pushing)

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

void *spiled_base;

void init_reading_constants() {
  spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE , 0);
}

int listen_event() {
  uint32_t knobs = *(uint32_t *)(spiled_base + SPILED_REG_KNOBS_8BIT_o);
  printf("I read: red=%d, green=%d, blue=%d.\n", (knobs >> 16) & 255, (knobs >> 8) & 255, knobs & 255);
  return 0;
}