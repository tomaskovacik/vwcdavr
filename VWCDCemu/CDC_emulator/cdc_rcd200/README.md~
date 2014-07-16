RCD200 CDC EMULATOR
===================

This is the code for *Arduino Uno R3 (uses _atmega328_)* RCD 200 (VW polo HU) CDC emulator. It basically emulates a CD changer and offers the ability to get input from GND, L+, R+ wires via a 3.5mm jack (that can be connected to a phone, mp3 player, etc). This is a currently unstable working version.


Working:

- CDC emulation


Bugs:

- HU sometimes returns NO CD / CHECK and the audio isn't played as there is no CD playing from the emulator. There is a workaround for this: when you get this message, stop and start your HU quite fast ( 1-2 seconds - tested ) and you will get the audio AUX playing instantly.


To Do:

- fix above bug
- finish reading the data from HU and offer some functions to process it via arduino / offer it to another microcontroller, device, via serial or SPI
- put pictures with connections and everything else involved
- translate the entire used protocol from the current big project to Panasonic protocol ( it seems it works best with this HU )



Further information and attachments
===================================

- RCD200 maker & details:


      Radio Code & Decoding Service
          
      VW RCD 200 Polo GP BVX
      6Q0 035 152
      Blaupunkt 7 644 235 360
          
      As originally fitted to the : 
          
      VW Polo
    
    
- Basically, after the information above, you can see the new information about the protocol in the ``cd_changer_protocol.pdf`` file in this folder.
