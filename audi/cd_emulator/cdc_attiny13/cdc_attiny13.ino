// Definitions
#define rxPin NULL
#define txPin 4

#define CPU_CLOCK 16
#define TIMER 0
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


#define DataOut 2 //signal from radio, attiny85 pin 7- INT0
#define DataOutINT 0 //INT0
//data to radio, attiny85 pin 6
#define DataIn 1
//clk to radio, attiny85 pin 5
#define CLK 0

#define BUFFER 10

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

#define BYTES_DELAY 750//874
#define PACKET_DALEY 100

volatile uint8_t cd=0xBE;
volatile uint8_t tr=0xFE;
volatile uint8_t mode=MODE_PLAY;

void send_package();
void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);

void setup(){
  pinMode(DataOut,INPUT);
  digitalWrite(DataOut,HIGH);

  send_package(0x34,cd,tr,0xFF,0xFF,mode,0xFA,0x3C); //load disc
  delay(100);
  send_package(0x74,cd,tr,0xFF,0xFF,mode,0x8F,0x7C); //idle
  delay(10);
}

void loop(){



    send_package(0x34,cd,tr,0xFF,0xFF,mode,0xCF,0x7c);
  delay(PACKET_DALEY);

}


void send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7)
{
  myShiftOut(DataIn,CLK, MSBFIRST, c0);
  //tinySPI::transfer(c0);
  delayMicroseconds(BYTES_DELAY);
  myShiftOut(DataIn,CLK, MSBFIRST, c1);
  //tinySPI::transfer(c1);
  delayMicroseconds(BYTES_DELAY);
  myShiftOut(DataIn,CLK, MSBFIRST, c2);
  //tinySPI::transfer(c2);
  delayMicroseconds(BYTES_DELAY);
  myShiftOut(DataIn,CLK, MSBFIRST, c3);
  //tinySPI::transfer(c3);
  delayMicroseconds(BYTES_DELAY);
  myShiftOut(DataIn,CLK, MSBFIRST, c4);
  //tinySPI::transfer(c4);
  delayMicroseconds(BYTES_DELAY);
  myShiftOut(DataIn,CLK, MSBFIRST, c5);
  //tinySPI::transfer(c5);
  delayMicroseconds(BYTES_DELAY);
  myShiftOut(DataIn,CLK, MSBFIRST, c6);
  //tinySPI::transfer(c6);
  delayMicroseconds(BYTES_DELAY);
  myShiftOut(DataIn,CLK, MSBFIRST, c7);
  //tinySPI::transfer(c7);
}


void myShiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
  uint8_t i;

  for (i = 0; i < 8; i++)  {
    if (bitOrder == LSBFIRST)
      digitalWrite(dataPin, !!(val & (1 << i)));
    else	
      digitalWrite(dataPin, !!(val & (1 << (7 - i))));

    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);		
  }
}


