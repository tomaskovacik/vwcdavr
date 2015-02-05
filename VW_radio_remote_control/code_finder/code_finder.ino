/*

license: GPL 

code finder for audi radios (probably also for vw, skoda, seat)
using ebay LCD shield(2x16) with 5 buttons on A0 
output (remote signal) is on pin 2

more info:
http://kovo-blog.blogspot.sk/2013/10/remote-control-audi-radio.html

5V logic
start bit:
 9ms LOW
4.55ms HIGH

 logic 0:
~600us LOW ~1700us HIGH

logic 1:
~600us LOW ~600us HIGH

stop bit:
~600us LOW

I have MFSW without telephone option, so I read codes just for radio:

UP:       0x41E8D02F
DOWN: 0x41E850AF
LEFT:   0x41E840BF
RIGHT: 0x41E8C03F
VOL+:  0x41E8807F
VOL-:    0x41E800FF

code is always 0x41 0xE8 X 0xFF-X

*/


//UP: 0xD0
#define UP 0xD0
//0,1,0,0,0,0,0,1,1,1,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,1,0,1,1,1,1
//DOWN: 0x50
#define DOWN 0x50
//0,1,0,0,0,0,0,1,1,1,1,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,1,1,1,1
//LEFT: 0x40
#define LEFT 0x40
//0,1,0,0,0,0,0,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,1,1,1,1,1,1
//RIGHT: 0xC0
#define RIGHT 0xC0
//0,1,0,0,0,0,0,1,1,1,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1
//VOL+: 0x80
//0,1,0,0,0,0,0,1,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1
//VOL-: 0x00
//0,1,0,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1
#include <LiquidCrystal.h>

//lcd pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


int adc_key_val[5] ={
  30, 150, 360, 535, 760 };
int NUM_KEYS = 5;
int adc_key_in;
int key=-1;
int oldkey=-1;

int h=0x00;

void setup() {
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
  lcd.begin(16,2);
  lcd.home();
  lcd.clear();
  Serial.begin(9600);
}

void loop() {

  adc_key_in = analogRead(0);    // read the value from the sensor
  key = get_key(adc_key_in);     // convert into key press
  if (key != oldkey)             // if keypress is detected
  {
    delay(50);          // wait for debounce time
    adc_key_in = analogRead(0);    // read the value from the sensor
    key = get_key(adc_key_in);                  // convert into key press
    if (key != oldkey)
    {
      oldkey = key;
      switch (key)
      {
      case 0:
        // righ - increment and send
        h++;
        sendByte(h);
        break;
      case 1:
        // up - increment but dont send
        h++;
          lcd.clear();
  lcd.home();
  lcd.print(h,HEX);
        break;
      case 2:
        // down - decrement bu dont send
        h--; 
  lcd.clear();
  lcd.home();
  lcd.print(h,HEX);
        break;
      case 3:
        // left - decrement and send
        if(h>0)
        h--; 
        sendByte(h);
        break;
      case 4:
        // select - repeat sending of last number
        sendByte(h);
        break;
      }

     }
    }
  }
 
void sendByte(int cislo) {
  lcd.clear();
  lcd.home();
  lcd.print(cislo,HEX);
  int c_cislo=0xFF-cislo;
  int cmdStart[32]={0,1,0,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  for (int i = 23;i>=16;i-- ){
          if (cislo%2  != 0) cmdStart[i]=1;
          cislo = cislo/2;
  }

  for (int i = 31;i>=24;i-- ){
          if (c_cislo%2  != 0) cmdStart[i]=1;
          c_cislo = c_cislo/2;
  }
  digitalWrite(2,LOW);
  delay(9);
  digitalWrite(2,HIGH);
  delayMicroseconds(4500);
  for (int i=0;i<32;i++){
    Serial.print(cmdStart[i]);
    Serial.print(",");
    digitalWrite(2,LOW);
    delayMicroseconds(600);
    digitalWrite(2,HIGH);
    if (cmdStart[i]==0)
      delayMicroseconds(600);
    if (cmdStart[i]==1)
      delayMicroseconds(1700);
  }
  Serial.println("");
  digitalWrite(2,LOW);
  delayMicroseconds(600);
  digitalWrite(2,HIGH);

}
// Convert ADC value to key number
int get_key(unsigned int input)
{
  int k;
  for (k = 0; k < NUM_KEYS; k++)
  {
    if (input < adc_key_val[k])
    {
      return k;
    }
  }
  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed
  return k;
}
