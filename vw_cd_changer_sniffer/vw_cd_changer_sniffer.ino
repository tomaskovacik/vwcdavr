/*****************************************************************************

   Audi/Volkswagen Audi Interface sniffer
   Target: arduino atmega328(tested)
   
   Author: Vincent
   Date: January, 2011
   Version: 0.01

   Based on code originally ported from VWCDPIC Project:
   http://www.k9spud.com/vwcdpic/
  
   Ported from Matthias Koelling's AVR Code: 
   www.mikrocontroller.net/attachment/31279/vwcdc.c

   ---------------------------------
   16. Mar 2015:
   https://github.com/tomaskovacik/
   arduino duemilanove works:
   
   RADIO PIN -> arduino pin
   
   DataOut   -> digital 8
   DataIn    -> digital 13
   Clock     -> digital 12

   License: GNU GPL v2

 *****************************************************************************/

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

#define TX_BUFFER_END   12
#define CAP_BUFFER_END	24

#define Baudrate  9600

#define RADIO_COMMAND      PB0 //pb0=digital 8 on arduino nano (Seeeduino Mega PIN PD4)
#define RADIO_COMMAND_DDR  DDB0
#define Clk 3
#define DataIn 4

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

#define DUMPMODE
//#define DUMPMODE2

const uint8_t sDATAERR[] PROGMEM = "dataerr\n\r";
const uint8_t sOVERFLOW[] PROGMEM = "overflow\n\r";
const uint8_t sNEWLINE[] PROGMEM = "\n\r";
const uint8_t sHEX[] PROGMEM = {
   '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0, '9', 0, 'A', 0, 'B', 0, 'C', 0, 'D', 0, 'E', 0, 'F', 0
};

uint8_t scanptr; // pointer to command byte to inspect next
uint8_t fsr;
uint8_t scanbyte; // most recently retrieved command byte
// the 'capbusy' flag will be set when the ISR is busy capturing a command
// packet from the head unit. The TMR0 ISR will clear it once the recieve
// timeout has been exceeded or the PWTX capture ISR will clear it once
// 32 bits have been captured (command packets are supposed to be 32 bits
// long only).
uint8_t capbusy;

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


uint8_t txinptr;
uint8_t txoutptr;
uint8_t const *txbuffer[TX_BUFFER_END]; // 39-26-1 = 12 serial output strings queue
uint8_t capbuffer[CAP_BUFFER_END]; // 64-39-1 = 24 bytes for head unit command
static void EnqueueString(const uint8_t *addr PROGMEM);
static void EnqueueHex(uint8_t hexbyte_u8);
static void printstr_p(const char *s);

#define TRUE 1
#define FALSE 0

uint8_t mode=0;
uint8_t cd=0;
//int cdpointer=0;
uint8_t play=0;
uint8_t tr=0;
uint8_t minutes=0;
uint8_t sec=0;
uint8_t state=0;
uint8_t state2=0;
uint8_t ack=0;
uint8_t ack_cd=0;
volatile uint64_t cmd=0;
volatile uint64_t prev_cmd=0;
volatile int cmdbit=0;
volatile uint8_t newcmd=0;
uint8_t nl=0;


int verbose=1;


void readDataIn()
{

  if(!newcmd)
 {

    if(digitalRead(DataIn))
    {//1
      cmd=(cmd<<1)|1;
    }
    else
    {//0
      cmd = (cmd<<1);
    }
    cmdbit++;
  }

  if(cmdbit==64)
  {
    newcmd=1;
    cmdbit=0;
  }
}




void Init_VWCDC(void)
{
   DDRB &= ~_BV(RADIO_COMMAND_DDR); // input capture as input
   PORTB |= _BV(RADIO_COMMAND); // enable pull up
   TCCR1A = 0x00; // Normal port operation, OC1A/OC1B/OC1C disconnected
   TCCR1B = _BV(ICNC1); // noise canceler, int on falling edge
   TCCR1B |= _BV(CS11); // prescaler = 8 -> 1 timer clock tick is 0.5µs long

   capptr = 0; // indirect pointer to capture buffer
   scanptr = 0;
   capbit = -8;
   txinptr = 0; // queue pointers
   txoutptr = 0;

   capbusy = FALSE; // reset flags

   overflow = FALSE;
   dataerr = FALSE;

#ifdef DUMPMODE
   startbit = FALSE;
#endif

   TIFR1 |= _BV(ICF1); // clear pending interrupt
   TIMSK1 |= _BV(ICIE1); // enable input capture interrupt on timer1

   SREG |= 0x80;   // enable interrupts

  pinMode(Clk,INPUT);
  pinMode(DataIn,INPUT);
 
  attachInterrupt(digitalPinToInterrupt(Clk),readDataIn,FALLING);
}

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


void CDC_Protocol(void)
{
  if (Serial.available() > 0)
  {
    if (Serial.read() == 'v') verbose=!verbose;
  }
 
  if (newcmd && prev_cmd != cmd && cmd != 0 && nl==0)
  {
    prev_cmd = cmd;
    newcmd = 0;
    uint8_t temp;
        
    if (verbose) {
      mode = ((cmd >> 56) & 0xFF);
      Serial.print(mode, HEX);
      Serial.print(" ");
      cd = ((cmd >> 48) & 0xFF);
      Serial.print(cd, HEX);
      Serial.print(" ");
      tr = ((cmd >> 40) & 0xFF);
      Serial.print(tr, HEX);
      Serial.print(" ");
      minutes = ((cmd >> 32) & 0xFF);
      Serial.print(minutes, HEX);
      Serial.print(" ");
      sec = ((cmd >> 24) & 0xFF);
      Serial.print(sec, HEX);
      Serial.print(" ");

      state = ((cmd >> 16) & 0xFF);
      Serial.print(state, HEX);
      Serial.print(" ");
      state2 = ((cmd >> 8) & 0xFF);
      Serial.print(state2, HEX);
      Serial.print(" ");
      temp = (cmd & 0xFF);
      Serial.print(temp, HEX);
      Serial.print("               ");
      Serial.print("   CD: ");
      Serial.print(cd ^ 0xBF, HEX);
      Serial.print(" track: ");
      Serial.print(tr ^ 0xFF, HEX);
      Serial.print("   min: ");
      Serial.print(minutes ^ 0xFF, HEX  );
      Serial.print(" sek: ");
      Serial.print(sec ^ 0xFF, HEX);
//      Serial.print(" state: ");
//      Serial.print(state, HEX);
//      Serial.print(" state2: ");
//      Serial.println(state2, HEX);
      Serial.println();
    }
    else
    {
      Serial.print("   CD: ");
      Serial.print(int (((cmd >> 48) & 0xFF) ^ 0xBF), HEX);
      Serial.print(" track: ");
      Serial.print(int (((cmd >> 40) & 0xFF) ^ 0xFF), HEX);
      Serial.print("   min: ");
      Serial.print(int (((cmd >> 32) & 0xFF) ^ 0xFF), HEX);
      Serial.print(" sek: ");
      Serial.print(int (((cmd >> 24) & 0xFF) ^ 0xFF), HEX);
      Serial.print(" mode: ");
      Serial.print(int (((cmd >> 16) & 0xFF)), HEX);
      Serial.println();
    }
  }
  
  if (newcmd && prev_cmd == cmd)
  {
    newcmd=0;
  }
  
  if (newcmd && cmd==0)
  {
    newcmd=0;
  }
  
   uint8_t decimal_adjust_u8;

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

   if (startbit == TRUE) // have we just recieved a start bit?
   {
      startbit = FALSE;
      nl=0;
      // EnqueueString(sNEWLINE); // yes, start a new line
   }

   fsr = scanptr;

   while (GetCaptureByte() == TRUE)
   {
      scanptr = fsr;
      EnqueueHex(scanbyte);
      nl++;
   }
   if (nl==4){
    EnqueueString(sNEWLINE);
    nl=0;
   }

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

static void EnqueueString(const uint8_t *addr PROGMEM)
{
   txbuffer[txinptr] = addr;
   txinptr++;

   if (txinptr == TX_BUFFER_END)
   {
      txinptr = 0;
   }
}

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

int main()
{
   Serial.begin(Baudrate);
   Serial.println("CDC sniffer");
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
