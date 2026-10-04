#include <VWCDC.h>

VWCDC cdc(Serial);

void setup() {
  Serial.begin(9600);
  cdc.begin();
}

void loop() {
  cdc.loop();
}
