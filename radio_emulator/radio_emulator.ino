/*

atmega328 only

 mega + serial console:
 
 next track button       =
 previous track button   -
 next CD                 ]
 previous CD             [
 play/stop               p
 CD1                     1
 CD2                     2
 CD3                     3
 CD4                     4
 CD5                     5
 CD6                     6
 seek forward            f
 seek rewind             r
 scan mode               s
 shuffle mode            l
 help                    h
 
 CDC sniffer
 - emulate RADIO DataOut signals 
 - receive and print CD changer responce to serial console 
 
 DataOut -> arduino pin 12
 Clk     -> arduino pin 3
 DataIn  -> arduino pin 4
 
 */
#define DataOut 12
#define Clk 3
#define DataIn 4
#define ClkInt 1


#define CDC_END_CMD 0x14
#define CDC_END_CMD2 0x38
#define CDC_PLAY 0xE4
#define CDC_STOP 0x10
#define CDC_NEXT 0xF8
#define CDC_PREV 0x78
#define CDC_SEEK_FWD 0xD8
#define CDC_SEEK_RWD 0x58
#define CDC_CD1 0x0C
#define CDC_CD2 0x8C
#define CDC_CD3 0x4C
#define CDC_CD4 0xCC
#define CDC_CD5 0x2C
#define CDC_CD6 0xAC
#define CDC_SCAN 0xA0
#define CDC_SFL 0x60
#define CDC_PLAY_NORMAL 0x08
#define CDC_PREV_CD 0x18

//#include <LiquidCrystal.h>
//
////lcd pins
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
//
//int adc_key_val[5] ={
//  30, 150, 360, 535, 760 };
//int NUM_KEYS = 5;
//int adc_key_in;
//int key=-1;
//int oldkey=-1;
//int cd[6]={
//  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC};
int cd=0;
//int cdpointer=0;
int play=0;
int tr=0;
int minutes=0;
int sec=0;
int mode=0;
int ack=0;
int ack_cd=0;
volatile uint64_t cmd=0;
volatile uint64_t prev_cmd=0;
volatile int cmdbit=0;
volatile uint8_t newcmd=0;
int incomingByte = 0;

int verbose=0;

#define TX_BUFFER_END  12
uint16_t txbuffer[TX_BUFFER_END];
uint8_t txinptr=0;
uint8_t txoutptr=0;
static void Enqueue(uint16_t num);

void setup()
{

  //time to catche start of transmition form emulator
  cli();//stop interrupts
  //for atmegax8 micros
  //TODO: cpu detection ... 
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCCR1C = 0;
  TCNT1  = 0;//initialize counter value to 0;
  TIMSK1 |= _BV(OCIE1A);
  OCR1A = 700;//700*64us=44,8ms
//OCR1A = 778;//50048us=50,048ms
  TCCR1B |= _BV(CS12)|_BV(CS10); //prescaler 1024*1/16000000 -> 64us tick
  sei();//allow interrupts
  //  TIFR1 |= _BV(TOV1);//clear overflow flag
  Serial.begin(9600);
  Serial.println("vw group radio emulator");
  pinMode(DataOut,OUTPUT);
  digitalWrite(DataOut,LOW);
  pinMode(Clk,INPUT);
  pinMode(DataIn,INPUT);

  //  lcd.begin(16,2);
  //  lcd.home();
  //  lcd.clear();
  //  lcd.print("cd ");
  //  lcd.print(cdpointer+1);
  //  lcd.print(" pause");
  //  lcd.setCursor(0, 1);
  //  lcd.print("tr ");
  //  lcd.print(tr);

  attachInterrupt(ClkInt,readDataIn,FALLING);
}

void loop() {
  //    delay(50);          // wait for debounce time
  //    adc_key_in = analogRead(0);    // read the value from the sensor
  //    key = get_key(adc_key_in);                  // convert into key press
  //    if (key != oldkey)
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    incomingByte = Serial.read();
    //     oldkey = key;
    switch (incomingByte)
    {
    case '=':
      // NEXT SONG
      send_cmd(CDC_NEXT);
      send_cmd(CDC_END_CMD);
      break;
    case ']':
      // NEXT CD
      send_cmd(CDC_END_CMD2);
      send_cmd(CDC_END_CMD);
      break;
    case '[':
      // PREVIOUS CD
      send_cmd(CDC_PREV_CD);
      send_cmd(CDC_END_CMD);
      break;
    case '-':
      // PREVIOUS SONG
      send_cmd(CDC_PREV);
      send_cmd(CDC_END_CMD);
      break;
    case 'p':
      // PLAY/PAUSE
      play=!play;
      if (play){
        send_cmd(CDC_PLAY);
        send_cmd(CDC_END_CMD);
      }
      else
      {
        send_cmd(CDC_STOP);
        send_cmd(CDC_END_CMD);
      }
      break;
    case '1':
      // CD 1
      send_cmd(CDC_CD1);
      send_cmd(CDC_END_CMD2);      
      break;
    case '2':
      // CD 2
      send_cmd(CDC_CD2);
      send_cmd(CDC_END_CMD2);      
      break;
    case '3':
      // CD 3
      send_cmd(CDC_CD3);
      send_cmd(CDC_END_CMD2);      
      break;
    case '4':
      // CD 4
      send_cmd(CDC_CD4);
      send_cmd(CDC_END_CMD2);      
      break;
    case '5':
      // CD 5
      send_cmd(CDC_CD5);
      send_cmd(CDC_END_CMD2);      
      break;
    case '6':
      // CD 1
      send_cmd(CDC_CD6);
      send_cmd(CDC_END_CMD2);      
      break;
      // seek forward            f
      // seek rewind             r
      // scan mode               s
      // shuffle mode            h
    case 'f':
      //  seek frward
      send_cmd(CDC_SEEK_FWD);
      send_cmd(CDC_PLAY);
      send_cmd(CDC_END_CMD2);      
      break;
    case 'r':
      // seek rewind
      send_cmd(CDC_SEEK_RWD);
      send_cmd(CDC_PLAY);
      send_cmd(CDC_END_CMD2);      
      break;
    case 's':
      // scan
      send_cmd(CDC_SCAN);
      break;
    case 'l':
      // shuffle
      send_cmd(CDC_SFL);
      break;
    case 'v': //verbose
      verbose=!verbose;
    break;
    case 'h': //help
      Serial.println("next track button       =");
      Serial.println("previous track button   -");
      Serial.println("next CD                 ]");
      Serial.println("previous CD             [");
      Serial.println("play/stop               p");
      Serial.println("CD1                     1");
      Serial.println("CD2                     2");
      Serial.println("CD3                     3");
      Serial.println("CD4                     4");
      Serial.println("CD5                     5");
      Serial.println("CD6                     6");
      Serial.println("seek forward            f");
      Serial.println("seek rewind             r");
      Serial.println("scan mode               s");
      Serial.println("shuffle mode            l");
      Serial.println("help                    h");   
    }    
    //    lcd.home();
    //    lcd.clear();
    //    if (play){
    //      lcd.print("cd ");
    //      lcd.print(cdpointer+1);
    //      lcd.print(" play");
    //    }
    //    else
    //    {
    //      lcd.print("cd ");
    //      lcd.print(cdpointer+1);
    //      lcd.print(" pause");
    //    }
    //    lcd.setCursor(0, 1);
    //    lcd.print("tr ");
    //    lcd.print(tr);   
  }

  if(newcmd && prev_cmd != cmd && cmd!=0)
  {
    prev_cmd=cmd;
    newcmd=0;      
   // TIMSK1 |= _BV(OCIE1A);
    //    for(int b=56;b>-1;b=b-8){
    //      uint8_t temp=((cmd>>b) & 0xFF);
    //      Serial.print(temp,HEX);
    ////      Serial.print(" ");
    //    }
    uint8_t temp;
    if (verbose){
    temp=((cmd>>56) & 0xFF);
    Serial.print(temp,HEX);
    temp=((cmd>>48) & 0xFF);
    Serial.print(temp,HEX);
    cd=temp;
    temp=((cmd>>40) & 0xFF);
    Serial.print(temp,HEX);
    tr=temp;
    temp=((cmd>>32) & 0xFF);
    Serial.print(temp,HEX);
    minutes=temp;
    temp=((cmd>>24) & 0xFF);
    Serial.print(temp,HEX);
    sec=temp;
    temp=((cmd>>16) & 0xFF);
    Serial.print(temp,HEX);
    mode=temp;
    temp=((cmd>>8) & 0xFF);
    Serial.print(temp,HEX);
    temp=(cmd & 0xFF);
    Serial.print(temp,HEX);
    Serial.print("   CD: ");
    Serial.print(cd,HEX);
    Serial.print(" track: ");
    Serial.print(tr,HEX);
    Serial.print("   min: ");
    Serial.print(minutes,HEX  );
    Serial.print(" sek: ");
    Serial.print(sec,HEX);
    Serial.print(" mode: ");
    Serial.println(mode,HEX);
    }
    else 
    {
    Serial.print("   CD: ");
    Serial.print(int (((cmd>>48) & 0xFF)^0xBF),HEX);
    Serial.print(" track: ");
    Serial.print(int (((cmd>>40) & 0xFF)^0xFF),HEX);
    Serial.print("   min: ");
    Serial.print(int (((cmd>>32) & 0xFF)^0xFF),HEX);
    Serial.print(" sek: ");
    Serial.print(int (((cmd>>24) & 0xFF)^0xFF),HEX);
    Serial.print(" mode: ");
    Serial.print(int (((cmd>>16) & 0xFF)),HEX);
    Serial.println();
    }

  }

  if (newcmd && prev_cmd == cmd)
  {
    newcmd=0;
  }
  
  if (newcmd && cmd==0)
  {
    newcmd=0;
  }
  
  while (txoutptr != txinptr)

  {

    Serial.println(txbuffer[txoutptr]);


    txoutptr++;

    if (txoutptr == TX_BUFFER_END)

    {
      txoutptr = 0;

    }

  }
 // Serial.println("loop");

}  

//no signal on ISP clock line for more then 45ms => next change is first bit of packet ...
ISR(TIMER1_COMPA_vect) {
//Enqueue(3);
  cmdbit=0;
  newcmd=0;
  cmd=0;
  

}

// Convert ADC value to key number
//int get_key(unsigned int input)
//{
//  int k;
//  for (k = 0; k < NUM_KEYS; k++)
//  {
//    if (input < adc_key_val[k])
//    {
//      return k;
//    }
//  }
//  if (k >= NUM_KEYS)
//    k = -1;     // No valid key pressed
//  return k;
//}

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
  digitalWrite(dataPin, HIGH);
//  delay(1);
}

void send_cmd(uint8_t cmd)
{
  digitalWrite(DataOut,LOW);
//  delay(1);
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
  //  delayMicroseconds(550);
  //  digitalWrite(DataOut,LOW);
  //  delayMicroseconds(550);
  //  digitalWrite(DataOut,HIGH);
  Serial.println("---------------------------------------------");
  Serial.print(0x53,HEX);
  Serial.print(0x2C,HEX);
  Serial.print(cmd,HEX);
  Serial.println(0xFF^cmd,HEX);
  Serial.println("---------------------------------------------");
}

void readDataIn()
{

//if (!digitalRead(Clk)){

  TIMSK1 &= ~_BV(OCIE1A);  
  TCNT1=0;//disable and reset counter while we recieving data ...
  if(!newcmd)
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
    TIMSK1 |= _BV(OCIE1A); //enable counter
  }

//Enqueue(cmdbit);
  if(cmdbit==64)
  {
    newcmd=1;
    cmdbit=0;
  }
//}

}


static void Enqueue(uint16_t num)

{

  txbuffer[txinptr] = num;

  txinptr++;

  if (txinptr == TX_BUFFER_END)

  {

    txinptr = 0;

  }

}









