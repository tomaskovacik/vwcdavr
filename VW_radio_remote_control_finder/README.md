code finder for audi radios (probably also for vw, skoda, seat)
using ebay LCD shield(2x16) with 5 buttons on A0 
output (remote signal) is on pin 2

more info:
http://kovo-blog.blogspot.sk/2013/10/remote-control-audi-radio.html

5V logic idle state is HIGH (5V)

- start bit:	9ms LOW 4.55ms HIGH
- logic 0:	~600us LOW ~1700us HIGH
- logic 1: 	~600us LOW ~600us HIGH
- stop bit:	~600us LOW

I have MFSW without telephone option, so I read codes just for radio:

UP:       0x41E8D02F

DOWN: 0x41E850AF

LEFT:   0x41E840BF

RIGHT: 0x41E8C03F

VOL+:  0x41E8807F

VOL-:    0x41E800FF

code is always 0x41 0xE8 X 0xFF-X

discovered codes on my audi concert I unit:

0x00 	Volume down
0x01 	mem/cd1
0x03 	mem/cd3
0x05 	mem/cd5
0x07 	search up
0x09 	reg on/off
0x0B 	tp
0x11 	AM
0x15 	AS-STORE
0x17 	FM
0x1D 	search down
0x20 	AM->AM+AS->FM1->FM2->FM1+AS->FM2->AS/in CD mode “RD”
0x22 	AM->AM+AS->FM1->FM2->FM1+AS->FM2->AS/in CD mode “RD”
0x24 	AM->AM+AS->FM1->FM2->FM1+AS->FM2->AS/in CD mode “RD”
0x26 	AM->AM+AS->FM1->FM2->FM1+AS->FM2->AS/in CD mode “RD”
0x29 	TP
0x2B 	search up
0x3C 	seek up
0x40 	LEFT FM1 6-5-4-3-2-1-FM2 6-5-4-3-2-1
0x42 	LEFT FM1 6-5-4-3-2-1-FM2 6-5-4-3-2-1
0x44 	LEFT FM1 6-5-4-3-2-1-FM2 6-5-4-3-2-1
0x46 	LEFT FM1 6-5-4-3-2-1-FM2 6-5-4-3-2-1
0x50 	Seek down/FR
0x52 	Seek down/FR
0x54 	Seek down/FR
0x56 	Seek down/FR
0x60 	Seek down/FR
0x62 	seek down
0x64 	seek down
0x80 	Volume up
0x81 	Volume up
0x82 	Volume up
0x83 	Volume up
0x84 	Volume up
0x85 	Volume up
0x86 	Volume up
0x89 	-2 Volume down bas/treble down/fade rear/bal left
0x8B 	-4 Volume down bas/treble down/fade rear/bal left
0x8D 	-6 Volume down bas/treble down/fade rear/bal left
0x8F 	-8 Volume down/fade rear/bal left
0x97 	TP
0x9B 	SCAN
0xA0 	MODE
0xA2 	MODE
0xA4 	MODE
0xA6 	MODE
0xC0 	RIGHT FM1 6-5-4-3-2-1-FM2 6-5-4-3-2-1
0xC2 	RIGHT FM1 6-5-4-3-2-1-FM2 6-5-4-3-2-1
0xC4 	RIGHT FM1 6-5-4-3-2-1-FM2 6-5-4-3-2-1
0xC6 	RIGHT FM1 6-5-4-3-2-1-FM2 6-5-4-3-2-1
0xD0 	Seek up/FF
0xD2 	Seek up/FF
0xD4 	Seek up/FF
0xD6 	Seek up/FF
0xE0 	seek up
0xE2 	seek up
0xE4 	seek up
0xE6 	seek up

