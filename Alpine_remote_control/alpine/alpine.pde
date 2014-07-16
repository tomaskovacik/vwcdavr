// Peugeot 206 1998 steering wheel remote control interface for Apline
// JACK:
//      +--------+
//  OOOO|        |== = - < signal to alpPin
//      +--------+
//                ^
//               GND 
// 
// Arduino:       analogPin0       analogPin1
//                    ^                ^
//        +-------+   |                |   +-------+
// +5V o--|  680  |---+       GND      +---|  680  |--o +5V
//        +-------+   |        ^       |   +-------+
//                    |        |       |
//                    ^        ^       ^
// remote:           red     white    blue
//

#define CHECK_MS 5
#define DOUBLE_MS 200
#define HOLD_MS 1200

#define NO_B 1023 //nothing pressed
#define MODE_B 213 // MODE
#define NEXT_UP_B 211 // NEXT UP
#define NEXT_DOWN_B 0 // NEXT DOWN
#define CHANGE_B 0 // CHANGE
#define VOLUME_UP_B 498 // VOLUME UP
#define VOLUME_DOWN_B 515 // VOLUME DOWN

#define alpPin 3

boolean volUp[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 1,1,0,1,1,0,1,1, 1,1,0,1,0,1,1,0, 1,1,0,1,0,1,0,1};
boolean volDn[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,1,0,1,1,0,1, 1,1,1,1,0,1,1,0, 1,1,0,1,0,1,0,1};
boolean mute[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 1,0,1,0,1,1,0,1, 1,1,1,0,1,1,1,0, 1,1,0,1,0,1,0,1};
//folder/memorys
boolean pstUp[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 1,0,1,0,1,0,1,1, 1,1,1,0,1,1,1,1, 0,1,0,1,0,1,0,1};
boolean pstDn[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,0,1,0,1,0,1, 1,1,1,1,1,1,1,1, 0,1,0,1,0,1,0,1};
//tuner/cd/usb/aux
boolean source[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 1,0,1,1,0,1,1,1, 1,1,0,1,1,0,1,1, 0,1,0,1,0,1,0,1};
boolean trkUp[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 1,0,1,1,1,0,1,1, 1,1,0,1,1,0,1,0, 1,1,0,1,0,1,0,1};
boolean trkDn[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,0,1,1,1,0,1, 1,1,1,1,1,0,1,0, 1,1,0,1,0,1,0,1};
//on/off
boolean power[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,1,1,0,1,1,1, 1,1,1,0,1,0,1,1, 0,1,0,1,0,1,0,1};
//play/pause
boolean entPlay[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,0,1,0,1,1,1, 1,1,1,1,1,1,0,1, 0,1,0,1,0,1,0,1};
//fm1/fm2/fm3/wm
boolean bandProg[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,1,0,1,0,1,1, 1,1,1,1,0,1,1,1, 0,1,0,1,0,1,0,1};
boolean defeat[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,1,0,1,1,1,1, 1,1,1,0,1,1,0,1, 0,1,0,1,0,1,0,1};

boolean spFlag = 0;
boolean dpFlag = 0;
boolean hpFlag = 0;

int button = 0; 
int spbutton = 0;

void setup() {
Serial.begin(9600);
pinMode(7, OUTPUT);
}

void loop() {
static unsigned long oldTime = 0;
int switchStatus = 0;
if (millis() >= oldTime + CHECK_MS) {
oldTime = millis();
switchStatus = CheckSwitch();

if (switchStatus == 0) {
//no button press

} else if (switchStatus == 1) {
// single press
switch (spbutton) {
case 0: // no butto, why are wh here
break;
case 1: // next up
trkUpSend();
break;
case 2: // next down
trkDnSend();
break;
case 3: // vol up
volUpSend();
break;
case 4: // vol down
volDnSend();
break;
case 5: // change
sourceSend();
break;
case 6: // mode
modeSend();
break;
} 
} else if (switchStatus == 2) {
//double press
switch (spbutton) {
case 0: // no butto, why are wh here
break;
case 1: // next up
pstUpSend();
break;
case 2: // next down
pstDnSend();
break;
case 3: // vol up
volUpSend();
volUpSend();
break;
case 4: // vol down
volDnSend();
volDnSend();
break;
case 5: // change
sourceSend();
sourceSend();
break;
case 6: // mode
modeSend();
modeSend();
break;
} 
} else if (switchStatus == 3) {
//hold
switch (spbutton) {
case 0: // no butto, why are wh here
break;
case 1: // next up
trkUpSend();
break;
case 2: // next down
trkDnSend();
break;
case 3: // vol up
volUpSend();
break;
case 4: // vol down
volDnSend();
break;
case 5: // change
powerSend();
break;
case 6: // mode
muteSend();
break;
}}}
}

// Service routine called by a timer interrupt
int CheckSwitch() {
static uint16_t State = 0; // Current debounce status
static int curTime = 0;

State=(State<<1) | !RawKeyPressed() | 0xe000;
if (!spFlag) {
if (hpFlag) {
if (State == 0xe000) {
return 3;
} else {
hpFlag = 0;
}
}
if (State==0xf000) {
spFlag = 1;
hpFlag = 1;
dpFlag = 1;
curTime = 0;
spbutton = button;
return 0;
}
} 
if (hpFlag && State != 0xe000) {
hpFlag = 0;
}
if (spFlag && hpFlag && curTime > HOLD_MS/CHECK_MS) {
spFlag = 0;
dpFlag = 0;
return 3;
} else if (spFlag && dpFlag && State == 0xf000 && curTime < DOUBLE_MS) {
spFlag = 0;
hpFlag = 0;
dpFlag = 0;
return 2;
} else if (spFlag && !hpFlag && curTime > DOUBLE_MS) {
spFlag = 0;
hpFlag = 0;
dpFlag = 0;
return 1;
} else {
curTime = curTime + CHECK_MS;
return 0;
}
}

boolean RawKeyPressed() {
int anaPin1 = 0;
int anaPin2 = 0;
anaPin1 = analogRead(0);
anaPin2 = analogRead(1);
if (anaPin1 > NO_B - 20 && anaPin2 > NO_B - 20) {
button = 0;
return 0;
} else if (anaPin2 > NEXT_UP_B - 20 && anaPin2 < NEXT_UP_B + 20 ) {
button = 1;
return 1;
} else if (anaPin2 < NEXT_DOWN_B + 20 ) {
button = 2;
return 1;
} else if (anaPin2 > VOLUME_UP_B - 20 && anaPin2 < VOLUME_DOWN_B + 20 ) {
button = 3;
return 1;
} else if (anaPin1 > VOLUME_DOWN_B - 20 && anaPin1 < VOLUME_DOWN_B + 20 ) {
button = 4;
return 1;
} else if (anaPin1 < CHANGE_B + 20 ) {
button = 5;
return 1;
} else if (anaPin1 > MODE_B - 20 && anaPin1 < MODE_B + 20 ) {
button = 6;
return 1;
}else {
button = 0;
return 0;
}
}

//-----------SOURCE------------------
void sourceSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (source[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}

//---------POWER-----------------------------------------------
void powerSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (power[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}

//---------DEFEAT-----------------------------------------------
void defeatSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (defeat[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}

//---------MODE-----------------------------------------------
void modeSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (entPlay[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}


//---------NEXT UP-----------------------------------------------
void trkUpSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (trkUp[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}

//---------NEXT DOWN----------------------------------------------
void trkDnSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (trkDn[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}

//---------FOLDER/MEMORY UP-----------------------------------------------
void pstUpSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (pstUp[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}

//---------FOLDER/MEMORY DOWN----------------------------------------------
void pstDnSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (pstDn[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}

//---------MUTE---------------------------------------------------
void muteSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (mute[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}

//---------VOL UP-----------------------------------------------
void volUpSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (volUp[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}


//---------VOL DOWN-----------------------------------------------
void volDnSend() {
//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (volDn[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 
}

