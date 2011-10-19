#include "cc1111.h"
#include <string.h>
#include "radio.h"
#include "board.h"

static unsigned char rf_packet[PAYLOAD_BYTES+2];
static unsigned char rf_packet_ix=0;
static unsigned char rf_packet_n=0;

static char rf_mode_tx=0;

void t2_isr(void)  __interrupt (10) __using (1) {
  static unsigned char slot=0;
  const char ping[]={4,'L','O','L'};
  slot++;

  if (slot % 32 == 0) {
    radio_send_packet(ping);
  }
//  else
//    LED2=0;
  
}

void rftxrx_ISR(void)  __interrupt (0) __using (1) {
  if (rf_packet_ix < rf_packet_n)       //If packet is not complete,
    if (rf_mode_tx)
      RFD = rf_packet[rf_packet_ix++];  // (TX) send data to radio peripheral
    else
      rf_packet[rf_packet_ix++] = RFD;  // (RX) write received byte to buffer
  else
    RFTXRXIE=0;    //Packet is complete, so disable the interrupt.
}


char radio_still_sending(){ //Poll whether the radio is still transmitting
//    return (rf_packet_ix < rf_packet_n);
  return !(RFIF & RFIF_IM_DONE);
}

void radio_recv_packet_block(void *packet) {  //go into receive mode, wait for a complete packet, write it to *packet
  
    RFST = RFST_SRX;  
    rf_packet_n=PAYLOAD_BYTES+2;     //PAYLOAD_BYTES payload bytes, then RSSI, then CRC+LQI
    rf_packet_ix=0;
    RFTXRXIE=1;
    
    RFIF &= ~RFIF_IM_DONE;
    while (!(RFIF & RFIF_IM_DONE));
    
    memcpy(packet,rf_packet,PAYLOAD_BYTES+2);
}

void radio_send_packet(const void *packet) { //send the packet over RF

      DMAARM |= DMAARM_DMAARM0;
      memcpy(&rf_packet[0],packet,PAYLOAD_BYTES);        

      RFST = RFST_STX;
      
      RFIF &= ~RFIF_IM_DONE;      
}

void radio_init(void) {
  static __xdata struct cc_dma_channel dma0 = {(unsigned int)rf_packet & 0xFF00, (unsigned int)rf_packet & 0x00FF,
                                       0xDF, 0xD9,//(unsigned int)&RFD & 0x00FF  , 
                                       0, PAYLOAD_BYTES,
                                       DMA_CFG0_WORDSIZE_8 | DMA_CFG0_TMODE_SINGLE | DMA_CFG0_TRIGGER_RADIO,
                                       DMA_CFG1_SRCINC_1 | DMA_CFG1_DESTINC_0  | DMA_CFG1_PRIORITY_NORMAL};
/* RF settings SoC
  Base frequency = 891
 Carrier frequency = 891 
 Modulated = true 
 Modulation format = GFSK 
 Manchester enable = false 
 Sync word qualifier mode = 15/16 sync word bits detected, no carrier sense
 Preamble count = 2   
 Channel spacing = 199.951172 
 Carrier frequency = 914.999969 
 Data rate = 4.00448 
 RX filter BW = 58.035714 
  = Normal mode 
 Length config = Variable packet length mode. Packet length configured by the first byte after sync word 
 CRC enable = true 
 Packet length = 255 
 Device address = 0 
 Address config = No address check 
  = false 
 PA ramping = false 
 TX power = 10 

 */

ADDR      =     0x00;       // Device Address 
MCSM2     =     0x07;       // Main Radio Control State Machine Configuration 
MCSM1     =     0x30;       // Main Radio Control State Machine Configuration 
MCSM0     =     0x18;       // Main Radio Control State Machine Configuration 
FOCCFG    =     0x17;       // Frequency Offset Compensation Configuration 
BSCFG     =     0x6C;       // Bit Synchronization Configuration 
AGCCTRL2  =     0x03;       // AGC Control 
AGCCTRL1  =     0x40;       // AGC Control 
AGCCTRL0  =     0x91;       // AGC Control 
SYNC1     =     0xD3;       // Sync Word, High Byte 
SYNC0     =     0x91;       // Sync Word, Low Byte 
PKTLEN    =     PAYLOAD_BYTES;       // Packet Length 
PKTCTRL1  =     0x04;       // Packet Automation Control 
PKTCTRL0  =     0x04;       // Packet Automation Control 
TEST2     =     0x81;       // Various Test Settings 
TEST1     =     0x35;       // Various Test Settings 
TEST0     =     0x09;       // Various Test Settings 
PA_TABLE7 =     0x00;       // PA Power Setting 7 
PA_TABLE6 =     0x00;       // PA Power Setting 6 
PA_TABLE5 =     0x00;       // PA Power Setting 5 
CHANNR    =     0x00;       // Channel Number 
PA_TABLE4 =     0x00;       // PA Power Setting 4 
FSCTRL1   =     0x06;       // Frequency Synthesizer Control 
FSCTRL0   =     0x00;       // Frequency Synthesizer Control 
FREQ2     =     0x22;       // Frequency Control Word, High Byte 
FREQ1     =     0x44;       // Frequency Control Word, Middle Byte 
FREQ0     =     0xEC;       // Frequency Control Word, Low Byte 
MDMCFG4   =     0xF7;       // Modem configuration 
MDMCFG3   =     0x43;       // Modem Configuration 
FREND1    =     0x56;       // Front End RX Configuration 
FREND0    =     0x10;       // Front End TX Configuration 
FSCAL3    =     0xE9;       // Frequency Synthesizer Calibration 
FSCAL2    =     0x2A;       // Frequency Synthesizer Calibration 
FSCAL1    =     0x00;       // Frequency Synthesizer Calibration 
FSCAL0    =     0x1F;       // Frequency Synthesizer Calibration 
MDMCFG2   =     0x15;       // Modem Configuration 
MDMCFG1   =     0x02;       // Modem Configuration 
MDMCFG0   =     0xF8;       // Modem Configuration 
DEVIATN   =     0x24;       // Modem Deviation Setting 
PA_TABLE3 =     0x00;       // PA Power Setting 3 
PA_TABLE2 =     0x00;       // PA Power Setting 2 
PA_TABLE1 =     0x00;       // PA Power Setting 1 
PA_TABLE0 =     0xC2;       // PA Power Setting 0 
IOCFG2    =     0x00;       // Radio Test Signal Configuration (P1_7) 
IOCFG1    =     0x00;       // Radio Test Signal Configuration (P1_6) 
IOCFG0    =     0x00;       // Radio Test Signal Configuration (P1_5) 

  RFST = RFST_SIDLE;
  
  //Set up MAC timer
  T2CT = 0xFF;  //don't start right away...
  T2PR = 22;
  T2CTL = T2CTL_INT | T2CTL_TIG | T2CTL_TIP_64;
  IEN1 |= IEN1_T2IE;

 

  DMA0CFGH = (unsigned int)&dma0 >> 8;
  DMA0CFGL = (unsigned int)&dma0;
  
                                    

  //

}
