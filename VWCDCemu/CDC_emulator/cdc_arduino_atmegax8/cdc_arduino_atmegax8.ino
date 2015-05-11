//tested:
// atmega328P-PU
// atmega8A-PU

//#define USESPI
#ifdef USESPI
#include <SPI.h>
#endif

#define DataIn 11
#define ClockPin 13
#define DataOut 2

#define BYTES_DELAY 874
#define PACKET_DALEY 50

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
#define CDC_PREV_CD 0x18

#define MODE_PLAY 0xFF
#define MODE_SHFFL 0x55
#define MODE_SCAN 0x00

#define RADIO_OUT_IS_HIGH (PIND & (1<<PD2))

volatile uint16_t captimehi = 0;
volatile uint16_t captimelo = 0;
volatile uint8_t capturingstart = 0;
volatile uint8_t capturingbytes = 0;
volatile uint32_t cmd = 0;
volatile uint8_t cmdbit = 0;
volatile uint8_t newcmd = 0;
volatile uint8_t prevcmd = 0;

volatile uint8_t cd=1;
volatile uint8_t tr=1;
volatile uint8_t mode=MODE_PLAY;
volatile uint8_t idle=0;
volatile uint8_t DO_UPDATE=1;
volatile uint8_t load_cd=0;
volatile long previousMillis=0;

uint8_t getCommand(uint32_t cmd2);
void cdc_read_Data_out();
void cdc_send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7);
void cdc_setup(int DataOut_pin);
void myTransfer(uint8_t val);

//#define TX_BUFFER_END 12
//uint16_t txbuffer[TX_BUFFER_END];
//uint8_t txinptr=0;
//uint8_t txoutptr=0;
//static void Enqueue(uint16_t num);

void setup(){

  Serial.begin(9600);
  Serial.println("vw group cd changer emulator");

  cdc_setup(DataOut);
  //init RADIO:
  //  send_package(0x74,0xFF^cd,0xFF^tr,0xFF,0xFF,mode,0x8F,0x7C); //idle
  //  delay(10);
  //  send_package(0x34,0xFF^cd,0xFF^tr,0xFF,0xFF,mode,0xFA,0x3C); //load disc
  //  delay(100);
  //  send_package(0x74,0xFF^cd,0xFF^tr,0xFF,0xFF,mode,0x8F,0x7C); //idle
  //  delay(10);
}

void loop(){

  if(newcmd)
  {
    newcmd=0;
    uint8_t c = getCommand(cmd);
//    Serial.println(cmd,HEX);

    if (c){
//      Serial.println(c,HEX);
      switch(c)
      {
      case CDC_STOP:
        prevcmd=CDC_STOP;
        break;
      case CDC_PLAY_NORMAL:
      case CDC_PLAY:
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
      case CDC_PREV_CD:
        prevcmd=CDC_PREV_CD;
        break;
      case CDC_END_CMD:
        if (prevcmd==CDC_PLAY_NORMAL || prevcmd==CDC_PLAY)
        {
          idle=0;
          mode = MODE_PLAY;
          //14BEFEFFFFFFAE1C
          send_package(0x14,0xFF^cd,0xFF^tr,0xFF,0xFF,mode,0xAE,0x1C);
          //previousMillis=millis();
          DO_UPDATE=1;
        }
        else if (prevcmd==CDC_PREV)
        {
          idle=0;
          tr--;
          //FF
          if(tr == 0) tr = 153;
          if((tr & 0xF) == 0xF) tr = tr-6;
          DO_UPDATE=1;
        }
        else if (prevcmd==CDC_NEXT)
        {
          idle=0;
          tr++;
          if(tr == 154) tr = 1;
          if((tr & 0xF) == 0xA) tr = tr+6;
          DO_UPDATE=1;
        }
        else if (prevcmd==CDC_END_CMD2) //NEXTCD
        {
          idle=0;
          cd++;
          if(cd==7)cd=1;
          DO_UPDATE=1;
        }
        else if (prevcmd==CDC_PREV_CD)
        {
          idle=0;
          cd--;
          if(cd==0)cd=6;
          DO_UPDATE=1;
        } 
        else if (prevcmd==CDC_STOP)
        {
          idle=1;
          idle_sended=0;
          //54BEFEFFFEFF8F5C
          send_package(0x54,0xFF^cd,0xFF^tr,0xFF,0xFF,mode,0x8f,0x5C);
          //previousMillis=millis();
          DO_UPDATE=1;
        }
        prevcmd=CDC_END_CMD;
        break;
      case CDC_END_CMD2:
        if (prevcmd==CDC_CD1)
        {
          idle=0;
          cd = 1;//0xBE;
          DO_UPDATE=1;
        }
        else if (prevcmd==CDC_CD2)
        {
          idle=0;
          cd = 2;//0xBD;
          DO_UPDATE=1;
        }
        else if (prevcmd==CDC_CD3)
        {
          idle=0;
          cd = 3;//0xBC;
          DO_UPDATE=1;
        }
        else if (prevcmd==CDC_CD4)
        {
          idle=0;
          cd = 4;//0xBB;
          DO_UPDATE=1;
        }
        else if (prevcmd==CDC_CD5)
        {
          idle=0;
          cd = 5;//0xBA;
          DO_UPDATE=1;
        }
        else if (prevcmd==CDC_CD6)
        {
          idle=0;
          cd = 6;//0xB9;
          DO_UPDATE=1;
        }
        else if (prevcmd==CDC_END_CMD)
        {
          //NEXT CD
          prevcmd=CDC_END_CMD2;
        }
        else {
          prevcmd=0;
        }
        break;
      }

      //    Serial.print("CD: ");
      //    Serial.print(cd);
      //    Serial.print("  TR: ");
      //    Serial.print(tr);
      //    Serial.print("  mode: ");
      //    Serial.print(mode,HEX);
      //    Serial.print("  idle: ");
      //    Serial.println(idle);
    }
  }

  if ((millis()-previousMillis)>PACKET_DALEY || DO_UPDATE){
    if(idle){
        send_package(0x74,0xFF^cd,0xFF^tr,0xFF,0xFF,mode,0x8F,0x7C);
    }
    else
    {
      send_package(0x34,0xFF^cd,0xFF^tr,0xFF,0xFF,mode,0xCF,0x3C);
    }

    previousMillis=millis();
    DO_UPDATE=0; 
  }

//  while (txoutptr != txinptr)
//
//  {
//
//    Serial.println(txbuffer[txoutptr]);
//
//
//    txoutptr++;
//
//    if (txoutptr == TX_BUFFER_END)
//
//    {
//      txoutptr = 0;
//
//    }
//
//  }

}

void cdc_setup(int pin){

  pinMode(pin,INPUT);

  if (pin == 2) attachInterrupt(0,read_Data_out,CHANGE);
  if (pin == 3) attachInterrupt(1,read_Data_out,CHANGE);
  cli();//stop interrupts
  //for atmegax8 micros
  //TODO: cpu detection ... 
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0;
  // Set CS11 bit for 8 => tick every 1us@8MHz, 0.5us@16MHz
  // Set CS11 bit and CS10 for 64 prescaler => tick every 8us@8MHz, 4us@16MHz
  TCCR1B |= (1 << CS11);
  sei();//allow interrupts
#ifdef USESPI
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV128); //62.5kHz@8Mhz 125kHz@16MHz
  //SPI.setClockDivider(SPI_CLOCK_DIV64);//125kHz@8Mhz
#else
  pinMode(DataIn,OUTPUT);
  pinMode(ClockPin,OUTPUT);
#endif
}

void send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7)
{
//  Serial.print(c0,HEX);
//  Serial.print(",");
//  Serial.print(c1,HEX);
//  Serial.print(",");
//  Serial.print(c2,HEX);
//  Serial.print(",");
//  Serial.print(c3,HEX);
//  Serial.print(",");
//  Serial.print(c4,HEX);
//  Serial.print(",");
//  Serial.print(c5,HEX);
//  Serial.print(",");
//  Serial.print(c6,HEX);
//  Serial.print(",");
//  Serial.println(c7,HEX);
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

void myTransfer(uint8_t val){
//  Serial.println(val,HEX);
#ifdef USESPI
  SPI.transfer(val);
#else
  for (uint8_t i = 0; i < 8; i++)  {
    digitalWrite(ClockPin, HIGH);
    digitalWrite(DataIn, !!(val & (1 << (7 - i))));
    delayMicroseconds(5);
    digitalWrite(ClockPin, LOW);
    delayMicroseconds(5);
  }
#endif
}

void read_Data_out() //remote signals
{
//  Enqueue(cmdbit);
  if(RADIO_OUT_IS_HIGH/*digitalRead(DataOut)*/)
  {
    if (capturingstart || capturingbytes)
    {
      captimelo = TCNT1;
//            Enqueue(0);
//            Enqueue(captimelo);
    }
    else
      capturingstart = 1;
    TCNT1 = 0;

    //eval times
    //high: 9000us = 18000tics
    //low:  4500us = 9000tics(0.5us tick@16MHz)
    
//    if (captimehi > 17800 && captimehi < 18200 && captimelo > 8800 && captimelo < 9200)
    if (captimehi > 16600 && captimelo > 7000)
    {
      capturingstart = 0;
      capturingbytes = 1;
//           cmdbit=0;
//            cmd=0;
    }
    //high: 550us  = 1100ticks (0.5us tick@16MHz)
    //low:  1700us = 3400ticks (0.5us tick@16MHz)
//    else if(capturingbytes && captimehi > 900 && captimehi < 1300 && captimelo > 3200 && captimelo < 3500)
    else if(capturingbytes && captimelo > 3000)
    {
      cmd = (cmd<<1) | 0x00000001;
      cmdbit++;
    }
    //high: 550us = 1100ticks (0.5us tick@16MHz)
    //low:  550us = 1100ticks (0.5us tick@16MHz)
//    else if (capturingbytes && captimehi > 900 && captimehi < 1300 &&  captimelo > 900 && captimelo < 1300)
    else if (capturingbytes &&  captimelo > 1000)
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
//          Enqueue(1);
//          Enqueue(captimehi);
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


ISR(TIMER1_OVF_vect)
{
cmdbit=0;
Serial.printl("OVF");
}

//static void Enqueue(uint16_t num)
//
//{
//
//  txbuffer[txinptr] = num;
//
//  txinptr++;
//
//  if (txinptr == TX_BUFFER_END)
//
//  {
//
//    txinptr = 0;
//
//  }
//
//}





