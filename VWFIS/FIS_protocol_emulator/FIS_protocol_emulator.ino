//data pin 4
//clk  pin 3
//ena  pin 2

//WRITE TO CLUSTER
#define FIS_WRITE_ENA 2 
#define FIS_WRITE_CLK 3 
#define FIS_WRITE_DATA 4 
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
uint8_t FIS_WRITE_CRC=0;
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
digitalWrite(FIS_WRITE_ENA,LOW);
pinMode(FIS_WRITE_CLK, OUTPUT); 
digitalWrite(FIS_WRITE_CLK, HIGH);
pinMode(FIS_WRITE_DATA, OUTPUT); 
digitalWrite(FIS_WRITE_DATA, HIGH);
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
    if(millis()-FIS_WRITE_last_refresh>500 && (FIS_WRITE_line1_length>0 || FIS_WRITE_line2_length>0)){
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
  Serial.println(FIS_WRITE_line1);
  Serial.println(FIS_WRITE_line2);
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

FIS_WRITE_CRC=(0xFF^FIS_WRITE_START);

FIS_WRITE_startENA();
FIS_WRITE_sendByte(FIS_WRITE_START);
  for (int i = 0; i <= 7; i++)
  { 
    FIS_WRITE_sendByte(0xFF^FIS_WRITE_line1[i]);
    FIS_WRITE_CRC+=FIS_WRITE_line1[i];
  }
    for (int i = 0; i <= 7; i++)
  { 
    FIS_WRITE_sendByte(0xFF^FIS_WRITE_line2[i]);
    FIS_WRITE_CRC+=FIS_WRITE_line2[i];
  }
  
FIS_WRITE_sendByte(FIS_WRITE_CRC%0x100);

FIS_WRITE_stopENA();
}

void FIS_WRITE_sendByte(int Byte){
  static int iResult[8];
  for (int i = 0; i <= 7; i++)
  {    
    iResult[i] = Byte % 2;
    Byte = Byte / 2;
  }
  for(int i=7;i>=0;i--){
  switch (iResult[i]) {
    case 1: digitalWrite(FIS_WRITE_DATA,HIGH);
            break;
    case 0:digitalWrite(FIS_WRITE_DATA,LOW);
           break;
    }
    digitalWrite(FIS_WRITE_CLK,LOW);
    delayMicroseconds(FIS_WRITE_PULSEW);
    digitalWrite(FIS_WRITE_CLK,HIGH);
    delayMicroseconds(FIS_WRITE_PULSEW);
}
}

void FIS_WRITE_startENA(){
 if (!digitalRead(FIS_WRITE_ENA)) {
  digitalWrite(FIS_WRITE_ENA,HIGH);
//  delayMicroseconds(FIS_WRITE_STARTPULSEW);
//  digitalWrite(FIS_WRITE_ENA,LOW);
//  delayMicroseconds(FIS_WRITE_STARTPULSEW);
//  digitalWrite(FIS_WRITE_ENA,HIGH);
//  delayMicroseconds(FIS_WRITE_STARTPULSEW);
 // FIS_WRITE_ENA_STATUS=1;
  }
}

void FIS_WRITE_stopENA(){
 digitalWrite(FIS_WRITE_ENA,LOW);
// FIS_WRITE_ENA_STATUS=0;
}
//END WRITE TO CLUSTER

