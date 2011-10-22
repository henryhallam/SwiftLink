#include "cc1111.h"
#include "hal.h"
#include "modem.h"
#include <stdint.h>

char modem_tx_packet[MODEM_TX_BUFF_SIZE];

// LSB = half-bit index
// 3:1 = bit index
static int8_t modem_tx_state;

// 0xFF = preamble
static uint8_t modem_tx_byte_index;
static uint8_t modem_tx_bytes;


void modem_init() {
  // Set output pin direction
  MODEM_TX_PIN_DIR_SETUP;

  // Put transmit state machine into "idle" state
  modem_tx_send(0);

  // Now set up the timer
  // First nuke the settings, clear the flags etc
  T1CTL = 0;
  
  T1CC0H = (MODEM_T1_PERIOD) >> 8;
  T1CC0L = (MODEM_T1_PERIOD) & 0xFF;
  
  // set Timer 1 to modulo mode
  T1CTL = T1CTL_DIV_1 | T1CTL_MODE_MODULO;
  TIMIF |= 1<<6;  // enable Timer1 overflow interrupt

  // enable timer interrupt
  IEN1 |= IEN1_T1IE;

  // global interrupt enable
  EA = 1;
  
}

void modem_tx_send(uint8_t packet_size) {
  modem_tx_state = 0x00;  // start of preamble
  modem_tx_byte_index = 0xFF;
  modem_tx_bytes=packet_size;
}

char modem_tx_still_sending() {
  if (modem_tx_bytes)
    return 1;
  return 0;
}

void t1_isr(void)  __interrupt (9) __using (2) {
  // Transmitter:
  
  // Differential manchester encoding.
  // Each bit is 2 half-bits, ab
  // Let p = previous half-bit, i.e. xp ab
  // To send 0,           a = !p
  // To send 1,           a = p
  // For all data bits,   b = !a.   
  //   a = b implies an error or reset condition.  This is used in the preamble.

  // Prescaler: The transmitter only runs at 2x bitrate, not 6x.
  static uint8_t tx_prescale = 3;
  uint8_t x;
  

  if (!--tx_prescale) {
    tx_prescale = 3;
    
    x = modem_tx_state & 0x0F;  // get the half-bit index

    if (modem_tx_byte_index == 0xFF) {
      // We're sending the preamble.
      // Preamble "byte" = 000 111 01 01 01 01 01   half-bits
      //                 =  X   X   0  0  0  0  0   bits
//      leds_set(2,0);
      if (x < 3)
        MODEM_TX_PIN = 0;
      else if (x<6)
        MODEM_TX_PIN = 1;
      else
        MODEM_TX_PIN = (x & 1);

    } else {
      // We're sending data bits.
//      leds_set(1,0);
      if (x & 1) {
        // Second half-bit.  Just invert it.
        MODEM_TX_PIN = ! MODEM_TX_PIN;
      } else {
        // First half-bit.
        x >>= 1;
        if (!(modem_tx_packet[modem_tx_byte_index] & (1 << x)))
          // Data bit is 0, therefore invert the line.
          MODEM_TX_PIN = ! MODEM_TX_PIN;
      }

    }

    if ((++modem_tx_state) & (1<<4)) {
      // We just finished a byte or the preamble.
      //P1_4 ^= 1;
      //leds_set(1,0);
      modem_tx_state = 0;
     // P1_4 = 1;
        //leds_set(2,0);
      if (++modem_tx_byte_index == modem_tx_bytes) {
        //leds_set(0,1);
        // No more bytes to transmit! We'll just send repeated preambles.
        modem_tx_byte_index = 0xFF;
        modem_tx_bytes = 0;
      }
      //leds_set(0,0);
    }
   // P1_4 = (modem_tx_state & (1<<4))?1:0; 
  }
  
  
}


