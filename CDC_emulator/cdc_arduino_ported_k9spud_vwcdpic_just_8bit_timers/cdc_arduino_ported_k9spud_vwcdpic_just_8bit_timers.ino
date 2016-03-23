/*****************************************************************************
 * 
 * 
 * Audi/Volkswagen Audi Interface
 * 
 * Target: arduino atmega328(tested)
 *         attinu85 (tested)
 * 
 * 
 * Author: Vincent
 * 
 * Date: January, 2011
 * 
 * Version: 0.01
 * 
 * 
 * 
 * Originally ported from VWCDPIC Project:
 * 
 * http://www.k9spud.com/vwcdpic/
 * 
 * 
 * 
 * Ported from Matthias Koelling's AVR Code: 
 * 
 * www.mikrocontroller.net/attachment/31279/vwcdc.c
 * 
 * ---------------------------------
 * 16. Mar 2015:
 * 
 * https://github.com/tomaskovacik/
 * arduino duemilanove works:
 * 
 * UNO,Duemilanove,nanoe ... atmegax8 base boards:
 * RADIO PIN -> arduino pin 
 * 
 * DataOut   -> digital 2 (INT0)
 * DataIn    -> digital 13(PB5)
 * Clock     -> digital 12(PB4)
 *
 * PC PIN    -> arduino pin
 * Serial TX -> digital 0 (PD0)
 * Serial RX -> digital 1 (PD1)
 *
 * ANDROID support:
 * PLAY BUTTON -> digital 5 (PD5)
 * PREVIOUS BUTTON -> digital 9 (PB1)
 * NEXT BUTTON -> digital 6 (PD6)
 *
 * to enable android remote control over headphone buttons emulation uncoment #define ANDROID_HEADPHONES
 * for one button control (SONY) uncoment #define ANDROID_HEADPHONES_ONE_BUTTON
 *
 * ATTINYx5 version:
 *
 * RADIO PIN -> ATTINYx5 PIN
 * DataOut   -> 7 (INT0)
 * DataIn    -> 6 (PB1)
 * Clock     -> 5 (PB0)
 * 
 * PC PIN    -> ATTINYx5 PIN
 * Serial TX -> 2 (PB3) using tinydebug lib
 * Serial RX -> not connected
 * 
 * android headphone support, pinout:
 * play button control:arduino pin 5 (chip pin 11)
 * previous button control: arduino pin 9 (chip pin 15)
 * next button control: arduino pin 6 (chip pin 12)
 *
 * 6. Aug 2015  tomaskovacik
 *  - 2 8-bit timers used
 *  - INT0 is used
 *  - attiny85 pinout
 *  - merge with attiny85 port
 *
 * 8. Oct 2015 tomas
 *  - init support for arduino 3-button remote control over headphone mic line
 *  - init support got sony single buttone remote control on headphone mic line
 *
 *****************************************************************************/



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
#define TIFR0 TIFR
#define OCF2A OCF2
#define TCCR0A TCCR0
#define TCCR0B TCCR0
#define EIMSK GICR
#define OCIE0A OCIE0
#define EICRA MCUCR
#define EIFR GIFR
#endif

// uncoment this to have original fSerial outputs for PJRC players
//#define PJRC

/* enable hex control command on Serial line to control mpd control
 * script with shyd.de control script
 */
//#define JUST_HEX_TO_Serial

/* enable bluetooth module control over Serial line
 * XS3868
 */
#define BLUETOOTH

/*
 * read disc# track# status over serial line
 */
//#define DISC_TRACK_NUMBER_FROM_MPD

/*
 * ANDROID HEADPRONES SUPPORT
 * HTC: 3buttons, 220ohm previous|0ohm play/pause|470ohm next
 */
//#define ANDROID_HEADPHONES

/*
 * ANDROID HEADPHONES SUPORT
 * SONY: 1button mode: 1xpush play/pause;2xpush previous;3xpush next
 */
//#define ANDROID_HEADPHONES_ONE_BUTTON

#ifdef ANDROID_HEADPHONES_ONE_BUTTON //just to be shure
#define ANDROID_HEADPHONES
#endif

#if defined(__AVR_ATtiny85__)
#include <TinyDebugSerial.h>
TinyDebugSerial mySerial = TinyDebugSerial();
//tiny has only Rx, so by default we disable all features which use TX
#undef DISC_TRACK_NUMBER_FROM_MPD
#undef ANDROID_HEADPHONES
#undef ANDROID_HEADPHONES_ONE_BUTTON
#define Serial mySerial
#else
#include <HardwareSerial.h>
#endif
 


/* -- Includes ------------------------------------------------------------- */



#include <avr/io.h>

#include <avr/sfr_defs.h>

#include <stdlib.h>

#include <avr/interrupt.h>

#include <avr/pgmspace.h>



#include <util/delay.h>







/* -- Configuration Parameters --------------------------------------------- */



/* -- Makros --------------------------------------------------------------- */



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

#define SCANWAIT        -100        // wait 100 * 50ms to get 5sec (50ms*100 = 5000ms = 5s)

#define _50MS            500

#define _700US            7

#define TX_BUFFER_END   12

#define CAP_BUFFER_END	24

//android headphones
#ifdef ANDROID_HEADPHONES

#define ANDROID_PUSH_COUNT 2 //100ms push - how long push take 

#define ANDROID_DELAY_COUNT 1 //50ms delay - how long we wait between pushes

#endif

#ifdef ANDROID_HEADPHONES_ONE_BUTTON

#define ANDROID_PLAY_COUNT 1 //how much pushes need to make it play

#define ANDROID_NEXT_COUNT 2 //how much pushes need to make it next

#define ANDROID_PREV_COUNT 3 //how much pushes need to make it previous

#endif

#define VER_MAJOR       '1'

#define VER_MINOR       '0'

#define VER_PATCHLEVEL  'c'

#if defined(__AVR_ATtiny85__)

#define RADIO_COMMAND      PB2 

#define RADIO_COMMAND_DDR  DDRB

#define RADIO_COMMAND_DDRbit  DDB2

#define RADIO_COMMAND_PORT PORTB

#define RADIO_COMMAND_INPUT_PIN_ADDRESS PINB

#define RADIO_CLOCK        PB1 

#define RADIO_CLOCK_DDR    DDRB

#define RADIO_CLOCK_DDRbit    DDB1

#define RADIO_CLOCK_PORT    PORTB

#define RADIO_DATA         PB0 

#define RADIO_DATA_DDR     DDRB

#define RADIO_DATA_DDRbit     DDB0

#define RADIO_DATA_PORT    PORTB

#else
//INT0 = PD2
#define RADIO_COMMAND      PD2

#define RADIO_COMMAND_DDR  DDRD

#define RADIO_COMMAND_DDRbit  DDD2

#define RADIO_COMMAND_PORT PORTD

#define RADIO_COMMAND_INPUT_PIN_ADDRESS PIND

#define RADIO_CLOCK        PB4

#define RADIO_CLOCK_DDR    DDRB

#define RADIO_CLOCK_DDRbit    DDB4

#define RADIO_CLOCK_PORT    PORTB

#define RADIO_DATA        PB5

#define RADIO_DATA_DDR     DDRB

#define RADIO_DATA_DDRbit     DDB5

#define RADIO_DATA_PORT    PORTB
#endif

#ifdef ANDROID_HEADPHONES

#define ANDROID_PLAY       PD5 //android play button control arduino pin 5 (chip pin 11)

#define ANDROID_PLAY_DDR   DDRD

#define ANDROID_PLAY_DDRbit   DDD5

#define ANDROID_PLAY_PORT    PORTD

#define ANDROID_PREV       PB1 //android previous button control arduino pin 9 (chip pin 15)

#define ANDROID_PREV_DDR   DDRB

#define ANDROID_PREV_DDRbit    DDB1

#define ANDROID_PREV_PORT    PORTB

#define ANDROID_NEXT       PD6 //android next button control arduino pin 6 (chip pin 12)

#define ANDROID_NEXT_DDR   DDRD

#define ANDROID_NEXT_DDRbit    DDB6

#define ANDROID_NEXT_PORT    PORTD

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

#define  Do_LOADCD			0x01	// not used

#define  Do_ENABLE_MK      0x08  // mk concert1

#define  Do_CD1            0x0C  // CD 1

#define  Do_DISABLE        0x10  // DISABLE

#define  Do_CHANGECD       0x14  // Change CD (changer ignores & no ACK)

#define  Do_PREVCD         0x18  // PREVIOUS CD (Audi Concert head unit)

#define  Do_CD5            0x2C  // CD 5

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

//#define DUMPMODE
//#define DUMPMODE2

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

  StatePlay

};



/* -- Types ---------------------------------------------------------------- */



/* -- Extern Global Variables ---------------------------------------------- */

#ifdef BLUETOOTH

const uint8_t sDATAERR[] PROGMEM = "\r\n";

const uint8_t sOVERFLOW[] PROGMEM = "\r\n";

const uint8_t sMDISABLE[] PROGMEM = "AT#MA\r\n";

const uint8_t sMENABLE[] PROGMEM = "AT#MA\r\n";

const uint8_t sMINQUIRY[] PROGMEM = "MINQUIRY\r\n";

const uint8_t sPRV_LIST[] PROGMEM = "AT#MA\r\n";

const uint8_t sNXT_LIST[] PROGMEM = "AT#MA\r\n";

const uint8_t sLIST1[] PROGMEM = "AT#MA\r\n";

const uint8_t sLIST2[] PROGMEM = "AT#MA\r\n";

const uint8_t sLIST3[] PROGMEM = "AT#MA\r\n";

const uint8_t sLIST4[] PROGMEM = "AT#MA\r\n";

const uint8_t sLIST5[] PROGMEM = "AT#MA\r\n";

const uint8_t sLIST6[] PROGMEM = "AT#MA\r\n";

const uint8_t sRANDOM[] PROGMEM = "RANDOM\r\n";

const uint8_t sPLAY[] PROGMEM = "AT#MA\r\n";

const uint8_t sSCAN[] PROGMEM = "SCAN\r\n";

const uint8_t sSTOP[] PROGMEM = "AT#MA\r\n";

const uint8_t sNEXT[] PROGMEM = "AT#MD\r\n";

const uint8_t sPREVIOUS[] PROGMEM = "AT#ME\r\n";

const uint8_t sRING[] PROGMEM = "\r\n";

const uint8_t sIDENTIFY[] PROGMEM = "\r\n";

const uint8_t sNEWLINE[] PROGMEM = "\r\n";

const uint8_t sDASH[] PROGMEM = "";

const uint8_t sHEX[] PROGMEM = "";

const uint8_t sVERSION[] PROGMEM = "";

#else

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



uint8_t disc;

uint8_t track;

uint8_t minute=0;

uint8_t second=0;



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



// The ISR will set 'dataerr' flag if it detected a framing error

// or 'overflow' if it overflowed the capture buffer queue.

uint8_t overflow;

uint8_t dataerr;



#ifdef DUMPMODE

// The ISR will set 'startbit' flag if it detected a start bit.

uint8_t startbit;

#endif







uint16_t captime; // timer count of low pulse (temp)

uint16_t captime_ovf; //overflow counts of 8bit timer1

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


uint16_t counter_50ms = _50MS; // counter for 10ms intervals 100us*500=50ms - just for counting second

int counter_to_send_packet = _50MS; // intervals betwen packets?

uint8_t flag_50ms = false; // indicates that a period of 50ms isover

uint8_t display_byte_counter_u8 = 0;

#ifdef ANDROID_HEADPHONES

//play button: tested: play button pushed for 100ms = 10 cycles of 10ms counter
uint8_t play_count = 0;
//next button: tested: next button pushed for 100ms = 10 cycles of 10ms counter
#ifndef ANDROID_HEADPHONES_ONE_BUTTON
uint8_t next_count = 0;
//prev button: tested: prev button pushed twise for 100ms = 2x20 cycles of 10ms counter, 
uint8_t prev_count = 0;
#endif
//test: 10m between updates
uint8_t prev_count_delay = 0;
#ifdef ANDROID_HEADPHONES_ONE_BUTTON
uint8_t play_count_delay = 0;
uint8_t play_count_push = 0;
#endif
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

static void SendStateIdle(void);

static void SendStatePlayLeadInEnd(void);

static void SendPacket(void);

static void SendStateInitPlayEnd(void);

static void SendStateInitPlayAnnounceCD(void);

static void SendStatePlayLeadInAnnounceCD(void);

static void printstr_p(const char *s);

#ifdef ANDROID_HEADPHONES

static void android_buttons();

#endif

#define TRUE 1

#define FALSE 0



/* -- Makros --------------------------------------------------------------- */



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
  //on arduino timer0 is used for millis(), we change prescaler, but also need to disable overflow interrupt
  //#if defined(__AVR_ATtiny85__)
  //  TIMSK = 0x00;
  //#else
  //  TIMSK0 = 0x00;
  //#endif

  RADIO_CLOCK_DDR |= _BV(RADIO_CLOCK_DDRbit);

  RADIO_DATA_DDR  |= _BV(RADIO_DATA_DDRbit);

  RADIO_COMMAND_DDR &= ~_BV(RADIO_COMMAND_DDRbit); // input capture as input

  RADIO_COMMAND_PORT |= _BV(RADIO_COMMAND); // enable pull up

#ifdef ANDROID_HEADPHONES

  ANDROID_PLAY_DDR |= _BV(ANDROID_PLAY_DDRbit);
  ANDROID_NEXT_DDR |= _BV(ANDROID_NEXT_DDRbit); 
  ANDROID_PREV_DDR |= _BV(ANDROID_PREV_DDRbit);
  ANDROID_PLAY_PORT &= ~_BV(ANDROID_PLAY);
  ANDROID_NEXT_PORT &= ~_BV(ANDROID_NEXT);
  ANDROID_PREV_PORT &= ~_BV(ANDROID_PREV);
  
#endif

  //attinx5 - > timer1, atmegax8 -> timer0
#if defined(__AVR_ATtiny85__)
  TCCR1=0x00;
  TCNT1=0;
  TIMSK=0x00;
  TCCR1 |= _BV(CS11) | _BV(CS10); //prescaler 4 @ 8MHz tick ever 0.5us but we have 8bit timer, need to catch overflows
  TIMSK |= _BV(TOIE1); //enable execution of overflow interrupt handling vector
#else //atmega chips .. timer0 is used for 100us ticks to time 700us between packet to head unit and for second (50ms mark)
  TCCR0A=0x00;
  TCCR0B=0x00;
  TCNT0=0;
  TIMSK0=0x00;
  TCCR0B |= _BV(CS01); //prescaler 8 @ 16MHz tick ever 0.5us but we have 8bit timer, need to catch overflows
  TIMSK0 |= _BV(TOIE0); //enable execution of overflow interrupt handling vector
#endif


  //Timer 2 init (timer0 on attin85)
  //Timer 2 used to make all 100us timing
#if defined(__AVR_ATtiny85__)
  TCCR0A = 0x00; // Normal port operation, OC0 disconnected
  TCCR0A |= _BV(WGM01); // CTC mode
  TCCR0B |= _BV(CS01);// prescaler = 8 -> 1 timer clock tick is 1us long
  OCR0A = 100;//run compare rutine every 100us;
  TCNT0=0;
  TIMSK |= _BV(OCIE0A); // enable output compare interrupt A on timer0
#else
  TCCR2A = 0x00; // Normal port operation, OC0 disconnected
  TCCR2B = 0x00; // Normal port operation, OC0 disconnected
  TCCR2A |= _BV(WGM21); // CTC mode
  TCCR2B |= _BV(CS21);// prescaler = 8 -> 1 timer clock tick is 0.5us long @ 16Mhz
  OCR2A = 200;//run compare rutine every 100us, 0.5x200
  TCNT2=0;
  TIMSK2 |= _BV(OCIE2A); // enable output compare interrupt A on timer0
#endif


#if defined(__AVR_ATtiny85__)
  GIMSK |= _BV(INT0); //INT0 enable
  MCUCR |= _BV(ISC01);
  MCUCR &= ~_BV(ISC00);// falling edge fire interupt routine
#else
  EIMSK |= _BV(INT0); //INT0 enable
  EICRA |= _BV(ISC01);
  EICRA &= ~_BV(ISC00);// falling edge fire interupt routine
#endif


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

  track = 0x01 ; // track 1

  poweridentcount = POWERIDENTWAIT;



  ResetTime();

  SetStateIdleThenPlay();

  EnqueueString(sIDENTIFY);

  EnqueueString(sVERSION);

  EnqueueString(sNEWLINE);

  EnqueueString(sRING);

  SendPacket(); // force first display update packet

  sei();

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



void OutputPacket(void)
{

  static uint8_t display_byte_counter_u8 = 0;

  uint8_t byte_u8;

  if (display_byte_counter_u8 < 8)

  {

    byte_u8 = display_byte_buffer_mau8[display_byte_counter_u8];



#ifdef DUMPMODE2

    Serial.print("|");

    Serial.print(byte_u8,HEX);

    Serial.print("|");

#endif


    for (sendbitcount = -8; sendbitcount != 0; sendbitcount++)

    {

      RADIO_CLOCK_PORT |= _BV(RADIO_CLOCK); // SCLK high

      _delay_loop_1(40);

      if ((byte_u8 & 0x80) == 0) // mask highest bit and test if set

      {

        RADIO_DATA_PORT |= _BV(RADIO_DATA); // DATA high

      }

      else

      {

        RADIO_DATA_PORT &= ~_BV(RADIO_DATA); // DATA low

      }



      byte_u8 <<= 1; // load the next bit

      RADIO_CLOCK_PORT &= ~_BV(RADIO_CLOCK); // SCLK low

      _delay_loop_1(40);

    }

  }
  counter_to_send_packet = _700US;

  display_byte_counter_u8++;

  if (display_byte_counter_u8==8)
  {//wait 50ms
    display_byte_counter_u8 = 0;
    counter_to_send_packet = _50MS;
  }
}



//-----------------------------------------------------------------------------

/*!
 
 \brief     ISR(TIMER0_COMP_vect)
 
 *on atmega8 overflow is used
 
 
 
 timer0 output compare interrupt service routine for cdc protocol
 
 radio display update
 
 
 
 \author     Koelling
 
 \date       04.10.2007
 
 
 
 \param[in]  none
 
 \param[out] none
 
 \return     void
 
 */

//-----------------------------------------------------------------------------

#if defined(__AVR_ATtiny85__)
ISR(TIM0_COMPA_vect)
{

  TCNT0=0;
#else
ISR(TIMER2_COMPA_vect) //100us
{

  TCNT2=0;
#endif
  counter_50ms--;

  if (counter_to_send_packet>0) counter_to_send_packet--; //if we are under = we sending something out...

  if (counter_50ms == 0)

  {

    counter_50ms = _50MS;

    flag_50ms = TRUE; 
    
#ifdef ANDROID_HEADPHONES
android_buttons();  
#endif

  }

  if (counter_to_send_packet == 0)

  {
    counter_to_send_packet--; //make it -1 to stop counting..
    OutputPacket();

  }

}


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


#if defined(__AVR_ATtiny85__)
ISR(TIMER1_OVF_vect)
#else
ISR(TIMER0_OVF_vect)
#endif

{
  /*

   has 8bit timer, max value=255
   
   pulses are: 4500 us startbit = 9000 pulses = 35*255+75
   550 us logic 1  = 1100 pulses = 4*255+80
   1700 us logic 0  = 3400 pulses = 13*255+85
   
   */


  captime_ovf=captime_ovf+255;
  //Serial.println("TIMER1_OVF_vect");

  if (captime_ovf > 8925) {//35*255= 8925

      captime_ovf=0;

    //Serial.println("TIMER1_OVF_vect");

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

}



//-----------------------------------------------------------------------------

/*!
 
 \brief     ISR(INT0_vect)
 
 
 
 input capture interrupt service routine for cdc protocol
 
 
 
 \author     Koelling
 
 \date       26.09.2007
 
 
 
 \param[in]  none
 
 \param[out] none
 
 \return     void
 
 */

//-----------------------------------------------------------------------------



ISR(INT0_vect)
{

  captime = captime_ovf+TCNT0;

  //Serial.println(1);
#if defined(__AVR_ATtiny85__)
  TCNT1 = 0;
#else
  TCNT0 = 0; // clear timer0
#endif

  captime_ovf=0; // clear timer0 overflow count



  if ((RADIO_COMMAND_INPUT_PIN_ADDRESS & _BV(RADIO_COMMAND)) == 0)

  {

    // We have interrupted at beginning of low pulse (falling edge)

    // Low pulse length must be timed to determine bit value

#if defined(__AVR_ATtiny85__)
    TIFR  |= _BV(TOV1); // clear timer1 overflow flag
    TIMSK |= _BV(TOIE1); // enable timer1 interrupt on overflow
    MCUCR |= _BV(ISC01) | _BV(ISC00); // change input capture to rising edge
    GIFR  |= _BV(INTF0); // clear input  interrupt request flag
#else
    TIFR0  |= _BV(TOV0); // clear time0 overflow flag
    TIMSK0 |= _BV(TOIE0); // enable timer0 interrupt on overflow
    EICRA |= _BV(ISC01) | _BV(ISC00); // change input capture to rising edge
    EIFR  |= _BV(INTF0); // clear input  interrupt request flag
#endif



  }

  else

  {

    // We have interrupted at beginning of high pulse (rising edge)

    // High pulse length doesn't matter. We need to check out

    // captured low pulse width if we are capturing data at the moment

    capbusy = TRUE;



#if defined(__AVR_ATtiny85__)
    MCUCR |= _BV(ISC01);
    MCUCR &= ~_BV(ISC00);// change input capture to falling edge
    GIFR  |= _BV(INTF0); // clear input  interrupt request flag
    
    if (TIMSK & _BV(TOIE1)) // are we trying to capture data?
#else
    EICRA |= _BV(ISC01);
    EICRA &= ~_BV(ISC00);// change input capture to falling edge
    EIFR  |= _BV(INTF0); // clear input  interrupt request flag

    if (TIMSK0 & _BV(TOIE0)) // are we trying to capture data?
#endif
    {

      capbusy = TRUE;

#if defined(__AVR_ATtiny85__)
      TIMSK &= ~_BV(TOIE1); // turn off capturing time for high pulse
#else
      TIMSK0 &= ~_BV(TOIE0); // turn off capturing time for high pulse
#endif
     // Serial.println(captime);

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

    SendPacket();

    scancount++;

    if (scancount == 0)

    {

      scancount = SCANWAIT;

      scan = FALSE; // turn off scan display

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

    EnqueueString(sMENABLE);

#ifdef ANDROID_HEADPHONES
    play_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif

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

    EnqueueString(sMDISABLE);

#ifdef ANDROID_HEADPHONES
    play_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif

    break;



  case Do_SEEKBACK:

  case Do_PREVCD:


#ifndef DISC_TRACK_NUMBER_FROM_MPD

    disc--;

    track = 1;

    ResetTime();
    
    if ((disc & 0x0F) == 0)

    {

      disc = 0x46; // set back to CD 1

    }
#endif

    EnqueueString(sPRV_LIST);


    
    break;



  case Do_SEEKFORWARD:    

  case Do_SEEKFORWARD_MK:

    

    if (cd_button == FALSE) // mk don't increment when previous command was a cd button

    {
      EnqueueString(sNXT_LIST);

#ifndef DISC_TRACK_NUMBER_FROM_MPD

      ResetTime();

      track = 1;

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

#ifdef ANDROID_HEADPHONES
    play_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif

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
    EnqueueString(sSCAN); // 
#endif

#ifdef ANDROID_HEADPHONES
    play_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif

    break;



  case Do_UP:

  case Do_UP_MK3:

    if (playing == TRUE) // skip track lead-in if not in play mode

    {

      SetStateTrackLeadIn();

    }



#ifndef DISC_TRACK_NUMBER_FROM_MPD

    track++;
    
    ResetTime();

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

#ifdef ANDROID_HEADPHONES
#ifdef ANDROID_HEADPHONES_ONE_BUTTON
   play_count_delay = ANDROID_DELAY_COUNT;
   play_count_push = ANDROID_NEXT_COUNT;
#else
    next_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_NEXT pin
#endif
#endif

    break;



  case Do_DOWN:

  case Do_DOWN_MK3:

    if (playing == TRUE) // skip track lead-in if not in play mode

    {

      SetStateTrackLeadIn();

    }

#ifndef DISC_TRACK_NUMBER_FROM_MPD

    decimal_adjust_u8 = track & 0x0F; // skip past hexidecimal codes

    if (decimal_adjust_u8 == 0) // are we at x0?

    {

      track -= 6; // yes, subtract 6 and we'll be at x9 instead

    }

    track--;
    
    ResetTime();

    if (track == 0) // have we gone below Track 1?

    { // yes, rollover to Track 99 so that jog wheels

      track = 0x99; // can continue rolling (Audi Concert II)

    }

#endif

    EnqueueString(sPREVIOUS);

#ifdef ANDROID_HEADPHONES
#ifdef ANDROID_HEADPHONES_ONE_BUTTON
   play_count_delay = ANDROID_DELAY_COUNT;
   play_count_push = ANDROID_PREV_COUNT;
#else
    prev_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PREV pin
    prev_count_delay=ANDROID_DELAY_COUNT; // 50ms high on ANDROID_PREV pin
#endif
#endif

    break;



  case Do_CD1:

    cd_button = TRUE; // mk store cd button pressed

#ifndef DISC_TRACK_NUMBER_FROM_MPD

    disc = 0x41; // set CD 1
    
    ResetTime();

#endif

    EnqueueString(sLIST1);

#ifdef ANDROID_HEADPHONES
    play_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif

    break;



  case Do_CD2:


    cd_button = TRUE; // mk store cd button pressed

#ifndef DISC_TRACK_NUMBER_FROM_MPD

    disc = 0x42; // set CD 2

    ResetTime();

#endif

    EnqueueString(sLIST2);

#ifdef ANDROID_HEADPHONES
    play_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif

    break;



  case Do_CD3:

    cd_button = TRUE; // mk store cd button pressed

#ifndef DISC_TRACK_NUMBER_FROM_MPD

    disc = 0x43; // set CD 3

    ResetTime();
    
#endif

    EnqueueString(sLIST3);

#ifdef ANDROID_HEADPHONES
    play_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif

    break;



  case Do_CD4:

    cd_button = TRUE; // mk store cd button pressed

#ifndef DISC_TRACK_NUMBER_FROM_MPD

    disc = 0x44; // set CD 4
    
    ResetTime();

#endif

    EnqueueString(sLIST4);

#ifdef ANDROID_HEADPHONES
    play_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif

    break;



  case Do_CD5:

    cd_button = TRUE; // mk store cd button pressed

#ifndef DISC_TRACK_NUMBER_FROM_MPD

    disc = 0x45; // set CD 5
    
    ResetTime();

#endif

    EnqueueString(sLIST5);

#ifdef ANDROID_HEADPHONES
    play_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif

    break;



  case Do_CD6:

    cd_button = TRUE; // mk store cd button pressed

#ifndef DISC_TRACK_NUMBER_FROM_MPD

    disc = 0x46; // set CD 6
    
    ResetTime();

#endif

    EnqueueString(sLIST6);

#ifdef ANDROID_HEADPHONES
    play_count=ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif

    break;



  default:



    /* if execution reaches here, we have verified that we got
     * a valid command packet, but the command code received is not
     * one that we understand.
     *
     * Dump the unknown command code for the user to view.
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
  Serial.begin(9600);
#else
  Serial.begin(9600);
#endif

  Init_VWCDC();

  //start in idle mode
  //SetStateIdle();



  while (1)

  {

#ifdef DISC_TRACK_NUMBER_FROM_MPD
    if (Serial.available() > 0) {
      int r = Serial.read();
      //r has new data
      if(r <= 0xFF)
      {
        //send CD No.
        if((r & 0xF0) == 0xC0)
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
            playing=FALSE;
            SetStateIdle();
          }
          else if (r == 0xCE)
          {
            mix = FALSE;
          }                                
          else
            if ((r & 0x0F) != ( disc & 0x0F))
            { 
              disc = (r & 0x4F);
            }
        }
        //send TR No.
        else 
          if (track != r)
        {
          track = r;
          ResetTime();
        }
      }
    }
#endif

    CDC_Protocol();

  }



}
#ifdef ANDROID_HEADPHONES
static void android_buttons(){

    //android headphone control, this is fired every 50ms
    //play button
    if(play_count > 0){
      play_count--;
      ANDROID_PLAY_PORT |= _BV(ANDROID_PLAY); //high
      //digitalWrite(ANDROID_PLAY,HIGH);
    } 
    else {
      ANDROID_PLAY_PORT &= ~_BV(ANDROID_PLAY); //low
      //digitalWrite(ANDROID_PLAY,LOW);
#ifdef ANDROID_HEADPHONES_ONE_BUTTON
      if(play_count_delay > 0 ){ //counting delay low
        play_count_delay--;
	if(play_count_delay==0 && play_count_push>0){
          play_count = ANDROID_PUSH_COUNT;
          play_count_delay=ANDROID_DELAY_COUNT;
          if (play_count_push>0)
            play_count_push--;
        }
      }
#endif
    }

#ifndef ANDROID_HEADPHONES_ONE_BUTTON
    //next button
    if(next_count > 0){
      next_count--;
      ANDROID_NEXT_PORT |= _BV(ANDROID_NEXT); //high
      //digitalWrite(ANDROID_NEXT,HIGH);
    } 
    else {
      ANDROID_NEXT_PORT &= ~_BV(ANDROID_NEXT); //low
      //digitalWrite(ANDROID_NEXT,LOW);
    }


    //prev button, double push, head unit goes to previous song, no to start of the song
    if(prev_count > 0){
      prev_count--;
      ANDROID_PREV_PORT |= _BV(ANDROID_PREV); //high
      //digitalWrite(ANDROID_PREV,HIGH);
    } 
    else {
      //wait between pushes
      ANDROID_PREV_PORT &= ~_BV(ANDROID_PREV);
      //digitalWrite(ANDROID_PREV,LOW);
      if(prev_count_delay > 0 ){ //we ended first push and do not finished second push
        prev_count_delay--;
	if(prev_count_delay==0) //we are at last run of delay loop
          prev_count = ANDROID_PUSH_COUNT;
      }
    }
#endif
}
#endif

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










