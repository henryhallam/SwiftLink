#define DC_EST_INIT 150
#define DC_EST_IIR_SHIFT 8
#define BIT_THRES 4

#include <ioCC1110.h>
#include "mancdiff.h"

void manc_diff_decoder_init(manc_diff_decoder *m, void (*decoded_bit_callback)(signed char)) {

  m->dc_est = DC_EST_INIT;
  m->dc_est <<= DC_EST_IIR_SHIFT;
  m->prev = 0;
  m->run_len = 0;
  m->state = invalid;
  m->decoded_bit_callback = decoded_bit_callback;

}


void manc_decode(manc_diff_decoder *m, unsigned char adc8) {

  static unsigned char dc;
  static  signed char level;
 
  //First run the IIR low pass on the ADC value to estimate the DC level
  //Caution: sucks balls
 // m->dc_est -= (m->dc_est >> DC_EST_IIR_SHIFT);
//  m->dc_est += adc8;
  


  //Decide if this sample is positive, negative or indeterminate
  dc = 161;
    
  if (adc8 > dc)
    level = +1;
  else
    level = -1;
    
  //Detect edges
  if (level != m->prev) {
    //Found an edge (or a glitch)
    if (m->run_len == 1) {     
      m->state = invalid;
      m->decoded_bit_callback(-1);    
    }
    else if (m->run_len >= 2 && m->run_len <= 4) {
      //A short run
      switch(m->state) {
//    case invalid:          Unnecessary - we'll just leave it as invalid
//      m->state = ambiguous;
//      break;        
      case bit:
        //The previous edge was in between two bit periods,
        // so this one must be the clock.
        m->state = clock; 
        //This was a short run, so there was a transition at
        // the beginning of the bit period, so this is a 0
        m->decoded_bit_callback(0);
        break;
      case clock:
        //Previous edge was the clock, so this edge is between two bit periods.
        m->state = bit;
        break;
      }
    } else if (m->run_len >= 5 && m->run_len <= 7) {
      //A long run, so this edge  must be a clock
      m->state = clock;
      //No transition at start of bit period, so this is a 1
      m->decoded_bit_callback(1);
    }
    m->run_len = 1;
  }
  else{
    m->run_len++;
    if (m->run_len > 7) {
      //Run is impossibly long
      m->run_len = 8;
      m->state = invalid;
      m->decoded_bit_callback(-1);
    }
  }
  

  m->prev = level;
  
}
  

