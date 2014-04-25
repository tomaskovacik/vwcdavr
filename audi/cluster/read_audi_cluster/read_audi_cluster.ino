//#define intENA 2
//#define intCLK 3
//#define intDATA 4
//#define ENA_in 21
//#define CLK 20
//#define DATA 19
//#define ENA_out 18
#define intENA 0
#define intCLK 1
//#define intDATA 4
#define ENA_in 2
#define CLK 3
#define DATA 11
//#define ENA_out 18
#include <LiquidCrystal.h>
volatile int _WORD[144];
int text[16];
int i_char;
int i_sum_of_received_chars;
volatile int _bit=0;
int capturing=0;
long delta_last_byte=0;
volatile int ack=0;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void read_data_line(){
if(_bit>143){
_bit=0;
}
 if (!digitalRead(CLK)){// && digitalRead(ENA_in)){
    _WORD[_bit++]=digitalRead(DATA);
 }
}

void enaUP(){
  //starting of WORD
  _bit=0;
}



//void f_ack(){
//    detachInterrupt(intENA);
//    digitalWrite(ENA_out,LOW);
//    delay(500);
//    digitalWrite(ENA_out,HIGH);
//    delay(10);
//    attachInterrupt(intENA,enaUP,RISING);
//}

void setup() {
 lcd.begin(16,2);
 lcd.home();
 lcd.clear();
  attachInterrupt(intENA,enaUP,RISING);
  attachInterrupt(intCLK,read_data_line,CHANGE);
  pinMode(DATA,INPUT);
pinMode(CLK,INPUT);
  pinMode(ENA_in,INPUT);
 // pinMode(ENA_out,OUTPUT);
 // digitalWrite(ENA_out,HIGH);
  delay(500);
 // f_ack();
}

void loop() {
 
if(_bit==144){ //last bit is 143

if ((_WORD[0]*128+_WORD[1]*64+_WORD[2]*32+_WORD[3]*16+_WORD[4]*8+_WORD[5]*4+_WORD[6]*2+_WORD[7])==15) {
int line_int=0;
  i_sum_of_received_chars=240;
  for (int i=8;i<136;i++){
    i_char=(255-(_WORD[i]*128+_WORD[++i]*64+_WORD[++i]*32+_WORD[++i]*16+_WORD[++i]*8+_WORD[++i]*4+_WORD[++i]*2+_WORD[++i]));
    text[line_int++]=i_char;
    i_sum_of_received_chars+=i_char;
  }
 if((i_sum_of_received_chars%256)==(_WORD[136]*128+_WORD[137]*64+_WORD[138]*32+_WORD[139]*16+_WORD[140]*8+_WORD[141]*4+_WORD[142]*2+_WORD[143])){
lcd.home();
lcd.clear();
  for(int i=0;i<16;i++){
    lcd.write(text[i]);
    if (i==7)
      lcd.setCursor(0,1);
    }
 }
}
 //f_ack();
}
}


