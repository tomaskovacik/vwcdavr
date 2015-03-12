#include <SoftwareSerial.h>
SoftwareSerial mySerial(NULL, 4); // RX, TX

// SPI part for ATTINY85 written by Nick Gammon
// http://www.gammon.com.au/forum/?id=10892
// March 2013

// ATMEL ATTINY45 / ARDUINO pin mappings
//
//                         +-\/-+
// RESET  Ain0 (D 5) PB5  1|    |8  Vcc
// CLK1   Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1  SCK  / USCK / SCL
// CLK0   Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1  MISO / DO
//                   GND  4|    |5  PB0 (D 0) pwm0  MOSI / DI / SDA
//                         +----+


#define DataOut PB2 //signal from radio, attiny85 pin 7- INT0
//data to radio, attiny85 pin 6
#define DataPin 1
//clk to radio, attiny85 pin 5
#define ClockPin 0

//#define USESPI //nospi on attiny85

#ifdef USESPI
//#include <SPI.h>//nospi on attiny85 
#endif

#define RADIO_OUT_IS_HIGH (PINB & (1<<DataOut))

#define CDC_PREFIX1 0x53
#define CDC_PREFIX2 0x2C

#define CDC_END_CMD 0x14
#define CDC_END_CMD2 0x38
#define CDC_PLAY 0xE4
#define CDC_STOP 0x10
#define CDC_NEXT 0xF8
#define CDC_PREV 0x78
#define CDC_SEEK_FWD 0xD8
#define CDC_SEEK_RWD 0x58
#define CDC_CD1 0x0C
#define CDC_CD2 0x8C
#define CDC_CD3 0x4C
#define CDC_CD4 0xCC
#define CDC_CD5 0x2C
#define CDC_CD6 0xAC
#define CDC_SCAN 0xA0
#define CDC_SFL 0x60
#define CDC_PLAY_NORMAL 0x08

#define MODE_PLAY 0xFF
#define MODE_SHFFL 0x55
#define MODE_SCAN 0x00

#define BYTES_DELAY 874 //in microseconds
#define PACKET_DELAY 1000 //in miliseconds

volatile uint16_t captimehi = 0;
volatile uint16_t captimelo = 0;
uint16_t old_captimehi = 0;
uint16_t old_captimelo = 0;
volatile uint8_t capturingstart = 0;
volatile uint8_t capturingbytes = 0;
volatile uint32_t cmd;
volatile uint8_t cmd_write_buffer_pointer = 0;
volatile uint8_t cmd_read_buffer_pointer = 0;
volatile uint8_t cmdbit = 0;
volatile uint8_t tmp = 0;
volatile uint8_t newcmd;
volatile uint8_t prevcmd = 0;

volatile uint8_t cd=0xBE;
volatile uint8_t tr=0xFE;
volatile uint8_t mode=MODE_PLAY;
volatile uint8_t idle=1;
volatile uint8_t load_cd=0;
volatile uint8_t busy=0;

long last_packet=0;

uint8_t getCommand(uint32_t cmd2);
void cdc_read_Data_out();
void cdc_send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7);
void cdc_setup(int DataOut_pin);
void myTransfer(uint8_t val);

void setup(){
  
  mySerial.begin(9600);
  mySerial.println("attiny85 start");

  cdc_setup(DataOut);



 
  send_package(0x34,cd,tr,0xFF,0xFF,mode,0xFA,0x3C); //load disc
  delay(100);
  send_package(0x74,cd,tr,0xFF,0xFF,mode,0x8F,0x7C); //idle
  delay(10);
}

void loop(){

  if(newcmd)
  {
   Serial.println(cmd,HEX);
   switch(getCommand(cmd))
    {
    case CDC_STOP:
      prevcmd=CDC_STOP;
      break;
    case CDC_PLAY_NORMAL: 
      prevcmd=CDC_PLAY_NORMAL;
      break;
    case CDC_PREV:
      prevcmd=CDC_PREV;
      break;
    case CDC_NEXT:
      prevcmd=CDC_NEXT;
      break;
    case CDC_CD1:
      prevcmd=CDC_CD1;
      break;
    case CDC_CD2:
      prevcmd=CDC_CD2;
      break;
    case CDC_CD3:
      prevcmd=CDC_CD3;
      break;
    case CDC_CD4:
      prevcmd=CDC_CD4;
      break;
    case CDC_CD5:
      prevcmd=CDC_CD5;
      break;
    case CDC_CD6:
      prevcmd=CDC_CD6;
      break;
    case CDC_END_CMD:
      if (prevcmd==CDC_PLAY_NORMAL)
      {
        idle=0;
        mode = MODE_PLAY;
      }
      else if (prevcmd==CDC_PREV)
      {
        idle=0;
        tr++;
      }
      else if (prevcmd==CDC_NEXT)
      {
        idle=0;
        tr--;
      }
      else if (prevcmd==CDC_NEXT)
      {
        idle=0;
        tr=tr-1;
      } 
      else if (prevcmd==CDC_STOP)
      {
        idle=1;
      }
      prevcmd=0;
      break;
    case CDC_END_CMD2:
      if (prevcmd==CDC_CD1)
      {
        idle=0;
        cd = 0xBE;
      }
      else if (prevcmd==CDC_CD2)
      {
        idle=0;
        cd = 0xBD;
      }
      else if (prevcmd==CDC_CD3)
      {
        idle=0;
        cd = 0xBC;
      }
      else if (prevcmd==CDC_CD4)
      {
        idle=0;
        cd = 0xBB;
      }
      else if (prevcmd==CDC_CD5)
      {
        idle=0;
        cd = 0xBA;
      }
      else if (prevcmd==CDC_CD6)
      {
        idle=0;
        cd = 0xB9;
      }
      prevcmd=0;
      break;
    }
   newcmd=0;
    Serial.print("CD: ");
    Serial.print(cd,HEX);
    Serial.print("  TR: ");
    Serial.print(tr,HEX);
    Serial.print("  mode: ");
    Serial.print(mode,HEX);
    Serial.print("  idle: ");
    Serial.println(idle);
  }


if((millis()-last_packet)>PACKET_DELAY)
{
 if(idle){
 send_package(0x74,cd,tr,0xFF,0xFF,mode,0x8F,0x7C);
 last_packet=millis();
 }
 else
 {
 send_package(0x34,cd,tr,0xFF,0xFF,mode,0xCF,0x7c);
 last_packet=millis();
 }
// delay(PACKET_DELAY);
}

}

void cdc_setup(int pin){
  
  pinMode(pin,INPUT);
  
  GIMSK|=(1<<INT0); //INT0 enable
  MCUCR|=(1<<ISC00);// any logic change fire interupt routine
  cli();//stop interrupts
  //for attiny85
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  // 8bit counters here
  //prescaler:             1024  256    64 ...
  //1 tick in us:           128   32     8 ...
  //tics per pulse: 9000us:  70  281  1125 
  // only way is to use 1024 prescler
  TCCR0B |= (1<<CS02) | (1<<CS00);
  TCNT0=0;
  sei();//allow interrupts
#ifdef USESPI
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV128); //62.5kHz@8Mhz 125kHz@16MHz
  //SPI.setClockDivider(SPI_CLOCK_DIV64);//125kHz@8Mhz
#else
  pinMode(DataPin,OUTPUT);
  pinMode(ClockPin,OUTPUT);
#endif
  
  
}

void send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7)
{
  myTransfer(c0);
  delayMicroseconds(874);
  myTransfer(c1);
  delayMicroseconds(874);
  myTransfer(c2);
  delayMicroseconds(874);
  myTransfer(c3);
  delayMicroseconds(874);
  myTransfer(c4);
  delayMicroseconds(874);
  myTransfer(c5);
  delayMicroseconds(874);
  myTransfer(c6);
  delayMicroseconds(874);
  myTransfer(c7);
}

void myTransfer(uint8_t val){
#ifdef USESPI
  SPI.transfer(val);
#else
  for (uint8_t i = 0; i < 8; i++)  {
    digitalWrite(ClockPin, HIGH);
    digitalWrite(DataPin, !!(val & (1 << (7 - i))));
    delayMicroseconds(50);
    digitalWrite(ClockPin, LOW);
    delayMicroseconds(50);
  }
#endif
}

void read_Data_out() //remote signals
{
  if(digitalRead(DataOut))
  {
    if (capturingstart || capturingbytes)
    {
      captimelo = TCNT1;
    }
    else
    capturingstart = 1;
    TCNT1 = 0;

    //eval times
    //high: 9000us = 18000tics
    //low:  4500us = 9000tics(0.5us tick@16MHz)
    //ATTINY85 @ 8MHz and 8bit counter:
    //high: 9000us = 70ticks (128us tick@8MHz)
    //low:  4500us = 35ticks (128us tick@8MHz)
    //if (captimehi > 17900 && captimehi < 18100 && captimelo > 8900 && captimelo < 9100)
if (captimehi == 70 && captimelo == 35 )
    {
      capturingstart = 0;
      capturingbytes = 1;
      cmdbit=0;
      cmd=0;
    }
    //high: 550us  = 1100ticks (0.5us tick@16MHz)
    //low:  1700us = 3400ticks (0.5us tick@16MHz)
    //ATTINY85 @ 8MHz and 8bit counter:
    //high: 550us = 4ticks (128us tick@8MHz)
    //low:  1700us = 13ticks (128us tick@8MHz)
    //else if(capturingbytes && captimehi > 1000 && captimehi < 1200 && captimelo > 3300 && captimelo < 3500)
    else if(capturingbytes && captimehi == 4 && captimelo == 13)
    {
      //Serial.println("bit 1");
      cmd = (cmd<<1) | 0x00000001;
      cmdbit++;
    }
    //high: 550us = 1100ticks (0.5us tick@16MHz)
    //low:  550us = 1100ticks (0.5us tick@16MHz)
    //else if (capturingbytes && captimehi > 1000 && captimehi < 1200 &&  captimelo > 1000 && captimelo < 1200)
    //ATTINY85 @ 8MHz and 8bit counter:
    //high: 550us = 4ticks (128us tick@8MHz)
    //low:  550us = 4ticks (128us tick@8MHz)
    else if (capturingbytes && captimehi == 4 &&  captimelo == 4 )
    {
      cmd = (cmd<<1);
      cmdbit++;
    }

    if(cmdbit == 32)
    {
      newcmd = 1;
      cmdbit = 0;
      capturingbytes = 0;
    }
  }
  else
  {
      captimehi = TCNT1; 
      TCNT1 = 0;
  }
}

uint8_t getCommand(uint32_t cmd2)
{
  if (((cmd2>>24) & 0xFF) == CDC_PREFIX1 && ((cmd2>>16) & 0xFF) == CDC_PREFIX2)
    if (((cmd2>>8) & 0xFF) == (0xFF^((cmd2) & 0xFF)))
      return (cmd2>>8) & 0xFF;
  return 0;
}
