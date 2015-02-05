arduino code to emulate remote control on multifunction steering wheel (MFSW) on VW group cars to year 1999 maybe later 
(+ dumps of audi steering wheel controls to HU(radio) made by https://github.com/gillham/logic_analyzer)

blog: http://kovo-blog.blogspot.sk/2013/10/remote-control-audi-radio.html

5V LOGIC idle state is HIGH (5V)

- start bit:    9ms LOW 4.55ms HIGH
- logic 0:      ~600us LOW ~1700us HIGH
- logic 1:      ~600us LOW ~600us HIGH
- stop bit:     ~600us LOW


I have MFSW without telephone option, so I read codes just for radio:

UP:       0x41E8D02F
DOWN: 0x41E850AF
LEFT:   0x41E840BF
RIGHT: 0x41E8C03F
VOL+:  0x41E8807F
VOL-:    0x41E800FF

code is always 0x41 0xE8 X 0xFF-X
