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
   https://github.com/tomaskovacik/
   arduino duemilanove:
   
   RADIO PIN -> arduino pin
   
   DataOut   -> digital 8
   DataIn    -> digital 13
   Clock     -> digital 12

 *****************************************************************************/



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



#define REFRESH_PERIOD   50        // default to refresh head unit every 50.0ms

#define SECONDWAIT      -20         // wait 20 * 50ms to get 1 second (50ms*20 = 1000ms = 1s)

#define POWERIDENTWAIT  -15         // wait 10 * 1s to 10 seconds between VWCDPICx.x display

#define SCANWAIT        -50             // wait 100 * 50ms to get 5sec (50ms*100 = 5000ms = 5s)

#define _10MS           156



#define TX_BUFFER_END   12

#define CAP_BUFFER_END	24



#define VER_MAJOR       '1'

#define VER_MINOR       '0'

#define VER_PATCHLEVEL  'b'



#define Baudrate9600



#define RADIO_COMMAND      PB0 //(Seeeduino Mega PIN PD4)

#define RADIO_COMMAND_DDR  DDB0

#define RADIO_CLOCK        PB4 //(Seeeduino Mega PIN 10)

#define RADIO_CLOCK_DDR    DDB4

#define RADIO_DATA         PB5 //(Seeeduino Mega PIN 11)

#define RADIO_DATA_DDR     DDB5

//#define MP3_ENABLE         PC3

//#define MP3_ENABLE_DDR     DDC3

//#define KOMBI_CLOCK      PB7

//#define KOMBI_CLOCK_DDR  DDB7









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



const uint8_t sDATAERR[] PROGMEM = "dataerr\n\r";

const uint8_t sOVERFLOW[] PROGMEM = "overflow\n\r";

const uint8_t sMDISABLE[] PROGMEM = "MDISABLE\n\r";

const uint8_t sMENABLE[] PROGMEM = "MENABLE\n\r";

const uint8_t sMINQUIRY[] PROGMEM = "MINQUIRY\n\r";

const uint8_t sPRV_LIST[] PROGMEM = "PRV_LIST\n\r";

const uint8_t sNXT_LIST[] PROGMEM = "NXT_LIST\n\r";

const uint8_t sLIST1[] PROGMEM = "LIST1\n\r";

const uint8_t sLIST2[] PROGMEM = "LIST2\n\r";

const uint8_t sLIST3[] PROGMEM = "LIST3\n\r";

const uint8_t sLIST4[] PROGMEM = "LIST4\n\r";

const uint8_t sLIST5[] PROGMEM = "LIST5\n\r";

const uint8_t sLIST6[] PROGMEM = "LIST6\n\r";

const uint8_t sRANDOM[] PROGMEM = "RANDOM\n\r";

const uint8_t sPLAY[] PROGMEM = "PLAY\n\r";

const uint8_t sSTOP[] PROGMEM = "STOP\n\r";

const uint8_t sNEXT[] PROGMEM = "NEXT\n\r";

const uint8_t sPREVIOUS[] PROGMEM = "PREVIOUS\n\r";

const uint8_t sRING[] PROGMEM = "RING\n\r";

const uint8_t sIDENTIFY[] PROGMEM = "Audi Concert I Multimedia Gateway Ver.";

const uint8_t sNEWLINE[] PROGMEM = "\n\r";

const uint8_t sDASH[] PROGMEM = "_";

const uint8_t sHEX[] PROGMEM = {

   '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0, '9', 0, 'A', 0, 'B', 0, 'C', 0, 'D', 0, 'E', 0, 'F', 0

};

const uint8_t sVERSION[] PROGMEM = {

   VER_MAJOR, '.', VER_MINOR, VER_PATCHLEVEL, 0

};





uint8_t sendreg;

uint8_t sendbitcount; // Used in SendByte routine



uint8_t disc;

uint8_t track;

uint8_t minute;

uint8_t second;



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

uint8_t cd_button;



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

static void Start_Timer(uint16_t time_u16);

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

 //  DDRA |= _BV(DDA6); // debug pin

   DDRB |= _BV(RADIO_CLOCK_DDR) | _BV(RADIO_DATA_DDR);// | KOMBI_CLOCK_DDR;

 //  DDRC |= _BV(MP3_ENABLE_DDR);

   DDRB &= ~_BV(RADIO_COMMAND_DDR); // input capture as input

   PORTB |= _BV(RADIO_COMMAND); // enable pull up





   //Timer1 init

   //Used for timing the incoming commands from headunit

   TCCR1A = 0x00; // Normal port operation, OC1A/OC1B/OC1C disconnected

   TCCR1B = _BV(ICNC1); // noise canceler, int on falling edge

   TCCR1B |= _BV(CS11); // prescaler = 8 -> 1 timer clock tick is 0.5µs long





   //Timer 2 Init

   //Timer 2 used to time the intervals between package bytes

   OCR2A = 175; // 4µs x 175 = 700µs

   TCCR2A |= _BV(WGM21); // Timer2 in CTC Mode

   TCCR2B |= _BV(CS20) + _BV(CS21); // prescaler = 64 -> 1 timer clock tick is 4µs long.







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

   SetStateIdleThenPlay();



   TIFR1 |= _BV(ICF1); // clear pending interrupt

   TIMSK1 |= _BV(ICIE1); // enable input capture interrupt on timer1



   EnqueueString(sIDENTIFY);

   EnqueueString(sVERSION);

   EnqueueString(sNEWLINE);

   EnqueueString(sRING);



   //Timer 0 init

   //Timer 0 used to time the interval between each update

   OCR0A = _10MS; // 10ms Intervall

   TCCR0A = 0x00; // Normal port operation, OC0 disconnected

   TCCR0A |= _BV(WGM01); // CTC mode

   TCCR0B |= _BV(CS00) + _BV(CS02); // prescaler = 1024 -> 1 timer clock tick is 64us long

   TIMSK0 |= _BV(OCIE0A); // enable output compare interrupt on timer0





   SendPacket(); // force first display update packet



   SREG |= 0x80;   // enable interrupts









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

   //Serial.println("TIMER2_COMPA_vect");



//   PORTB |= _BV(KOMBI_CLOCK);

   TCCR2B &= ~_BV(CS20); // stop Timer2

   TCCR2B &= ~_BV(CS21);

   TCNT2 = 0; // clear Timer2

   TIFR2 |= _BV(OCF2A);





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

         PORTB |= _BV(RADIO_CLOCK); // SCLK high

         //Start_Timer(3);					// to ensure original PIC timing

         _delay_loop_1(40);

         if ((byte_u8 & 0x80) == 0) // mask highest bit and test if set

         {

            PORTB |= _BV(RADIO_DATA); // DATA high

         }

         else

         {

            PORTB &= ~_BV(RADIO_DATA); // DATA low

         }



         byte_u8 <<= 1; // load the next bit

         PORTB &= ~_BV(RADIO_CLOCK); // SCLK low

         //Start_Timer(3);					// timing delay to soften emi noise

         _delay_loop_1(40);

      }

      display_byte_counter_u8++;

      TCCR2B |= _BV(CS22); // prescaler = 64 -> 1 timer clock tick is 4us long

      //TCCR2B |= _BV(CS20);



   }

   else

   {

      display_byte_counter_u8 = 0;

      TIMSK2 &= ~_BV(OCIE2A); // disable output compare interrupt on timer2

   }

//   PORTB &= ~_BV(KOMBI_CLOCK);

}



//-----------------------------------------------------------------------------

/*!

 \brief     ISR(TIMER0_COMP_vect)



 timer0 output compare interrupt service routine for cdc protocol

 radio display update



 \author     Koelling

 \date       04.10.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



ISR(TIMER0_COMPA_vect)

{

   //OCR0 = TCNT0 + _10MS;

#ifdef DUMPMODE2

   Serial.println();

#endif





counter_10ms_u8++;

   if (counter_10ms_u8 == 5)

   {

      counter_10ms_u8 = 0;

      flag_50ms = TRUE;

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



ISR(TIMER1_OVF_vect)

{

   //Serial.println("TIMER1_OVF_vect");

   TIMSK1 &= ~_BV(TOIE1); // disable further timer 0 interrupts

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

   //Serial.println("TIMER1_CAPT_vect");

   captime = ICR1; // save a copy of current TMR1 count

   // in case PWTXCaptureBit needs it

   TCNT1 = 0; // clear timer1



   if ((PINB & _BV(RADIO_COMMAND)) == 0)

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

//<Serial.println(captime);

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

//      if (bit_is_set(PORTA, PA6))
//
//      {
//
//         PORTA &= ~_BV(PA6);
//
//      }
//
//      else
//
//      {
//
//         PORTA |= _BV(PA6);
//
//      }

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

   //txoutptr = 0;

   

}





//-----------------------------------------------------------------------------

/*!

 \brief    void ScanCommandBytes(void)



 ScanCommandBytes - Looks in the command receive buffer and tries

 to identify valid command codes.



 \author     Koelling

 \date       05.10.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void ScanCommandBytes(void)

{



   fsr = scanptr;



FirstByteLoop:

   //printstr_p(PSTR("1"),DEBUG);

   if (GetCaptureByte() == FALSE)

   {

      return;

   }



FirstByteTest:

   //printstr_p(PSTR("2"),DEBUG);

   if (scanbyte == 0x53)

   {

      goto SecondByte;

   }

   // this byte doesn't match the beginning of a normal command packet,

   EnqueueHex(scanbyte);

   scanptr = fsr; // save scanptr, won't look at this byte again

   goto FirstByteLoop;



SecondByte:

   //printstr_p(PSTR("3"),DEBUG);

   if (GetCaptureByte() == FALSE)

   {

      return;

   }

   if (scanbyte == 0x2C) // verify that byte 2 is 0x2C)

   {

      goto ThirdByte;

   }

   // the first byte was a match, but the second byte failed.

   // dump first byte and then see if this one is the real first byte.

   EnqueueHex(0x53);

   goto FirstByteTest;



ThirdByte:

   //printstr_p(PSTR("4"),DEBUG);

   if (GetCaptureByte() == FALSE)

   {

      return;

   }

   cmdcode = scanbyte; // save command code for later use.



FourthByte:

   //printstr_p(PSTR("5"),DEBUG);

   if (GetCaptureByte() == FALSE)

   {

      return;

   }

   // if execution reaches here, we have already verified that

   // bytes 1 and 2 are valid for a command packet.



   // verify that (Byte 3 + Byte 4) = 0xFF

   if ((cmdcode + scanbyte) == 0xFF)

   {

      //printstr_p(PSTR("6"),DEBUG);



      if ((cmdcode & 0x03) == 0) // verify that Byte 3 is a multiple of 4

      {

         //printstr_p(PSTR("7"),DEBUG);



         ACKcount = -4; // acknowledge command

         scanptr = fsr; // save scanptr, won't look at this byte again



         // Now, let's jump to the section of code that handles the

         // command we just received.



         DecodeCommand();

         //printstr_p(PSTR("\n"),DEBUG);



      }

      else

      {

         DumpFullCommand(); // ABORT: dump invalid packet for display

      }

   }

   else

   {

      DumpFullCommand(); // ABORT: dump invalid packet for display

   }

}



//-----------------------------------------------------------------------------

/*!

 \brief    void DumpFullCommand(void)



 dump all received command bytes



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void DumpFullCommand(void)

{

   fsr = scanptr; // restart back at the beginning of the packet

   if (GetCaptureByte() == TRUE) // send byte 1

   {

      EnqueueHex(scanbyte);

   }

   if (GetCaptureByte() == TRUE) // send byte 2

   {

      EnqueueHex(scanbyte);

   }

   if (GetCaptureByte() == TRUE) // send byte 3

   {

      EnqueueHex(scanbyte);

   }

   if (GetCaptureByte() == TRUE) // send byte 4

   {

      EnqueueHex(scanbyte);

   }

   EnqueueString(sNEWLINE);

   scanptr = fsr; // save scanptr, won't look at this byte again

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



   switch (cmdcode) {

      case Do_CHANGECD:

         // Head unit seems to send this after each CDx number change

         // but the CD Changer seems to completely ignore (doesn't even ACK it).

         ACKcount = 0; // do not ack this command

         EnqueueString(sRANDOM);

         break;



      case Do_ENABLE:

      case Do_ENABLE_MK:

         //PORTC |= _BV(MP3_ENABLE);

         mix = FALSE;

         if (playing == FALSE)

         {

            SetStateInitPlay(); // skip this if already playing

         }

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

        // PORTC &= ~_BV(MP3_ENABLE);

         SetStateIdle(); // skip this if we're already in idle mode



         disc = 0x41; // set back to CD 1

         EnqueueString(sMDISABLE);

         break;



      case Do_SEEKBACK:

      case Do_PREVCD:

         ResetTime();

         disc--;

         if ((disc & 0x0F) == 0)

         {

            disc = 0x46; // set back to CD 1

         }

         EnqueueString(sPRV_LIST);

         break;



      case Do_SEEKFORWARD:    

      case Do_SEEKFORWARD_MK:

         ResetTime();

         if (cd_button == FALSE) // mk don't increment when previous command was a cd button

         {

            disc++;

            if (disc > 0x46)

            {

               disc = 0x41;

            }

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

         EnqueueString(sNXT_LIST);

         break;



      case Do_MIX:

      case Do_MIX_CD:

         if (mix == FALSE)

         {

            mix = TRUE;

         }

         else

         {

            mix = FALSE;

         }

         EnqueueString(sRANDOM);

         break;



      case Do_PLAY:

         EnqueueString(sPLAY); // this will make the PJRC play/pause

         break;



      case Do_SCAN:



         scancount = SCANWAIT;

         if (scan == FALSE)

         {

            scan = TRUE;

         }

         else

         {

            scan = FALSE;

         }

         EnqueueString(sPLAY); // this will make the PJRC play/pause

         break;



      case Do_UP:

      case Do_UP_MK3:

         if (playing == TRUE) // skip track lead-in if not in play mode

         {

            SetStateTrackLeadIn();

         }

         ResetTime();

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

         EnqueueString(sNEXT);

         break;



      case Do_DOWN:

      case Do_DOWN_MK3:

         if (playing == TRUE) // skip track lead-in if not in play mode

         {

            SetStateTrackLeadIn();

         }

         ResetTime();

         decimal_adjust_u8 = track & 0x0F; // skip past hexidecimal codes

         if (decimal_adjust_u8 == 0) // are we at x0?

         {

            track -= 6; // yes, subtract 6 and we'll be at x9 instead

         }

         track--;

         if (track == 0) // have we gone below Track 1?

         { // yes, rollover to Track 99 so that jog wheels

            track = 1; // can continue rolling (Audi Concert II)

         }

         EnqueueString(sPREVIOUS);

         break;



      case Do_CD1:

         cd_button = TRUE; // mk store cd button pressed

         disc = 0x41; // set CD 1

         EnqueueString(sLIST1);

         break;



      case Do_CD2:

         cd_button = TRUE; // mk store cd button pressed

         disc = 0x42; // set CD 1

         EnqueueString(sLIST2);

         break;



      case Do_CD3:

         cd_button = TRUE; // mk store cd button pressed

         disc = 0x43; // set CD 1

         EnqueueString(sLIST3);

         break;



      case Do_CD4:

         cd_button = TRUE; // mk store cd button pressed

         disc = 0x44; // set CD 1

         EnqueueString(sLIST4);

         break;



      case Do_CD5:

         cd_button = TRUE; // mk store cd button pressed

         disc = 0x45; // set CD 1

         EnqueueString(sLIST5);

         break;



      case Do_CD6:

         cd_button = TRUE; // mk store cd button pressed

         disc = 0x46; // set CD 1

         EnqueueString(sLIST6);

         break;



      default:



         // if execution reaches here, we have verified that we got

         // a valid command packet, but the command code received is not

         // one that we understand.

         //

         // Dump the unknown command code for the user to view.



         EnqueueString(sDASH);

         EnqueueHex(cmdcode);

         EnqueueString(sNEWLINE);

         break;

   }

}







//-----------------------------------------------------------------------------

/*!

 \brief    uint8_t GetCaptureByte(void)



 checks wether a command byte is still in queue



 \author     Koelling

 \date       05.10.2007



 \param[in]  none

 \param[out] none

 \return     FALSE ->	no more bytes to collect

 TRUE  -> scanbyte contains next byte

 */

//-----------------------------------------------------------------------------



static uint8_t GetCaptureByte(void)

{

   uint8_t return_u8 = FALSE;

   // have we already caught up with capturer?

   if (fsr != capptr)

   {

      scanbyte = capbuffer[fsr]; // get a byte from the capture buffer

      fsr++;

      if (fsr == CAP_BUFFER_END) // have we overflowed the

      { // capture buffer?

         fsr = 0;

      } // yes, roll over to beginning

      return_u8 = TRUE;

   }

   return return_u8;

}



//-----------------------------------------------------

// Display Update Packets

//-----------------------------------------------------



//-----------------------------------------------------------------------------

/*!

 \brief    void SetStateIdle(void)



 Idle State

 74 BE FE FF FF FF 8F 7C

 74 BE FE FF FF FF 8F 7C

 ...



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     none



 */

//-----------------------------------------------------------------------------



static void SetStateIdle(void)

{

   playing = FALSE;

   BIDIstate = StateIdle;

}



//-----------------------------------------------------------------------------

/*!

 \brief     SetStateIdleThenPlay(void)



 Real CD Changer doesn't really do this, but we're gonna do it to try

 and make sure we unmute the audio even if the user didn't connect

 the PW-TX pin properly.



 \author     Koelling

 \date       27.09.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void SetStateIdleThenPlay(void)

{

   playing = 0;

   BIDIstate = StateIdleThenPlay;

   BIDIcount = -20;

}





//-----------------------------------------------------------------------------

/*!

 \brief    void SetStatePlay(void)



 set state to play mode



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     none



 */

//-----------------------------------------------------------------------------



static void SetStatePlay(void)

{

   playing = TRUE;

   BIDIstate = StatePlay;

}





//-----------------------------------------------------------------------------

/*!

 \brief    void SetStateInitPlay(void)



 Initiate Playing



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     none



 */

//-----------------------------------------------------------------------------



static void SetStateInitPlay(void)

{

   playing = TRUE;

   BIDIstate = StateInitPlay;

   discload = 0xD1;

   BIDIcount = -24;

}





//-----------------------------------------------------------------------------

/*!

 \brief    void SetStatePlayLeadIn(void)



 34 BE FE FF FF FF AE 3C (play lead-in)

 34 2E ED DE AF B7 FF 3C

 34 BE FE FF FF FF AE 3C

 34 2E ED DE AF B7 FF 3C

 34 BE FE FF FF FF AE 3C

 34 2E ED DE AF B7 FF 3C

 34 BE FE FF FF FF AE 3C

 34 2E ED DE AF B7 FF 3C

 34 BE FE FF FF FF AE 3C



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     none



 */

//-----------------------------------------------------------------------------



static void SetStatePlayLeadIn(void)

{

   playing = TRUE;

   BIDIstate = StatePlayLeadIn;

   BIDIcount = -10;

}





//-----------------------------------------------------------------------------

/*!

 \brief    void SetStateTrackLeadIn(void)



 34BEFEFFEEFFCF3C (playing)

 n34BEFEFFEEFFCF3C

 14BEFDFFFFFFCF1C (ack)

 14BEFDFFFFFFAE1C (track lead in)

 14BEFDFFFFFFAE1C

 34BEFDFFFFFFAE3C

 34BEFDFFFFFFAE3C

 34BEFDFFFFFFAE3C

 34BEFDFFFFFFAE3C

 34BEFDFFFFFFAE3C

 34BEFDFFFFFFAE3C

 34BEFDFFFFFFAE3C

 34BEFDFFFFFFAE3C

 34BEFDFFFFFFAE3C

 34BEFDFFFFFFAE3C

 34BEFDFFFFFFCF3C (playing)

 34BEFDFFFFFFCF3C



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     none



 */

//-----------------------------------------------------------------------------



static void SetStateTrackLeadIn(void)

{

   playing = TRUE;

   BIDIstate = StateTrackLeadIn;

   BIDIcount = -12;

}



// TODO: We might implement one more state machine for

// the CHANGECD/INQUIRY command. (mute byte goes 0x6F and 0xFF cd load

// while changer is busy motoring next CD into position). Then

// again, maybe we don't need to implement any busy states since

// we are instantly ready (no motoring here!).



// =========================================================================

// SEND DISPLAY UPDATE PACKETS

// =========================================================================





//-----------------------------------------------------------------------------

/*!

 \brief    void SendDisplayBytes(void)



 send display bytes to head unit



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     none



 */

//-----------------------------------------------------------------------------



static void SendDisplayBytes(void)

{

   SendByte(disc); // disc display value

   SendDisplayBytesNoCD();

}





//-----------------------------------------------------------------------------

/*!

 \brief    void SendDisplayBytesNoCD(void)





 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     none



 */

//-----------------------------------------------------------------------------



static void SendDisplayBytesNoCD(void)

{

   uint8_t send_byte_u8 = 0;



   SendByte(track);

   SendByte(minute);

   SendByte(second);



   // D4 - scan on, mix on

   // D0 - scan on, mix off

   // 04 - scan off, mix on

   // 00 - scan off, mix off



   if (mix == TRUE) // mode (scan/mix)

   {

      send_byte_u8 |= 0x20; // turn on mix light

   }

   if (scan == TRUE)

   {

      send_byte_u8 |= 0x10; // turn on scan display

   }

   SendByte(send_byte_u8);

}





//-----------------------------------------------------------------------------

/*!

 \brief    void SendDisplayBytesInitCD(void)



 When sending an "init cd" packet, we need to send it the number of

 tracks and whatnot available on the CD. Required on Audi Concert II so

 that track up/dn buttons work.



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     none



 */

//-----------------------------------------------------------------------------



static void SendDisplayBytesInitCD(void)

{

   SendByte(0x99); // number of tracks total (99)?

   SendByte(0x99); // total minutes?

   SendByte(0x59); // total seconds?

   SendByte(0x48); // 48, 53, 31, 25, and 37 seen from real CDC,

   // no idea what it really means.

}





//-----------------------------------------------------------------------------

/*!

 \brief    void SendFrameByte(uint8_t byte_u8)



 SendFrameByte - sends a framing byte to head unit (first and last bytes).



 If the ACK flag is set, we modify the send byte to send an

 acknowledgement.



 \author     Koelling

 \date       06.10.2007



 \param[in]  byte_u8 -> byte to send

 \param[out] none

 \return     none



 */

//-----------------------------------------------------------------------------



static void SendFrameByte(uint8_t byte_u8)

{

   if (ACKcount == 0)

   {

      SendByte(byte_u8);

   }

   else

   {

      byte_u8 |= 0x20; // flag acknowledgement

      ACKcount++;

      SendByte(byte_u8);

   }

}





//-----------------------------------------------------------------------------

/*!

 \brief    void SendFrameByte(uint8_t byte_u8)



 SendByte - sends a byte to head unit.





 \author     Koelling

 \date       06.10.2007



 \param[in]  byte_u8 -> byte to send

 \param[out] none

 \return     none



 */

//-----------------------------------------------------------------------------



static void SendByte(uint8_t byte_u8)

{

   static uint8_t display_byte_counter_u8 = 0;

   // wait for head unit to store sent byte

   // 335us didn't work so good on late 2003 wolfsburg double din,

   // so we now wait 700us instead.

   display_byte_buffer_mau8[display_byte_counter_u8] = byte_u8;

   display_byte_counter_u8++;

   if (display_byte_counter_u8 == 8)

   {

      display_byte_counter_u8 = 0;

      TCCR2B |= _BV(CS22); // prescaler = 64 -> 1 timer clock tick is 4us long

      //TCCR2A |= _BV(CS20);

      TIMSK2 |= _BV(OCIE2A); // enable output compare interrupt on timer2

   }

}





//-----------------------------------------------------------------------------

/*!

 \brief     void EnqueueString(const uint8_t addr PROGMEM)



 EnqueueString - Adds a new string pointer into the outgoing serial string

 queue.



 \author     Koelling

 \date       02.10.2007



 \param[in]  const uint8_t addr PROGMEM -> start address of string to display

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void EnqueueString(const uint8_t *addr PROGMEM)

{

   txbuffer[txinptr] = addr;

   txinptr++;

   if (txinptr == TX_BUFFER_END)

   {

      txinptr = 0;

   }

}



//-----------------------------------------------------------------------------

/*!

 \brief     void EnqueueHex(uint8_t hexbyte_u8)



 The byte is converted to a two byte ASCII hexidecimal string



 \author     Koelling

 \date       05.10.2007



 \param[in]  uint8_t hexbyte -> hexbyte to display

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void EnqueueHex(uint8_t hexbyte_u8)

{

   uint8_t nibble_u8;



   nibble_u8 = hexbyte_u8 >> 4; // send high nibble first

   nibble_u8 <<= 1; // multiply high nibble by 2

   EnqueueString(&sHEX[nibble_u8]);



   nibble_u8 = hexbyte_u8 & 0x0F; // prepare low nibble

   nibble_u8 <<= 1; // multiply low nibble by 2

   EnqueueString(&sHEX[nibble_u8]);

}







//-----------------------------------------------------------------------------

/*!

 \brief     ResetTime(void)



 reset time information



 \author     Koelling

 \date       27.09.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void ResetTime(void)

{

   secondcount = SECONDWAIT;

   second = 0;

   minute = 0;

}



//-----------------------------------------------------------------------------

/*!

 \brief     void SendStateIdle(void)



 send data for idle state



 \author     Koelling

 \date       29.09.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void SendStateIdle(void)

{

   secondcount = SECONDWAIT; // stop display from ticking time

   SendFrameByte(0x8B);

   SendDisplayBytes();

   SendByte(0x70); // mutes audio on Monsoon head units

   SendFrameByte(0x83);

}



//-----------------------------------------------------------------------------

/*!

 \brief     SendStatePlayLeadInEnd(void)



 send data for state PlayLeadInEnd



 \author     Koelling

 \date       29.09.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void SendStatePlayLeadInEnd(void)

{

   SendFrameByte(0xC3);

   BIDIcount++;

   if (BIDIcount == 0)

   {

      SetStatePlay();

   }

}



//-----------------------------------------------------------------------------

/*!

 \brief     SendStateInitPlayEnd(void)



 send data for state StateInitPlayEnd



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void SendStateInitPlayEnd(void)

{

   SendFrameByte(0xC3);

   BIDIcount++;

   if (BIDIcount == 0)

   {

      SetStatePlayLeadIn();

   }

}





//-----------------------------------------------------------------------------

/*!

 \brief     SendStateInitPlayAnnounceCD(void)



 send data for state StateInitPlayAnnounceCD



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void SendStateInitPlayAnnounceCD(void)

{

   // 0xF6..0xF0: CD-ROM Loaded (seen on changer)

   // 0xE6..0xE0: CD-ROM Loaded. (made up)

   // 0x96..0x90: Slot Empty (seen on changer)

   // 0x86..0x80: Slot Empty (made up)

   // 0xD6..0xD0: AUDIO CD Loaded. (seen on changer)

   SendByte(discload);

   if (discload == 0xD6)

   {

      discload = 0xD1;

   }

   else

   {

      discload++;

   }

   SendDisplayBytesInitCD();

   SendByte(0x00);

   SendStateInitPlayEnd();

}



//-----------------------------------------------------------------------------

/*!

 \brief     SendStatePlayLeadInAnnounceCD(void)



 send data for state StatePlayLeadInAnnounceCD



 \author     Koelling

 \date       06.10.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void SendStatePlayLeadInAnnounceCD(void)

{

   SendByte((disc & 0x0F) | 0xD0);

   SendDisplayBytesInitCD();

   SendByte(0x00);

   SendStatePlayLeadInEnd();

}





//-----------------------------------------------------------------------------

/*!

 \brief     void SendPacket(void)



 depending on BIDIstate data packet will be sent



 \author     Koelling

 \date       27.09.2007



 \param[in]  none

 \param[out] none

 \return     void

 */

//-----------------------------------------------------------------------------



static void SendPacket(void)

{

   switch (BIDIstate) {

      case StateIdle:

         SendStateIdle();

         break;



      case StateIdleThenPlay:

         BIDIcount++;

         if (BIDIcount == 0)

         {

            SetStateInitPlay();

            SendStateIdle();

         }

         else

         {

            SendStateIdle();

         }

         break;



         // 34 2E ED DE AF B7 FF 3C

         // 34 BE FE FF FF FF EF 3C

         // 34 2D EB BE AB AC FF 3C

         // 34 BE FE FF FF FF EF 3C

         // 34 2C EC CE AA CE FF 3C

         // 34 BE FE FF FF FF EF 3C

         // 34 2B EE EE B7 DA FF 3C

         // 34 BE FE FF FF FF EF 3C

         // 34 2A EB BE A6 C8 FF 3C

         // 34 BE FE FF FF FF EF 3C

         // 34 69 00 FF FF FF FF 3C

         // 34 BE FE FF FF FF EF 3C



      case StateInitPlay:

         secondcount = SECONDWAIT; // stop display from ticking time

         SendFrameByte(0xCB);

         if ((BIDIcount & 0x01) == 0)

         {

            SendStateInitPlayAnnounceCD();

            break;

         }

         SendDisplayBytes();

         SendByte(0x10);



      case StateInitPlayEnd:

         SendStateInitPlayEnd();

         break;



      case StateInitPlayAnnounceCD:

         SendStateInitPlayAnnounceCD();

         break;



      case StatePlayLeadIn:

         // 34 BE FE FF FF FF AE 3C (play lead-in)

         // 34 2E ED DE AF B7 FF 3C

         // 34 BE FE FF FF FF AE 3C

         // 34 2E ED DE AF B7 FF 3C

         // 34 BE FE FF FF FF AE 3C

         // 34 2E ED DE AF B7 FF 3C

         // 34 BE FE FF FF FF AE 3C

         // 34 2E ED DE AF B7 FF 3C

         // 34 BE FE FF FF FF AE 3C

         secondcount = SECONDWAIT; // stop display from ticking time

         SendFrameByte(0xCB);



         if ((BIDIcount & 0x01) == 0)

         {

            SendStatePlayLeadInAnnounceCD();

            break;

         }

         SendDisplayBytes();

         SendByte(0x51);



      case StatePlayLeadInEnd:

         SendStatePlayLeadInEnd();

         break;



      case StatePlayLeadInAnnounceCD:

         SendStatePlayLeadInAnnounceCD();

         break;



      case StateTrackLeadIn:

         secondcount = SECONDWAIT; // stop display from ticking time

         SendFrameByte(0xCB);

         SendDisplayBytes();

         SendByte(0x51);

         SendFrameByte(0xC3);

         BIDIcount++;



         if (BIDIcount == 0)

         {

            break;

         }

         SetStatePlay();

         break;



      case StatePlay:

         SendFrameByte(0xCB);

         SendDisplayBytes();

         SendByte(0x30);

         SendFrameByte(0xC3);

         break;

      default:

         break;

   }



}



int main()

{

   Serial.begin(9600);

   Init_VWCDC();



   while (1)

   {

      CDC_Protocol();

   }



}



static void printstr_p(const char *s)

{

   char c;



   for (c = pgm_read_byte(s); c; ++s, c = pgm_read_byte(s))

   {

      if (c == '\r')

         break;

      Serial.print(c);

   }

}





// eof //




