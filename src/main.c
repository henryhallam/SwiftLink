#include "cc1111.h"
#include "hal.h"
#include "radio.h"
#include "modem.h"
#include <string.h>

void main() {
  int i=0;

  setup_led();

  CLKCON = CLKCON_OSC32K_RC | CLKCON_OSC_XTAL | CLKCON_TICKSPD_1_2 | CLKCON_CLKSPD_1; //24MHz crystal oscillator for cpu, 12MHz for timers
  while (CLKCON & CLKCON_OSC_RC);  //wait for clock stability
  
  
  
  //radio_init();
  modem_init();

  
  while(1) {
//    leds_set(radio_still_sending(),!radio_still_sending());
//    LED=radio_still_sending();

    modem_tx_packet[0] = 5;
    strcpy(modem_tx_packet+1,"COCKS");
    modem_tx_send(6);

    leds_set(1,0);
    while (modem_tx_still_sending()); 
    leds_set(0,0);
    
//    while (--i);
 //   modem_tx_send(22);
  }
}
