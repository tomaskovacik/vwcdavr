/*
 * VAG_CDC.c
 *
 * Created: 23.06.2013 20:00:51
 *  Author: Dennis Schuett, dev.shyd.de
 *  port for arduino by kovo
 */ 
//#include <SPI.h>

#define DataIn 11
#define ClockPin 13
#define SSPin 10
#define DataOut 2

#define PACKET_DALEY 50

#define UART_BAUD_RATE 9600
//#define LED_PWR PA0
#define RADIO_OUT PD2 //aka digital 2 on UNO
//#define FT_CBUS1 PD3
#define RADIO_OUT_IS_HIGH (PIND & (1<<RADIO_OUT))

#define CDC_PREFIX1 0x53
#define CDC_PREFIX2 0x2C

#define CDC_END_CMD 0x14
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
#define CDC_END_CMD2 0x38

#define MODE_PLAY 0xFF
#define MODE_SHFFL 0x55
#define MODE_SCAN 0x00

volatile uint16_t captimehi = 0;
volatile uint16_t captimelo = 0;
volatile uint8_t capturingstart = 0;
volatile uint8_t capturingbytes = 0;
volatile uint32_t cmd = 0;
volatile uint8_t cmdbit = 0;
volatile uint8_t newcmd = 0;

volatile uint8_t cd=1;
volatile uint8_t tr=1;
volatile uint8_t mode=MODE_PLAY;

volatile uint8_t c=0;
volatile uint8_t prev_c=0;
volatile long previousMillis=0;

uint8_t spi_xmit(uint8_t val);
void send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7);
uint8_t getCommand(uint32_t cmd);

ISR(INT0_vect) //remote signals
{
	if(RADIO_OUT_IS_HIGH)
	{
		if (capturingstart || capturingbytes)
		{
			captimelo = TCNT1;
		}
		else
			capturingstart = 1;
		TCNT1 = 0;
		
		//eval times
		if (captimehi > 16600 && captimelo > 7000)
		{
			capturingstart = 0;
			capturingbytes = 1;
			//uart_puts("startseq found\r\n");
		}
		else if(capturingbytes && captimelo > 3000)
		{
			//uart_puts("bit 1\r\n");
			cmd = (cmd<<1) | 0x00000001;
			cmdbit++;
		}
		else if (capturingbytes && captimelo > 1000)
		{
			//uart_puts("bit 0\r\n");
			cmd = (cmd<<1);
			cmdbit++;
		}
		else
		{
			//uart_puts("nothing found\r\n");
		}
		if(cmdbit == 32)
		{
			//uart_puts("new cmd\r\n");
			newcmd = 1;
			cmdbit = 0;
			capturingbytes = 0;
		}
	}
	else
	{
		captimehi = TCNT1;
		TCNT1 = 0;
	}
}

//ISR(INT1_vect) //ft230x cbus1
//{
//	if (PIND & (1<<FT_CBUS1)) //reset radio display to 'PLAY' CD01 TR00
//	{
//		//PORTA &= ~(1<<LED_PWR);
//		cd = 0xBE;
//		tr = 0xFF;
//		mode = 0xFF;
//	}
//	else //usb connect
//	{
//		PORTA |= (1<<LED_PWR);
//		_delay_ms(50);
//		PORTA &= ~(1<<LED_PWR);
//	}
//}

uint8_t spi_xmit(uint8_t val)
{
	SPDR = val;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

void myTransfer(uint8_t val){
//arduino SPI library
//SPI.transfer(val);
//software "spi"
//  for (uint8_t i = 0; i < 8; i++)  {
//    digitalWrite(ClockPin, HIGH);
//    digitalWrite(DataIn, !!(val & (1 << (7 - i))));
//    //delayMicroseconds(5);
//    digitalWrite(ClockPin, LOW);
//    //delayMicroseconds(5);
//  }
//hw spi by shyd
spi_xmit(val);
}

void send_package(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6, uint8_t c7)
{
  myTransfer(c0);
  delayMicroseconds(874);
  myTransfer(c1);
  delayMicroseconds(874);
  myTransfer(c2);
  delayMicroseconds(874);
  myTransfer(c3);
  delayMicroseconds(874);
  myTransfer(c4);
  delayMicroseconds(874);
  myTransfer(c5);
  delayMicroseconds(874);
  myTransfer(c6);
  delayMicroseconds(874);
  myTransfer(c7);
}

void setup()
{
	//LEDs
	//DDRA |= (1<<LED_PWR);
	
	//pullup
	//PORTD |= (1<<FT_CBUS1);
	
	//uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, 16000000));
	Serial.begin(UART_BAUD_RATE);

	//init SPI - no need for arduino SPI.h
        pinMode(DataIn,OUTPUT);
        pinMode(ClockPin,OUTPUT);
        pinMode(SSPin,OUTPUT);//SS output to enable spi

	// SPI Type: Master
	// SPI Clock Rate: 62,500 kHz
	// SPI Clock Phase: Cycle Start
	// SPI Clock Polarity: Low
	// SPI Data Order: MSB First
	SPCR=0x57;
	SPSR=0x00;

//arduino sPI library ... do not forget to uncomeny define SPI.h at top
//        SPI.begin();
//        SPI.setBitOrder(MSBFIRST);
//        SPI.setDataMode(SPI_MODE1);
//        SPI.setClockDivider(SPI_CLOCK_DIV128); //62.5kHz@8Mhz 125kHz@16MHz
        
        //beta commands -> cdc
        TCCR1A = 0;// set entire TCCR1A register to 0
        TCCR1B = 0;// same for TCCR1B
        TCNT1  = 0;//initialize counter value to 0;
	TCCR1B |= (1<<CS11);    // no prescaler 8 -> 1 timer clock tick is 0.5us long on 16Mhz
	EIMSK |= (1<<INT0);
	EICRA |= (1<<ISC00); //any change on INT0 and INT1
	sei();
	
	//init led on
	//PORTA |= (1<<LED_PWR);
	//delayMicroseconds(500);
	//uart_puts("VAG_CDC ready...\r\n");
	Serial.write(0xAA);
	Serial.write(0x55);
	//PORTA &= ~(1<<LED_PWR);
	
//	send_package(0x74,0xBE,0xFE,0xFF,0xFF,0xFF,0x8F,0x7C); //idle
//	delayMicroseconds(10);
//	send_package(0x34,0xFF,0xFE,0xFE,0xFE,0xFF,0xFA,0x3C); //load disc
//	delayMicroseconds(100);
//	send_package(0x74,0xBE,0xFE,0xFF,0xFF,0xFF,0x8F,0x7C); //idle
//	delayMicroseconds(10);
}


void loop()
{	
  if ((millis()-previousMillis)>PACKET_DALEY){
      send_package(0x34,0xBF^cd,0xFF^tr,0xFF,0xFF,mode,0xCF,0x3C);
    previousMillis=millis();
  }

  if (Serial.available() > 0) {

		int r = Serial.read();
		//r has new data
		if(r <= 0xFF)
		{
			//send CD No.
			if((r & 0xF0) == 0xC0)
			{
				if (r == 0xCA)
					mode = MODE_SCAN;
				else if (r == 0xCB)
					mode = MODE_SHFFL;
				else if (r == 0xCC)
					mode = MODE_PLAY;
				else
					cd = (r & 0x0F);
			}
			//send TR No.
			else
				tr = r;
		}
  }
  
		if (newcmd)
		{
			newcmd = 0;
			c = getCommand(cmd);
                        if (c)
                        {
                          if (c == CDC_END_CMD && prev_c == CDC_END_CMD2) //hack to allow next CD on audi concert1/chorus1 unit
                          {
                            
                            if (cd == 1)
                              Serial.write(CDC_CD2);
                            if (cd == 2)
                              Serial.write(CDC_CD3);
                            if (cd == 3)
                              Serial.write(CDC_CD4);
                            if (cd == 4)
                              Serial.write(CDC_CD5);
                            if (cd == 5)
                              Serial.write(CDC_CD6);
                            if (cd == 6)
                              Serial.write(CDC_CD1);
                              Serial.write(CDC_END_CMD2);
                          }
                          else if (c == CDC_END_CMD && prev_c == CDC_PREV_CD) //hack to allow previous CD on audi concert1/chorus1 unit
                          {
                            
                            if (cd == 1)
                              Serial.write(CDC_CD6);
                            if (cd == 2)
                              Serial.write(CDC_CD1);
                            if (cd == 3)
                              Serial.write(CDC_CD2);
                            if (cd == 4)
                              Serial.write(CDC_CD3);
                            if (cd == 5)
                              Serial.write(CDC_CD4);
                            if (cd == 6)
                              Serial.write(CDC_CD5);
                              Serial.write(CDC_END_CMD2);
                          }
			  else
			  {
                            Serial.write(c);
			  }
                          prev_c=c;
                          c=0;
                        }
		}
}

ISR(TIMER1_OVF_vect)
{
cmdbit=0;
}

// check captured bytes
uint8_t getCommand(uint32_t cmd)
{
	if (((cmd>>24) & 0xFF) == CDC_PREFIX1 && ((cmd>>16) & 0xFF) == CDC_PREFIX2)
		if (((cmd>>8) & 0xFF) == (0xFF^((cmd) & 0xFF)))
			return (cmd>>8) & 0xFF;
	return 0;
}
