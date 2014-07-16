// works also with ATMEGA8 - tested
// SPI PINS used for transfer from CDCemulator to RADIO:
// MOSI (arduino pin 11) -> DataIN
// CLK (arduino pin 13) -> CLK

#include <SPI.h>
//#include <SoftwareSerial.h>

#define PREV 9 //PB1,pin 15
#define PLAY 5 //PD5, pin 11
#define NEXT 6 //PD6,pin 12
#define DataOut 2
#define BUFFER 10



#define BYTES_DELAY 874//874
#define PACKET_DALEY 100

#ifdef DataOut==2
#define Data_out_interrupt 0
#elif DataOut==3
#define Data_out_interrupt 1
#endif

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
#define cd1 0x0C
#define cd2 0x8C
#define cd3 0x4C
#define cd4 0xCC
#define cd5 0x2C
#define cd6 0xAC
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
volatile uint32_t cmd[BUFFER];
volatile uint8_t cmd_write_buffer_pointer = 0;
volatile uint8_t cmd_read_buffer_pointer = 0;
volatile uint8_t cmdbit = 0;
volatile uint8_t newcmd[BUFFER];
volatile uint8_t prev_cmd = 0;

volatile uint8_t cd=0xBE;
volatile uint8_t tr=0xFE;
volatile uint8_t mode=MODE_PLAY;
volatile uint8_t idle=1;
volatile uint8_t load_cd=0;

uint8_t getCommand(uint32_t cmd2);
void send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7);
void read_Data_out();
void prev_song();
void toggle_play_mode();
void next_song();
void play_cd(uint8_t cd);

void setup(){
  pinMode(PREV, OUTPUT);
  pinMode(PLAY, OUTPUT);
  pinMode(NEXT, OUTPUT);
  digitalWrite(PREV, LOW);
  digitalWrite(PLAY, LOW);
  digitalWrite(NEXT, LOW);

  pinMode(DataOut,INPUT_PULLUP);
  attachInterrupt(Data_out_interrupt,read_Data_out,CHANGE);
  cli();//stop interrupts
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0;
  // Set CS11 bit for 8 => tick every 1us@8MHz, 0.5us@16MHz
  // Set CS11 bit and CS10 for 64 prescaler => tick every 8us@8MHz, 4us@16MHz
  TCCR1B |= (1 << CS11);//| (1 << CS10);
  sei();//allow interrupts
//  Serial.begin(115200);
//  Serial.println("start");
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV128); //62.5kHz@8Mhz 125kHz@16MHz
  //SPI.setClockDivider(SPI_CLOCK_DIV64);//125kHz@8Mhz

}

void loop(){

  send_package(0xCB,0xb1,0x01,0x01,0x50,0x08,0x30,0xc3);
  delay(8);

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
  SPI.transfer(c0);
  delayMicroseconds(BYTES_DELAY);
  SPI.transfer(c1);
  delayMicroseconds(BYTES_DELAY);
  SPI.transfer(c2);
  delayMicroseconds(BYTES_DELAY);
  SPI.transfer(c3);
  delayMicroseconds(BYTES_DELAY);
  SPI.transfer(c4);
  delayMicroseconds(BYTES_DELAY);
  SPI.transfer(c5);
  delayMicroseconds(BYTES_DELAY);
  SPI.transfer(c6);
  delayMicroseconds(BYTES_DELAY);
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
    //tick @0.5us
    //9000us HIGH and 4500us LOW
    if (captimehi > 17000 && captimelo > 8000 )//&& captimehi < 19000/8 && captimelo < 10000)
    {
      capturingstart = 0;
      capturingbytes = 1;
//      cmdbit=0;
//      cmd=0;
      //Serial.println("startseq found");
    } //logic one = 1700us
    else if(capturingbytes && captimelo > 3300 )//&& captimelo < 3500)
    {
      //Serial.println("bit 1");
      cmd[cmd_write_buffer_pointer] = (cmd[cmd_write_buffer_pointer]<<1) | 0x00000001;
      cmdbit++;
    }
    else if (capturingbytes && captimelo > 1000)// && captimelo < 1200)
    {
      //Serial.println("bit 0");
      cmd[cmd_write_buffer_pointer] = (cmd[cmd_write_buffer_pointer]<<1);
      cmdbit++;
    }

    if(cmdbit == 32)
    {
      //Serial.println("32bit");
      //Serial.println(cmd[cmd_write_buffer_poiter],HEX);
      newcmd[cmd_write_buffer_pointer] = 1;
      capturingbytes = 0;
      cmdbit=0;
      cmd_write_buffer_pointer++;
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

void prev_song()
{
digitalWrite(PREV, HIGH);
delay(100);
digitalWrite(PREV, LOW);
delay(25);
digitalWrite(PREV, HIGH);
delay(100);
digitalWrite(PREV, LOW);
}
void toggle_play_mode()
{
digitalWrite(PLAY,HIGH);
delay(100);
digitalWrite(PLAY, LOW);
}
void next_song()
{
digitalWrite(NEXT,HIGH);
delay(100);
digitalWrite(NEXT, LOW);
}

void play_cd(uint8_t cd)
{
toggle_play_mode();
}