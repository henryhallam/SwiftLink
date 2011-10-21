#include "cc1111.h"
#include "hal.h"

void modem_init() {
  // Clear Timer 1 channel 1 and overflow interrupt flag
  // CPU interrupt flag (IRCON) for Timer 1 is cleared automatically by hardware.
  T1CTL &= ~T1CTL_OVFIF;
  
  // Enable Timer 1 interrupts by setting [IEN1.T1IE=1]
//  IEN1 |= IEN1_T1IE;
  
  T1CC0H = 0x06;
  T1CC0L = 0x83;
  
  // set Timer 1 to modulo mode
  T1CTL = T1CTL_DIV_1 | T1CTL_MODE_MODULO;
  TIMIF |= 1<<6;  // enable Timer1 overflow interrupt


  IEN1 |= IEN1_T1IE;
  
 //output pin
  P0DIR |= 1<<1;
}

void t1_isr(void)  __interrupt (9) __using (2) {

//  leds_set(3,3);
//  leds_set(0,0);
//

  static char sub_bit=0;

  sub_bit++;
  sub_bit %= 6;
  if (sub_bit == 3) {
    leds_set(1,0);
    P0_1 = 1;
    
  } else if (sub_bit == 0) {
    leds_set(2,0);
    P0_1 = 0;
  }

}


