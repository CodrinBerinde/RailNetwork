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
uint32_t prev_knob_red, prev_knob_green, prev_knob_blue;

void init_reading_constants() {
  spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE , 0);
  uint32_t knobs = *(uint32_t *)(spiled_base + SPILED_REG_KNOBS_8BIT_o);
  prev_knob_red = (knobs >> 16) & 255;
  prev_knob_green = (knobs >> 8) & 255;
  prev_knob_blue = knobs & 255;
}

int listen_event() {
  int res = 0;
  uint32_t knobs = *(uint32_t *)(spiled_base + SPILED_REG_KNOBS_8BIT_o);
  uint32_t knob_red = (knobs >> 16) & 255;
  uint32_t knob_green = (knobs >> 8) & 255;
  uint32_t knob_blue = knobs & 255;
  printf("Hello from the events! now red is %d, previus it was %d.\n", knob_red, prev_knob_red);
  if(knob_red != prev_knob_red) {
    if(knob_red > prev_knob_red) {
      if(knob_red - prev_knob_red < SAFETY_KNOB_DIFFERENCE) //it means that the knob moved up
        res = 2;
      else        //the knob moved down
        res = 4;
    } else {
      if(prev_knob_red - knob_red < SAFETY_KNOB_DIFFERENCE) //it means that the knob moved down
        res = 4;
      else
        res = 2;
    }
    prev_knob_red = knob_red;
  }

  //printf("I read: red=%d, green=%d, blue=%d.\n", (knobs >> 16) & 255, (knobs >> 8) & 255, knobs & 255);
  return res;
}