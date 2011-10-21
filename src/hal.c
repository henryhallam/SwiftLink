/*
 * CC Bootloader - Hardware Abstraction Layer 
 *
 * Fergus Noble, Henry Hallam (c) 2011
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

// This is for the JimboLink TX 1b board.


#include "cc1111.h"

void setup_led() {
  // Setup LED and turn it off
  P1DIR |= 0x74; //P1.5 and P1.6 for LED3, P1.4 and P1.2 for LED2
  P1_5 = 0;
  P1_6 = 0;
  P1_4 = 0;
  P1_2 = 0;
}

void leds_set(char a, char b) { //a and b may be 0, 1 or 2
  P1_5 = (a & 2)?1:0;
  P1_6 = (a & 1)?1:0;

  P1_2 = (b & 2)?1:0;
  P1_4 = (b & 1)?1:0;
}

void led_on() {
  P1_6 = 1;
}

void led_off() {
  P1_6 = 0;
}

void usb_up() {
  // Bring up the USB link
	P1DIR |= 1;
	P1_0 = 1;
}

void usb_down() {
  // Bring down the USB link
  P1_0 = 0;
  P1DIR &= ~1;
}
