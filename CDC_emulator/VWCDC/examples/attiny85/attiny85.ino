#include <TinyDebugSerial.h>
#include <VWCDC.h>

TinyDebugSerial mySerial;
VWCDC cdc(mySerial);

void setup() {
  mySerial.begin(9600);
  cdc.begin();
}

void loop() {
  cdc.loop();
}
