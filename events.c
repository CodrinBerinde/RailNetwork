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
#include "mzapo_phys.h"
#include "mzapo_regs.h"

extern void *spiled_base;
uint32_t prev_knob_red, prev_knob_green; //the previous rotary value on the knobs (needed to sense if a knob was rotated)
int push_state, push_state_blue; //the states of the buttons (red or green together, and blue)

void init_reading_constants() {
  uint32_t knobs = *(uint32_t *)(spiled_base + SPILED_REG_KNOBS_8BIT_o); //goes to the place from where the knob data will be read
  prev_knob_red = (knobs >> 16) & 255; //initialize the values of the previous rotary values of the knobs with the current ones
  prev_knob_green = (knobs >> 8) & 255;
  push_state = 0; //also the push states on all buttons is 0 (buttons initially not pressed)
  push_state_blue = 0;
}

int listen_event() {
  int res = 0;
  uint32_t knobs = *(uint32_t *)(spiled_base + SPILED_REG_KNOBS_8BIT_o);
  uint32_t knob_red = (knobs >> 16) & 255; //computes current value of red knob
  uint32_t knob_green = (knobs >> 8) & 255; //computes current value of green knob

  if(knob_red != prev_knob_red && knob_red % 4 == 0) { //if the value on the red knob changed from last time (and it is also multiple of 4, because
                                                        // this is how the knob rotates in steps)
    if(knob_red > prev_knob_red) {
      if(knob_red - prev_knob_red < SAFETY_KNOB_DIFFERENCE) //it means that the knob moved up
        res = 2;
      else        //the knob moved down but the value indicated overflowed
        res = 4;
    } else {
      if(prev_knob_red - knob_red < SAFETY_KNOB_DIFFERENCE) //it means that the knob moved down
        res = 4;
      else        //the knob moved up but the value indicated overflowed
        res = 2;
    }
    prev_knob_red = knob_red; //update the previous value with the current value
    return res;
  }

  if(knob_green != prev_knob_green && knob_green % 4 == 0) { //if the value on the green knob changed from last time (and it is also multiple of 4)
    if(knob_green > prev_knob_green) {
      if(knob_green - prev_knob_green < SAFETY_KNOB_DIFFERENCE) //it means that the knob moved up
        res = 1;
      else        //the knob moved down but the value indicated overflowed
        res = 3;
    } else {
      if(prev_knob_green - knob_green < SAFETY_KNOB_DIFFERENCE) //it means that the knob moved down
        res = 3;
      else        //the knob moved up but the value indicated overflowed
        res = 1;
    }
    prev_knob_green = knob_green; //update previous value with current value
    return res;
  }

  if((knobs >> 24) & 1) { //the blue knob is pushed
    if(push_state_blue == 0) { //and previously it was not
      push_state_blue = 1; //store that the button is pushed
      return 6;
    }
  } else { //if it is not pushed
    if(push_state_blue == 1) //but previously it was
      push_state_blue = 0; //update that the button is not pushed actually
  }

  if((knobs >> 25)) { //either the red or the green knobs is pushed
    if(push_state == 0) { //but they were previously not
      push_state = 1; //store that one of them is pushed
      return 5;
    }
  } else { //if none of them is pushed
    if(push_state == 1) //but previously one of them was
      push_state = 0; //update that none of the buttons is actually pushed
  }

  return 0;
}