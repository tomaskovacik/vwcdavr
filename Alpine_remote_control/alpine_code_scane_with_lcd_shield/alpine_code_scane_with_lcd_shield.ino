//example use of LCD4Bit_mod library

#include <EEPROM.h>
#include <LCD4Bit_mod.h> 
//create object to control an LCD.  
//number of lines in display=1
LCD4Bit_mod lcd = LCD4Bit_mod(2); 

//Key message
int  adc_key_val[5] ={30, 150, 360, 535, 760 };
int NUM_KEYS = 5;
int adc_key_in;
int key=-1;
int oldkey=-1;

// constants
const unsigned int outputPin = 3;

// variables
unsigned int iProbe[] = {128, 128, 128};
unsigned int iMemPos = 10;
unsigned int iHello[24] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1};
unsigned int iFounded[] = {87, 253, 85, 107, 247, 85, 111, 237, 85, 119, 235, 85, 173, 238, 213, 183, 219, 85, 187, 218, 213, 93, 250, 213, 219, 214, 213, 109, 246, 213, 171, 239, 85, 85, 255, 85}; // replace with your values
unsigned int iLoopState = 0; // 0 = stopped, 1 = started
unsigned int iCurrentCombination;

// functions
int *dec2bin(int iDec);
void sendPrevious();
void sendButton();
void sendNext();
void saveCombination();
void printCombinations();
void importCombinations();
void sendNextCombination();
void sendPreviousCombination();
void changeState();

void setup() {
lcd.init();
lcd.clear();
lcd.printIn("Program started");
Serial.begin(9600);
  Serial.println("Program started");
  // initialize pins
  // output pin
  pinMode(outputPin, OUTPUT);
  // read last known values from eeprom
  for (int i = 0; i < 3; i++)
  {
    iProbe[i] = EEPROM.read(i);
  }
  
  // read last save position from eeprom
  iMemPos = EEPROM.read(5);
  if (iMemPos == 0) iMemPos = 10;
  
  // print saved combinations
  printCombinations();
    
}

void loop() {

	adc_key_in = analogRead(0);    // read the value from the sensor  
  digitalWrite(13, HIGH);  
  key = get_key(adc_key_in);		        // convert into key press
	
	if (key != oldkey)				    // if keypress is detected
	{
    delay(50);		// wait for debounce time
		adc_key_in = analogRead(0);    // read the value from the sensor  
    key = get_key(adc_key_in);		        // convert into key press
    if (key != oldkey)				
    {			
      oldkey = key;
      if (key >=0){
      lcd.cursorTo(2, 0);  //line=2, x=0
      switch (key) {

case 0: 
lcd.clear();
lcd.printIn("Send Next");
		sendNext(); 
//		sendNextCombination();
break;
case 1:
	// misc
//		eraseEeprom();
		saveCombination();
//		importCombinations();
break;
case 2: 
  lcd.clear();
  lcd.printIn("Repeate last");
        // resend last combination
		sendButton();
break;
case 3:
  lcd.clear();
  lcd.printIn("Send Previous");
		sendPrevious();
break;
case 4:
		changeState();
//		sendPreviousCombination();
break;

} 
      }
    }
  }
  
  if (iLoopState == 1)
  {
   sendNext();     
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

void changeState() // function that starts or stops looping for the codes
{
    lcd.clear();
  if (iLoopState == 0)
  {
    iLoopState = 1;
    Serial.print("Start looping");
      lcd.printIn("Start looping");
  } else {
    iLoopState = 0;
    Serial.print("Stop looping");
    lcd.printIn("Stop looping");
  }
}

int* dec2bin(int iDec) // function for converting decimal to binary
{
  static int iResult[7];
  for (int i = 0; i <= 7; i++)
  {    
    iResult[i] = iDec % 2;
    iDec = iDec / 2;
  } 
  
  return iResult;
}

void printArray(int* iArray) // function for output of array in reversed mode for binary values
{
  
  for (int x = 0; x <= 7; x++)
  {
    Serial.print(iArray[7-x]);
  }
  
  Serial.print(" ");
}

void sendPrevious() // function that sends previous code
{
  if (iProbe[2] > 85)
  {
    iProbe[2] = 85;
    goto sendme;
  } 
  if (iProbe[1] > 214)
  {
    --iProbe[1];
   iProbe[2] = 213;
    goto sendme;
  } 
  if (iProbe[0] > 85)
  {
    iProbe[2] = 213;
    iProbe[1] = 255;
    --iProbe[0];
    goto sendme;
  }  
sendme:
  sendButton();
}

void sendNext() // function that sends next code
{
  if (iProbe[2] < 213)
  {
    iProbe[2] = 213;
    goto sendme;
  }
     
  if (iProbe[1] < 255)
  {
    ++iProbe[1];
    iProbe[2] = 85;
    goto sendme;
  } 
  if (iProbe[0] < 219)
  {
    ++iProbe[0];
    iProbe[2] = 85;
    iProbe[1] = 214;
    goto sendme;
  }    
sendme:
  sendButton();
}

void sendButton() // main function that sends codes on the output pin
{
  // debug
  Serial.println("Probing button combination:");
  for (int i = 0; i < 3; i++)
  {
    EEPROM.write(i,iProbe[i]);
    Serial.print(iProbe[i]);
    Serial.print(" ");
  }
  Serial.print("\n");
  
  printArray(dec2bin(iProbe[0]));
  Serial.print(" ");
  printArray(dec2bin(iProbe[1]));
  Serial.print(" ");
  printArray(dec2bin(iProbe[2]));
  Serial.print("\n");

  // init part
  //first send 8ms high
  digitalWrite(outputPin, HIGH);
  delay(8);
  // send 4.5ms low
  digitalWrite(outputPin, LOW);
  delayMicroseconds(4500);

  // send hello part
  for (int i = 0; i < 24; i++)
  {
    if (iHello[i] == 0)
    {
      digitalWrite(outputPin, LOW);
    } else {
      digitalWrite(outputPin, HIGH);
    }
    // end part
    delayMicroseconds(500);
    // wait 0.5ms
    digitalWrite(outputPin, LOW);
    delayMicroseconds(500);
  }
  
  for (int i = 0; i < 3; i++)
  {
    int *iArr = dec2bin(iProbe[i]);
    
    // test part
    for (int x = 0; x < 8; x++)
    {
      if (iArr[7-x] == 0)
      {
        digitalWrite(outputPin, LOW);
      } else {
        digitalWrite(outputPin, HIGH);
      }
      // end part
      delayMicroseconds(500);
      // wait 0.5ms
      digitalWrite(outputPin, LOW);
      delayMicroseconds(500);
    }
  }

  // end part
  // send 41ms low
  digitalWrite(outputPin, LOW);
  delay(41); 
}

void eraseEeprom() // function for erasing of the eeprom
{
  lcd.clear();
  lcd.printIn("EEprom erase.");
  iProbe[0] = 85;
  iProbe[1] = 214;
  iProbe[2] = 85;

  for (int i = 0; i<3; i++)
  {
    EEPROM.write(i, iProbe[i]);
  }
  EEPROM.write(5, 10);
  iMemPos = 10;
  Serial.println("EEprom erased.");
  lcd.cursorTo(2,0);
  lcd.printIn("Done.");
}

void saveCombination() // function for saving of founded combination on the eeprom
{
  lcd.clear();
  lcd.printIn("Saving combination:");
  for (int i = 0; i < 3; i++)
  {
    EEPROM.write(iMemPos + i, iProbe[i]);
  }
  EEPROM.write(5, iMemPos+3);
  iMemPos = iMemPos + 3;
  Serial.println("Combination saved.");
  lcd.cursorTo(2,0);
  lcd.printIn("Done.");
  
}

void printCombinations() // function that print list of codes saved on eeprom
{
  Serial.print("Number of saved configurations: ");
  Serial.print((iMemPos - 10) / 3, DEC);
  Serial.print("\n");
  if (iMemPos == 10) return;
  int x = 0;
  for (int i = 10; i < iMemPos; i++)
  {   
   if (x == 3)
   {
     Serial.print("\n");
     x = 0;
   }
   x = x + 1;
   Serial.print(EEPROM.read(i), DEC);
   Serial.print(" ");   
  } 
  
}

void sendNextCombination() // function that sends next combination from eeprom
{
  if(iCurrentCombination == ((iMemPos - 10) / 3) - 1)
  {
    iCurrentCombination = 0;
  } else {
    iCurrentCombination++;
  }
  
  Serial.print("\nSending combination nr ");
  Serial.print(iCurrentCombination, DEC);
  Serial.print("\n");
  
  for(int i = 0; i < 3; i++)
  {
    iProbe[i] = EEPROM.read(i + 10 + (iCurrentCombination * 3));
    Serial.print(" ");
  }
  
  Serial.print("\n");
  sendButton();
}

void sendPreviousCombination() // function that sends previous combination from eeprom
{
  if(iCurrentCombination == 0)
  {
    iCurrentCombination = ((iMemPos - 10) / 3) - 1;
  } else {
    iCurrentCombination--;
  }
  
  Serial.print("\nSending combination nr ");
  Serial.print(iCurrentCombination, DEC);
  Serial.print("\n");
  
  for(int i = 0; i < 3; i++)
  {
    iProbe[i] = EEPROM.read(i + 10 + (iCurrentCombination * 3));
    Serial.print(" ");
  }
  
  Serial.print("\n");
  
  sendButton();
}

void importCombinations() // function that import combinations from iFounded array and saves them to eeprom
{  
  Serial.print("\n");
  Serial.print("Importing combinations.");
  
  for (int i = 0; i < (sizeof(iFounded) / 2); i++)
  {
     EEPROM.write(i+10, iFounded[i]);
  }
  
  iMemPos =  (sizeof(iFounded) / 2) + 10;
  
  EEPROM.write(5, iMemPos); 
  
  Serial.print("\n");
  Serial.print("Importing done.");
  Serial.print("\n");
}
