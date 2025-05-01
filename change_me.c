/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  change_me.c      - main file

  include your name there and license for distribution.

  Remove next text: This line should not appear in submitted
  work and project name should be change to match real application.
  If this text is there I want 10 points subtracted from final
  evaluation.

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

#define PARLCD_WIDTH 480
#define PARLCD_HEIGHT 320

typedef struct fbuf {
  int width;
  int height;
  uint16_t *data;
} fbuf_t;

uint16_t fbuf0_data[PARLCD_WIDTH * PARLCD_HEIGHT];

fbuf_t fbuf0 = {
  .width = PARLCD_WIDTH,
  .height = PARLCD_HEIGHT,
  .data = fbuf0_data,
};

void fb_draw(fbuf_t *fb, unsigned char *parlcd_base)
{
  uint16_t* p = fb->data;
#ifndef QTRVSIM
  parlcd_write_cmd(parlcd_base, 0x2c);

  for (int i = 0; i < fb->height * fb->width; i++)
    parlcd_write_data(parlcd_base, *(p++));
#else
  memcpy(parlcd_base, fb->data, fb->height * fb->width * 2);
#endif
}

void fb_clear(fbuf_t *fb, int color);

void fb_pixel(fbuf_t *fb, int x, int y, int color);

void fb_rectangle(fbuf_t *fb, int x0, int y0, int w, int h, int color);

int fb_char(fbuf_t *fb, int x0, int y0, font_descriptor_t *font, int size, int color, char ch);

int main(int argc, char *argv[])
{

  /* Serialize execution of applications */

#ifndef QTRVSIM
  /* Try to acquire lock the first */
  if (serialize_lock(1) <= 0) {
    printf("System is occupied\n");

    if (1) {
      printf("Waitting\n");
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
