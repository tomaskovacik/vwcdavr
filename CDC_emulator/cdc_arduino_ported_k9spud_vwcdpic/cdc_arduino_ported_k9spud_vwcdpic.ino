/*****************************************************************************


   Audi/Volkswagen Audi Interface

   Target: arduino atmega328(tested)


   Author: Vincent

   Date: January, 2011

   Version: 0.01



   Originally ported from VWCDPIC Project:

   http://www.k9spud.com/vwcdpic/



   Ported from Matthias Koelling's AVR Code:

   www.mikrocontroller.net/attachment/31279/vwcdc.c

   ---------------------------------
   16. Mar 2015:

   https://github.com/tomaskovacik/
   arduino duemilanove works:

   RADIO PIN -> arduino pin

   DataOut   -> digital 8 (ICP1)
   DataIn    -> digital 11(PB3)
   Clock     -> digital 13(PB5)

   computer -> arduino pin
   serial TX -> digital 0 (PD0)
   serial RX -> digital 1 (PD1)

   19. Mar 2015  tomaskovacik
   port to atmega8

   14.Jun 2019 kovo
   this compile fine against mega324(P/A/PA/PB)

 *****************************************************************************/

//#define DUMPMODE
//#define DUMPMODE2

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega128__)
#define TCCR2A TCCR2
#define TCCR2B TCCR2
#define COM2A1 COM21
#define COM2A0 COM20
#define OCR2A OCR2
#define TIMSK2 TIMSK
#define OCIE2A OCIE2
#define TIMER2_COMPA_vect TIMER2_COMP_vect
#define TIMSK1 TIMSK
#define TIFR1 TIFR
#define ICIE1 TICIE1
#define TIMSK0 TIMSK
#define TIFR2 TIFR
#define OCF2A OCF2
#define TCCR0B TCCR0
#endif

// uncoment this to have original serial outputs for PJRC players
//#define PJRC

/* enable hex control command on serial line to control mpd control
   script with shyd.de control script
*/
//#define JUST_HEX_TO_SERIAL

/* enable bluetooth module control over serial line
   XS3868
*/
#define BLUETOOTH

/*
   read disc# track# status over serial line
*/
//#define DISC_TRACK_NUMBER_FROM_MPD

/* -- Includes ------------------------------------------------------------- */

//#include <avr/io.h>
//#include <avr/sfr_defs.h>
//#include <stdlib.h>
//#include <avr/interrupt.h>
//#include <avr/pgmspace.h>
//#include <util/delay.h>

/* -- Defines -------------------------------------------------------------- */

#define TRUE 1

// Low period of PWTX line:
// 0: ~650us
// 1: ~1.77ms
// S: ~4.57ms

// one tick is 0.5µs
#define STARTTHRESHOLD  6400            // greater than this signifies START bit
#define HIGHTHRESHOLD   2496       // greater than this signifies 1 bit.
#define LOWTHRESHOLD     512        // greater than this signifies 0 bit.
#define PKTSIZE          -32            // command packets are 32 bits long.

// do not refresh head unit faster than 5.5ms (currently not implemented)
// 5.24s slow refresh rate when head unit in FM/AM/Tape mode (not implemented)

//#define REFRESH_PERIOD   50        // default to refresh head unit every 50.0ms
#define SECONDWAIT      -20         // wait 20 * 50ms to get 1 second (50ms*20 = 1000ms = 1s)
#define POWERIDENTWAIT  -10         // wait 10 * 1s to 10 seconds between VWCDPICx.x display
#define SCANWAIT        -100             // wait 100 * 50ms to get 5sec (50ms*100 = 5000ms = 5s)
#define _10MS           156
#define _50MS            500
#define _700US            187//175 //4µs x 175 = 700µs; 4*187=748
#define CLK_DELAY 40 //40:using _delay_loop_1 @16Mhz 1tick = .18750us ,40 => 7.5us(8.5us@16MHz on scope)  ; 10=>1.8us

#define TX_BUFFER_END   12
#define CAP_BUFFER_END	24

#define VER_MAJOR       '1'
#define VER_MINOR       '0'
#define VER_PATCHLEVEL  'b'

#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328A__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PA__) || defined(__AVR_ATmega328PB__)
#define RADIO_COMMAND      PB0 //ICP
#define RADIO_COMMAND_DDR  DDRB
#define RADIO_COMMAND_PORT  PORTB
#define RADIO_COMMAND_PIN PINB
#define RADIO_CLOCK        PD4//PB5
#define RADIO_CLOCK_DDR    DDRD//DDRB
#define RADIO_CLOCK_PORT  PORTD//PORTB
#define RADIO_DATA         PC3//PB3
#define RADIO_DATA_DDR     DDRC//DDRB
#define RADIO_DATA_PORT  PORTC//PORTB
#define RADIO_ACC 3 // PD3 = INT1
#endif



#if defined(__AVR_ATmega324__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__)
#define RADIO_COMMAND      PD6 //ICP 
#define RADIO_COMMAND_DDR  DDRD
#define RADIO_COMMAND_PORT  PORTD
#define RADIO_COMMAND_PIN PIND
#define RADIO_CLOCK        PB7
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT  PORTB
#define RADIO_DATA         PB5
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT  PORTB
#define RADIO_ACC 3
#endif


// Command Codes
// -------------
//
// First byte is always 53
// Second byte is always 2C
// Third and Fourth bytes always add up to FF
// All command codes seem to be a multiple of 4.
//
//
// 53 2C 0C F3 CD 1
// 53 2C 10 EF DISABLE
// 53 2C 14 EB Change CD (ignored)
// 53 2C 18 E7 Previous CD (only on Audi Concert <)
// 53 2C 2C D3 CD 5 (first packet)
// 53 2C 38 C7 Change CD/Next CD (aka MINQUIRY)
// 53 2C 4C B3 CD 3 (first packet)
// 53 2C 58 A7 Seek Back Pressed
// 53 2C 60 9F Mix 1
// 53 2C 68 97 Up on Mk3 premium (Adam Yellen)
// 53 2C 78 87 Dn
// 53 2C 8C 73 CD 2 (first packet)
// 53 2C A0 5F Scan
// 53 2C A4 5B something to do with power on (Audi Concert)
// 53 2C A8 57 Dn on Mk3 premium (Adam Yellen <adam@yellen.com>)
// 53 2C AC 53 CD 6 (first packet)
// 53 2C CC 33 CD 4 (first packet)
// 53 2C D8 27 Seek Forward Pressed
// 53 2C E0 1F Mix 6
// 53 2C E4 1B ENABLE
// 53 2C F8 07 Up


//#define  Do_PLAY           0x08  // mix button held down (RCD300 head unit only)
#define  Do_PLAY           0x03  // mix button held down (RCD300 head unit only)
#define  Do_LOADCD	  0x01	// not used
#define  Do_ENABLE_MK      0x08  // mk concert1
#define  Do_CD1            0x0C  // CD 1
#define  Do_DISABLE        0x10  // DISABLE
#define  Do_CHANGECD       0x14  // Change CD (changer ignores & no ACK)
#define  Do_PREVCD         0x18  // PREVIOUS CD (Audi Concert head unit)
#define  Do_CD5            0x2C  // CD 5
#define  Do_TP             0x30  // TP info (TP button pressed)
#define  Do_SEEKFORWARD_MK 0x38  // mk concert 1 LOAD CD (aka MINQUIRY).
// Also means "Next CD" if no CD button pressed
#define  Do_CD3            0x4C  // CD 3
#define  Do_SEEKBACK       0x58  // SEEK BACK
#define  Do_MIX_CD         0x60  // MIX 1 (mix tracks within one disc)
#define  Do_UP_MK3         0x68  // UP (Mk3 head unit)
#define  Do_DOWN           0x78  // DOWN
#define  Do_CD2            0x8C  // CD 2
#define  Do_SCAN           0xA0  // SCAN
#define  Do_UNKNOWNCMD     0xA4  // power on CD mode?? (Audi Concert head unit)
#define  Do_DOWN_MK3       0xA8  // DOWN (Mk3 head unit only)
#define  Do_CD6            0xAC  // CD 6
#define  Do_CD4            0xCC  // CD 4
#define  Do_SEEKFORWARD    0xD8  // Seek Forward
#define  Do_MIX            0xE0  // MIX 6 (mix tracks across all discs)
#define  Do_ENABLE         0xE4  // ENABLE
#define  Do_UP             0xF8  // UP

enum STATES
{
  StateIdle,
  StateIdleThenPlay,
  StateInitPlay,
  StateInitPlayEnd,
  StateInitPlayAnnounceCD,
  StatePlayLeadIn,
  StatePlayLeadInEnd,
  StatePlayLeadInAnnounceCD,
  StateTrackLeadIn,
  StatePlay,
  StateTP
};

/* -- Extern Global Variables ---------------------------------------------- */

#ifdef BLUETOOTH
const uint8_t sBK8000L_PAIRING_INIT[] PROGMEM = "AT+CA\r\n"; //  pairing   AT+CA\r\n
const uint8_t sBK8000L_PAIRING_EXIT[] PROGMEM = "AT+CB\r\n"; //  Exit pairing  AT+CB\r\n
const uint8_t sBK8000L_CONNECT_LAST_DEVICE[] PROGMEM = "AT+CC\r\n"; //  The last paired device connected  AT+CC\r\n     what this should do? connect to last connected device?
const uint8_t sBK8000L_DISCONNECT[] PROGMEM = "AT+CD\r\n";

const uint8_t sDATAERR[] PROGMEM = "";
const uint8_t sOVERFLOW[] PROGMEM = "";
const uint8_t sMDISABLE[] PROGMEM = "AT+MC\r\n";
const uint8_t sMENABLE[] PROGMEM = "AT+MA\r\n";
const uint8_t sMINQUIRY[] PROGMEM = "";
const uint8_t sPRV_LIST[] PROGMEM = "AT+MA\r\n";
const uint8_t sNXT_LIST[] PROGMEM = "AT+MA\r\n";
const uint8_t sLIST1[] PROGMEM = "AT+MA\r\n";
const uint8_t sLIST2[] PROGMEM = "AT+MA\r\n";
const uint8_t sLIST3[] PROGMEM = "AT+MA\r\n";
const uint8_t sLIST4[] PROGMEM = "AT+MA\r\n";
const uint8_t sLIST5[] PROGMEM = "AT+CE\r\n";
const uint8_t sLIST6[] PROGMEM = "AT+CF\r\nAT+CG\r\n";
const uint8_t sRANDOM[] PROGMEM = "\r\n";
const uint8_t sPLAY[] PROGMEM = "AT+MA\r\n";
const uint8_t sSCAN[] PROGMEM = "SCAN\r\n";
const uint8_t sSTOP[] PROGMEM = "AT+MA\r\n";
const uint8_t sNEXT[] PROGMEM = "AT+MD\r\n";
const uint8_t sPREVIOUS[] PROGMEM = "AT+ME\r\n";
const uint8_t sRING[] PROGMEM = "";
const uint8_t sIDENTIFY[] PROGMEM = "";
const uint8_t sNEWLINE[] PROGMEM = "";
const uint8_t sDASH[] PROGMEM = "";
const uint8_t sHEX[] PROGMEM = "";
const uint8_t sVERSION[] PROGMEM = "";
#else
const uint8_t sBK8000L_PAIRING_INIT[] PROGMEM = "";
const uint8_t sBK8000L_PAIRING_EXIT[] PROGMEM = "";
const uint8_t sBK8000L_CONNECT_LAST_DEVICE[] PROGMEM = "";
const uint8_t sBK8000L_DISCONNECT[] PROGMEM = "";
const uint8_t sDATAERR[] PROGMEM = "dataerr\r\n";
const uint8_t sOVERFLOW[] PROGMEM = "overflow\r\n";
const uint8_t sMDISABLE[] PROGMEM = "MDISABLE\r\n";
const uint8_t sMENABLE[] PROGMEM = "MENABLE\r\n";
const uint8_t sMINQUIRY[] PROGMEM = "MINQUIRY\r\n";
const uint8_t sPRV_LIST[] PROGMEM = "PRV_LIST\r\n";
const uint8_t sNXT_LIST[] PROGMEM = "NXT_LIST\r\n";
const uint8_t sLIST1[] PROGMEM = "LIST1\r\n";
const uint8_t sLIST2[] PROGMEM = "LIST2\r\n";
const uint8_t sLIST3[] PROGMEM = "LIST3\r\n";
const uint8_t sLIST4[] PROGMEM = "LIST4\r\n";
const uint8_t sLIST5[] PROGMEM = "LIST5\r\n";
const uint8_t sLIST6[] PROGMEM = "LIST6\r\n";
const uint8_t sRANDOM[] PROGMEM = "RANDOM\r\n";
const uint8_t sPLAY[] PROGMEM = "PLAY\r\n";
const uint8_t sSCAN[] PROGMEM = "SCAN\r\n";
const uint8_t sSTOP[] PROGMEM = "STOP\r\n";
const uint8_t sNEXT[] PROGMEM = "NEXT\r\n";
const uint8_t sPREVIOUS[] PROGMEM = "PREVIOUS\r\n";
const uint8_t sRING[] PROGMEM = "RING\r\n";
const uint8_t sIDENTIFY[] PROGMEM = "Audi Concert I Multimedia Gateway Ver.";
const uint8_t sNEWLINE[] PROGMEM = "\r\n";
const uint8_t sDASH[] PROGMEM = "_";
const uint8_t sHEX[] PROGMEM = {
  '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0, '9', 0, 'A', 0, 'B', 0, 'C', 0, 'D', 0, 'E', 0, 'F', 0
};
const uint8_t sVERSION[] PROGMEM = {
  VER_MAJOR, '.', VER_MINOR, VER_PATCHLEVEL, 0
};
#endif

uint8_t sendreg;
uint8_t sendbitcount; // Used in SendByte routine

uint8_t disc = 1;
uint8_t track = 1;
uint8_t minute = 0;
uint8_t second = 0;
uint8_t leds = 0;
uint8_t scanptr; // pointer to command byte to inspect next
uint8_t fsr;
uint8_t scanbyte; // most recently retrieved command byte
uint8_t cmdcode; // command code received from head unit (byte 3)

// these storage bytes are for the ISR to save process state so that it
// doesn't adversely affect the main loop code. you must -not- use these
// variables for anything else, as the ISR -will- corrupt them.

uint8_t intwsave;
uint8_t intstatussave;
uint8_t intfsrsave;

// the 'capbusy' flag will be set when the ISR is busy capturing a command
// packet from the head unit. The TMR0 ISR will clear it once the recieve
// timeout has been exceeded or the PWTX capture ISR will clear it once
// 32 bits have been captured (command packets are supposed to be 32 bits
// long only).

uint8_t capbusy;

// 'mix' and 'scan' flags specify whether we want to light up the MIX light
// or SCAN display on the head unit.

uint8_t mix;
uint8_t scan;
uint8_t playing;
uint8_t cd_button = 0;
uint8_t mix_button = 0;
// The ISR will set 'dataerr' flag if it detected a framing error
// or 'overflow' if it overflowed the capture buffer queue.
uint8_t overflow;
uint8_t dataerr;

#ifdef DUMPMODE
// The ISR will set 'startbit' flag if it detected a start bit.
uint8_t startbit;
#endif



uint16_t captime; // timer count of low pulse (temp)
int8_t capbit; // bits left to capture for this byte
int8_t capbitpacket; // bits left to capture for the entire packet
uint8_t capptr; // pointer to packet capture buffer loc

uint8_t BIDIstate; // pointer to the current state handler routine
int8_t BIDIcount; // counts how long to stay in current state
uint8_t ACKcount; // number of ACK bits to set.
uint8_t discload; // next disc number to load

int8_t poweridentcount; // counts down until we should send VWCDPICx.x
uint8_t secondcount; // counts down until one second has passed
int8_t scancount; // used to count down displaying SCAN mode

uint8_t txinptr;
uint8_t txoutptr;
uint8_t display_byte_buffer_mau8[8]; // holds display bytes sent to the head unit
uint8_t const *txbuffer[TX_BUFFER_END]; // 39-26-1 = 12 serial output strings queue
uint8_t capbuffer[CAP_BUFFER_END]; // 64-39-1 = 24 bytes for head unit command
uint8_t counter_10ms_u8 = 0; // counter for 10ms intervals
uint8_t flag_50ms = false; // indicates that a period of 50ms isover
#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega128__)
uint8_t counter_timer0_overflows = 0; //timer0 overflow counts to calc 10ms
#endif

/* -- Modul Global Function Prototypes ------------------------------------- */

static void ScanCommandBytes(void);
static void DumpFullCommand(void);
static void DecodeCommand(void);
static uint8_t GetCaptureByte(void);
static void SetStateIdle(void);
static void SetStatePlay(void);
static void SetStateInitPlay(void);
static void SetStatePlayLeadIn(void);
static void SetStateTrackLeadIn(void);
static void SendDisplayBytes(void);
static void SendDisplayBytesNoCD(void);
static void SendDisplayBytesInitCD(void);
static void SendFrameByte(uint8_t byte_u8);
static void SendByte(uint8_t byte_u8);
static void EnqueueString(const uint8_t *addr PROGMEM);
static void EnqueueHex(uint8_t hexbyte_u8);
static void ResetTime(void);
static void SetStateIdleThenPlay(void);
static void SetStateTP(void);
static void SendStateIdle(void);
static void SendStatePlayLeadInEnd(void);
static void SendPacket(void);
static void SendStateInitPlayEnd(void);
static void SendStateInitPlayAnnounceCD(void);
static void SendStatePlayLeadInAnnounceCD(void);
static void SendStateTP(void);
static void printstr_p(const char *s);
static uint8_t cdButtonPushed(uint8_t cdnumber);
#define TRUE 1
#define FALSE 0

/* -- Implementation Functions --------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
  \brief     Init_VWCDC
  initialization for cdc protocol
  \author     Koelling
  \date       26.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

void Init_VWCDC(void)
{
  cli();  
  TIMSK0 = 0x00; //on arduino timer0 is used for millis(), we change prescaler, but also need to disable overflow interrupt
  RADIO_CLOCK_DDR |= _BV(RADIO_CLOCK);
  RADIO_DATA_DDR  |= _BV(RADIO_DATA);
  RADIO_COMMAND_DDR &= ~_BV(RADIO_COMMAND); // input capture as input
  RADIO_COMMAND_PORT |= _BV(RADIO_COMMAND); // enable pull up

  //Timer1 init
  //Used for timing the incoming commands from headunit
  TCCR1A = 0x00; // Normal port operation, OC1A/OC1B/OC1C disconnected
  TCCR1B = _BV(ICNC1); // noise canceler, int on falling edge
  TCCR1B |= _BV(CS11); // prescaler = 8 -> 1 timer clock tick is 0.5µs long
  TIFR1 |= _BV(ICF1); // clear pending interrupt
  TIMSK1 |= _BV(ICIE1); // enable input capture interrupt on timer1

  //Timer 2 Init
  //Timer 2 used to time the intervals between package bytes
  OCR2A = _700US; // 4µs x 175 = 700µs
  TCCR2A = _BV(WGM21); // Timer2 in CTC Mode
  TCCR2B = _BV(CS22); // prescaler = 64 -> 1 timer clock tick is 4us long
  TIMSK2 |= _BV(OCIE2A);
  
  capptr = 0; // indirect pointer to capture buffer
  scanptr = 0;
  capbit = -8;
  txinptr = 0; // queue pointers
  txoutptr = 0;

  capbusy = FALSE; // reset flags
  mix = FALSE;
  scan = FALSE;
  playing = FALSE;
  overflow = FALSE;
  dataerr = FALSE;

#ifdef DUMPMODE
  startbit = FALSE;
#endif
  ACKcount = 0;

  // these values can be set depending on the state of mp3
  // it has to be evaluated wether CD number can be grater than 6
  disc = 0x41; // CD 1
  track = 0x01; // track 1

  poweridentcount = POWERIDENTWAIT;

  ResetTime();
  SetStateIdle(); //start at idle!!! no idle then play!!

  EnqueueString(sIDENTIFY);
  EnqueueString(sVERSION);
  EnqueueString(sNEWLINE);
  EnqueueString(sRING);

  //Timer 0 init
  //Timer 0 used to time the interval between each update
#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega128__)
  TIMSK |= _BV(TOIE0); //no CTc mode, we must overflow
  TCCR0B |= _BV(CS00) | _BV(CS01); // prescaler = 64 -> timer clock tick is 4us long
  //timer0 overflow after 256counts, oveflow interup routine is fired every 1024us (4*256)
  //we need 50 overflows to count to 50ms
#define _TIMER0_OVERFLOW_COUNTS 50

#else
  OCR0A = _10MS; // 10ms Intervall
  TCCR0A = 0x00; // Normal port operation, OC0 disconnected
  TCCR0A |= _BV(WGM01); // CTC mode
  TCCR0B |= _BV(CS00) | _BV(CS02); // prescaler = 1024 -> 1 timer clock tick is 64us long
  TIMSK0 |= _BV(OCIE0A); // enable output compare interrupt on timer0
#endif

  SendPacket(); // force first display update packet
  sei();
  //    SREG |= 0x80;   // enable interrupts
}

//-----------------------------------------------------------------------------
/*!
  \brief    ISR(TIMER2_COMPA_vect)
  Timer2 ensures 700µs timing between display package bytes
  Shift bytes out to head unit
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------

ISR(TIMER2_COMPA_vect)
{
  static uint8_t display_byte_counter_u8 = 0;
  uint8_t byte_u8;
  //TCCR2B &= ~_BV(CS20); //set to 0 already
  //TCCR2B &= ~_BV(CS21); //set to 0 already
  TCCR2B &= ~_BV(CS22); // stop Timer2, CS22 was set, prescaler 64
  //TCNT2 = 0; // clear Timer2
  //TIFR2 |= _BV(OCF2A); //no need to do this

  if (display_byte_counter_u8 < 8)
  {
    byte_u8 = display_byte_buffer_mau8[display_byte_counter_u8];

#ifdef DUMPMODE2
    Serial.print("|");
    Serial.print(byte_u8, HEX);
    Serial.print("|");
#endif

    for (sendbitcount = -8; sendbitcount != 0; sendbitcount++)
    {
      RADIO_CLOCK_PORT |= _BV(RADIO_CLOCK); // SCLK high
      _delay_loop_1(CLK_DELAY);
      if ((byte_u8 & 0x80) == 0) // mask highest bit and test if set
      {
        RADIO_DATA_PORT |= _BV(RADIO_DATA); // DATA high
      }
      else
      {
        RADIO_DATA_PORT &= ~_BV(RADIO_DATA); // DATA low
      }
      //_delay_loop_1(CLK_DELAY);
      byte_u8 <<= 1; // load the next bit
      RADIO_CLOCK_PORT &= ~_BV(RADIO_CLOCK); // SCLK low
      _delay_loop_1(CLK_DELAY);
    }

    display_byte_counter_u8++;

    TCCR2B |= _BV(CS22); // prescaler = 64 -> 1 timer clock tick is 4us long
    //TCCR2B |= _BV(CS20);
  }
  else
  { //display_byte_counter_u8 is ==8
#ifdef DUMPMODE2
    Serial.println();
#endif
    display_byte_counter_u8 = 0;
    //TIMSK2 &= ~_BV(OCIE2A); // disable output compare interrupt on timer2
  }
}

//-----------------------------------------------------------------------------
/*!
  \brief     ISR(TIMER0_COMP_vect)
  on atmega8 overflow is used
  timer0 output compare interrupt service routine for cdc protocol
  radio display update
  \author     Koelling
  \date       04.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega128__)

ISR(TIMER0_OVF_vect) {
  counter_timer0_overflows++;

  if (counter_timer0_overflows = _TIMER0_OVERFLOW_COUNTS )
  {
    counter_timer0_overflows = 0;
    flag_50ms = TRUE;
  }
  TIFR &= ~(1 << TOV0);
  TCNT0 = 5;// to get overflowed after 1000us 1024-(6x4us)=1000us  counting from 0... 0,1,2,3,4,5 = 6ticks
}

#else

ISR(TIMER0_COMPA_vect)
{
  counter_10ms_u8++;

  if (counter_10ms_u8 == 5)
  {
    counter_10ms_u8 = 0;
    flag_50ms = TRUE;

  }
}
#endif

//-----------------------------------------------------------------------------
/*!
  \brief     ISR(TIMER1_OVF_vect)
  timer1 overflow interrupt service routine for cdc protocol
  \author     Koelling
  \date       26.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

ISR(TIMER1_OVF_vect)
{
  TIMSK1 &= ~_BV(TOIE1); // disable further timer 1 interrupts
  capbusy = FALSE; // set flag signifying packet capture done

  if (capbit > -8) // are we already capturing on a blank byte?
  {
    dataerr = TRUE;
    // Note: This should never happen on normal head unit sending 32 bit
    //        command strings with error free data.
    //
    // if the capture bits were not a complete 8 bits, we need to finish
    // rotating the bits upward so that the data is nicely formatted

    while (capbit != 0) // have we finished rotating all bits up?
    {
      capbuffer[capptr] <<= 1; // rotate in 0 bit
      capbit++;
    }

    capbit = -8;
    capptr++; // move to new capture byte

    if (capptr == CAP_BUFFER_END) // have we gone past the end of the
    { // capture buffer?
      capptr = 0; // yes, roll over to beginning
    }

    if (capptr == scanptr) // have we overflowed the capture queue?
    {
      overflow = TRUE; // yes, set error flag
    }
  }
}



//-----------------------------------------------------------------------------
/*!
  \brief     ISR(TIMER1_CAPT_vect)
  input capture interrupt service routine for cdc protocol
  \author     Koelling
  \date       26.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

ISR(TIMER1_CAPT_vect)
{
  captime = ICR1; // save a copy of current TMR1 count
  // in case PWTXCaptureBit needs it
  TCNT1 = 0; // clear timer1

  if ((RADIO_COMMAND_PIN & _BV(RADIO_COMMAND)) == 0)
  {
    // We have interrupted at beginning of low pulse (falling edge)
    // Low pulse length must be timed to determine bit value
    TIFR1 |= _BV(TOV1); // clear timer1 overflow flag
    TIMSK1 |= _BV(TOIE1); // enable timer1 interrupt on overflow
    TCCR1B |= _BV(ICES1); // change input capture to rising edge
    TIFR1 |= _BV(ICF1); // clear input capture interrupt request flag
  }
  else
  {
    // We have interrupted at beginning of high pulse (rising edge)
    // High pulse length doesn't matter. We need to check out
    // captured low pulse width if we are capturing data at the moment

    capbusy = TRUE;
    TCCR1B &= ~_BV(ICES1); // change input capture to falling edge
    TIFR1 |= _BV(ICF1); // clear input capture interrupt request flag

    if (TIMSK1 & _BV(TOIE1)) // are we trying to capture data?
    {
      capbusy = TRUE;
      TIMSK1 &= ~_BV(TOIE1); // turn off capturing time for high pulse

      if (captime > STARTTHRESHOLD)
      { // yes, start bit
#ifdef DUMPMODE
        startbit = TRUE;
#endif
        capbitpacket = PKTSIZE;

        // don't store start bits, just frame around them
        if (capbit > -8) // are we already capturing on a blank byte?
        {
          dataerr = TRUE;
          // Note: This should never happen on normal head unit sending 32 bit
          //       command strings with error free data.
          //
          // if the capture bits were not a complete 8 bits, we need to finish
          // rotating the bits upward so that the data is nicely formatted

          while (capbit != 0) // have we finished rotating all bits up?
          {
            capbuffer[capptr] <<= 1; // rotate in 0 bit
            capbit++;
          }
          capbit = -8;
          capptr++; // move to new capture byte

          if (capptr == CAP_BUFFER_END) // have we gone past the end of the
          { // capture buffer?
            capptr = 0; // yes, roll over to beginning
          }

          if (capptr == scanptr) // have we overflowed the capture queue?
          {
            overflow = TRUE; // yes, set error flag
          }
        }
      }
      else
      { // no, just a regular data bit
        if (captime > LOWTHRESHOLD)
        { // yes, go ahead and store this data
          capbuffer[capptr] <<= 1; // nope

          if (captime > HIGHTHRESHOLD)
          {
            capbuffer[capptr] |= 1;
          }

          capbitpacket++;

          if (capbitpacket == 0)
          {
            // we've received PKTSIZE number of bits, so let's assume that we're done
            // capturing bits for now.
            capbusy = FALSE; // clear capture busy flag
          }

          capbit++;

          if (capbit == 0) // have we collected all 8 bits?
          { // yep, get ready to capture next 8 bits
            capbit = -8;
            capptr++; // move to new capture byte

            if (capptr == CAP_BUFFER_END) // have we gone past the end of the
            { // capture buffer?
              capptr = 0; // yes, roll over to beginning
            }

            if (capptr == scanptr) // have we overflowed the capture queue?
            {
              overflow = TRUE; // yes, set error flag
            }
          }
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/*!
  \brief     CDC_Protocol(void)
  cyclic called main program for cdc protocol (50ms?)
  \author     Koelling
  \date       26.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

void CDC_Protocol(void)
{
  uint8_t decimal_adjust_u8;

  if (flag_50ms == TRUE)
  {
    flag_50ms = FALSE;
        SendPacket()  ;
    scancount++;
    if (scancount == 0)
    {
      scancount = SCANWAIT;
      scan = FALSE; // turn off scan display
      leds &= 0xEF;
    }
    secondcount++;
    if (secondcount == 0)
    {
      secondcount = SECONDWAIT;
      poweridentcount++;

      if (poweridentcount == 0)
      {
        poweridentcount = POWERIDENTWAIT;
        EnqueueString(sIDENTIFY);
        EnqueueString(sVERSION);
        EnqueueString(sNEWLINE);
      }

      second++; // increment the time display
      decimal_adjust_u8 = second & 0x0F; // skip past hexidecimal codes
      if (decimal_adjust_u8 == 0x0A) // are with at xA?
      {
        second += 6; // yes, add 6 and we'll be at x0 instead
      }

      if (second == 0x60)
      {
        second = 0;
        minute++;
        decimal_adjust_u8 = minute & 0x0F; // skip past hexidecimal codes
        if (decimal_adjust_u8 == 0x0A) // are with at xA?
        {
          minute += 6; // yes, add 6 and we'll be at x0 instead
        }

        if (minute == 0xA0) // have we gone beyond 99 minutes?
        {
          minute = 0;
        }
      }
    }
  }

  if (overflow == TRUE) // has the command receive code detected
  { // an overflow error?
    overflow = FALSE; // clear error flag
    EnqueueString(sOVERFLOW);
  }

  if (dataerr == TRUE) // has the command receive code detected
  { // a framing type data error?
    dataerr = FALSE; // clear error flag
    EnqueueString(sDATAERR);
  }

#ifndef DUMPMODE
  ScanCommandBytes();
#else

  if (startbit == TRUE) // have we just recieved a start bit?
  {
    startbit = FALSE;
    EnqueueString(sNEWLINE); // yes, start a new line
  }

  fsr = scanptr;

  while (GetCaptureByte() == TRUE)
  {
    scanptr = fsr;
    EnqueueHex(scanbyte);
  }

#endif

  while (txoutptr != txinptr)
  {
    printstr_p((char*) txbuffer[txoutptr]);
    txoutptr++;
    if (txoutptr == TX_BUFFER_END)
    {
      txoutptr = 0;
    }
  }
}

//-----------------------------------------------------------------------------
/*!
  \brief    void DecodeCommand(void)
  decode cmdcode and do required actions
  ;--------------------------------------------------------------------------
  ; Button Push Packets
  ;--------------------------------------------------------------------------
  ; 532C609F Mix 1
  ; 532CE01F Mix 6
  ; 532CA05F Scan
  ;     Note: Blaupunkt Gamma V head unit will continue to send scan key code
  ;       unless display is switched into scan mode.
  ;       (reported by tony.gilbert@orange.co.uk)
  ; 532C10EF Head Unit mode change. Emitted at power up, power down, and
  ;        any mode change. (disable playing)
  ; 532C58A7 Seek Back Pressed
  ; 532CD827 Seek Forward Pressed
  ; 532C7887 Dn
  ; 532CA857 Dn on Mk3 premium (Adam Yellen <adam@yellen.com>)
  ; 532CF807 Up
  ; 532C6897 Up on Mk3 premium (Adam Yellen)
  ; 532C38C7 CD Change (third packet)
  ; 532CE41B Seek Forward Released (enable playing)
  ; 532CE41B Seek Back Released (enable playing)
  ; 532CE41B CD Mode selected. Emitted at power up (if starting in CD
  ;            mode), change to CD mode. (enable playing)
  ; 532C14EB CD Change (second packet)
  ; 532C0CF3 CD 1 (first packet)
  ; 532C8C73 CD 2 (first packet)
  ; 532C4CB3 CD 3 (first packet)
  ; 532CCC33 CD 4 (first packet)
  ; 532C2CD3 CD 5 (first packet)
  ; 532CAC53 CD 6 (first packet)
  ;
  ; Monsoon State Changes:
  ; 532CE41B enable playing (transition to State 2)
  ; 532C38C7 disc loaded inquiry (transition to State 5)
  ; 532C10EF disable playing (transition to State 1)
  ;--------------------------------------------------------------------------
  \author     Koelling
  \date       05.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

static void DecodeCommand(void)
{
  uint8_t decimal_adjust_u8 = 0;

#ifdef JUST_HEX_TO_SERIAL
  Serial.write(cmdcode);
#endif

  switch (cmdcode) {
    case Do_CHANGECD:
      // Head unit seems to send this after each CDx number change
      // but the CD Changer seems to completely ignore (doesn't even ACK it).
      ACKcount = 0; // do not ack this command
#ifdef PJRC
      EnqueueString(sRANDOM);
#endif
      break;

    case Do_ENABLE:
    case Do_ENABLE_MK:
      mix = FALSE;
      if (playing == FALSE)
      {
        SetStateInitPlay(); // skip this if already playing
      }
      if (!mix_button)
        EnqueueString(sMENABLE);
      break;

    case Do_LOADCD:
      if (playing == TRUE)
      {
        SetStateInitPlay(); // skip this if we're in idle mode
      }
      ResetTime();
      EnqueueString(sMINQUIRY);
      break;

    case Do_DISABLE:
      SetStateIdle(); // skip this if we're already in idle mode
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      //      disc = 0x41; // set back to CD 1
#endif
      EnqueueString(sMDISABLE);
      break;

    case Do_SEEKBACK:
    case Do_PREVCD:
      ResetTime();
      EnqueueString(sPRV_LIST);
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc--;
      if ((disc & 0x0F) == 0)
      {
        disc = 0x46; // set back to CD 1
      }
#endif
      break;

    case Do_SEEKFORWARD:
    case Do_SEEKFORWARD_MK:
      ResetTime();
      if (cd_button == FALSE) // mk don't increment when previous command was a cd button
      {
        EnqueueString(sNXT_LIST);
#ifndef DISC_TRACK_NUMBER_FROM_MPD
        disc++;
        if (disc > 0x46)
        {
          disc = 0x41;
        }
#endif
        // Going beyond CD9 displays hex codes on premium head unit.
        // Examples: "CD A"
        //           "CD B"
        //           "CD C" etc...
        //
        // However, going beyond CD6 mutes audio on monsoon head unit, so we
        // definitely don't want to do that.
      }
      else
      {
        cd_button = FALSE; // mk clear cd button flag
      }
      break;

    case Do_MIX:
    case Do_MIX_CD:
      mix_button = 1;
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      if (mix == FALSE)
      {
        mix = TRUE;
      }
      else
      {
        mix = FALSE;
      }
#endif
      EnqueueString(sRANDOM);
      break;

    case Do_PLAY:
      EnqueueString(sPLAY); // this will make the PJRC play/pause
      break;

    case Do_SCAN:

      scancount = SCANWAIT;
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      if (scan == FALSE)
      {
        scan = TRUE;
      }
      else
      {
        scan = FALSE;

      }
#endif

#ifdef PJRC
      EnqueueString(sPLAY); // this will make the PJRC play/pause
#else
      EnqueueString(sSCAN);
#endif
      break;

    case Do_UP:
    case Do_UP_MK3:
      if (playing == TRUE) // skip track lead-in if not in play mode
      {
        SetStateTrackLeadIn();
      }
      ResetTime();
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      track++;
      decimal_adjust_u8 = track & 0x0F; // skip past hexidecimal codes
      if (decimal_adjust_u8 == 0x0A) // are with at xA?
      {
        track += 6; // yes, add 6 and we'll be at x0 instead
      }
      if (track == 0xA0) // have we gone beyond Track 99?
      { // yes, rollover to Track 01 so that jog wheels
        track = 1; // can continue rolling (Audi Concert II)
      }
#endif
      EnqueueString(sNEXT);
      break;

    case Do_DOWN:
    case Do_DOWN_MK3:
      if (playing == TRUE) // skip track lead-in if not in play mode
      {
        SetStateTrackLeadIn();
      }
      ResetTime();
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      decimal_adjust_u8 = track & 0x0F; // skip past hexidecimal codes
      if (decimal_adjust_u8 == 0) // are we at x0?
      {
        track -= 6; // yes, subtract 6 and we'll be at x9 instead
      }
      track--;
      if (track == 0) // have we gone below Track 1?
      { // yes, rollover to Track 99 so that jog wheels
        track = 0x99; // can continue rolling (Audi Concert II)
      }
#endif
      EnqueueString(sPREVIOUS);
      break;

    case Do_CD1:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x41; // set CD 1
#endif
      if (cdButtonPushed(1))
          EnqueueString(sLIST1);
      break;

    case Do_CD2:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x42; // set CD 2
#endif
      if (cdButtonPushed(1))
      EnqueueString(sLIST2);
      break;

    case Do_CD3:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x43; // set CD 3
#endif
      if (cdButtonPushed(3))
          EnqueueString(sLIST3);
      break;

    case Do_CD4:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x44; // set CD 4
#endif
      if (cdButtonPushed(4))
          EnqueueString(sLIST4);
      break;

    case Do_CD5:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x45; // set CD 5
#endif
      if (cdButtonPushed(5))
          EnqueueString(sLIST5);
      break;

    case Do_CD6:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x46; // set CD 6
#endif
      if (cdButtonPushed(6))
          EnqueueString(sLIST6);
      break;

    case Do_TP:
      if (playing == TRUE) {
        SetStateTP();
      } else {
        SetStateInitPlay();
      }
      break;
    default:

      /* if execution reaches here, we have verified that we got
         a valid command packet, but the command code received is not
         one that we understand.

         Dump the unknown command code for the user to view.
      */

      EnqueueString(sDASH);
      EnqueueHex(cmdcode);
      EnqueueString(sNEWLINE);
      break;
  }
}

int main()
{

#ifdef BLUETOOTH
#if defined(__AVR_ATmega324__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__) || defined(__AVR_ATmega324PB__)
  Serial1.begin(9600);
#else 
Serial.begin(9600);
#endif
#endif

#ifdef DISC_TRACK_NUMBER_FROM_MPD
#if defined(__AVR_ATmega324__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__) || defined(__AVR_ATmega324PB__)
  Serial1.begin(115200);
#else
  Serial.begin(115200);
#endif
#endif

  Init_VWCDC();

#ifdef DISC_TRACK_NUMBER_FROM_MPD
  //start in idle mode
  SetStateIdle();
#endif

  while (1)
  {
#ifdef DISC_TRACK_NUMBER_FROM_MPD
    if (Serial1.available() > 0) {
      int r = Serial1.read();
      //r has new data
      if (r <= 0xFF)
      {
        //send CD No.
        if ((r & 0xF0) == 0xC0)
        {
          if (r == 0xCA)
          {
            scan = TRUE;
          }
          else if (r == 0xCB)
          {
            mix = TRUE;
          }
          else if (r == 0xCC)
          {
            SetStatePlay();
          }
          else if (r == 0xCD)
          {
            mix = FALSE;
            scan = FALSE;
            playing = FALSE;
            SetStateIdle();
          }
          else if (r == 0xCE)
          {
            mix = FALSE;
          }
          else if ((r & 0x0F) != ( disc & 0x0F))
          {
            disc = (r & 0x4F);
          }
        }
        //send TR No.
        else if (track != r)
        { track = r;
          ResetTime();
        }
      }
    }
#endif
    CDC_Protocol();
  }
}

static void printstr_p(const char *s)
{
  char c;

  for (c = pgm_read_byte(s); c; ++s, c = pgm_read_byte(s))
  {
    Serial.print(c);
    if (c == '\n')
      break;
  }
}

// eof //
