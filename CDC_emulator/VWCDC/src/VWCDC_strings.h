#pragma once

#ifdef BLUETOOTH
const uint8_t sDATAERR[] PROGMEM = "\r\n";
const uint8_t sOVERFLOW[] PROGMEM = "\r\n";
const uint8_t sMDISABLE[] PROGMEM = "AT#MA\r\n";
const uint8_t sMENABLE[] PROGMEM = "AT#MA\r\n";
const uint8_t sMINQUIRY[] PROGMEM = "MINQUIRY\r\n";
const uint8_t sPRV_LIST[] PROGMEM = "AT#MA\r\n";
const uint8_t sNXT_LIST[] PROGMEM = "AT#MA\r\n";
const uint8_t sLIST1[] PROGMEM = "AT#MA\r\n";
const uint8_t sLIST2[] PROGMEM = "AT#MA\r\n";
const uint8_t sLIST3[] PROGMEM = "AT#MA\r\n";
const uint8_t sLIST4[] PROGMEM = "AT#MA\r\n";
const uint8_t sLIST5[] PROGMEM = "AT#MA\r\n";
const uint8_t sLIST6[] PROGMEM = "AT#MA\r\n";
const uint8_t sRANDOM[] PROGMEM = "RANDOM\r\n";
const uint8_t sPLAY[] PROGMEM = "AT#MA\r\n";
const uint8_t sSCAN[] PROGMEM = "SCAN\r\n";
const uint8_t sSTOP[] PROGMEM = "AT#MA\r\n";
const uint8_t sNEXT[] PROGMEM = "AT#MD\r\n";
const uint8_t sPREVIOUS[] PROGMEM = "AT#ME\r\n";
const uint8_t sRING[] PROGMEM = "\r\n";
const uint8_t sIDENTIFY[] PROGMEM = "\r\n";
const uint8_t sNEWLINE[] PROGMEM = "\r\n";
const uint8_t sDASH[] PROGMEM = "";
const uint8_t sHEX[] PROGMEM = "";
const uint8_t sVERSION[] PROGMEM = "";
#else
const uint8_t sDATAERR[] PROGMEM = "dataerr\r\n";
const uint8_t sOVERFLOW[] PROGMEM = "overflow\r\n";
const uint8_t sMDISABLE[] PROGMEM = "MDISABLE\r\n";
const uint8_t sMENABLE[] PROGMEM = "MENABLE\r\n";
const uint8_t sMINQUIRY[] PROGMEM = "MINQUIRY\r\n";
const uint8_t sPRV_LIST[] PROGMEM = "PRV_LIST\r\n";
const uint8_t sNXT_LIST[] PROGMEM = "NXT_LIST\r\n";
const uint8_t sLIST1[] PROGMEM = "LIST1\r\n";
const uint8_t sLIST2[] PROGMEM = "LIST2\r\n";
const uint8_t sLIST3[] PROGMEM = "LIST3\r\n";
const uint8_t sLIST4[] PROGMEM = "LIST4\r\n";
const uint8_t sLIST5[] PROGMEM = "LIST5\r\n";
const uint8_t sLIST6[] PROGMEM = "LIST6\r\n";
const uint8_t sRANDOM[] PROGMEM = "RANDOM\r\n";
const uint8_t sPLAY[] PROGMEM = "PLAY\r\n";
const uint8_t sSCAN[] PROGMEM = "SCAN\r\n";
const uint8_t sSTOP[] PROGMEM = "STOP\r\n";
const uint8_t sNEXT[] PROGMEM = "NEXT\r\n";
const uint8_t sPREVIOUS[] PROGMEM = "PREVIOUS\r\n";
const uint8_t sRING[] PROGMEM = "RING\r\n";
const uint8_t sIDENTIFY[] PROGMEM = "Audi Concert I Multimedia Gateway Ver.";
const uint8_t sNEWLINE[] PROGMEM = "\r\n";
const uint8_t sDASH[] PROGMEM = "_";
const uint8_t sTP[] PROGMEM = "TP\r\n";
const uint8_t sHEX[] PROGMEM = {
  '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0, '9', 0, 'A', 0, 'B', 0, 'C', 0, 'D', 0, 'E', 0, 'F', 0
};
const uint8_t sVERSION[] PROGMEM = {
  VER_MAJOR, '.', VER_MINOR, VER_PATCHLEVEL, 0
};
#endif
