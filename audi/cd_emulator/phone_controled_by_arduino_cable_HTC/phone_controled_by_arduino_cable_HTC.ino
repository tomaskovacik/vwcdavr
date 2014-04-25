
#define PREV 2
#define PLAY 1
#define NEXT 3

#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, NULL); // RX, TX

char sread;
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(PREV, OUTPUT);
  pinMode(PLAY, OUTPUT);    
  pinMode(NEXT, OUTPUT);
  digitalWrite(PREV, LOW);
  digitalWrite(PLAY, LOW);
  digitalWrite(NEXT, LOW);    
  mySerial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  if (mySerial.available())
  {
    sread=mySerial.read();
    switch (sread){
      case 0xC4: digitalWrite(PREV, HIGH);delay(100);digitalWrite(PREV, LOW);delay(25);digitalWrite(PREV, HIGH);break;
      case 0xD1:
      case 0xD2:
      case 0xD3:
      case 0xD4:
      case 0xD5:
      case 0xD6:digitalWrite(PLAY,HIGH);break;
      case 0xC8:digitalWrite(NEXT,HIGH);break;
      }
  }
  sread='foo';
  delay(100);
  digitalWrite(PREV, LOW);
  digitalWrite(PLAY, LOW);
  digitalWrite(NEXT, LOW);
}
