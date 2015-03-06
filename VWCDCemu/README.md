CDC_sniffer: arduino based sniffer for comunication between audi radio and cd changer, I made this to revers eng. on vwcdpic and improve arduino version of CDchanger better

 - emulate RADIO DataOut signals 
 - receive and print CD changer responce to serial console 

CDC_emulator: arduino based cd changer emulator for audi radio based on http://dev.shyd.de/2013/09/avr-raspberry-pi-vw-beta-vag-cdc-faker/ more here: http://kovo-blog.blogspot.sk/2014/02/cd-changer-emulator.html, still alpha version, i work on my free time to improve it

extentions( to vwcdpic :

- phone_controled_by_arduino_cable_HTC
	- attiny85 code
	- HW part: https://github.com/tomaskovacik/hw/tree/master/fritzing/vwcdpic/HTC

- phone_controled_by_arduino_cable_iOs
	- teoretic code for arduino (mega88/168/328)
	- for iOs deices based on work from David Carne (https://web.archive.org/web/20131128010310/http://david.carne.ca/shuffle_hax/shuffle_remote.html)
	- no HW was made
	- no test was made

- phone_controled_by_arduino_over_BT
	- attiny85 code
	- for BT module MB-CM15113 from sure electronics: http://store.sure-electronics.com/audio/audio-amplifier-accessories/mb-cm15113
	- HW part: https://github.com/tomaskovacik/hw/tree/master/fritzing/vwcdpic/Bluetooth
	- Fritzing part: https://github.com/tomaskovacik/hw/tree/master/fritzing/parts/MB-CM15113

VW group CD Changer(CDC) emulators code for arduino:

- cdc_with_headphone_control - cdc emulator with support of controling phone over hedphones 4th ring (shorting microphone with diferent resistance, in my case HTC: http://www.george-smart.co.uk/wiki/HTC_Headphones, schematics: https://github.com/tomaskovacik/hw/tree/master/kicad/arduino_based_cdc_emulator/atmegax8_dip

todo:
- cdc_16MHz    cdc_attiny13  cdc_rcd200		  cdc_with_headphone_control		  README.md
cdc_atmega8  cdc_attiny85  cdc_with_cluster_info  	




