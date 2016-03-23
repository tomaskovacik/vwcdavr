
radio_emulator: arduino (ATMEGAX8 boards only) based vw/audi/skoda/seat radio emulator, I made this to revers eng. on vwcdpic and improve arduino version of CDchanger

 - emulate RADIO DataOut signal
 - receive and print CD changer responce to serial console

serial output is 9600

commands:
 
- next track button       =
- previous track button   -
- next CD                 ]
- previous CD             [
- play/stop               p
- CD1                     1
- CD2                     2
- CD3                     3
- CD4                     4
- CD5                     5
- CD6                     6
- seek forward            f
- seek rewind             r
- scan mode               s
- shuffle mode            l
- help                    h
 
connectios:

 DataOut -> arduino pin 12

 Clk     -> arduino pin 3

 DataIn  -> arduino pin 4 

CDC_emulator: arduino based cd changer emulator for vw/audi/skoda/seat radios, based on avr port of vwcdpic(see header of file for more info)

current status (see readme inside dir):
- tested with my old audi concert 1 radio, and RCD300 (only enable 1CD, TODO: make all 6CD works)
- ARDUINO headphones support
- ARDUINO headphones single button support
- bluetooth support for modules with AT commands (ovc3860 based: XS3868, S3860M-S, BLK-MD-SPK-B) - one way only, no feedback from module - TODO

vw_cd_changer_sniffer - sniffer for radio comunication
