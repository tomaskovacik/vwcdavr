#ifndef vwcdc_h
#define vwcdc_h

#include "Arduino.h"
#include "SPI.h"
#include "avr/interrupt.h" 

#define BYTES_DELAY 874
#define PACKET_DALEY 30

#define CDC_PREFIX1 0x53
#define CDC_PREFIX2 0x2C

#define CDC_END_CMD 0x14
#define CDC_END_CMD2 0x38
#define CDC_PLAY 0xE4
#define CDC_STOP 0x10
#define CDC_NEXT 0xF8
#define CDC_PREV 0x78
#define CDC_SEEK_FWD 0xD8
#define CDC_SEEK_RWD 0x58
#define CDC_CD1 0x0C
#define CDC_CD2 0x8C
#define CDC_CD3 0x4C
#define CDC_CD4 0xCC
#define CDC_CD5 0x2C
#define CDC_CD6 0xAC
#define CDC_SCAN 0xA0
#define CDC_SFL 0x60
#define CDC_PLAY_NORMAL 0x08
#define CDC_PREV_CD 0x18

#define MODE_PLAY 0xFF
#define MODE_SHFFL 0x55
#define MODE_SCAN 0x00

class cdc
{
	public:
		cdc(int DataOut_pin,boolean USESPI,int DataIn_pin,int Clk_pin);
		void setup();
		void init();
		int getCommand(long cmd);
		void send_package(int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7);
		void read_Data_out();
		int newcmd;
		long cmd;
	private:
		void myTransfer(int val);
		int cmdbit;
		int _DataOut_pin;
		int _DataIn_pin;
		int _Clk_pin;
		boolean _USESPI;
		int captimehi;
		int captimelo;
		int capturingstart;
		int capturingbytes;
};

#endif
