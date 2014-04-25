/* 

Alpine code scanner v 0.24 by Amar Kulo - dzoner@gmail.com

Program sends wired remote code combinations to the HU with button press or via loop.

Five buttons are present: 1. previous code/combination
                           2. repeat code
                           3. next code/combination
                           4. start/stop looping
                           5. save combination/import combinations/erase eeprom
                           
When code is sent it's automatically showed on serial console.

*/
#include <EEPROM.h>


// constants
const unsigned int outputPin = 13;
const unsigned int iButtonPin[] = {12,11,10,9,8};
const unsigned int iButtons = 4;

// variables
unsigned int iButtonState[5];
unsigned long lButtonDown[5];
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
  // initialize the serial console
  Serial.begin(9600);
  Serial.println("Program started");
  // initialize pins
  // output pin
  pinMode(outputPin, OUTPUT);
  // input pins
  for (int i = 0; i <= iButtons + 1; i++) {
    pinMode(iButtonPin[i], INPUT);
  }
  
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

void loop(){
  for (int i = 0; i <= iButtons; i++) {
    // read the state of the button
    iButtonState[i] = digitalRead(iButtonPin[i]);

    // check if the button is pressed
    if (iButtonState[i] == 1){
      if(lButtonDown[i] == 0) {
        // save time when button is pressed down
        lButtonDown[i] = millis();
      }
    }

    if (iButtonState[i] == 0){
        if(lButtonDown[i] > 0) {
          if (i == 0) {
            // previous
            sendPrevious();
//            sendPreviousCombination();
          } else if (i == 1) {
            // resend last combination
            sendButton();
          } else if (i == 2){
            // next
            sendNext(); 
//            sendNextCombination();
          } else if (i == 3) {
            changeState();
          } else if (i == 4){  
            // misc
            eraseEeprom();
           // saveCombination();
//            importCombinations();
          }
          // cleanup
          lButtonDown[i] = 0;
          iButtonState[i] = 0;
        }
    }
  }
  if (iLoopState == 1)
  {
   sendNext();     
  }  
}

void changeState() // function that starts or stops looping for the codes
{
  if (iLoopState == 0)
  {
    iLoopState = 1;
    Serial.print("Start looping");
  } else {
    iLoopState = 0;
    Serial.print("Stop looping");
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
}

void saveCombination() // function for saving of founded combination on the eeprom
{
  for (int i = 0; i < 3; i++)
  {
    EEPROM.write(iMemPos + i, iProbe[i]);
  }
  EEPROM.write(5, iMemPos+3);
  iMemPos = iMemPos + 3;
  Serial.println("Combination saved.");
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
