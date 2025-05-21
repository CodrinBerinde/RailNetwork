/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  change_me.c      - main file

  Berinde Codrin
  berinde.gh.codrin@student.utcluj.ro
  Technical University of Cluj-Napoca, Romania

  developed at the Czech Technical University in Prague
  Prague, Czech Republic - may 2025

 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#ifndef QTRVSIM
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "mzapo_parlcd.h"
#else
#include "qtrvsim_regs.h"
#endif
#include "font_types.h"

#include "play.h"
#include "events.h"
#include "rendering.h"
#include "generator.h"

int main(int argc, char *argv[])
{

  /* Serialize execution of applications */

#ifndef QTRVSIM
  /* Try to acquire lock the first */
  if (serialize_lock(1) <= 0) {
    printf("System is occupied\n");

    if (1) {
      printf("Waiting\n");
      /* Wait till application holding lock releases it or exits */
      serialize_lock(0);
    }
  }
#endif

  printf("Hello world\n");

#ifndef QTRVSIM
  void *spiled_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE , 0);
  unsigned char *parlcd_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE , 0);
#else
  void *spiled_base = (void *)SPILED_REG_BASE;
  unsigned char *parlcd_base = (unsigned char *)LCD_FB_START;
#endif

  fbuf_t *fb = &fbuf0;

while(1) {

  uint32_t color_rgb888;

  color_rgb888 = *(volatile uint32_t*)(spiled_base + SPILED_REG_KNOBS_8BIT_o);
  *(volatile uint32_t*)(spiled_base + SPILED_REG_LED_LINE_o) = color_rgb888;
  *(volatile uint32_t*)(spiled_base + SPILED_REG_LED_RGB1_o) = color_rgb888;

  //fb_clear(fb, 0);
  //fb_rectangle(fb, 100, 50, 200, 100, 0xf800);
  //fb_str(fb, 100, 100, &font_winFreeSystem14x16, 1, 0xffff, "Hello APO!");
  fb_draw(fb, parlcd_base);

}
#ifndef QTRVSIM
  sleep(1);
#endif

  printf("Goodbye world\n");

#ifndef QTRVSIM
  /* Release the lock */
  serialize_unlock();
#endif

  return 0;
}
