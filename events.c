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
#include <stdio.h>

#include "play.h"
#include "events.h"
#include "rendering.h"
#include "generator.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

extern void *spiled_base;
uint32_t prev_knob_red, prev_knob_green, prev_knob_blue;
int push_state, push_state_blue;

void init_reading_constants() {
  uint32_t knobs = *(uint32_t *)(spiled_base + SPILED_REG_KNOBS_8BIT_o);
  prev_knob_red = (knobs >> 16) & 255;
  prev_knob_green = (knobs >> 8) & 255;
  prev_knob_blue = knobs & 255;
  push_state = 0;
  push_state_blue = 0;
}

int listen_event() {
  int res = 0;
  uint32_t knobs = *(uint32_t *)(spiled_base + SPILED_REG_KNOBS_8BIT_o);
  uint32_t knob_red = (knobs >> 16) & 255;
  uint32_t knob_green = (knobs >> 8) & 255;
  uint32_t knob_blue = knobs & 255;

  if(knob_red != prev_knob_red && knob_red % 4 == 0) {
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
    return res;
  }

  if(knob_green != prev_knob_green && knob_green % 4 == 0) {
    if(knob_green > prev_knob_green) {
      if(knob_green - prev_knob_green < SAFETY_KNOB_DIFFERENCE) //it means that the knob moved up
        res = 1;
      else        //the knob moved down
        res = 3;
    } else {
      if(prev_knob_green - knob_green < SAFETY_KNOB_DIFFERENCE) //it means that the knob moved down
        res = 3;
      else
        res = 1;
    }
    prev_knob_green = knob_green;
    return res;
  }

  if((knobs >> 24) & 1) { //the blue knob is pushed
    if(push_state_blue == 0) {
      push_state_blue = 1;
      return 6;
    }
  } else {
    if(push_state_blue == 1)
      push_state_blue = 0;
  }

  if((knobs >> 25)) { //either the red or the green knobs is pushed
    if(push_state == 0) {
      push_state = 1;
      return 5;
    }
  } else {
    if(push_state == 1)
      push_state = 0;
  }

  return 0;
}