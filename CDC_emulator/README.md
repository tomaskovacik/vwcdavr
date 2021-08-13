Little talk/history: http://kovo-blog.blogspot.sk/2014/02/cd-changer-emulator.html

CDC_emulator: Arduino based CD changer emulator for VW/Audi/Skoda/Seat radios, based on AVR port of vwcdpic (see header of file for more info)

-Android headphones three button support, with cdc_arduino_ported_k9spud_vwcdpic_headphone_support
- Android headphones single button support - dropped (if you wanna have this, take a look at history of .ino files and make patch, I do not have time to test this...)
- Bluetooth support for modules with serial comunication, not fully implemented yet 

port_of_shyd_version_for_mpd:
	Arduino port of SHYD version of his emulator, currently not used by me anymore ( http://dev.shyd.de/2013/09/avr-raspberry-pi-vw-beta-vag-cdc-faker/ )

cdc_arduino_ported_k9spud_vwcdpic:
- Arduino Uno/Duemilanove/Nano AVR port of vwcdpic project, initial work: http://forums.hackaday.com/viewtopic.php?f=3&t=123
- support for: ATmega328, ATmega328A, ATmega328P, ATmega328PA, ATmega324, ATmega324P, ATmega324A, ATmega324PA
- original vwcdpic PJRC controler support
- full SHYD mpd control support
- minimal Bluetooth support for BT with serial control support
- timer1 is used for timing incomming control data from radio pin DataOut, timer interrupt (ICP) is used
- timer2 is used for timing between pakcets and also 800us gap between bytes in packet
- timer0 is used for 10ms timing used for 50ms timing between packets and also for 1second count for play time count
- software serial us used to comunicate with BT over serial

Connections are defined by these defines:

for ATmegaXX8YY chips:
```
#define RADIO_COMMAND      PB0 //ICP
#define RADIO_COMMAND_DDR  DDRB
#define RADIO_COMMAND_PORT  PORTB
#define RADIO_COMMAND_PIN PINB
#define RADIO_CLOCK        PB5
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT  PORTB
#define RADIO_DATA         PB3
#define RADIO_DATA     DDB3
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT  PORTB
```

in Arduino world, it is:
```
 * RADIO PIN -> Arduino pin
 * 
 * DataOut   -> digital 8 (ICP1)
 * DataIn    -> digital 11(PB3)
 * Clock     -> digital 13(PB5)
 *
 * computer -> Arduino pin
 * serial TX -> digital 0 (PD0)
 * serial RX -> digital 1 (PD1)
```

for ATmegaXX4YY chips:
```
#define RADIO_COMMAND      PD6 //ICP 
#define RADIO_COMMAND_DDR  DDRD
#define RADIO_COMMAND_PORT  PORTD
#define RADIO_COMMAND_PIN PIND
#define RADIO_CLOCK        PB7
#define RADIO_CLOCK_DDR    DDRB
#define RADIO_CLOCK_PORT  PORTB
#define RADIO_DATA         PB5
#define RADIO_DATA_DDR     DDRB
#define RADIO_DATA_PORT  PORTB
```

in Arduino world, if my board is used (https://www.tindie.com/products/tomaskovacik/nanoxx4/) then this traslate to:
```
 * RADIO PIN -> Arduino pin
 *
 * DataOut   -> digital 8 (ICP1)
 * DataIn    -> digital 11(PB5)
 * Clock     -> digital 13(PB7)
 *
 * computer -> Arduino pin
 * serial TX -> digital 0 (PD0)
 * serial RX -> digital 1 (PD1)
```

cdc_arduino_ported_k9spud_vwcdpic_headphone_support:
- Support for: ATmega328, ATmega328A, ATmega328P, ATmega328PA, ATmega324, ATmega324P, ATmega324A, ATmega324PA
- Android three button headphones support or MP3 player buttons support
- Tested on Arduino Uno (ATmega328P)
- For more info, check the comment at the top of the file

Connections:
```
   UNO, Duemilanove, Nano ... ATmegaX8 base boards:
   RADIO PIN -> Arduino pin

   DataOut   -> digital 8 (PB0)
   DataIn    -> digital 11(PB5)
   Clock     -> digital 13(PB7)

   PC PIN    -> Arduino pin
   Serial TX -> digital 0 (PD0)
   Serial RX -> digital 1 (PD1)
   
   ANDROID support:
   NEXT button -> digital 5 (PD5)
   PLAY button -> digital 6 (PD6)
   PREV button -> digital 7 (PD7)
```

cdc_arduino_ported_k9spud_vwcdpic_just_8bit_timers:
same as previous, but use 2 8bit timers, mostly for ATTINY chips 
 - tested on ATmega328, ATmega8, ATtiny85
 - with ATtiny85 chip only serial output is used, no input (using tinydebug serial @ 9600) - BT support posible only if BT chip has default baudrate set to 9600
 - this version has support to control Android headphone simulating buttons on headphone jack (I do not remember if I tested it or not, sorry), also has single button support (single, double, triple click)

Connections:
```
   UNO, Duemilanove, Nano ... ATmegaX8 base boards:
   RADIO PIN -> Arduino pin

   DataOut   -> digital 2 (INT0)
   DataIn    -> digital 11(PB3)
   Clock     -> digital 13(PB5)

   PC PIN    -> Arduino pin
   Serial TX -> digital 0 (PD0)
   Serial RX -> digital 1 (PD1)

   ANDROID support:
   PREVIOUS BUTTON -> digital 7 (PD7)
   PLAY BUTTON -> digital 6 (PD6)
   NEXT BUTTON -> digital 5 (PD5)

   ATTINYx5 version:

   RADIO PIN -> ATTINYx5 PIN
   DataOut   -> 7 (INT0)
   DataIn    -> 6 (PB0)
   Clock     -> 5 (PB1)

   PC PIN    -> ATTINYx5 PIN
   Serial TX -> 2 (PB3) using tinydebug lib
   Serial RX -> not connected
```

   What you need to install:
   ATTinycore: https://github.com/SpenceKonde/ATTinyCore (supported directly in Arduino from version 1.6.3 via board manager)
   TinyDebugSerial from: https://github.com/jscrane/TinyDebugSerial (download and extract into libraries Arduino folder)

   select attiny25/45/85 board from tools-> board -> (ATTinyCore) ATtiny25/45/85
   select attiny85 from tools->chip->ATtiny85
   select internal 8Mhz from tools->Clock -> 8MHz (internal)

   to use digispark uncoment line //#define DIGISPARK
   then use 16MHz-noUSB settings 
   tested only with my ATTinyCore fork with Digispark support: https://github.com/tomaskovacik/ATTinyCore/
