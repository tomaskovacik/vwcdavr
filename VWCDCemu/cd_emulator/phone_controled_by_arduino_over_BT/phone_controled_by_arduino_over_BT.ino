#include <SoftwareSerial.h>

SoftwareSerial mySerial(1,NULL); // RX, TX

#define POWERON 4
#define PLAY 3
#define PREV 2
#define NEXT 0 
//int btLEDPOWERON = 6; //only used on mega?28
//int btLEDCONNECTED = 7;//only used on mega?28
//int btON=0;
//int btCONNECTED=0;
int sread;

void setup() {                
  pinMode(POWERON, OUTPUT);
  pinMode(PLAY, OUTPUT); 
  pinMode(PREV, OUTPUT);   
  pinMode(NEXT, OUTPUT);
digitalWrite(POWERON,LOW);
digitalWrite(PLAY,LOW);
digitalWrite(PREV,LOW);
digitalWrite(NEXT,LOW);
  //pinMode(btLEDPOWERON,INPUT);//only used on mega?28
  //pinMode(btLEDCONNECTED,INPUT); //only used on mega?28
  //Serial.begin(9600);
  mySerial.begin(9600);
//check bt module state
//  Serial.println("starting BT");
  digitalWrite(POWERON,HIGH);
//  Serial.println("Waiting to BT modul power up: ");
//  while(digitalRead(btLEDPOWERON)==HIGH) { //bt module is powered, but not ON
//  delay(300);
//    Serial.print(".");
//  }
delay(4000);
  digitalWrite(POWERON,LOW);
//  btON=1;
//  Serial.println("");
//  Serial.println("Done");
//  Serial.println ("Waiting for device pairing:");
//  while(digitalRead(btLEDCONNECTED)==HIGH){
//  delay(1000);
//    Serial.print(".");
//  }
//  Serial.println("");
//  Serial.println("Connected!");
//  btCONNECTED=1;
}


void loop() {
  if (mySerial.available())
  {
    sread=mySerial.read();
    switch (sread){
      case 0xC4: digitalWrite(PREV, HIGH);delay(100);digitalWrite(PREV, LOW);delay(25);digitalWrite(PREV, HIGH);/*Serial.println("PREV")*/;break;//prev
      case 0xD1: digitalWrite(PLAY,HIGH);/*Serial.println("PLAY");*/break;
      case 0xD2: digitalWrite(PLAY,HIGH);/*Serial.println("PLAY");*/break;
      case 0xD3: digitalWrite(PLAY,HIGH);/*Serial.println("PLAY");*/break;
      case 0xD4: digitalWrite(PLAY,HIGH);/*Serial.println("PLAY");*/break;
      case 0xD5: digitalWrite(PLAY,HIGH);/*Serial.println("PLAY");*/break;
      case 0xD6: digitalWrite(PLAY,HIGH);/*Serial.println("PLAY");*/break;
      case 0xC8: digitalWrite(NEXT,HIGH);/*Serial.println("NEXT");*/break;//next
    }
  }
  sread='foo';
  delay(100);
  digitalWrite(PLAY, LOW);
  digitalWrite(PREV, LOW);
  digitalWrite(NEXT, LOW);
}

