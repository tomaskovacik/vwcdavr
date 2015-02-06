/*

mega + lcd shield from ebay (5 buttons on analog 0)

CDC sniffer
 - emulate RADIO DataOut signals 
 - receive and print CD changer responce to serial console 
  
DataOut -> arduino pin 12
Clk     -> arduino pin 3
DataIn  -> arduino pin 11

*/
#define DataOut 12
#define Clk 3
#define DataIn 11

#ifdef Clk=2
#define ClkInt 0
#endif

#ifdef Clk=3
#define ClkInt 1
#endif

#include <LiquidCrystal.h>

//lcd pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int adc_key_val[5] ={
  30, 150, 360, 535, 760 };
int NUM_KEYS = 5;
int adc_key_in;
int key=-1;
int oldkey=-1;
int cd[6]={
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC};
int cdpointer=0;
int play=0;
int tr=1;
int ack=0;
int ack_cd=0;
volatile uint64_t cmd=0;
volatile int cmdbit=0;
volatile uint8_t newcmd=0;

void setup()
{
  Serial.begin(9600);
  Serial.println("start");
  pinMode(DataOut,OUTPUT);
  digitalWrite(DataOut,LOW);
  pinMode(Clk,INPUT);
  pinMode(DataIn,INPUT);

  lcd.begin(16,2);
  lcd.home();
  lcd.clear();
  lcd.print("cd ");
  lcd.print(cdpointer+1);
  lcd.print(" pause");
  lcd.setCursor(0, 1);
  lcd.print("tr ");
  lcd.print(tr);

  attachInterrupt(ClkInt,readDataIn,FALLING);
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
        // righ
        ack=1;
        send_cmd(0x78);
        if (++tr==100)
          tr=01;
        break;
      case 1:
        // up
        ack_cd=1;
        if (++cdpointer == 6)
          cdpointer=0;
        send_cmd(cd[cdpointer]);
        break;
      case 2:
        // down
        ack_cd=1;
        if (--cdpointer == -1)
          cdpointer=5;
        send_cmd(cd[cdpointer]);
        break;
      case 3:
        // left
        ack=1;
        send_cmd(0xf8);
        if (--tr==0)
          tr=99;
        break;
      case 4:
        // select
        ack=1;
        play=!play;
        if (play){
          send_cmd(0xE4);
        }
        else
        {
          send_cmd(0x10);
        }
        break;
      }
    }

    if(ack)
    {
      //send_cmd(0xE4);
      send_cmd(0x14);
      ack=0;
    }
    if(ack_cd)
    {
      send_cmd(0x38);
      ack_cd=0;
    }
    lcd.home();
    lcd.clear();
    if (play){
      lcd.print("cd ");
      lcd.print(cdpointer+1);
      lcd.print(" play");
    }
    else
    {
      lcd.print("cd ");
      lcd.print(cdpointer+1);
      lcd.print(" pause");
    }
    lcd.setCursor(0, 1);
    lcd.print("tr ");
    lcd.print(tr);   
  }

  if(newcmd){
    
//    for(int b=56;b>-1;b=b-8){
//      uint8_t temp=((cmd>>b) & 0xFF);
//      Serial.print(temp,HEX);
////      Serial.print(" ");
//    }
    uint8_t temp;
    temp=((cmd>>56) & 0xFF);
    Serial.print(temp,HEX);
    temp=((cmd>>48) & 0xFF);
    Serial.print(temp,HEX);
    temp=((cmd>>40) & 0xFF);
    Serial.print(temp,HEX);
    temp=((cmd>>32) & 0xFF);
    Serial.print(temp,HEX);
    temp=((cmd>>24) & 0xFF);
    Serial.print(temp,HEX);
    temp=((cmd>>16) & 0xFF);
    Serial.print(temp,HEX);
    temp=((cmd>>8) & 0xFF);
    Serial.print(temp,HEX);
    temp=(cmd & 0xFF);
    Serial.print(temp,HEX);
    Serial.println();
    newcmd=0;
  }
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

void shiftOutPulse(uint8_t dataPin, uint8_t val)
{
  uint8_t i;

  for (i = 0; i < 8; i++)  {
    digitalWrite(dataPin, HIGH);
    delayMicroseconds(550);
    digitalWrite(dataPin, LOW);
    if(!!(val & (1 << (7 - i))) == 1)
    {// logic 1 = 1700us 
      delayMicroseconds(1700);        
    }
    else
    {
      delayMicroseconds(550);
    }
  }
}

void send_cmd(uint8_t cmd)
{
  digitalWrite(DataOut, HIGH);
  delay(9); //9000us
  digitalWrite(DataOut,LOW);
  delay(4);
  delayMicroseconds(500); //4500us :)
  shiftOutPulse(DataOut,0x53);
  shiftOutPulse(DataOut,0x2C);
  shiftOutPulse(DataOut,cmd);
  shiftOutPulse(DataOut,0xFF^cmd);
  digitalWrite(DataOut, HIGH);
  delayMicroseconds(550);
  digitalWrite(DataOut,LOW);
  Serial.print(0x53,HEX);
  Serial.print(0x2C,HEX);
  Serial.print(cmd,HEX);
  Serial.println(0xFF^cmd,HEX);

}

void readDataIn()
{
  if(digitalRead(DataIn))
  {//1
    cmd=(cmd<<1)|1;
  }
  else
  {//0
    cmd = (cmd<<1);
  }
  cmdbit++;

  if(cmdbit==64){
    newcmd=1;
    cmdbit=0;
  }
}





