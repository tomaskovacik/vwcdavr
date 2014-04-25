#include <SPI.h>

#define DataOut 2

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

volatile uint16_t captimehi = 0;
volatile uint16_t captimelo = 0;
volatile uint8_t capturingstart = 0;
volatile uint8_t capturingbytes = 0;
volatile uint32_t cmd = 0;
volatile uint8_t tmp_cmd = 0;
volatile uint8_t cmdbit = 0;
volatile uint8_t newcmd = 0;
volatile uint8_t prevcmd = 0;

volatile uint8_t cd=0xBE;
volatile uint8_t tr=0xFE;
volatile uint8_t mode=MODE_PLAY;
volatile uint8_t idle=1;
volatile uint8_t load_cd=0;

//#include <SoftwareSerial.h>

uint8_t getCommand(uint32_t cmd2);

void send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7)
{
  SPI.transfer(c0);
  delayMicroseconds(874);
  SPI.transfer(c1);
  delayMicroseconds(874);
  SPI.transfer(c2);
  delayMicroseconds(874);
  SPI.transfer(c3);
  delayMicroseconds(874);
  SPI.transfer(c4);
  delayMicroseconds(874);
  SPI.transfer(c5);
  delayMicroseconds(874);
  SPI.transfer(c6);
  delayMicroseconds(874);
  SPI.transfer(c7);
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

    //eval times
    if (captimehi > 8300/8 && captimelo > 3500/8)
    {
      capturingstart = 0;
      capturingbytes = 1;
      cmdbit=0;
      cmd=0;
      //Serial.println("startseq found");
    }
    else if(capturingbytes && captimelo > 1500/8)
    {
      //Serial.println("bit 1");
      cmd = (cmd<<1) | 0x00000001;
      cmdbit++;
    }
    else if (capturingbytes && captimelo > 500/8)
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
      captimehi = TCNT1; 
      TCNT1 = 0;
  }
 // }
}

void setup(){

  pinMode(DataOut,INPUT);
  attachInterrupt(0,read_Data_out,CHANGE);
  //  cli();//stop interrupts
  //set timer1 interrupt at 20kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0;
  // set timer count for 20khz increments
  //OCR1A = 65000;// = (16*10^6) / (1000000*8) - 1 (must be <256)
  // turn on CTC mode
  //TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 => tick every 1us;64 prescaler => tick every 8us
  TCCR1B |= (1 << CS11)| (1 << CS10);
  // enable timer compare interrupt
  //TIMSK1 |= (1 << OCIE1A);
  sei();//allow interrupts
  Serial.begin(57600);
  Serial.println("start");
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV128); //62.5kHz@8Mhz
  //SPI.setClockDivider(SPI_CLOCK_DIV64);//125kHz@8Mhz
  //  
//  send_package(0x74,cd,tr,0xFF,0xFF,mode,0x8F,0x7C); //idle
//  delay(10);
//
//  send_package(0x34,0xBF,tr,0xFF,0xFF,mode,0xFA,0x3C); //load disc
//  delay(100);
//  send_package(0x74,cd,tr,0xFF,0xFF,mode,0x8F,0x7C); //idle
//  delay(10);
}

void loop(){

  if(newcmd)
  {
  // tmp_cmd=getCommand(cmd);
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


  if(idle){
    send_package(0x74,cd,tr,0xFF,0xFF,mode,0x8F,0x7C);
  }
  else
  {
    send_package(0x34,cd,tr,0xFF,0xFF,mode,0xCF,0x7c);
  }
  delay(100);

}

uint8_t getCommand(uint32_t cmd2)
{
  if (((cmd2>>24) & 0xFF) == CDC_PREFIX1 && ((cmd2>>16) & 0xFF) == CDC_PREFIX2)
    if (((cmd2>>8) & 0xFF) == (0xFF^((cmd2) & 0xFF)))
      return (cmd2>>8) & 0xFF;
  return 0;
}








