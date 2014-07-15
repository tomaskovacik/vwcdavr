litle talk/history: http://kovo-blog.blogspot.sk/2014/02/cd-changer-emulator.html

based on work of SHYD: http://dev.shyd.de/2013/09/avr-raspberry-pi-vw-beta-vag-cdc-faker/

cdc: 16Mhz version (current code on github) with support for HTC headphone "remote control emualtor" (http://www.george-smart.co.uk/wiki/HTC_Headphones), PCB (currently untested) https://github.com/tomaskovacik/hw/tree/master/kicad/arduino_based_cdc_emulator/atmegax8_dip

 
cdc_16MHz: 16Mhz version (current code on github), PCB (currently untested) https://github.com/tomaskovacik/hw/tree/master/kicad/arduino_based_cdc_emulator/atmegax8_dip

cdc_atmega8: just code from testing atmega8,added support of switch between SPI and simple shiftout functionality (use #def)


cdc_attiny13: unfinished attiny13 version

cdc_attiny85: unfinished attiny85 version

cdc_with_cluster_info: combination of CDCemu + FIS information about tracks played by mp3, idea is to use it with rPI (unfinished projec)

phone_controled_by_arduino_cable_HTC: arduino code for vwcdpic support circute, PCB: https://github.com/tomaskovacik/hw/tree/master/fritzing/vwcdpic/HTC

phone_controled_by_arduino_cable_iOs: arduino code for vwcdpic support circute to control apple devices emulating headphones (untested), apple remote control interface: https://web.archive.org/web/20131204052111/http://david.carne.ca/shuffle_hax/shuffle_remote.html, NO PCB

phone_controled_by_arduino_over_BT: arduino code for vwcdpic support circute, PCB: https://github.com/tomaskovacik/hw/tree/master/fritzing/vwcdpic/Bluetooth 
