#include <SPI.h>

/////////////////////////////////////////
// definitions
/////////////////////////////////////////
//WRITE TO CLUSTER
#define FIS_WRITE_ENA 3
#define FIS_WRITE_CLK 4
#define FIS_WRITE_DATA 5
#define FIS_WRITE_PULSEW 50
#define FIS_WRITE_STARTPULSEW 100
#define FIS_WRITE_START 15
//END WRITE TO CLUSTER

//READ MSG FROM RADIO
#define FIS_READ_intCLK 1 //interupt on FIS_READ_CLK line
#define FIS_READ_CLK 3 //clk
#define FIS_READ_DATA 11 //data
#define FIS_READ_ENA 2 //enable 
//END READ MSG FROM RADIO
//CDC EMULATION
#define CPU_CLOCK 16

// SPI PINS used for CDC_transfer from CDCemulator to RADIO:
// MOSI (arduino pin 11) -> DataIN
// CLK (arduino pin 13) -> CLK
#define CDC_DataOut 2
#define CDC_intDataOut 0
#define CDC_BUFFER 10

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

#define CDC_MODE_PLAY 0xFF
#define CDC_MODE_SHIFT 0x55
#define CDC_MODE_SCAN 0x00

#define CDC_BYTES_DELAY 750//874 //microseconds
#define CDC_RADIO_REFRESH 100 //miliseconds
//END CDC EMULATION

/////////////////////////////////////////
// end definitions
/////////////////////////////////////////

/////////////////////////////////////////
// variables
/////////////////////////////////////////
//WRITE TO CLUSTER
String FIS_WRITE_line1="";
String FIS_WRITE_line2="";
long FIS_WRITE_rotary_position_line1=-8;
long FIS_WRITE_rotary_position_line2=-8;
char FIS_WRITE_CHAR_FROM_SERIAL;
int FIS_WRITE_line=1;
long FIS_WRITE_last_refresh=0;
int FIS_WRITE_CD_TEN=1;
int FIS_WRITE_TR_TEN=1;
//END WRITE TO CLUSTER

//READ MSG FROM RADIO
volatile uint8_t FIS_READ_adr=0;
volatile uint64_t FIS_READ_msg1=0;
volatile uint64_t FIS_READ_msg2=0;
volatile uint8_t FIS_READ_cksum=0;
volatile uint8_t FIS_READ_msgbit=0;
volatile uint8_t FIS_READ_newmsg1=0;
volatile uint8_t FIS_READ_newmsg2=0;
volatile uint8_t FIS_READ_adrok=0;
volatile uint8_t FIS_READ_cksumok=0;
volatile uint8_t FIS_READ_tmp_cksum=0;
volatile uint8_t FIS_READ_TP_ON=0;
volatile uint8_t FIS_READ_CDC_ON=0;
//END READ MSG FROM RADIO

// CDC EMULATION
volatile uint16_t CDC_captimehi = 0;
volatile uint16_t CDC_captimelo = 0;
volatile uint8_t CDC_capturingstart = 0;
volatile uint8_t CDC_capturingbytes = 0;
volatile uint32_t CDC_cmd[CDC_BUFFER];
volatile uint8_t CDC_cmd_write_buffer_poiter = 0;
volatile uint8_t CDC_cmd_read_buffer_pointer = 0;
volatile uint8_t CDC_cmdbit = 0;
volatile uint8_t CDC_new_cmd[CDC_BUFFER];
volatile uint8_t CDC_prev_cmd = 0;

volatile uint8_t CDC_cd=0xBE;
volatile uint8_t CDC_tr=0xFE;
volatile uint8_t CDC_mode=CDC_MODE_PLAY;
volatile uint8_t CDC_idle=1;
volatile uint8_t CDC_load_cd=0;
long CDC_last_refresh_radio=0;
// END CDC EMULATION

/////////////////////////////////////////
// end variables
/////////////////////////////////////////


/////////////////////////////////////////
// functions
/////////////////////////////////////////
//WRITE TO CLUSTER
void FIS_WRITE_sendTEXT(String FIS_WRITE_line1,String FIS_WRITE_line2);
void FIS_WRITE_sendBit(int Bit);
void FIS_WRITE_startENA();
void FIS_WRITE_stopENA();
//END WRITE TO CLUSTER

//READ MSG FROM RADIO
void FIS_READ_read_data_line();
void FIS_READ_read_cksum();
void FIS_READ_read_msg1();
void FIS_READ_read_msg2();
void FIS_READ_read_adr();
//END READ MSG FROM RADIO

//CDC EMULATION
void CDC_decode();
uint8_t CDC_getCommand(uint32_t CDC_cmd2);
void CDC_send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7);
void CDC_read_DataOut();
//END CDC EMULATION
/////////////////////////////////////////
// end functions
/////////////////////////////////////////

void setup(){
  //WRITE TO CLUSTER
  pinMode(FIS_WRITE_ENA, OUTPUT);
  pinMode(FIS_WRITE_CLK, OUTPUT);
  pinMode(FIS_WRITE_DATA, OUTPUT);
  digitalWrite(FIS_WRITE_ENA,LOW);
  digitalWrite(FIS_WRITE_CLK,HIGH);
  digitalWrite(FIS_WRITE_DATA,HIGH);
  Serial.begin(9600);
  //END WRITE TO CLUSTER

  //READ MSG FROM RADIO
  pinMode(FIS_READ_ENA,INPUT);
  pinMode(FIS_READ_CLK,INPUT_PULLUP);
  pinMode(FIS_READ_DATA,INPUT_PULLUP);
  attachInterrupt(FIS_READ_intCLK,FIS_READ_read_data_line,CHANGE);
  //END READ MSG FROM RADIO
  
  // CDC EMULATION
  pinMode(CDC_DataOut,INPUT_PULLUP);
  attachInterrupt(CDC_intDataOut,CDC_read_DataOut,CHANGE);
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0;
  TCCR1B |= (1 << CS11);//| (1 << CS10);
  sei();//allow interrupts
  Serial.begin(115200);
  Serial.println("start");
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV128); //62.5kHz@8Mhz 125kHz@16MHz
  CDC_send_package(0x34,CDC_cd,CDC_tr,0xFF,0xFF,CDC_mode,0xFA,0x3C); //load disc
  delay(100);
  CDC_send_package(0x74,CDC_cd,CDC_tr,0xFF,0xFF,CDC_mode,0x8F,0x7C); //CDC_idle
  delay(10);
  //END CDC EMULATION
}

void loop(){
//WRITE TO CLUSTER
if (FIS_READ_CDC_ON==1 && Serial.available()) { //serial input is interesting only if we are in CDC
        FIS_WRITE_CHAR_FROM_SERIAL=(char)Serial.read();
        
        if (FIS_WRITE_CHAR_FROM_SERIAL == '\n') {
            FIS_WRITE_line++;
        } else {
          switch (FIS_WRITE_line){
            case 1:
              FIS_WRITE_line1+=FIS_WRITE_CHAR_FROM_SERIAL;
              break;
            case 2:
              FIS_WRITE_line2+=FIS_WRITE_CHAR_FROM_SERIAL;
              break;
            case 3:
            //control CD
              switch (FIS_WRITE_CHAR_FROM_SERIAL){
              case '+': CDC_cd++; break;
              case '-': CDC_cd--;break;
              case '0':
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
                if(FIS_WRITE_CD_TEN) {
                  CDC_cd=FIS_WRITE_CHAR_FROM_SERIAL*10;
                  FIS_WRITE_CD_TEN=0;
                } else {
                  CDC_cd=CDC_cd+FIS_WRITE_CHAR_FROM_SERIAL;
                  FIS_WRITE_CD_TEN=1;
                }
              break;
              }
              break;
            case 4:
            //control tr
              switch (FIS_WRITE_CHAR_FROM_SERIAL){
              case '+': CDC_tr++; break;
              case '-': CDC_tr--;break;
              case '0':
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
                if(FIS_WRITE_TR_TEN) {
                  CDC_tr=FIS_WRITE_CHAR_FROM_SERIAL*10;
                  FIS_WRITE_TR_TEN=0;
                } else {
                  CDC_tr=CDC_tr+FIS_WRITE_CHAR_FROM_SERIAL;
                  FIS_WRITE_TR_TEN=1;
                }
              break;
              }
              break;
            case 5:
              FIS_WRITE_line=1;
              FIS_WRITE_line1="";
              FIS_WRITE_rotary_position_line1=-8;
              FIS_WRITE_line1+=FIS_WRITE_CHAR_FROM_SERIAL;
              FIS_WRITE_line2="";
              FIS_WRITE_rotary_position_line2=-8;
              break;
          }
        }

 } else {
    for(int i=56;i>=0;i=i-8){
      FIS_WRITE_line1[i/8]=(0xFF^((FIS_READ_msg1>>i) & 0xFF));//bolo - dal som ^
    }
    for(int i=56;i>=0;i=i-8){
      FIS_WRITE_line2[i/8]=(0xFF^((FIS_READ_msg2>>i) & 0xFF));//bolo - dal som ^   
    }
 }
   
  int FIS_WRITE_line1_length=FIS_WRITE_line1.length();
  int FIS_WRITE_line2_length=FIS_WRITE_line2.length();
  String FIS_WRITE_sendline1="        ";
  String FIS_WRITE_sendline2="        ";
  
   
    //do rotary and refresh each 0.5second
    //refresh custer each 5s 
    if(millis()-FIS_WRITE_last_refresh>500){
      
      if (FIS_WRITE_line1_length>8){
      for (int i=0;i<8;i++){
        if (FIS_WRITE_rotary_position_line1+i>=0 && (FIS_WRITE_rotary_position_line1+i)<FIS_WRITE_line1_length) {
          FIS_WRITE_sendline1[i]=FIS_WRITE_line1[FIS_WRITE_rotary_position_line1+i];
        }
      }
       if (FIS_WRITE_rotary_position_line1<FIS_WRITE_line1_length){
              FIS_WRITE_rotary_position_line1++;
       } else {
              FIS_WRITE_rotary_position_line1=-8;
              FIS_WRITE_sendline1="        ";
       }
     } else {
       FIS_WRITE_sendline1=FIS_WRITE_line1;
     }
     if (FIS_WRITE_line2_length>8){
      for (int i=0;i<8;i++){
        if (FIS_WRITE_rotary_position_line2+i>=0 && (FIS_WRITE_rotary_position_line2+i)<FIS_WRITE_line2_length) {
          FIS_WRITE_sendline2[i]=FIS_WRITE_line2[FIS_WRITE_rotary_position_line2+i];
        }
      }
       if (FIS_WRITE_rotary_position_line2<FIS_WRITE_line2_length){
              FIS_WRITE_rotary_position_line2++;
       } else {
             
              FIS_WRITE_rotary_position_line2=-8;
       }
     } else {
       FIS_WRITE_sendline2=FIS_WRITE_line2;
     }
      // Serial.println("refresh"); 
      FIS_WRITE_sendTEXT(FIS_WRITE_sendline1,FIS_WRITE_sendline2);
      FIS_WRITE_last_refresh=millis();
    //end refresh
  }
//END WRITE TO CLUSTER

  //READ MSG FROM RADIO
  if(FIS_READ_cksumok){
    FIS_READ_newmsg1=0;
    FIS_READ_newmsg2=0;
    FIS_READ_adrok=0;
    FIS_READ_cksumok=0;
    digitalWrite(FIS_READ_ENA,HIGH);
    delayMicroseconds(100);
    digitalWrite(FIS_READ_ENA,LOW);
    delayMicroseconds(100);
    digitalWrite(FIS_READ_ENA,HIGH);
    delayMicroseconds(100);
    pinMode(FIS_READ_ENA,INPUT);
    //check if have TP on
    //2nd msg last two chars=TP
    if ((FIS_READ_msg2 & 0xFF)=='TP'){
      FIS_READ_TP_ON=1;
    } else {
      FIS_READ_TP_ON=0;
    }
      //check if we are usind CDC EMULATION
    //if so, hack it:
    if (((FIS_READ_msg1>>48) & 0xFF)=='CD') {
      FIS_READ_CDC_ON=1;
    } else {
      FIS_READ_CDC_ON=0;
    }
  }
  //END READ MSG FROM RADIO
  
  //CDC EMULATION
  CDC_decode();

  if ((millis()-CDC_last_refresh_radio)>CDC_RADIO_REFRESH){
    if(CDC_idle){
      CDC_send_package(0x74,CDC_cd,CDC_tr,0xFF,0xFF,CDC_mode,0x8F,0x7C);
    }
    else
    {
      CDC_send_package(0x34,CDC_cd,CDC_tr,0xFF,0xFF,CDC_mode,0xCF,0x7c);
    }
    CDC_last_refresh_radio=millis();
  }
  //END CDC EMULATION

}



/////////////////////////////////////////
// functions
/////////////////////////////////////////
//WRITE TO CLUSTER 
void FIS_WRITE_sendTEXT(String FIS_WRITE_line1,String FIS_WRITE_line2) {
  int FIS_WRITE_line1_length=FIS_WRITE_line1.length();
  int FIS_WRITE_line2_length=FIS_WRITE_line2.length();
    if (FIS_WRITE_line1_length<=8){
        for (int i=0;i<(8-FIS_WRITE_line1_length);i++){
          FIS_WRITE_line1+=" ";
        } 
    }
    if (FIS_WRITE_line2_length<=8){
      for (int i=0;i<(8-FIS_WRITE_line2_length);i++){
        FIS_WRITE_line2+=" ";
      }
    }

int crc=(255-FIS_WRITE_START+FIS_WRITE_line1[0]+FIS_WRITE_line1[1]+FIS_WRITE_line1[2]+FIS_WRITE_line1[3]+FIS_WRITE_line1[4]+FIS_WRITE_line1[5]+FIS_WRITE_line1[6]+FIS_WRITE_line1[7]+FIS_WRITE_line2[0]+FIS_WRITE_line2[1]+FIS_WRITE_line2[2]+FIS_WRITE_line2[3]+FIS_WRITE_line2[4]+FIS_WRITE_line2[5]+FIS_WRITE_line2[6]+FIS_WRITE_line2[7])%256;
FIS_WRITE_startENA();
FIS_WRITE_sendBit(FIS_WRITE_START);
FIS_WRITE_sendBit(255-FIS_WRITE_line1[0]);
FIS_WRITE_sendBit(255-FIS_WRITE_line1[1]);
FIS_WRITE_sendBit(255-FIS_WRITE_line1[2]);
FIS_WRITE_sendBit(255-FIS_WRITE_line1[3]);
FIS_WRITE_sendBit(255-FIS_WRITE_line1[4]);
FIS_WRITE_sendBit(255-FIS_WRITE_line1[5]);
FIS_WRITE_sendBit(255-FIS_WRITE_line1[6]);
FIS_WRITE_sendBit(255-FIS_WRITE_line1[7]);
FIS_WRITE_sendBit(255-FIS_WRITE_line2[0]);
FIS_WRITE_sendBit(255-FIS_WRITE_line2[1]);
FIS_WRITE_sendBit(255-FIS_WRITE_line2[2]);
FIS_WRITE_sendBit(255-FIS_WRITE_line2[3]);
FIS_WRITE_sendBit(255-FIS_WRITE_line2[4]);
FIS_WRITE_sendBit(255-FIS_WRITE_line2[5]);
FIS_WRITE_sendBit(255-FIS_WRITE_line2[6]);
FIS_WRITE_sendBit(255-FIS_WRITE_line2[7]);
FIS_WRITE_sendBit(crc);
FIS_WRITE_stopENA();

}

void FIS_WRITE_sendBit(int Bit){
  static int iResult[8];
  for (int i = 0; i <= 7; i++)
  {    
    iResult[i] = Bit % 2;
    Bit = Bit / 2;
  }
  for(int i=7;i>=0;i--){
  switch (iResult[i]) {
    case 1: digitalWrite(FIS_WRITE_DATA,HIGH);
            digitalWrite(FIS_WRITE_CLK,LOW);
            break;
    case 0:digitalWrite(FIS_WRITE_DATA,LOW);
           digitalWrite(FIS_WRITE_CLK,LOW);
           break;
    }
    delayMicroseconds(FIS_WRITE_PULSEW);
    digitalWrite(FIS_WRITE_CLK,HIGH);
    delayMicroseconds(FIS_WRITE_PULSEW);

}
}

void FIS_WRITE_startENA(){
  digitalWrite(FIS_WRITE_ENA,HIGH);
  delayMicroseconds(FIS_WRITE_STARTPULSEW);
  digitalWrite(FIS_WRITE_ENA,LOW);
  delayMicroseconds(FIS_WRITE_STARTPULSEW);
  digitalWrite(FIS_WRITE_ENA,HIGH);
  delayMicroseconds(FIS_WRITE_STARTPULSEW);
}

void FIS_WRITE_stopENA(){
 digitalWrite(FIS_WRITE_ENA,LOW);
 delay(1);
}
//END WRITE TO CLUSTER

//READ MSG FROM RADIO
void FIS_READ_read_data_line(){
  if (!digitalRead(FIS_READ_CLK)){
    if(!FIS_READ_adrok){
      FIS_READ_read_adr();
    }
    else if (!FIS_READ_newmsg1){
      FIS_READ_read_msg1();
    }
    else if (!FIS_READ_newmsg2){
      FIS_READ_read_msg2();
    }
    else if (!FIS_READ_cksumok){
      FIS_READ_read_cksum();
    }
  }
}

void FIS_READ_read_cksum(){
  if(digitalRead(FIS_READ_DATA)){
    FIS_READ_cksum = (FIS_READ_cksum<<1) | 0x00000001;
    FIS_READ_msgbit++;
  }
  else
  {
    FIS_READ_cksum = (FIS_READ_cksum<<1);
    FIS_READ_msgbit++;
  }
  if (FIS_READ_msgbit==8)
  {
    FIS_READ_tmp_cksum=(0xFF^FIS_READ_adr);//bolo - dal som ^
    for (int i=56;i>=0;i=i-8){
      FIS_READ_tmp_cksum=FIS_READ_tmp_cksum+(0xFF^((FIS_READ_msg1>>i) & 0xFF))//bolo - dal som ^
        +(0xFF^((FIS_READ_msg2>>i) & 0xFF));//bolo - dal som ^
    }
    if((FIS_READ_tmp_cksum%256)==FIS_READ_cksum)
      FIS_READ_cksumok=1;
    FIS_READ_msgbit=0;
  }

}

void FIS_READ_read_msg1(){
  if(digitalRead(FIS_READ_DATA)){
    FIS_READ_msg1 = (FIS_READ_msg1<<1) | 0x00000001;
    FIS_READ_msgbit++;
  }
  else
  {
    FIS_READ_msg1 = (FIS_READ_msg1<<1);
    FIS_READ_msgbit++;
  }
  if (FIS_READ_msgbit==64)
  {
    FIS_READ_newmsg1=1;
    FIS_READ_msgbit=0;
  }
}

void FIS_READ_read_msg2(){
  if(digitalRead(FIS_READ_DATA)){
    FIS_READ_msg2 = (FIS_READ_msg2<<1) | 0x00000001;
    FIS_READ_msgbit++;
  }
  else
  {
    FIS_READ_msg2 = (FIS_READ_msg2<<1);
    FIS_READ_msgbit++;
  }
  if (FIS_READ_msgbit==64)
  {
    FIS_READ_newmsg2=1;
    FIS_READ_msgbit=0;
  }
}

void FIS_READ_read_adr(){
  if(digitalRead(FIS_READ_DATA)){
    FIS_READ_adr = (FIS_READ_adr<<1) | 0x00000001;
    FIS_READ_msgbit++;
  }
  else
  {
    FIS_READ_adr = (FIS_READ_adr<<1);
    FIS_READ_msgbit++;
  }
  if (FIS_READ_msgbit==8 && FIS_READ_adr==0x0F)
  {
    FIS_READ_adrok=1;
    FIS_READ_msgbit=0;
  }
}
//END READ MSG FROM RADIO

//CDC EMULATION
void CDC_decode(){
  for(int CDC_cmd_read_buffer_pointer=0;CDC_cmd_read_buffer_pointer<CDC_BUFFER;CDC_cmd_read_buffer_pointer++)
  {
  if(CDC_new_cmd[CDC_cmd_read_buffer_pointer])
  {
   switch(CDC_getCommand(CDC_cmd[CDC_cmd_read_buffer_pointer]))
    {
    case CDC_STOP:
      CDC_prev_cmd=CDC_STOP;
      break;
    case CDC_PLAY_NORMAL: 
      CDC_prev_cmd=CDC_PLAY_NORMAL;
      break;
    case CDC_PLAY: 
      CDC_prev_cmd=CDC_PLAY;
      break;
    case CDC_PREV:
      CDC_prev_cmd=CDC_PREV;
      break;
    case CDC_NEXT:
      CDC_prev_cmd=CDC_NEXT;
      break;
    case CDC_CD1:
      CDC_prev_cmd=CDC_CD1;
      break;
    case CDC_CD2:
      CDC_prev_cmd=CDC_CD2;
      break;
    case CDC_CD3:
      CDC_prev_cmd=CDC_CD3;
      break;
    case CDC_CD4:
      CDC_prev_cmd=CDC_CD4;
      break;
    case CDC_CD5:
      CDC_prev_cmd=CDC_CD5;
      break;
    case CDC_CD6:
      CDC_prev_cmd=CDC_CD6;
      break;
    case CDC_END_CMD:
      if (CDC_prev_cmd==CDC_PLAY_NORMAL || CDC_prev_cmd==CDC_PLAY)
      {
        CDC_idle=0;
        CDC_mode = CDC_MODE_PLAY;
      }
      else if (CDC_prev_cmd==CDC_PREV)
      {
        CDC_idle=0;
        CDC_tr++;
      }
      else if (CDC_prev_cmd==CDC_NEXT)
      {
        CDC_idle=0;
        CDC_tr--;
      }
      else if (CDC_prev_cmd==CDC_NEXT)
      {
        CDC_idle=0;
        CDC_tr=CDC_tr-1;
      } 
      else if (CDC_prev_cmd==CDC_STOP)
      {
        CDC_idle=1;
      }
      CDC_prev_cmd=0;
      break;
    case CDC_END_CMD2:
      if (CDC_prev_cmd==CDC_CD1)
      {
        CDC_idle=0;
        CDC_cd = 0xBE;
      }
      else if (CDC_prev_cmd==CDC_CD2)
      {
        CDC_idle=0;
        CDC_cd = 0xBD;
      }
      else if (CDC_prev_cmd==CDC_CD3)
      {
        CDC_idle=0;
        CDC_cd = 0xBC;
      }
      else if (CDC_prev_cmd==CDC_CD4)
      {
        CDC_idle=0;
        CDC_cd = 0xBB;
      }
      else if (CDC_prev_cmd==CDC_CD5)
      {
        CDC_idle=0;
        CDC_cd = 0xBA;
      }
      else if (CDC_prev_cmd==CDC_CD6)
      {
        CDC_idle=0;
        CDC_cd = 0xB9;
      }
      CDC_prev_cmd=0;
      break;
    }
   CDC_new_cmd[CDC_cmd_read_buffer_pointer]=0;
  }
  }
}
uint8_t CDC_getCommand(uint32_t CDC_cmd2)
{
  if (((CDC_cmd2>>24) & 0xFF) == CDC_PREFIX1 && ((CDC_cmd2>>16) & 0xFF) == CDC_PREFIX2)
    if (((CDC_cmd2>>8) & 0xFF) == (0xFF^((CDC_cmd2) & 0xFF)))
      return (CDC_cmd2>>8) & 0xFF;
  return 0;
}

void CDC_read_DataOut() //remote signals
{
  if(digitalRead(CDC_DataOut))
  {
    if (CDC_capturingstart || CDC_capturingbytes)
    {
      CDC_captimelo = TCNT1;    
    }
    else
    CDC_capturingstart = 1;
    TCNT1 = 0;

    //eval times
    //tick @0.5us
    //9000us HIGH and 4500us LOW
    if (CDC_captimehi > (8500/(8/CPU_CLOCK)) && CDC_captimelo >  (4000/(8/CPU_CLOCK)) )
    {
      CDC_capturingstart = 0;
      CDC_capturingbytes = 1;
    } //logic one = 1700us
    else if(CDC_capturingbytes && CDC_captimelo >  (1650/(8/CPU_CLOCK)))
    {
      CDC_cmd[CDC_cmd_write_buffer_poiter] = (CDC_cmd[CDC_cmd_write_buffer_poiter]<<1) | 0x00000001;
      CDC_cmdbit++;
    }
    else if (CDC_capturingbytes && CDC_captimelo > (500/(8/CPU_CLOCK)))
    {
      CDC_cmd[CDC_cmd_write_buffer_poiter] = (CDC_cmd[CDC_cmd_write_buffer_poiter]<<1);
      CDC_cmdbit++;
    }

    if(CDC_cmdbit == 32)
    {
      CDC_new_cmd[CDC_cmd_write_buffer_poiter] = 1;
      CDC_capturingbytes = 0;
      CDC_cmdbit=0;
      CDC_cmd_write_buffer_poiter++;
      if(CDC_cmd_write_buffer_poiter==CDC_BUFFER) CDC_cmd_write_buffer_poiter=0;
    }
  }
  else
  {
      CDC_captimehi = TCNT1; 
      TCNT1 = 0;
  }
}

void CDC_send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7)
{
  SPI.transfer(c0);
  delayMicroseconds(CDC_BYTES_DELAY);
  SPI.transfer(c1);
  delayMicroseconds(CDC_BYTES_DELAY);
  SPI.transfer(c2);
  delayMicroseconds(CDC_BYTES_DELAY);
  SPI.transfer(c3);
  delayMicroseconds(CDC_BYTES_DELAY);
  SPI.transfer(c4);
  delayMicroseconds(CDC_BYTES_DELAY);
  SPI.transfer(c5);
  delayMicroseconds(CDC_BYTES_DELAY);
  SPI.transfer(c6);
  delayMicroseconds(CDC_BYTES_DELAY);
  SPI.transfer(c7);
}

//END CDC EMULATION
/////////////////////////////////////////
// end functions
/////////////////////////////////////////


