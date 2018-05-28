litle talk/history: http://kovo-blog.blogspot.sk/2014/02/cd-changer-emulator.html

CDC_emulator: arduino based cd changer emulator for vw/audi/skoda/seat radios, based on avr port of vwcdpic(see header of file for more info)

- ARDUINO headphones support - dropped  (if you wanna have this, take look at history of .ino files and make patch, i do not have time to test this...)
- ARDUINO headphones single button support - dropped (if you wanna have this, take look at history of .ino files and make patch, i do not have time to test this...)
- bluetooth support for modules with serial comunication using ATcommand library  https://github.com/botletics/AT-Command-Library will prebably rewrite it in code, cose more functions are needed (unexpected udpates from BT module)

port_of_shyd_version_for_mpd:
	arduino port of SHYD version of his emulator, currently not used by me anymore, ( http://dev.shyd.de/2013/09/avr-raspberry-pi-vw-beta-vag-cdc-faker/ )

connections:

- DataIn 11
- ClockPin 13
- RADIO_OUT PD2 //aka digital 2 on UNO

cdc_arduino_ported_k9spud_vwcdpic:
- arduino uno/duemilanove/nano avr port of vwcdpic project, initial work: http://forums.hackaday.com/viewtopic.php?f=3&t=123
- tested on atmega328,atmega8 chips
- original vwcdpic PJRC controler support
- full SHYD mpd control support
- minimal bluetooth support for BT with serial control support
- timer1 used for timing dataout incomming control data
- timer2 used for timing 50ms gap between pakcets and also 800us gap between bytes in packet
- timer0 is now free, used for timeout timing(using millis()) in serial comunication with BT module
- software serial us used to comunicate with BT over serial


for connection look to header of source code file

cdc_arduino_ported_k9spud_vwcdpic_just_8bit_timers:
same as previous, but use 2 8bit timers
 - tested on atmega328,atmega8, attiny85
 - attiny85 has only serial output, no input  (using tinydebug serial @ 9600) - BT support posible only if BT chip has default baudrate set to 9600 (use https://github.com/tomaskovacik/hw/blob/master/kicad/library/datasheet/OVC3860_revd_config_tool.exe to set it)

for connection look to header of source code file
