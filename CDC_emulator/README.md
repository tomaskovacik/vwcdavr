litle talk/history: http://kovo-blog.blogspot.sk/2014/02/cd-changer-emulator.html

CDC_emulator: arduino based cd changer emulator for vw/audi/skoda/seat radios, based on avr port of vwcdpic(see header of file for more info)

- ARDUINO headphones support
- ARDUINO headphones single button support
- bluetooth support for modules with AT commands (ovc3860 based: XS3868, S3860M-S, BLK-MD-SPK-B) - one way only, no feedback from module - TODO

port_of_shyd_version_for_mpd:
	arduino port of SHYD version of his emulator, currently not used by me anymore, ( http://dev.shyd.de/2013/09/avr-raspberry-pi-vw-beta-vag-cdc-faker/ )

connections:

DataIn 11
ClockPin 13
RADIO_OUT PD2 //aka digital 2 on UNO

cdc_arduino_ported_k9spud_vwcdpic:
- arduino uno/duemilanove/nano avr port of vwcdpic project, initial work: http://forums.hackaday.com/viewtopic.php?f=3&t=123
- tested on atmega328,atmega8 chips
- original vwcdpic PJRC controler support
- full SHYD mpd control support
- initial (aka oneway) bluetooth support without feadback from BT, for OVC3868 chipset based modules, cant use software serial here, it use timers ... bleah, so HW serial is used.

cdc_arduino_ported_k9spud_vwcdpic_just_8bit_timers:
same as previous, but use 2 8bit timers
 - tested on atmega328,atmega8, attiny85
 - attiny85 has only serial output, no input  (using tinydebug serial @ 9600)
 - BT support posible only if BT chip has default baudrate set to 9600 (use https://github.com/tomaskovacik/hw/blob/master/kicad/library/datasheet/OVC3860_revd_config_tool.exe to set it)
