#include "cc1111.h"
#include "hal.h"
#include "radio.h"
#include "modem.h"

void main() {


  setup_led();

  CLKCON = CLKCON_OSC32K_RC | CLKCON_OSC_XTAL | CLKCON_TICKSPD_1_2 | CLKCON_CLKSPD_1; //24MHz crystal oscillator for cpu, 12MHz for timers
  while (CLKCON & CLKCON_OSC_RC);  //wait for clock stability
  
  
  
  //radio_init();
  modem_init();
  EA=1;
  
  while(1) {
//    leds_set(radio_still_sending(),!radio_still_sending());
//    LED=radio_still_sending();

  }
}
