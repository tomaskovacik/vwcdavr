#pragma once

#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#ifdef DIGISPARK
#include <avr/power.h>
#endif

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega128__)
#define TCCR2A TCCR2
#define TCCR2B TCCR2
#define COM2A1 COM21
#define COM2A0 COM20
#define OCR2A OCR2
#define TIMSK2 TIMSK
#define OCIE2A OCIE2
#define TIMER2_COMPA_vect TIMER2_COMP_vect
#define TIMSK1 TIMSK
#define TIFR1 TIFR
#define ICIE1 TICIE1
#define TIMSK0 TIMSK
#define TIFR0 TIFR
#define OCF2A OCF2
#define TCCR0A TCCR0
#define TCCR0B TCCR0
#define EIMSK GICR
#define OCIE0A OCIE0
#define EICRA MCUCR
#define EIFR GIFR
#endif

// Opt-in user defines:
// #define PJRC
// #define JUST_HEX_TO_SERIAL
// #define BLUETOOTH
// #define DISC_TRACK_NUMBER_FROM_MPD
// #define ANDROID_HEADPHONES
// #define ANDROID_HEADPHONES_ONE_BUTTON
// #define DIGISPARK

#ifdef BLUETOOTH
#define SERIAL_BAUDRATE 9600
#else
#define SERIAL_BAUDRATE 9600
#endif

#ifdef ANDROID_HEADPHONES_ONE_BUTTON
#define ANDROID_HEADPHONES
#endif

#define TRUE 1
#define FALSE 0

#define STARTTHRESHOLD  6400
#define HIGHTHRESHOLD   2496
#define LOWTHRESHOLD    512
#define PKTSIZE         -32
#define SECONDWAIT      -20
#define POWERIDENTWAIT  -10
#define SCANWAIT        -100
#define _50MS           5000
#define _700US          70
#define _10US           1
#define CLK_DELAY       5
#define TX_BUFFER_END   12
#define CAP_BUFFER_END  24

#ifdef ANDROID_HEADPHONES
#define ANDROID_PUSH_COUNT 2
#define ANDROID_DELAY_COUNT 1
#endif
#ifdef ANDROID_HEADPHONES_ONE_BUTTON
#define ANDROID_PLAY_COUNT 1
#define ANDROID_NEXT_COUNT 2
#define ANDROID_PREV_COUNT 3
#endif

#define VER_MAJOR       '1'
#define VER_MINOR       '1'
#define VER_PATCHLEVEL  'd'

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
#define RADIO_COMMAND      PB2
#define RADIO_COMMAND_DDR  DDRB
#define RADIO_COMMAND_PORT PORTB
#define RADIO_COMMAND_PIN  PINB
#define RADIO_CLOCK        PB1
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT   PORTB
#define RADIO_DATA         PB0
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT    PORTB

#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328A__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PA__) || defined(__AVR_ATmega328PB__) || \
      defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) || defined(__AVR_ATmega168PB__) || \
      defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega88PA__) || defined(__AVR_ATmega88PB__)
#define RADIO_COMMAND      PD2
#define RADIO_COMMAND_DDR  DDRD
#define RADIO_COMMAND_PORT PORTD
#define RADIO_COMMAND_PIN  PIND
#define RADIO_CLOCK        PB5
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT   PORTB
#define RADIO_DATA         PB3
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT    PORTB

#elif defined(__AVR_ATmega324__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__)
#define RADIO_COMMAND      PB2
#define RADIO_COMMAND_DDR  DDRB
#define RADIO_COMMAND_PORT PORTB
#define RADIO_COMMAND_PIN  PINB
#define RADIO_CLOCK        PB7
#define RADIO_CLOCK_DDb    DDB7
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT   PORTB
#define RADIO_DATA         PB5
#define RADIO_DATA_DDb     DDB5
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT    PORTB

#elif defined(__AVR_ATtiny167__)
#define RADIO_COMMAND      PA4
#define RADIO_COMMAND_DDR  DDRA
#define RADIO_COMMAND_PORT PORTA
#define RADIO_COMMAND_PIN  PINA
#define RADIO_CLOCK        PB1
#define RADIO_CLOCK_DDb    DDB1
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT   PORTB
#define RADIO_DATA         PB0
#define RADIO_DATA_DDb     DDB0
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT    PORTB
#endif

#ifdef ANDROID_HEADPHONES
#define ANDROID_NEXT       PD5
#define ANDROID_NEXT_DDR   DDRD
#define ANDROID_NEXT_PORT  PORTD
#define ANDROID_PLAY       PD6
#define ANDROID_PLAY_DDR   DDRD
#define ANDROID_PLAY_PORT  PORTD
#define ANDROID_PREV       PD7
#define ANDROID_PREV_DDR   DDRD
#define ANDROID_PREV_PORT  PORTD
#endif

#define Do_PLAY           0x03
#define Do_LOADCD         0x01
#define Do_ENABLE_MK      0x08
#define Do_CD1            0x0C
#define Do_DISABLE        0x10
#define Do_CHANGECD       0x14
#define Do_PREVCD         0x18
#define Do_CD5            0x2C
#define Do_TP             0x30
#define Do_SEEKFORWARD_MK 0x38
#define Do_CD3            0x4C
#define Do_SEEKBACK       0x58
#define Do_MIX_CD         0x60
#define Do_UP_MK3         0x68
#define Do_DOWN           0x78
#define Do_CD2            0x8C
#define Do_SCAN           0xA0
#define Do_UNKNOWNCMD     0xA4
#define Do_DOWN_MK3       0xA8
#define Do_CD6            0xAC
#define Do_CD4            0xCC
#define Do_SEEKFORWARD    0xD8
#define Do_MIX            0xE0
#define Do_ENABLE         0xE4
#define Do_UP             0xF8

enum STATES
{
  StateIdle,
  StateIdleThenPlay,
  StateInitPlay,
  StateInitPlayEnd,
  StateInitPlayAnnounceCD,
  StatePlayLeadIn,
  StatePlayLeadInEnd,
  StatePlayLeadInAnnounceCD,
  StateTrackLeadIn,
  StatePlay,
  StateTP
};

class VWCDC {
public:
  VWCDC(Stream &serial = Serial);
  void begin();
  void loop();

  void setDisc(uint8_t d);
  void setTrack(uint8_t t);
  void setPlaying(bool p);

  void handleINT();
  void handleTimerOVF();
  void handleTimer10us();

  static VWCDC *_instance;

private:
  Stream &_serial;

  uint8_t sendreg;
  uint8_t sendbitcount;
  uint8_t disc, track, minute, second;
  uint8_t scanptr, fsr, scanbyte, cmdcode;
  uint8_t intwsave, intstatussave, intfsrsave;
  uint8_t capbusy;
  uint8_t mix, scan, playing;
  uint8_t cd_button, mix_button;
  uint8_t overflow, dataerr;
  uint8_t _1us_delay;
  uint16_t captime, captime_ovf;
  int8_t capbit, capbitpacket;
  uint8_t capptr;
  uint8_t BIDIstate;
  int8_t BIDIcount;
  uint8_t ACKcount, discload;
  int8_t poweridentcount;
  uint8_t secondcount;
  int8_t scancount;
  uint8_t txinptr, txoutptr;
  uint8_t display_byte_buffer_mau8[8];
  const uint8_t *txbuffer[12];
  uint8_t capbuffer[24];
  uint16_t counter_50ms;
  int counter_to_send_packet;
  uint8_t flag_50ms;
  uint8_t display_byte_counter_u8;
#ifdef ANDROID_HEADPHONES
  int8_t play_count;
#ifndef ANDROID_HEADPHONES_ONE_BUTTON
  int8_t next_count;
  uint8_t prev_count;
#endif
  uint8_t prev_count_delay;
#ifdef ANDROID_HEADPHONES_ONE_BUTTON
  uint8_t play_count_delay;
  uint8_t play_count_push;
#endif
#endif
#ifdef DUMPMODE
  uint8_t startbit;
#endif

  void setupTimers();
  void outputByte();
  void initVWCDC();
  void cdcProtocol();
  void decodeCommand();
  void scanCommandBytes();
  void dumpFullCommand();
  uint8_t getCaptureByte();
  void setStateIdle();
  void setStatePlay();
  void setStateInitPlay();
  void setStatePlayLeadIn();
  void setStateTrackLeadIn();
  void setStateIdleThenPlay();
  void setStateTP();
  void sendDisplayBytes();
  void sendDisplayBytesNoCD();
  void sendDisplayBytesInitCD();
  void sendFrameByte(uint8_t b);
  void sendByte(uint8_t b);
  void enqueueString(const uint8_t *addr);
  void enqueueHex(uint8_t h);
  void resetTime();
  void sendStateIdle();
  void sendStateTP();
  void sendStatePlayLeadInEnd();
  void sendStateInitPlayEnd();
  void sendStateInitPlayAnnounceCD();
  void sendStatePlayLeadInAnnounceCD();
  void sendPacket();
  void printstrP(const char *s);
  void androidButtons();
};
