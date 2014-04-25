#include <SPI.h>

//WRITE TO CLUSTER
#define FIS_WRITE_intENA 0 //interupt on FIS_READ_ENA line
#define FIS_WRITE_ENA 2
#define FIS_WRITE_CLK 4
#define FIS_WRITE_DATA 5
#define FIS_WRITE_PULSEW 50
#define FIS_WRITE_STARTPULSEW 100
#define FIS_WRITE_START 15
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
int FIS_WRITE_ack=0; //waiting for cluster to do ack?
//END WRITE TO CLUSTER

//WRITE TO CLUSTER
void FIS_WRITE_sendTEXT(String FIS_WRITE_line1,String FIS_WRITE_line2);
void FIS_WRITE_sendByte(int Bit);
void FIS_WRITE_startENA();
void FIS_WRITE_stopENA();
//END WRITE TO CLUSTER

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
}



void loop(){
//WRITE TO CLUSTER
if (Serial.available()) {
        FIS_WRITE_CHAR_FROM_SERIAL=(char)Serial.read();
        //Serial.print(s);
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
    //refresh custer each 5s 
    if(millis()-FIS_WRITE_last_refresh>500 && !FIS_WRITE_line1_length || !FIS_WRITE_line2_length){//&& !FIS_WRITE_ack){
      
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
FIS_WRITE_startENA();
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
//  //Serial.println(Bit);
// // Serial.println(Bit,BIN);
//  static int iResult[8];
//  for (int i = 0; i <= 7; i++)
//  {    
//    iResult[i] = Bit % 2;
//    Bit = Bit / 2;
//  //  Serial.print(iResult[i]);
//  }
//  //Serial.println();
//  for(int i=7;i>=0;i--){
//  switch (iResult[i]) {
//    case 1: digitalWrite(FIS_WRITE_DATA,HIGH);
//            digitalWrite(FIS_WRITE_CLK,LOW);
//            break;
//    case 0:digitalWrite(FIS_WRITE_DATA,LOW);
//           digitalWrite(FIS_WRITE_CLK,LOW);
//           break;
//    }
//    delayMicroseconds(FIS_WRITE_PULSEW);
//    digitalWrite(FIS_WRITE_CLK,HIGH);
//    delayMicroseconds(FIS_WRITE_PULSEW);
//}
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
 FIS_WRITE_ack=1;
 pinMode(FIS_WRITE_ENA,INPUT);
 attachInterrupt(FIS_WRITE_intENA,FIS_WRITE_check_for_ack,FALLING);
}

void FIS_WRITE_check_for_ack(){
  if (!digitalRead(FIS_WRITE_ENA)) {
    FIS_WRITE_ack=0;
    detachInterrupt(FIS_WRITE_intENA);
    pinMode(FIS_WRITE_ENA,OUTPUT);
  }
}
//END WRITE TO CLUSTER

