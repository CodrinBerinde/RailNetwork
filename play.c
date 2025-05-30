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

void *spiled_base;

int are_neighbours(cell_t *a, cell_t *b) {
  if(a->i == -1) //if cell a does not even exist (may happen when we work with a cell under construction and no cell is currently under construction)
    return -1;
  if(a->i == b->i) {
    if(a->j == b->j - 1) //if b is to the right of a
      return 0;
    if(a->j == b->j + 1) //if b is to the left of a
      return 2;
  }
  if(a->j == b->j) {
    if(a->i == b->i - 1) //if b is below a
      return 1;
    if(a->i == b->i + 1) //if b is above a
      return 3;
  }
  return -1;
}

void unify(board_t *board, int i0, int j0, int i1, int j1, int link, int *points, int *trees) {
  //a point is deduced if a new road is build (it must have not existed before)
  if(((board->data[i0 * BOARD_WIDTH + j0]) & (1 << (4 + link))) == 0)
    (*points)--;
  //board information is updated with the new segment (both in starting and end cell)
  board->data[i0 * BOARD_WIDTH + j0] |= (1 << (4 + link));
  board->data[i1 * BOARD_WIDTH + j1] |= (1 << (4 + (link + 2)%4));
  //getting information about the parents of each cell
  int parent0 = board->parents[i0 * BOARD_WIDTH + j0], parent1 = board->parents[i1 * BOARD_WIDTH + j1];
  int size0 = 0, size1 = 0, pop0 = 0, pop1 = 0;
  
  //now we perform the unification
  if(parent0 != parent1) {
    //we count the size of tree and population for parent0
    for(int i = 0; i < BOARD_HEIGHT; i++) {
      for(int j = 0; j < BOARD_WIDTH; j++) {
        if(((board->data[i * BOARD_WIDTH + j]) & (15)) != 0 && board->parents[i * BOARD_WIDTH + j] == parent0) {
          size0++;
          pop0 += (board->data[i * BOARD_WIDTH + j]) & (15);
        }
      }
    }

    //we count the size of tree and population for parent1
    for(int i = 0; i < BOARD_HEIGHT; i++) {
      for(int j = 0; j < BOARD_WIDTH; j++) {
        if(((board->data[i * BOARD_WIDTH + j]) & (15)) != 0 && board->parents[i * BOARD_WIDTH + j] == parent1) {
          size1++;
          pop1 += (board->data[i * BOARD_WIDTH + j]) & (15);
        }
      }
    }

    //we check if we unified two non-empty (inhabited) but still different subnetworks
    if(size0 > 0 && size1 > 0) {
      if(size0 == 1) //if subnetwork corresponding to parent0 has only one city
        (*points) += pop0; //then we gain the points for connecting that city for the first time
      if(size1 == 1) //if subnetwork corresponding to parent1 has only one city
        (*points) += pop1; //then we gain the points for connecting that city for the first time
      (*trees)--; //and also the number of total inhabited trees (subnetworks) in the map decreases by 1
    }

    //we write all cells with parent1 as parent0
    for(int i = 0; i < BOARD_HEIGHT; i++) {
      for(int j = 0; j < BOARD_WIDTH; j++) {
        if(board->parents[i * BOARD_WIDTH + j] == parent1) {
          board->parents[i * BOARD_WIDTH + j] = parent0;
        }
      }
    }
  }
}

int game(int difficulty, buf_t *buf) {
  //we allocate space for the new board
  uint8_t board_data[BOARD_HEIGHT * BOARD_WIDTH];
  int parent_vector[BOARD_HEIGHT * BOARD_WIDTH];
  
  int trees; //number of inhabited trees (subnetworks) in the game
  board_t board = {board_data, parent_vector};
  cell_t selected = {0, 0}, under_constr = {-1, 0}; //the selected and the under construction cell (initially no cell is under construction)

  int points = generate(&board, &trees) + 6 - 2 * difficulty; //generating the board and also accounting for the difficulty level (give less initial points for high difficulty)
  refresh_board(&board, buf, &selected, &under_constr, points); //draw the board for the first time
  put_buffer(buf);

  int event = 0;//variable that stores the curent action (request made by player)
  while (event != 6) {
    event = listen_event(); //checks whether the player has made a request
    if(event != 0) { //if an actual request was made
      switch(event) {
        case 0: break; //if no request was made, just exit
        case 1: //if the player requested to move right
          if(selected.j < BOARD_WIDTH - 1) //if we are not already on the rightmost cell 
            selected.j++; //go to the left cell
          break;
        case 2: //if the player requested to move down
          if(selected.i < BOARD_HEIGHT - 1) //if we are not already on the downmost cell
            selected.i++; //go down
          break;
        case 3: //if the player requested to move left
          if(selected.j > 0) //if we are not already on the leftmost cell
            selected.j--; //go left
          break;
        case 4: //if the player requested to move up
          if(selected.i > 0) //if we are not already on the upmost cell
            selected.i--; //go up
          break;
        case 5: //it means that the current cell was pressed
          if(under_constr.i == selected.i && under_constr.j == selected.j) { //if the selected cell is the same as the cell under construction
            under_constr.i = -1; //just cancel the construction
            break;
          }
          int link = are_neighbours(&under_constr, &selected); //we test if the two cells are neighbouring
          if(link != -1) { //if they are
            unify(&board, under_constr.i, under_constr.j, selected.i, selected.j, link, &points, &trees); //we perform the unification of these two cells
          }
          under_constr.i = selected.i; //the new under_construction cell becomes the actual selected cell (implemented like this for the ease of building)
          under_constr.j = selected.j;
        case 6: //if an exit request was made, just break
          break;
      }
      
      if(trees == 1) //if the number of subnetworks is one, it means that the game is won
        points = 0; //number of points reduced to zero only for display purposes
      refresh_board(&board, buf, &selected, &under_constr, points); //refresh information on the display, after this step
      if(event == 6 || trees == 1 || points == 0) //if either an exit request was made, or the game is won, or it is lost
        clear_display_data(buf); //we clear all the data that is displayed (lcd, leds)
      else
        put_buffer(buf); //otherwise, just put the buffer with the information already refreshed
      if(trees == 1) { //if the number of trees (subnetworks) is 1, it means that the game is won
        return 0;
      }
        
      if(points == 0) { //if the number of subnetworks is not 1, but the number of points is 0, the game is lost
        return 1;
      }
        
    }
    usleep(10000); //after each loop iteration we wait for 1ms
  }

  return -1; //if we exit the main loop (request 6 was made), the game is aborted
}

void play() {
  srand(time(NULL)); //randomization needed for choosing a board
  init_rendering_constants(); //initialization of constants needed for dislaying the information
  init_reading_constants(); //initialization of constants needed for reading of the events (from knobs)
  buf_t *buf = init_buffer(); //initialization of the buffer

  int opt = 0, win;
  int menu_row = 0;
  show_menu(menu_row, buf); //first display of the main menu
  put_buffer(buf);
  while(opt != 6) { //while the user option (request) is diffent from exit
    opt = listen_event(); //we listen to the request
    if(opt == 1 || opt == 2) { //if the request is either to move down or left
      if(menu_row < 3) //if the selected option on the menu is not already the last one
        menu_row++; //we move down on the menu
    }
    if(opt == 3 || opt == 4) { //if the request is either to move up or right
      if(menu_row > 0) //if the selected option on the menu is not already the first one
        menu_row--; //we move up on the menu
    }
    if(opt >= 1 && opt <= 4) { //if the request was to move into the menu
      show_menu(menu_row, buf); //we have to update what we display (because the selected option changed)
      put_buffer(buf);
    }
    if(opt == 5) { //if the option (request) is to click
      if(menu_row == 3) //if the selected menu row is the "Exit" one
        opt = 6; //option becomes to exit
      else {
        win = game(menu_row + 1, buf); //otherwise a game is initiated with the difficulty level according to the selected menu row
        if(win != -1) //if the game was not aborted (so either won or lost)
          write_final_message(buf, win); //we display an appropriate message on the screen
        show_menu(menu_row, buf); //then return to dislaying the main menu
        put_buffer(buf);
        opt = 0; //and the option is set to 0 because otherwise, if the game was aborted with option 6, also this loop will end, which we do not want
      }
    }
    if(opt == 6) { //if the option is to exit
      clear_display_data(buf); //we clear all the displayed data
    }
  }
  clear_mem_buffer(buf); //release the memory that was allocated for the buffer
}