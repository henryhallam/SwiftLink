//Status: Under construction

#include <string.h>
#include <ioCC1110.h>
#include "ioCCxx10_bitdef.h"
#include "radio.h"

#define UART_TX_FIFO_N 20

#define LED P1_1

static unsigned char uart_tx_fifo[UART_TX_FIFO_N];
static unsigned char uart_tx_fifo_wp=0;
static unsigned char uart_tx_fifo_rp=0;

void delay (unsigned char n);

void utx0_put(unsigned char b) {
  uart_tx_fifo[uart_tx_fifo_wp++] = b;
  uart_tx_fifo_wp %= UART_TX_FIFO_N;
  IEN2 |= IEN2_UTX0IE;
}

#pragma vector = UTX0_VECTOR
__interrupt void utx0_ISR(void) {
  if (uart_tx_fifo_rp != uart_tx_fifo_wp) { //reached end of buffer?
    UTX0IF=0;
    U0DBUF=uart_tx_fifo[uart_tx_fifo_rp++]; //no, send a byte
    uart_tx_fifo_rp %= UART_TX_FIFO_N;
  }
  else
    IEN2 &= ~IEN2_UTX0IE; //disable UTX interrupts until we have another byte in the buffer
    
}


int main( void )
{
  
  static unsigned char packet[BUFLEN+2];

  P1DIR=0x02;   //LED on  P1.1
  LED=0;
  
  CLKCON = (1<<7) | (0<<6) | (0<<3) | (0<<0); //26MHz crystal oscillator for cpu and timer
  while (CLKCON & CLKCON_OSC);

  //Setup UART
  P0SEL |= 1<<3;    //P0.3 = TX
  U0UCR=U0UCR_FLUSH | U0UCR_STOP;  //reset and 8-N-1
  U0BAUD=34;
  U0GCR=12; //12 = 115200 baud
  U0CSR=U0CSR_MODE; //UART not SPI.  RX disabled.
//  delay(100);
  U0DBUF='!';
  
  // Enable global interrupt by setting the [IEN0.EA=1]
  EA = 1;
  
  radio_init();
  
  utx0_put('B');
  utx0_put('o');
  utx0_put('o');
  utx0_put('t');
  
  while(1) {
    radio_recv_packet_block(packet);
    
    if (packet[BUFLEN+1] & 0x80)
      LED^=1;
/*    
      P1_1=0; //CRC match
    else
      P1_1=1; //CRC err
*/   

    
  };
 
  
}
