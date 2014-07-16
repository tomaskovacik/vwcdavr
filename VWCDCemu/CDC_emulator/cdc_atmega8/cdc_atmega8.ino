// works also with ATMEGA8 - tested
//SPI PINS used for transfer from CDCemulator to RADIO:
// MOSI (arduino pin 11) -> DataIN
// CLK (arduino pin 13) -> CLK


#define DEBUG
#define DataOut 2
#define BUFFER 10
#define TIMESTARTHIGH 17000
#define TIMESTARTLOW 8000
#define TIMEONE 3300
#define TIMEZERO 1000
//#define USESPI
#define DataPin 11
#define ClockPin 13

#ifdef USESPI
#include <SPI.h>
#endif

#define BYTES_DELAY 874 //in microseconds
#define PACKET_DELAY 1000 //in miliseconds

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


volatile uint16_t captimehi=0;
volatile uint16_t captimelo=0;
volatile uint8_t capturingstart=0;
volatile uint8_t capturingbytes=0;
volatile uint8_t bcmd[BUFFER];
volatile uint32_t cmd;
volatile uint8_t cmd_write_buffer_pointer=0;
volatile uint8_t cmd_read_buffer_pointer=0;
volatile uint8_t cmdbit=0;
volatile uint8_t newcmd[BUFFER];
volatile uint8_t prevcmd=0;

volatile uint8_t cd=0xBE;
volatile uint8_t tr=0xFE;
volatile uint8_t mode=MODE_PLAY;
volatile uint8_t idle=1;
volatile uint8_t load_cd=0;

long last_packet=0;

//#include <SoftwareSerial.h>

uint8_t getCommand(uint32_t cmd2);
void read_Data_out();
void send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7);
void myTransfer(uint8_t b);

void setup(){

  pinMode(DataOut,INPUT_PULLUP);
  pinMode(DataPin,OUTPUT);
  pinMode(ClockPin,OUTPUT);
  noInterrupts();//  cli();//stop interrupts
  //set timer1 interrupt at 20kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0;
  // Set CS11 bit for 8 => tick every 1us@8MHz, 0.5us@16MHz
  // Set CS11 bit and CS10 for 64 prescaler => tick every 8us@8MHz, 4us@16MHz
  TCCR1B |= (1 << CS11);//| (1 << CS10);
  attachInterrupt(0,read_Data_out,CHANGE);
  interrupts();//sei();//allow interrupts
  Serial.begin(9600);
  Serial.println("start");
#ifdef USESPI
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV128); //62.5kHz@8Mhz 125kHz@16MHz
#endif
  //SPI.setClockDivider(SPI_CLOCK_DIV64);//125kHz@8Mhz

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
      Serial.println(bcmd[cmd_read_buffer_pointer],HEX);
      newcmd[cmd_read_buffer_pointer]=0;
      bcmd[cmd_read_buffer_pointer]=0;
#ifdef DEBUG
      Serial.print("CD: ");
      Serial.print(cd,HEX);
      Serial.print("  TR: ");
      Serial.print(tr,HEX);
      Serial.print("  mode: ");
      Serial.print(mode,HEX);
      Serial.print("  idle: ");
      Serial.println(idle);
#endif
    }
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
//  delay(PACKET_DELAY);
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
  Serial.println();
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
Serial.print(val,HEX);
}
void read_Data_out() //remote signals
{
  //if (newcmd==0){
  if(digitalRead(DataOut))
  {
    if (capturingstart || capturingbytes)
    {
      captimelo = TCNT1;   
    }
    else
      capturingstart = 1;
    TCNT1 = 0;
//#ifdef DEBUG
//    Serial.print(cmdbit);
//    Serial.print(" ");
//    Serial.print(captimehi);
//    Serial.print(" ");
//    Serial.print(captimelo);
//#endif
    //eval times
    //tick @0.5us
    //9000us HIGH and 4500us LOW
    if (captimehi > TIMESTARTHIGH && captimelo > TIMESTARTLOW )
    {
      capturingstart = 0;
      capturingbytes = 1;
      //      cmdbit=0;
      //      cmd=0;
//#ifdef DEBUG
//      Serial.println("startseq found");
//#endif
    } //logic one = 1700us
    else if(capturingbytes && captimelo > TIMEONE)
    {
//#ifdef DEBUG
//      Serial.println(" bit 1");
//#endif
      cmd = (cmd<<1) | 0x00000001;
      cmdbit++;
    }
    else if (capturingbytes && captimelo > TIMEZERO)
    {
//#ifdef DEBUG
//      Serial.println(" bit 0");
//#endif
      cmd = (cmd<<1);
      cmdbit++;
    }

    if(cmdbit == 32)
    {
//#ifdef DEBUG
//      Serial.println("32bit");
//#endif
      if (getCommand(cmd))
      {
        bcmd[cmd_write_buffer_pointer]=getCommand(cmd);
        //Serial.println(bcmd[cmd_write_buffer_pointer],HEX);
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
    captimehi = TCNT1;
    TCNT1 = 0;
  }
  // }
}

