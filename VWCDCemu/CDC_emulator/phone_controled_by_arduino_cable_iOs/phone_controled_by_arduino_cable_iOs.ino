/*

idea of this is control ipod remote as easy as with HTC headphone, code based on 

htc hedphones ultrasound identification

site i currently unavalable, archive:
https://web.archive.org/web/20131128010310/http://david.carne.ca/shuffle_hax/shuffle_remote.html

or whole page with pics is on my github page

this is untested .....

no HW part was made, no test was made....

*/

#define F_CPU 16000000
//#include <avr/io.h>

#include <util/delay.h>

#define CTL_PIN (1<<5)
#define CTL_HI (!(ACSR & (1<<ACO)))


#define POWERON 2
#define PLAY 0
#define PREV 1
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
  	CLKPR = 0x80;
	CLKPR = 0x0;
	ADCSRB &= ~(1<< ACME);
	ACSR |= ( 1<< ACBG);
	TCCR0A = 0x03;
	TCCR0B = 0x09;
	OCR0B = 7;
	DDRC = CTL_PIN;
}

// the loop routine runs over and over again forever:
void loop() {
		PORTC = 0;	
		while (!CTL_HI);
		_delay_ms(4.1);
		PORTC |= CTL_PIN;
		_delay_ms(4.2);
		
		// 280khz burst
		TCCR0A = 0x23;
		TCNT0 = 0x0;
		OCR0A = 27;
		DDRD |= 1<<5;
		_delay_ms(1.5);
		
		// 245khz burst
		OCR0A = 33;
		TCNT0 = 0x0;
		_delay_ms(4.6);
		
		// burst off
		TCCR0A = 0x03;
		DDRD &= ~(1<<5);
		
		// wait for ack pulse
		while (CTL_HI);
		// kill init pulldown
		PORTC &= ~CTL_PIN;
		
		// Just hang around and 
		// wait for loss-of-signal
		while (1) {
			while (CTL_HI){
// controle of iPhone
if (mySerial.available())
  {
    sread=mySerial.read();
    switch (sread){
      case 0xC4: digitalWrite(PREV, HIGH);delay(100);digitalWrite(PREV, LOW);delay(100);digitalWrite(PREV, HIGH);break;
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
// end of controle of iPhone
			if (!CTL_HI)
				break;
		}
         }
}
