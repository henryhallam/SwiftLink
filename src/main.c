#include "board.h"
#include "radio.h"

void main() {
  P1DIR=0x03;   //LED on P1.1 and .0
  LED=0;
  LED2=0;
  CLKCON = CLKCON_OSC32K_RC| CLKCON_OSC_XTAL | CLKCON_TICKSPD_1 | CLKCON_CLKSPD_1; //26MHz crystal oscillator for cpu, 13MHz for timers
  while (CLKCON & CLKCON_OSC_RC);  //wait for clock stability
  
  
  
  radio_init();
  EA=1;
  
  while(1)
    LED=radio_still_sending();
}