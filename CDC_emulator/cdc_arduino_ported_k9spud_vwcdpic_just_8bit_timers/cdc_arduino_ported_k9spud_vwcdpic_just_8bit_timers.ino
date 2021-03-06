/*****************************************************************************
   Audi/Volkswagen Audi Interface
   Target: arduino atmega328(tested)
           attinu85 (tested)
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

   UNO,Duemilanove,nanoe ... atmegax8 base boards:
   RADIO PIN -> arduino pin

   DataOut   -> digital 2 (INT0)
   DataIn    -> digital 11(PB3)
   Clock     -> digital 13(PB5)

   PC PIN    -> arduino pin
   Serial TX -> digital 0 (PD0)
   Serial RX -> digital 1 (PD1)

   ANDROID support:
   PREVIOUS BUTTON -> digital 7 (PD7)
   PLAY BUTTON -> digital 6 (PD6)
   NEXT BUTTON -> digital 5 (PD5)

   to enable android remote control over headphone buttons emulation uncoment #define ANDROID_HEADPHONES
   for one button control (SONY) uncoment #define ANDROID_HEADPHONES_ONE_BUTTON

   android headphone support, pinout:
   play button control:arduino pin 5 (chip pin 11)
   previous button control: arduino pin 9 (chip pin 15)
   next button control: arduino pin 6 (chip pin 12)

   6. Aug 2015  tomaskovacik
    - 2 8-bit timers used
    - INT0 is used
    - attiny85 pinout
    - merge with attiny85 port

   8. Oct 2015 tomas
    - init support for arduino 3-button remote control over headphone mic line
    - init support for sony single buttone remote control on headphone mic line


   ATTINYx5 version:

   RADIO PIN -> ATTINYx5 PIN
   DataOut   -> 7 (INT0)
   Clock     -> 6 (PB1)
   DataIn    -> 5 (PB0)

   PC PIN    -> ATTINYx5 PIN
   Serial TX -> 2 (PB3) using tinydebug lib
   Serial RX -> not connected

   what you need  to install:
   ATTinycore: https://github.com/SpenceKonde/ATTinyCore (supported directly in arduino(from version 1.6.3) via board manager )
   TinyDebugSerial from: https://github.com/jscrane/TinyDebugSerial (download and extract into libraries Arduino folder)

   select attiny25/45/85 board from tools-> board -> (ATTinyCore) ATtiny25/45/85
   select attiny85 from tools->chip->ATtiny85
   select internal 8Mhz from tools->Clock -> 8MHz (internal)

   to use digispark uncoment line //#define DIGISPARK
   then use 16MHz-noUSB settings
   tested with ATTinyCore with Digispark support: https://github.com/tomaskovacik/ATTinyCore/
   maybe will be merged in oficial ATTinyCore in the future

   kovo 1.jul 2019
   - atmega324 using INT2 /on pins INT0 and INT1 is 2nd uart

   kovo 3.ju l2019
    - attiny167 using ICP1/digi10 for command and 0 and 1 for data/clk pins

 *****************************************************************************/
//#define DIGISPARK

#ifdef DIGISPARK
#include <avr/power.h>
#endif

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
   script with shyd.de control script
*/
//#define JUST_HEX_TO_SERIAL

/* enable bluetooth module control over Serial line
   XS3868
*/
//#define BLUETOOTH
#ifdef BLUETOOTH
#define SERIAL_BAUDRATE 9600
#else
#define SERIAL_BAUDRATE 9600
#endif
/*
   read disc# track# status over serial line
*/
//#define DISC_TRACK_NUMBER_FROM_MPD

/*
   ANDROID HEADPRONES SUPPORT
   HTC: 3buttons, 220ohm previous|0ohm play/pause|470ohm next
*/
//#define ANDROID_HEADPHONES

/*
   ANDROID HEADPHONES SUPORT
   SONY: 1button mode: 1xpush play/pause;2xpush previous;3xpush next
*/
//#define ANDROID_HEADPHONES_ONE_BUTTON

#ifdef ANDROID_HEADPHONES_ONE_BUTTON //just to be shure
#define ANDROID_HEADPHONES
#endif


#if defined(__AVR_ATtiny85__)
//https://github.com/jscrane/TinyDebugSerial
//#ifdef DIGISPARK
//no softwarserial in digispark ...
#include <TinyDebugSerial.h>
TinyDebugSerial mySerial = TinyDebugSerial();
//#else
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(NULL, PB3);
//#endif
//tiny has only Rx, so by default we disable all features which use TX
#undef DISC_TRACK_NUMBER_FROM_MPD
#undef ANDROID_HEADPHONES
#undef ANDROID_HEADPHONES_ONE_BUTTON
#define Serial mySerial
#else
#include <HardwareSerial.h>
#endif

#define TRUE 1

// Low period of PWTX line:
// 0: ~650us
// 1: ~1.77ms
// S: ~4.57ms

// one tick is 0.5µs
#define STARTTHRESHOLD  6400            // greater than this signifies START bit
#define HIGHTHRESHOLD   2496       // greater than this signifies 1 bit.
#define LOWTHRESHOLD    512        // greater than this signifies 0 bit.
#define PKTSIZE          -32            // command packets are 32 bits long.

// do not refresh head unit faster than 5.5ms (currently not implemented)
// 5.24s slow refresh rate when head unit in FM/AM/Tape mode (not implemented)

//#define REFRESH_PERIOD   50        // default to refresh head unit every 50.0ms
#define SECONDWAIT      -20         // wait 20 * 50ms to get 1 second (50ms*20 = 1000ms = 1s)
#define POWERIDENTWAIT  -10         // wait 10 * 1s to 10 seconds between VWCDPICx.x display
#define SCANWAIT        -100        // wait 100 * 50ms to get 5sec (50ms*100 = 5000ms = 5s)
#define _50MS            5000
#define _700US            70
#define _10US            1
#define CLK_DELAY 5
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
#define VER_MINOR       '1'
#define VER_PATCHLEVEL  'd'

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
#define RADIO_COMMAND      PB2
#define RADIO_COMMAND_DDR  DDRB
#define RADIO_COMMAND_PORT PORTB
#define RADIO_COMMAND_PIN PINB
#define RADIO_CLOCK        PB1
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT    PORTB
#define RADIO_DATA         PB0
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT    PORTB

#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328A__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PA__) || defined(__AVR_ATmega328PB__) || \
      defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) || defined(__AVR_ATmega168PB__) || \
       defined(__AVR_ATmega88__) ||  defined(__AVR_ATmega88A__) ||  defined(__AVR_ATmega88P__) ||  defined(__AVR_ATmega88PA__) ||  defined(__AVR_ATmega88PB__)
//INT0 = PD2
#define RADIO_COMMAND      PD2
#define RADIO_COMMAND_DDR  DDRD
#define RADIO_COMMAND_PORT PORTD
#define RADIO_COMMAND_PIN PIND
#define RADIO_CLOCK        PB5
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT    PORTB
#define RADIO_DATA        PB3
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT    PORTB

#elif defined(__AVR_ATmega324__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__)
//INT2 = PD6
#define RADIO_COMMAND      PB2 //PD
#define RADIO_COMMAND_DDR  DDRB
#define RADIO_COMMAND_PORT PORTB
#define RADIO_COMMAND_PIN PINB
#define RADIO_CLOCK        PB7
#define RADIO_CLOCK_DDb    DDB7
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT  PORTB
#define RADIO_DATA         PB5
#define RADIO_DATA_DDb     DDB5
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT  PORTB

#elif defined(__AVR_ATtiny167__)
//ICP1 = PA4
#define RADIO_COMMAND      PA4 //ICP1 10
#define RADIO_COMMAND_DDR  DDRA
#define RADIO_COMMAND_PORT PORTA
#define RADIO_COMMAND_PIN PINA
#define RADIO_CLOCK        PB1 //1
#define RADIO_CLOCK_DDb    DDB1
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT  PORTB
#define RADIO_DATA         PB0 //0
#define RADIO_DATA_DDb     DDB0
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT  PORTB

#endif

#ifdef ANDROID_HEADPHONES
#define ANDROID_NEXT       PD5 //android next button control arduino pin 5 (chip pin 11)
#define ANDROID_NEXT_DDR   DDRD
#define ANDROID_NEXT_PORT    PORTD
#define ANDROID_PLAY       PD6 //android play button control arduino pin 6 (chip pin 12)
#define ANDROID_PLAY_DDR   DDRD
#define ANDROID_PLAY_PORT    PORTD
#define ANDROID_PREV       PD7 //android previous button control arduino pin 7 (chip pin 13)
#define ANDROID_PREV_DDR   DDRD
#define ANDROID_PREV_PORT    PORTD
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
  StatePlay,
  StateTP
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
const uint8_t sTP[] PROGMEM = "TP\r\n";
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
uint8_t _1us_delay = 0;

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
int8_t play_count = 0;
//next button: tested: next button pushed for 100ms = 10 cycles of 10ms counter
#ifndef ANDROID_HEADPHONES_ONE_BUTTON
int8_t next_count = 0;
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
static void SetStateTP(void);
static void SendStateIdle(void);
static void SendStatePlayLeadInEnd(void);
static void SendPacket(void);
static void SendStateInitPlayEnd(void);
static void SendStateInitPlayAnnounceCD(void);
static void SendStatePlayLeadInAnnounceCD(void);
static void SendStateTP(void);
static void printstr_p(const char *s);
static void android_buttons();
static void OutputByte(void);
static void setup_timers(void);
#define TRUE 1
#define FALSE 0


int main()
{
  Serial.begin(SERIAL_BAUDRATE);

  Init_VWCDC();
  //start in idle mode
  //SetStateIdle();
  while (1)
  {
#ifdef DISC_TRACK_NUMBER_FROM_MPD
    if (Serial.available() > 0) {
      int r = Serial.read();
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
// eof //
