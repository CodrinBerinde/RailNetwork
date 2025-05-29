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

void unify(board_t *board, int i0, int j0, int i1, int j1, int link, int *points, int *trees) {
  board->data[i0 * BOARD_WIDTH + j0] |= (1 << (4 + link));
  board->data[i1 * BOARD_WIDTH + j1] |= (1 << (4 + (link + 2)%4));
  int parent0 = board->parents[i0 * BOARD_WIDTH + j0], parent1 = board->parents[i1 * BOARD_WIDTH + j1];
  int size0 = 0, size1 = 0, pop0 = 0, pop1 = 0;
  if(parent0 != parent1)
    (*points)--; //for the connection
  if(parent0 != parent1 && parent0 != 0 && parent1 != 0) {
    (*trees)--;
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
    if(size0 == 1)
      (*points) += pop0;
    if(size1 == 1)
      (*points) += pop1;
    //we write all cells with parent1 as parent0
    for(int i = 0; i < BOARD_HEIGHT; i++) {
      for(int j = 0; j < BOARD_WIDTH; j++) {
        if(board->parents[i * BOARD_WIDTH + j] == parent1) {
          board->parents[i * BOARD_WIDTH + j] = parent0;
        }
      }
    }
  } else if(parent1 == 0 && parent0 != 0) {
    board->parents[i1 * BOARD_WIDTH + j1] = parent0;
  } else if(parent1 != 0 && parent0 == 0) {
    board->parents[i0 * BOARD_WIDTH + j0] = parent1;
  }
}

int game(int difficulty, buf_t *buf) {
  uint8_t board_data[BOARD_HEIGHT * BOARD_WIDTH];
  int parent_vector[BOARD_HEIGHT * BOARD_WIDTH];
  
  int trees;
  board_t board = {board_data, parent_vector};
  cell_t selected = {0, 0}, under_constr = {-1, 0};

  int points = generate(&board, &trees) + 3 - difficulty;
  refresh_board(&board, buf, &selected, &under_constr, points);
  put_buffer(buf);

  int event = 0;
  while (event != 6) {
    printf("I am here.\n");
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
          if(under_constr.i == selected.i && under_constr.j == selected.j) {
            under_constr.i = -1;
            break;
          }
          int link = are_neighbours(&under_constr, &selected); //we test if the two cells are neighbouring
          if(link != -1) {
            unify(&board, under_constr.i, under_constr.j, selected.i, selected.j, link, &points, &trees);
          }
          under_constr.i = selected.i;
          under_constr.j = selected.j;
        case 6:
          break;
      }
      
      if(trees == 1)
        points = 0;
      refresh_board(&board, buf, &selected, &under_constr, points);
      if(event == 6 || trees == 1 || points == 0)
        clear_buf(buf, 0);
      
      put_buffer(buf);
      if(trees == 1) {
        return 0;
      }
        
      if(points == 0) {
        return 1;
      }
        
    }
    usleep(10000);
  }

  return -1;
}

void play() {
  srand(time(NULL));
  init_rendering_constants();
  init_reading_constants();
  buf_t *buf = init_buffer();

  int opt = 0, win;
  int menu_row = 0;
  show_menu(menu_row, buf);
  put_buffer(buf);
  while(opt != 6) {
    opt = listen_event();
    
    if(opt == 1 || opt == 2) {
      if(menu_row < 3)
        menu_row++;
    }
    if(opt == 3 || opt == 4) {
      if(menu_row > 0)
        menu_row--;
    }
    if(opt >= 1 && opt <= 4) {
      show_menu(menu_row, buf);
      put_buffer(buf);
    }
    if(opt == 5) {
      if(menu_row == 3)
        opt = 6;
      else {
        win = game(menu_row + 1, buf);
        if(win != -1)
          write_final_message(buf, win);
        show_menu(menu_row, buf);
        put_buffer(buf);
        opt = 0;
      }
    }
  }
  clear_buf(buf, 0);
  put_buffer(buf);
  clear_mem_buffer(buf);
}