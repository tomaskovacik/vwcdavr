//spi version:
//http://arduino.cc/en/Reference/SPI
//data MOSI pin -> uno/due: 11,mega: 51
//clk  SCK  pin -> uno/due: 13,mega: 52
//ena       pin -> uno/due/mega 2

#include <SPI.h> 

//WRITE TO CLUSTER
#define FIS_WRITE_ENA 2 //
#define FIS_WRITE_PULSEW 50
#define FIS_WRITE_STARTPULSEW 100
#define FIS_WRITE_START 15 //something like address, first byte is always 15
//END WRITE TO CLUSTER

//#define BINCODE 000011111011111010101010101110111011011010111100101100111010101010111101101011001011001110110000101010011011111010110100101101101011111010010011
//                000011111011111010101010101110111011011010111100101100111010101010111101101011001011001110110000101010011011111010110100101101101011111010010011

//int BINCODE[144]={
//0,0,0,0,1,1,1,1,
//1,0,1,1,1,1,1,0,
//1,0,1,0,1,0,1,0,
//1,0,1,1,1,0,1,1,
//1,0,1,1,0,1,1,0,
//1,0,1,1,1,1,0,0,
//1,0,1,1,0,0,1,1,
//1,0,1,0,1,0,1,0,
//1,0,1,1,1,1,0,1,
//1,0,1,0,1,1,0,0,
//1,0,1,1,0,0,1,1,
//1,0,1,1,0,0,0,0,
//1,0,1,0,1,0,0,1,
//1,0,1,1,1,1,1,0,
//1,0,1,1,0,1,0,0,
//1,0,1,1,0,1,1,0,
//1,0,1,1,1,1,1,0,
//1,0,0,1,0,0,1,1};

//WRITE TO CLUSTER
String FIS_WRITE_line1="";
String FIS_WRITE_line2="";
long FIS_WRITE_rotary_position_line1=-8;
long FIS_WRITE_rotary_position_line2=-8;
char FIS_WRITE_CHAR_FROM_SERIAL;
int FIS_WRITE_line=1;
long FIS_WRITE_last_refresh=0;
int FIS_WRITE_nl=0;
int FIS_WRITE_ENA_STATUS=0;
//END WRITE TO CLUSTER

//WRITE TO CLUSTER
void FIS_WRITE_sendTEXT(String FIS_WRITE_line1,String FIS_WRITE_line2);
void FIS_WRITE_sendByte(int Bit);
void FIS_WRITE_startENA();
void FIS_WRITE_stopENA();
//END WRITE TO CLUSTER

void setup(){
//WRITE TO CLUSTER
pinMode(FIS_WRITE_ENA, INPUT);
Serial.begin(9600);
//END WRITE TO CLUSTER
}

void loop(){
//WRITE TO CLUSTER
if (Serial.available()) {
        FIS_WRITE_CHAR_FROM_SERIAL=(char)Serial.read();
        Serial.print(FIS_WRITE_CHAR_FROM_SERIAL);
        if (FIS_WRITE_CHAR_FROM_SERIAL == '\n') {
          FIS_WRITE_nl=1;
          if (FIS_WRITE_line==1){
            FIS_WRITE_line=2;
          } else {
            FIS_WRITE_line=1;
           }
        } else {
          if (FIS_WRITE_line==1){
              if (FIS_WRITE_nl){
                FIS_WRITE_nl=0;
                FIS_WRITE_line1="";
                FIS_WRITE_rotary_position_line1=-8;
                }
              FIS_WRITE_line1+=FIS_WRITE_CHAR_FROM_SERIAL;
          } else {
              if (FIS_WRITE_nl){
                FIS_WRITE_nl=0;
                FIS_WRITE_line2="";
                FIS_WRITE_rotary_position_line2=-8;
                }
               FIS_WRITE_line2+=FIS_WRITE_CHAR_FROM_SERIAL;
          }
        }

 }
   
  int FIS_WRITE_line1_length=FIS_WRITE_line1.length();
  int FIS_WRITE_line2_length=FIS_WRITE_line2.length();
  String FIS_WRITE_sendline1="        ";
  String FIS_WRITE_sendline2="        ";

  
   
    //do rotary and refresh each 0.5second
    //refresh cluster each 5s 
    if(millis()-FIS_WRITE_last_refresh>500 && !FIS_WRITE_line1_length || !FIS_WRITE_line2_length){
    FIS_WRITE_startENA(); //try to enable 
     if (FIS_WRITE_ENA_STATUS){ //enable set to HIGH, good to go
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
      FIS_WRITE_stopENA()
    //end refresh
     }
  }
//END WRITE TO CLUSTER  
}

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
SPI.begin();
SPI.setDataMode(SPI_MODE2);
SPI.setBitOrder(MSBFIRST);
SPI.setClockDivider(SPI_CLOCK_DIV128);
FIS_WRITE_sendByte(FIS_WRITE_START);
FIS_WRITE_sendByte(255-FIS_WRITE_line1[0]);
FIS_WRITE_sendByte(255-FIS_WRITE_line1[1]);
FIS_WRITE_sendByte(255-FIS_WRITE_line1[2]);
FIS_WRITE_sendByte(255-FIS_WRITE_line1[3]);
FIS_WRITE_sendByte(255-FIS_WRITE_line1[4]);
FIS_WRITE_sendByte(255-FIS_WRITE_line1[5]);
FIS_WRITE_sendByte(255-FIS_WRITE_line1[6]);
FIS_WRITE_sendByte(255-FIS_WRITE_line1[7]);
FIS_WRITE_sendByte(255-FIS_WRITE_line2[0]);
FIS_WRITE_sendByte(255-FIS_WRITE_line2[1]);
FIS_WRITE_sendByte(255-FIS_WRITE_line2[2]);
FIS_WRITE_sendByte(255-FIS_WRITE_line2[3]);
FIS_WRITE_sendByte(255-FIS_WRITE_line2[4]);
FIS_WRITE_sendByte(255-FIS_WRITE_line2[5]);
FIS_WRITE_sendByte(255-FIS_WRITE_line2[6]);
FIS_WRITE_sendByte(255-FIS_WRITE_line2[7]);
FIS_WRITE_sendByte(crc);
SPI.end();
FIS_WRITE_stopENA();
}

void FIS_WRITE_sendByte(int Byte){
  SPI.transfer(Byte);
}

void FIS_WRITE_startENA(){
  if (!digitalRead(FIS_WRITE_ENA)) {
  digitalWrite(FIS_WRITE_ENA,HIGH);
  delayMicroseconds(FIS_WRITE_STARTPULSEW);
  digitalWrite(FIS_WRITE_ENA,LOW);
  delayMicroseconds(FIS_WRITE_STARTPULSEW);
  digitalWrite(FIS_WRITE_ENA,HIGH);
  delayMicroseconds(FIS_WRITE_STARTPULSEW);
  FIS_WRITE_ENA_STATUS=1;
  }
}

void FIS_WRITE_stopENA(){
 digitalWrite(FIS_WRITE_ENA,LOW);
 FIS_WRITE_ENA_STATUS=0;
}
//END WRITE TO CLUSTER

