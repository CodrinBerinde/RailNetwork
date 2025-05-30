/*******************************************************************
  events.h    - contains the functions which relate to the input peripherals
                of the board: knobs (both rotating and pushing)

  Berinde Codrin
  berinde.gh.codrin@student.utcluj.ro
  Technical University of Cluj-Napoca, Romania

  developed at the Czech Technical University in Prague
  Prague, Czech Republic - may 2025

 *******************************************************************/

#ifndef EVENTS_H
#define EVENTS_H

#define SAFETY_KNOB_DIFFERENCE 100 //a safety margin used for overflow of the value read from the knobs

//initializes the constants for reading data from the knobs
void init_reading_constants();

//returns 0 if no event is caught, otherwise the code of the event as following:
//  1 - move right
//  2 - move down
//  3 - move left
//  4 - move up
//  5 - click cell
//  6 - exit game
int listen_event();

#endif