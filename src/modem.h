// Headphone jack modem
// Henry Hallam 2011


// Configurable constants

// Set the modem bitrate here:
#define MODEM_T1_PERIOD 4000
// MODEM_T1_PERIOD = ((fRef / global timer prescaler) / ( 6 * bit rate)) - 1
//          Note: global timer prescaler defined in CLKCON.TICKSPD
// e.g. ((24 MHz / 2) / (6 * 1200)) - 1 = 1665.7
// Round to an integer and ensure this value is < 65536.
// Because of high clock information density in manchester encoding, large
// mismatches in bit rates are tolerated (up to 16% I think, maybe more?)

// Maximum packet size
#define MODEM_TX_BUFF_SIZE 40

// Pin definitions
#define MODEM_TX_PIN P0_1
#define MODEM_TX_PIN_DIR_SETUP P0DIR |= 1<<1


// General modem functions
void modem_init(void);

// Modem transmit API

// To send a packet: shove it in modem_tx_packet, and call modem_tx_send
// modem_tx_send(0); will continuously transmit preambles.
extern char modem_tx_packet[];
void modem_tx_send(uint8_t packet_size);

char modem_tx_still_sending();

// Modem receive API
// (not yet)

// The following declaration is here because SDCC manual section 3.9.1 says:
// "... a prototype of the isr MUST be present or included in the file that contains the function main."
void t1_isr(void)  __interrupt (9) __using (2);

