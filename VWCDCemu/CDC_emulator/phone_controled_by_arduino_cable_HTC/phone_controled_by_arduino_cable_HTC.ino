/*

attiny85 code to control phone (HTC in my case) over 4th ring on headphone jack
basicaly it act as 3buttons cable remote controle, applying special resistance on mic
line

more here: http://www.george-smart.co.uk/wiki/HTC_Headphones

HW: https://github.com/tomaskovacik/hw/tree/master/fritzing/vwcdpic/HTC

- read output from vwcdpic v2 with arhchos support (9600bks serial)
- control 3 transistors which act as buttons 

*/

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
      case 0xC4: digitalWrite(PREV, HIGH);delay(100);digitalWrite(PREV, LOW);delay(25);digitalWrite(PREV, HIGH);break; //prev. track - double push button, one push is just jump start of current track
      case 0xD1:
      case 0xD2:
      case 0xD3:
      case 0xD4:
      case 0xD5:
      case 0xD6:digitalWrite(PLAY,HIGH);break; //play/pause
      case 0xC8:digitalWrite(NEXT,HIGH);break; // next song
      }
  }
  sread='foo';
  delay(100);
  digitalWrite(PREV, LOW);//release of buttons
  digitalWrite(PLAY, LOW);
  digitalWrite(NEXT, LOW);
}
