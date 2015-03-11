#include "Arduino.h"
#include "SPI.h"
#include "avr/interrupt.h" 

class cdc
{
	public:
		cdc(int DataOut_pin);
		void setup();
		void init();
		int getCommand(int cmd2);
		void send_package(int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7);
		static void read_Data_out();
static long cmd;
static int cmdbit;
static int newcmd;
static int prevcmd;
	private:
		static int _DataOut_pin;
		static int captimehi;
		static int captimelo;
		static int capturingstart;
		static int capturingbytes;
};
