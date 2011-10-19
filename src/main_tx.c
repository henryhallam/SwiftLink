//Status: Seems to work quite reliably, still may be sensitive to DC bias

#include <string.h>
#include <ioCC1110.h>
#include "ioCCxx10_bitdef.h"
#include "mancdiff.h"
#include "radio.h"

#define LED P1_1

static unsigned int  adc_result=0xBEEF;


static volatile int tic=0;

void radio_init(void);
void delay (unsigned char n);


#pragma vector = T1_VECTOR
__interrupt void timer1_ISR(void) {
    // Overflow interrupt
    if (T1CTL & T1CTL_OVFIF)
    {
       // P1_1 = 1; //raise scope line
        tic++;
        // Read ADC
        adc_result = ADCL & 0xC0;
        adc_result |= (ADCH << 8);
        if (adc_result & 0x8000)
          adc_result = 0;
        adc_result >>= 7;
            
        
        // Start next conversion
        ADCCON1 |= 0x40;
        
     //   P1_1 = 0; //drop scope line
        
        // Clear Timer 1 overflow interrupt flag.
        T1CTL &= ~T1CTL_OVFIF;
    }
}




int main( void )
{
  
  P1DIR=0x02;   //LED on P1.1
  LED=0;
  
  
  CLKCON = (1<<7) | (0<<6) | (0<<3) | (0<<0); //26MHz crystal oscillator for cpu and timer
  while (CLKCON & CLKCON_OSC);

  ADCCFG |= ADCCFG_0; //enable ADC pin 0 (P0.0)
  
  /* Set [ADCCON2.EREF/EDIV/ECH] bits set according to ADC configuration */
  ADCCON2 = 0x20; //AIN0, Vref = 1.25V, 10 bits
  ADCCON1 = 0x33; //trigger on ST=1

  

  /***************************************************************************
  * Setup interrupt
  */

  // Clear Timer 1 channel 1 and overflow interrupt flag
  // CPU interrupt flag (IRCON) for Timer 1 is cleared automatically by hardware.
  T1CTL &= ~T1CTL_OVFIF;

  // Set individual interrupt enable bit in the peripherals SFR
  OVFIM = 1;                  // Enable overflow interrupt
  
  // Enable Timer 1 interrupts by setting [IEN1.T1IE=1]
  T1IE = 1;




  /***************************************************************************
   * Setup Timer settings
   */

  T1CC0H = 0x0E;
  T1CC0L = 0x1B;
  
  // set Timer 1 to modulo mode
  T1CTL = (T1CTL & ~(T1CTL_MODE | T1CTL_DIV)) | T1CTL_MODE_MODULO
      | T1CTL_DIV_1;

  // Timer 1 will now start counting...
  // Enable global interrupt by setting the [IEN0.EA=1]
  EA = 1;
  
  radio_init();
  
  const char *test="0123456789";
  
  while(1) {
    radio_send_packet(test);

    while (radio_still_sending());
    LED^=1;
  }
  
 
/*  
  unsigned char n;
  
  while(1) {
    RFST = RFST_SIDLE;
    delay(1);
    RFST = RFST_STX;
    delay(1);
  
    P1_1^=1;
    n=10;
    while (!RFTXRXIF); RFTXRXIF=0;
    RFD=n;
    while (n--) {
      while (!RFTXRXIF); RFTXRXIF=0;
      RFD=n;
    }
    RFIF &= ~RFIF_IRQ_DONE;
    while (!(RFIF & RFIF_IRQ_DONE));
  }
  
  */
  
}
