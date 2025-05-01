/*******************************************************************
  Simple program to demostrate binary reprezentation
  on QtMips emulator developed by Karel Koci and Pavel Pisa.

  qrmips_binrep.c       - main and only file

  (C) Copyright 2004 - 2019 by Pavel Pisa
      e-mail:   pisa@cmp.felk.cvut.cz
      homepage: http://cmp.felk.cvut.cz/~pisa
      work:     http://www.pikron.com/
      license:  any combination GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

#include <stdint.h>

/*
 * Next macros provides location of knobs and LEDs peripherals
 * implemented on QtMips simulator.
 *
 * More information can be found on page
 *   https://github.com/ppisa/QtMips
 */


/*
 * Base address of the region where simple serial port (UART)
 * implementation is mapped in emulated MIPS address space
 */
#define SERIAL_PORT_BASE   0xffffc000
/*
 * Byte offset of the 32-bit transition status register
 * of the serial port
 */
#define SERP_TX_ST_REG_o         0x08

/*
 * Mask of the bit which inform that peripheral is ready to accept
 * next character to send. If it is zero, then peripheral is
 * busy by sending of previous character.
 */
#define SERP_TX_ST_REG_READY_m    0x1
/*
 * Byte offset of the UART transmit register.
 * When the 32-bit word is written the least-significant (LSB)
 * eight bits are send to the terminal.
 */
#define SERP_TX_DATA_REG_o        0x0c

/*
 * Base address of the region where knobs and LEDs peripherals
 * are mapped in the emulated MIPS physical memory address space.
 */
#define SPILED_REG_BASE      0xffffc100

/* Valid address range for the region */
#define SPILED_REG_SIZE      0x00000100

/*
 * Byte offset of the register which controls individual LEDs
 * in the row of 32 yellow LEDs. When the corresponding bit
 * is set (value 1) then the LED is lit.
 */
#define SPILED_REG_LED_LINE_o           0x004

/*
 * The register to control 8 bit RGB components of brightness
 * of the first RGB LED
 */
#define SPILED_REG_LED_RGB1_o           0x010

/*
 * The register to control 8 bit RGB components of brightness
 * of the second RGB LED
 */
#define SPILED_REG_LED_RGB2_o           0x014

/*
 * The register which combines direct write to RGB signals
 * of the RGB LEDs, write to the keyboard scan register
 * and control of the two additional individual LEDs.
 * The direct write to RGB signals is orred with PWM
 * signal generated according to the values in previous
 * registers.
 */
#define SPILED_REG_LED_KBDWR_DIRECT_o   0x018

/*
 * Register providing access to unfiltered encoder channels
 * and keyboard return signals.
 */
#define SPILED_REG_KBDRD_KNOBS_DIRECT_o 0x020

/*
 * The register representing knobs positions as three
 * 8-bit values where each value is incremented
 * and decremented by the knob relative turning.
 */
#define SPILED_REG_KNOBS_8BIT_o         0x024

#define LCD_FB_START          0xffe00000
#define LCD_FB_END            0xffe4afff

