#include "vwcdc.h"
#include "SPI.h"

int cdc::getCommand(long cmd)
{
  if (((cmd>>24) & 0xFF) == CDC_PREFIX1 && ((cmd>>16) & 0xFF) == CDC_PREFIX2)
    if (((cmd>>8) & 0xFF) == (0xFF^((cmd) & 0xFF)))
      return (cmd>>8) & 0xFF;
  return 0;
}


/*

initialization of CDC pins

names are inverted, to coresponde labels on Head units (radios)

params:
(int)     DataOut pin for  data from radio
(boolean) USESPI - true = use spi, false=pulseout
(int)     DataIn pin if no SPI is used this is for output data stream
(int)     Clock  pin if no SPI is used this is for output clock to clocking DataIn
work in progress:
for now this all must be done in mail setup();
*/
cdc::cdc(int DataOut_pin,boolean USESPI,int DataIn_pin,int Clk_pin){
  _DataOut_pin=DataOut_pin;
  _DataIn_pin=DataIn_pin;
  _Clk_pin=Clk_pin;
  _USESPI=USESPI;
  newcmd=0;

  pinMode(_DataOut_pin,INPUT); // signals from radio
if(!_USESPI){
  pinMode(_DataIn_pin,INPUT); // data to radio
  pinMode(_Clk_pin,INPUT); // clock to radio
}


//work in progress:
//for now this all must be done in mail setup();
  cli();//stop interrupts

#if defined(__AVR_ATmega168__)\
||defined(__AVR_ATmega168P__)\
||defined(__AVR_ATmega328P__)\
||defined(__AVR_ATmega8__)
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCCR1B |= (1 << CS11);  // Set CS11 bit for 8 => tick every 1us@8MHz, 0.5us@16MHz 
  #define TIMER=TCNT1 
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV128); //62.5kHz@8Mhz
  //SPI.setClockDivider(SPI_CLOCK_DIV64);//125kHz@8Mhz
#elif defined(__AVR_ATtiny85__)
  GIMSK|=(1<<INT0); //INT0 enable
  MCUCR|=(1<<ISC00);// any logic change fire interupt routine

  //for attiny85
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  // 8bit counters here
  //prescaler:             1024  256    64 ...
  //1 tick in us:           128   32     8 ...
  //tics per pulse: 9000us:  70  281  1125 
  // only way is to use 1024 prescler
  TCCR0B |= (1<<CS02) | (1<<CS00);
  #define TIMER=TCNT0
  

#else
#error unknown CPU
#endif
  TIMER  = 0;//initialize counter value to 0;
  sei();//allow interrupts
  if (_DataOut_pin==2)
	attachInterrupt(0,read_Data_out,CHANGE);
  if (_DataOut_pin==3)
  	attachInterrupt(1,read_Data_out,CHANGE);

};

void cdc::init(){
  send_package(0x74,0xBE,0xFE,0xFF,0xFF,0xFF,0x8F,0x7C); //idle
  delay(10);
  send_package(0x34,0xBE,0xFE,0xFF,0xFF,0xFF,0xFA,0x3C); //load disc
  delay(100);
  send_package(0x74,0xBE,0xFE,0xFF,0xFF,0xFF,0x8F,0x7C); //idle
  delay(10);
}

void cdc::myTransfer(int val){
if (_USESPI){
  SPI.transfer(val);
} else {
  for (uint8_t i = 0; i < 8; i++)  {
    digitalWrite(_Clk_pin, HIGH);
    digitalWrite(_DataIn_pin, !!(val & (1 << (7 - i))));
    delayMicroseconds(10);
    digitalWrite(_Clk_pin, LOW);
    delayMicroseconds(10);
  }
}
}


void cdc::send_package(int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
  myTransfer(c0);
  delayMicroseconds(BYTES_DELAY);
  myTransfer(c1);
  delayMicroseconds(BYTES_DELAY);
  myTransfer(c2);
  delayMicroseconds(BYTES_DELAY);
  myTransfer(c3);
  delayMicroseconds(BYTES_DELAY);
  myTransfer(c4);
  delayMicroseconds(BYTES_DELAY);
  myTransfer(c5);
  delayMicroseconds(BYTES_DELAY);
  myTransfer(c6);
  delayMicroseconds(BYTES_DELAY);
  myTransfer(c7);
}

void cdc::read_Data_out() //remote signals
{
  if(digitalRead(_DataOut_pin))
  {
    if (capturingstart || capturingbytes)
    {
      captimelo = TIMER;
    }
    else
    capturingstart = 1;
    TIMER = 0;

    //eval times
    //cca 9000us HIGH, 4500us LOW,; 18000tiks and 9000tics(0.5us tick@16MHz)
    if (captimehi > 16600 && captimelo > 8000)
    {
      capturingstart = 0;
      capturingbytes = 1;
      cmdbit=0;
      cmd=0;
      //Serial.println("startseq found");
    }//cca 1700us; 3400ticks (0.5us tick@16MHz)
    else if(capturingbytes && captimelo > 3300)
    {
      //Serial.println("bit 1");
      cmd = (cmd<<1) | 0x00000001;
      cmdbit++;
    }//cca 550us; 1100ticks (0.5us tick@16MHz)
    else if (capturingbytes && captimelo > 1000)
    {
      //Serial.println("bit 0");
      cmd = (cmd<<1);
      cmdbit++;
    }

    if(cmdbit == 32)
    {
      newcmd = 1;
      capturingbytes = 0;
    }
  }
  else
  {
      captimehi = TIMER; 
      TIMER = 0;
  }
}
