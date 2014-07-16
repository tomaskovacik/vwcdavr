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
#define DataOutINT 0 //INT0
//data to radio, attiny85 pin 6
#define DataIn 1
//clk to radio, attiny85 pin 5
#define CLK 0

#define DEBUGCLK 3
#define DEBUGDATA 4

//shifout compatibility
#define DataPin DataIn
#define ClockPin CLK 

#define BUFFER 10

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
volatile uint8_t bcmd[BUFFER];
volatile uint32_t cmd;
volatile uint8_t cmd_write_buffer_pointer = 0;
volatile uint8_t cmd_read_buffer_pointer = 0;
volatile uint8_t cmdbit = 0;
volatile uint8_t tmp = 0;
volatile uint8_t newcmd[BUFFER];
volatile uint8_t prevcmd = 0;

volatile uint8_t cd=0xBE;
volatile uint8_t tr=0xFE;
volatile uint8_t mode=MODE_PLAY;
volatile uint8_t idle=1;
volatile uint8_t load_cd=0;
volatile uint8_t busy=0;

long last_packet=0;

//void read_Data_out();
//void send_package();
//uint8_t getCommand(uint32_t cmd2);
//void myTransfer(uint8_t val);

void setup(){
  
  mySerial.begin(9600);
  mySerial.println("attiny85 start");
  
  pinMode(DataOut,INPUT);
  //digitalWrite(DataOut,HIGH);
  pinMode(DataIn,OUTPUT);
  pinMode(CLK,OUTPUT);
  digitalWrite(CLK,LOW);
  cli();//stop interrupts
//#ifdef TIMER == 0
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCCR0B |= (1<<CS02) | (1<<CS00); //prescaler 1024 tick=1024*1/8000000 = 128us
//#define TIMER_COUNTER TCNT0
#define TIMESTARTHIGH 70 //9000us/tick(us) = 140,625
#define TIMESTARTLOW 35 //4500us/tick(us) = 70,3125 
#define TIMEONE 13 //1700us/tick(us) = 26,5625
#define TIMEZERO 4 //550us/tick(us) = 8,59375
//#elif TIMER == 1
//  TCCR1 = 0;// set entire TCCR1 register to 0
//  TCCR1 |= (1 << CS13)|(1 << CS11);//|(1<<CS10); //prescaler 512 tick=512*1/8000000 = 64us
////#define TIMER_COUNTER TCNT1
//#define TIMESTARTHIGH 140 //9000us/tick(us) = 562,5
//#define TIMESTARTLOW 70 //4500us/tick(us) = 281,25
//#define TIMEONE 37 //1700us/tick(us) = 106,25
//#define TIMEZERO 12 //550us/tick(us) = 34,375
TCNT0=0;
//#endif
//  TIMER_COUNTER  = 0;//initialize counter value to 0; 
  sei();//allow interrupts
//  attachInterrupt(DataOutINT,read_Data_out,CHANGE);
  GIMSK|=(1<<INT0); //INT0 enable
  MCUCR|=(1<<ISC00);// any logic change fire interupt routine
 
  send_package(0x34,cd,tr,0xFF,0xFF,mode,0xFA,0x3C); //load disc
  delay(100);
  send_package(0x74,cd,tr,0xFF,0xFF,mode,0x8F,0x7C); //idle
  delay(10);
}

void loop(){
  
  for(int cmd_read_buffer_pointer=0;cmd_read_buffer_pointer<BUFFER;cmd_read_buffer_pointer++)
  {
    if(newcmd[cmd_read_buffer_pointer])
    {
      switch(bcmd[cmd_read_buffer_pointer])
      {
      case CDC_STOP:
        prevcmd=CDC_STOP;
        break;
      case CDC_PLAY_NORMAL:
        prevcmd=CDC_PLAY_NORMAL;
        break;
      case CDC_PLAY:
        prevcmd=CDC_PLAY;
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
        if (prevcmd==CDC_PLAY_NORMAL || prevcmd==CDC_PLAY)
        {
          idle=0;
          mode = MODE_PLAY;
        }
        else if (prevcmd==CDC_PREV)
        {
          idle=0;
          tr++;
          if ((tr & 0xF)==0xA) tr=tr+6;
        }
        else if (prevcmd==CDC_NEXT)
        {
          idle=0;
          tr--;
          if (tr == 0xFF) tr=99; // no need to fix EF DF CF BF AF
          if ((tr & 0xF)==0xF) tr=tr-6; // no need to fix *E *D *C *B *A
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
      newcmd[cmd_read_buffer_pointer]=0;
      bcmd[cmd_read_buffer_pointer]=0;
    }
  }

//  //if((millis()-last_packet)>PACKET_DELAY)
// // {
//    if(idle){
//      send_package(0x74,cd,tr,0xFF,0xFF,mode,0x8F,0x7C);
//      //last_packet=millis();
//    }
//    else
//    {
//      send_package(0x34,cd,tr,0xFF,0xFF,mode,0xCF,0x7c);
//      //last_packet=millis();
//    }
//      delayMicroseconds(PACKET_DELAY*1000);
//  //}

if (old_captimehi!=captimehi)
{
mySerial.print("HI ");
mySerial.println(captimehi);
old_captimehi=captimehi;
}
if (old_captimelo!=captimelo)
{
mySerial.print("LO ");
mySerial.println(captimelo);
old_captimelo=captimelo;
}
}

uint8_t getCommand(uint32_t cmd2)
{
  if (((cmd2>>24) & 0xFF) == CDC_PREFIX1 && ((cmd2>>16) & 0xFF) == CDC_PREFIX2)
    if (((cmd2>>8) & 0xFF) == (0xFF^((cmd2) & 0xFF)))
      return (cmd2>>8) & 0xFF;
  return 0;
}

void send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7)
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
  delayMicroseconds(BYTES_DELAY);
}

ISR(INT0_vect) 
//void read_Data_out() //remote signals
{
  //send_package(RADIO_OUT_IS_HIGH,0,1,2,3,4,5,6);
//  busy=1;
  //if (newcmd==0){
  //if(digitalRead(DataOut))
 if(RADIO_OUT_IS_HIGH)
  {
    if (capturingstart || capturingbytes)
    {
      captimelo = TCNT0;   
    }
    else
      capturingstart = 1;
    TCNT0 = 0;
    //eval times
    //tick @0.5us
    //9000us HIGH and 4500us LOW
    if (captimehi > TIMESTARTHIGH && captimelo > TIMESTARTLOW )
    {
      capturingstart = 0;
      capturingbytes = 1;
      cmdbit=0;
      cmd=0;
    } //logic one = 1700us
    else if(capturingbytes && captimelo > TIMEONE)
    {
      cmd = (cmd<<1) | 0x00000001;
      cmdbit++;
    }
    else if (capturingbytes && captimelo > TIMEZERO)
    {
      cmd = (cmd<<1);
      cmdbit++;
    }

    if(cmdbit == 32)
    {
      if (getCommand(cmd))
      {
        bcmd[cmd_write_buffer_pointer]=getCommand(cmd);
        newcmd[cmd_write_buffer_pointer++] = 1;
      }
      
      capturingbytes = 0;
      cmdbit=0;
      cmd=0;
      if(cmd_write_buffer_pointer==BUFFER) cmd_write_buffer_pointer=0;
    }
  }
  else
  {
   captimehi = TCNT0;
   TCNT0 = 0;
  }
  // }
//  busy=0;
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
