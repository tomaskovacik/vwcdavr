#include <string.h>

#include "VWCDC.h"
#include "VWCDC_strings.h"

VWCDC *VWCDC::_instance = nullptr;

VWCDC::VWCDC(Stream &serial)
  : _serial(serial), sendreg(0), sendbitcount(0), disc(1), track(1), minute(0), second(0),
    scanptr(0), fsr(0), scanbyte(0), cmdcode(0), intwsave(0), intstatussave(0), intfsrsave(0),
    capbusy(0), mix(0), scan(0), playing(0), cd_button(0), mix_button(0), overflow(0), dataerr(0),
    _1us_delay(0), captime(0), captime_ovf(0), capbit(0), capbitpacket(0), capptr(0), BIDIstate(0),
    BIDIcount(0), ACKcount(0), discload(0), poweridentcount(0), secondcount(0), scancount(0),
    txinptr(0), txoutptr(0), counter_50ms(_50MS), counter_to_send_packet(_50MS), flag_50ms(false),
    display_byte_counter_u8(0)
#ifdef ANDROID_HEADPHONES
    , play_count(0)
#ifndef ANDROID_HEADPHONES_ONE_BUTTON
    , next_count(0), prev_count(0)
#endif
    , prev_count_delay(0)
#ifdef ANDROID_HEADPHONES_ONE_BUTTON
    , play_count_delay(0), play_count_push(0)
#endif
#endif
#ifdef DUMPMODE
    , startbit(0)
#endif
{
  memset(display_byte_buffer_mau8, 0, sizeof(display_byte_buffer_mau8));
  memset(txbuffer, 0, sizeof(txbuffer));
  memset(capbuffer, 0, sizeof(capbuffer));
  _instance = this;
}

void VWCDC::begin()
{
  initVWCDC();
}

void VWCDC::loop()
{
  cdcProtocol();
}

void VWCDC::setDisc(uint8_t d)
{
  disc = d;
}

void VWCDC::setTrack(uint8_t t)
{
  track = t;
}

void VWCDC::setPlaying(bool p)
{
  if (p)
  {
    setStatePlay();
  }
  else
  {
    setStateIdle();
  }
}

void VWCDC::setupTimers(void){
    //on arduino timer0 is used for millis(), we change prescaler, but also need to disable overflow interrupt
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
  TIMSK = 0x00;
#elif defined (__AVR_ATtiny167__)
  TIMSK0 = 0x00;
  TIMSK1 = 0x00;
#else
  TIMSK0 = 0x00;
#endif

  //Timing DataOut signal
  //attinx5 - > timer1
  //atmegax8 -> timer0
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
  TCCR1 = 0x00;
  TCNT1 = 0;
  TIMSK = 0x00;
  TCCR1 |= _BV(CS11) | _BV(CS10); //prescaler 4 @ 8MHz tick ever 0.5us but we have 8bit timer, need to catch overflows
  TIMSK |= _BV(TOIE1); //enable execution of overflow interrupt handling vector
#elif defined(__AVR_ATtiny167__) //digispark pro
  TCCR1A = 0x00; // Normal port operation, OC1A/OC1B/OC1C disconnected
  TCCR1B = _BV(ICNC1); // noise canceler, int on falling edge
  TCCR1B |= _BV(CS11); // prescaler = 8 -> 1 timer clock tick is 0.5µs long
#else //atmega chips .. timer0 is used for 100us ticks to time 700us between packet to head unit and for second (50ms mark)
  TCCR0A = 0x00;
  TCCR0B = 0x00;
  TCNT0 = 0;
  TIMSK0 = 0x00;
  TCCR0B |= _BV(CS01); //prescaler 8 @ 16MHz tick ever 0.5us but we have 8bit timer, need to catch overflows
  TIMSK0 |= _BV(TOIE0); //enable execution of overflow interrupt handling vector
#endif

  //100us timing
  //Timer 2 init (timer0 on attin85)
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
  TCCR0A = 0x00; // Normal port operation, OC0 disconnected
  TCCR0A |= _BV(WGM01); // CTC mode
  TCCR0B |= _BV(CS01);// prescaler = 8 -> 1 timer clock tick is 1us long
  OCR0A = 10;//run compare rutine every 10us;
  TCNT0 = 0;
  TIMSK |= _BV(OCIE0A); // enable output compare interrupt A on timer0
#elif defined(__AVR_ATtiny167__)
  TCCR0A = 0x00; // Normal port operation, OC0 disconnected
  TCCR0B = 0x00;
  TCCR0A |= _BV(WGM01); // CTC mode
  TCCR0B |= _BV(CS01);// prescaler = 8 -> 1 timer clock tick is 1us long
  OCR0A = 10;//run compare rutine every 10us;
  TCNT0 = 0;
  TIMSK0 |= _BV(OCIE0A); // enable output compare interrupt A on timer0
#else
  TCCR2A = 0x00; // Normal port operation, OC0 disconnected
  TCCR2B = 0x00; // Normal port operation, OC0 disconnected
  TCCR2A |= _BV(WGM21); // CTC mode
  TCCR2B |= _BV(CS21);// prescaler = 8 -> 1 timer clock tick is 0.5us long @ 16Mhz
  OCR2A = 20;//run compare rutine every 10us, 0.5x20
  TCNT2 = 0;
  TIMSK2 |= _BV(OCIE2A); // enable output compare interrupt A on timer0
#endif

  //HW input interrupt INT0 is used for capturing DATAOUT signal send from radio
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
  GIMSK |= _BV(INT0); //INT0 enable
  MCUCR |= _BV(ISC01);
  MCUCR &= ~_BV(ISC00);// falling edge fire interupt routine
#elif defined(__AVR_ATtiny167__)
  TIFR1 |= _BV(ICF1); // clear pending interrupt
  TIMSK1 |= _BV(ICIE1); // enable input capture interrupt on timer1
#elif defined(__AVR_ATmega324__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__)
  EIMSK |= _BV(INT2); //INT0 enable
  EICRA |= _BV(ISC21);
  EICRA &= ~_BV(ISC20);// falling edge fire interupt routine
#else
  EIMSK |= _BV(INT0); //INT0 enable
  EICRA |= _BV(ISC01);
  EICRA &= ~_BV(ISC00);// falling edge fire interupt routine
#endif

}

void VWCDC::handleTimer10us()
{
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny167__)
  TCNT0 = 0;
#else
  TCNT2 = 0;
#endif

  if(_1us_delay==1) _1us_delay--; //10us delay for timing output clock pulses
  counter_50ms--;
  if (counter_to_send_packet > 0) counter_to_send_packet--; //if we are under = we sending something out...
  if (counter_50ms == 0)
  {
    counter_50ms = _50MS;
    flag_50ms = TRUE;
#ifdef ANDROID_HEADPHONES
    androidButtons();
#endif
  }
  if (counter_to_send_packet == 0)
  {
    counter_to_send_packet--; //make it -1 to stop counting..
    outputByte();
  }
}

void VWCDC::handleTimerOVF()
{
  /*
    has 8bit timer, max value=255
    pulses are: 4500 us startbit = 9000 pulses = 35*255+75
    550 us logic 1  = 1100 pulses = 4*255+80
    1700 us logic 0  = 3400 pulses = 13*255+85
  */
#if !defined(__AVR_ATtiny167__)
  captime_ovf = captime_ovf + 0xFF;
#endif
  if (captime_ovf > 20000)//35*255= 8925, 65536 = REAL OVERFLOW
  {
    captime_ovf = 0;
    captime = 0;
    //disable timer:
    capbusy = FALSE;
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
    MCUCR |= _BV(ISC01);
    MCUCR &= ~_BV(ISC00);// change input capture to falling edge
    GIFR  |= _BV(INTF0); // clear input  interrupt request flag
#elif defined(__AVR_ATtiny167__)
    TIFR1 |= _BV(ICF1); // clear pending interrupt
    TIMSK1 |= _BV(ICIE1); // enable input capture interrupt on timer1
#elif defined(__AVR_ATmega324__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__)
    EICRA |= _BV(ISC21);
    EICRA &= ~_BV(ISC20);// change input capture to falling edge
    EIFR  |= _BV(INTF2); // clear input  interrupt request flag
#else
    EICRA |= _BV(ISC01);
    EICRA &= ~_BV(ISC00);// change input capture to falling edge
    EIFR  |= _BV(INTF0); // clear input  interrupt request flag
#endif
  }
}

void VWCDC::handleINT()
{
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
  captime = captime_ovf + TCNT1;
  TCNT1 = 0; //clear timer1
#elif defined(__AVR_ATtiny167__)
  captime = ICR1;
  TCNT1 = 0; //clear timer1
#else
  captime = captime_ovf + TCNT0;
  TCNT0 = 0; // clear timer0
#endif
  captime_ovf = 0;
  if ((RADIO_COMMAND_PIN & _BV(RADIO_COMMAND)) == 0)
  {
    // We have interrupted at beginning of low pulse (falling edge)
    // Low pulse length must be timed to determine bit value
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
    TIFR  |= _BV(TOV1); // clear timer1 overflow flag
    TIMSK |= _BV(TOIE1); // enable timer1 interrupt on overflow
    MCUCR |= _BV(ISC01) | _BV(ISC00); // change input capture to rising edge
    GIFR  |= _BV(INTF0); // clear input  interrupt request flag
#elif defined(__AVR_ATtiny167__)
    TIFR1 |= _BV(TOV1); // clear timer1 overflow flag
    TIMSK1 |= _BV(TOIE1); // enable timer1 interrupt on overflow
    TCCR1B |= _BV(ICES1); // change input capture to rising edge
    TIFR1 |= _BV(ICF1); // clear input capture interrupt request flag
#elif defined(__AVR_ATmega324__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__)
    TIFR0  |= _BV(TOV0); // clear time0 overflow flag
    TIMSK0 |= _BV(TOIE0); // enable timer0 interrupt on overflow
    EICRA |= _BV(ISC21) | _BV(ISC20); // change input capture to rising edge
    EIFR  |= _BV(INTF2); // clear input  interrupt request flag
#else
    TIFR0  |= _BV(TOV0); // clear time0 overflow flag
    TIMSK0 |= _BV(TOIE0); // enable timer0 interrupt on overflow
    EICRA |= _BV(ISC01) | _BV(ISC00); // change input capture to rising edge
    EIFR  |= _BV(INTF0); // clear input  interrupt request flag
#endif
  }
  else
  {
    // We have interrupted at beginning of high pulse (rising edge)
    // High pulse length doesn't matter. We need to check out
    // captured low pulse width if we are capturing data at the moment
    capbusy = TRUE;
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
    MCUCR |= _BV(ISC01);
    MCUCR &= ~_BV(ISC00);// change input capture to falling edge
    GIFR  |= _BV(INTF0); // clear input  interrupt request flag
    if (TIMSK & _BV(TOIE1)) // are we trying to capture data?
#elif defined(__AVR_ATtiny167__)
    TCCR1B &= ~_BV(ICES1); // change input capture to falling edge
    TIFR1 |= _BV(ICF1); // clear input capture interrupt request flag
    if (TIMSK1 & _BV(TOIE1)) // are we trying to capture data?
#elif defined(__AVR_ATmega324__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__)
    EICRA |= _BV(ISC21);
    EICRA &= ~_BV(ISC20);// change input capture to falling edge
    EIFR  |= _BV(INTF2); // clear input  interrupt request flag
    if (TIMSK0 & _BV(TOIE0)) // are we trying to capture data?
#else
    EICRA |= _BV(ISC01);
    EICRA &= ~_BV(ISC00);// change input capture to falling edge
    EIFR  |= _BV(INTF0); // clear input  interrupt request flag
    if (TIMSK0 & _BV(TOIE0)) // are we trying to capture data?
#endif
    {
      capbusy = TRUE;
#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
      TIMSK &= ~_BV(TOIE1); // turn off capturing time for high pulse
#elif defined(__AVR_ATtiny167__)
      TIMSK1 &= ~_BV(TOIE1); // turn off capturing time for high pulse
#else
      TIMSK0 &= ~_BV(TOIE0); // turn off capturing time for high pulse
#endif
      if (captime > STARTTHRESHOLD)
      { // yes, start bit
#ifdef DUMPMODE
        startbit = TRUE;
#endif
        capbitpacket = PKTSIZE;
        // don't store start bits, just frame around them
        if (capbit > -8) // are we already capturing on a blank byte?
        {
          dataerr = TRUE;
          // Note: This should never happen on normal head unit sending 32 bit
          //       command strings with error free data.
          //
          // if the capture bits were not a complete 8 bits, we need to finish
          // rotating the bits upward so that the data is nicely formatted
          while (capbit != 0) // have we finished rotating all bits up?
          {
            capbuffer[capptr] <<= 1; // rotate in 0 bit
            capbit++;
          }
          capbit = -8;
          capptr++; // move to new capture byte
          if (capptr == CAP_BUFFER_END) // have we gone past the end of the
          { // capture buffer?
            capptr = 0; // yes, roll over to beginning
          }
          if (capptr == scanptr) // have we overflowed the capture queue?
          {
            overflow = TRUE; // yes, set error flag
          }
        }
      }
      else
      { // no, just a regular data bit
        if (captime > LOWTHRESHOLD)
        { // yes, go ahead and store this data
          capbuffer[capptr] <<= 1; // nope
          if (captime > HIGHTHRESHOLD)
          {
            capbuffer[capptr] |= 1;
          }
          capbitpacket++;
          if (capbitpacket == 0)
          {
            // we've received PKTSIZE number of bits, so let's assume that we're done
            // capturing bits for now.
            capbusy = FALSE; // clear capture busy flag
          }
          capbit++;
          if (capbit == 0) // have we collected all 8 bits?
          { // yep, get ready to capture next 8 bits
            capbit = -8;
            capptr++; // move to new capture byte
            if (capptr == CAP_BUFFER_END) // have we gone past the end of the
            { // capture buffer?
              capptr = 0; // yes, roll over to beginning
            }
            if (capptr == scanptr) // have we overflowed the capture queue?
            {
              overflow = TRUE; // yes, set error flag
            }
          }
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/*!
  \brief    void scanCommandBytes(void)
  scanCommandBytes - Looks in the command receive buffer and tries
  to identify valid command codes.
  \author     Koelling
  \date       05.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::scanCommandBytes(void)
{
  fsr = scanptr;
FirstByteLoop:
  //printstrP(PSTR("1"),DEBUG);
  if (getCaptureByte() == FALSE)
  {
    return;
  }
FirstByteTest:
  //printstrP(PSTR("2"),DEBUG);
  if (scanbyte == 0x53)
  {
    goto SecondByte;
  }
  // this byte doesn't match the beginning of a normal command packet,
  enqueueHex(scanbyte);
  scanptr = fsr; // save scanptr, won't look at this byte again
  goto FirstByteLoop;
SecondByte:
  //printstrP(PSTR("3"),DEBUG);
  if (getCaptureByte() == FALSE)
  {
    return;
  }
  if (scanbyte == 0x2C) // verify that byte 2 is 0x2C)
  {
    goto ThirdByte;
  }
  // the first byte was a match, but the second byte failed.
  // dump first byte and then see if this one is the real first byte.
  enqueueHex(0x53);
  goto FirstByteTest;
ThirdByte:
  //printstrP(PSTR("4"),DEBUG);
  if (getCaptureByte() == FALSE)
  {
    return;
  }
  cmdcode = scanbyte; // save command code for later use.
FourthByte:
  //printstrP(PSTR("5"),DEBUG);
  if (getCaptureByte() == FALSE)
  {
    return;
  }
  // if execution reaches here, we have already verified that
  // bytes 1 and 2 are valid for a command packet.
  // verify that (Byte 3 + Byte 4) = 0xFF
  if ((cmdcode + scanbyte) == 0xFF)
  {
    //printstrP(PSTR("6"),DEBUG);
    if ((cmdcode & 0x03) == 0) // verify that Byte 3 is a multiple of 4
    {
      //printstrP(PSTR("7"),DEBUG);
      ACKcount = -4; // acknowledge command
      scanptr = fsr; // save scanptr, won't look at this byte again
      // Now, let's jump to the section of code that handles the
      // command we just received.
      decodeCommand();
      //printstrP(PSTR("\n"),DEBUG);
    }
    else
    {
      dumpFullCommand(); // ABORT: dump invalid packet for display
    }
  }
  else
  {
    dumpFullCommand(); // ABORT: dump invalid packet for display
  }
}
//-----------------------------------------------------------------------------
/*!
  \brief    void dumpFullCommand(void)
  dump all received command bytes
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::dumpFullCommand(void)
{
  fsr = scanptr; // restart back at the beginning of the packet
  if (getCaptureByte() == TRUE) // send byte 1
  {
    enqueueHex(scanbyte);
  }
  if (getCaptureByte() == TRUE) // send byte 2
  {
    enqueueHex(scanbyte);
  }
  if (getCaptureByte() == TRUE) // send byte 3
  {
    enqueueHex(scanbyte);
  }
  if (getCaptureByte() == TRUE) // send byte 4
  {
    enqueueHex(scanbyte);
  }
  enqueueString(sNEWLINE);
  scanptr = fsr; // save scanptr, won't look at this byte again
}
//-----------------------------------------------------------------------------
/*!
  \brief    uint8_t getCaptureByte(void)
  checks wether a command byte is still in queue
  \author     Koelling
  \date       05.10.2007
  \param[in]  none
  \param[out] none
  \return     FALSE ->	no more bytes to collect
  TRUE  -> scanbyte contains next byte
*/
//-----------------------------------------------------------------------------
uint8_t VWCDC::getCaptureByte(void)
{
  uint8_t return_u8 = FALSE;
  // have we already caught up with capturer?
  if (fsr != capptr)
  {
    scanbyte = capbuffer[fsr]; // get a byte from the capture buffer
    fsr++;
    if (fsr == CAP_BUFFER_END) // have we overflowed the
    { // capture buffer?
      fsr = 0;
    } // yes, roll over to beginning
    return_u8 = TRUE;
  }
  return return_u8;
}
//-----------------------------------------------------
// Display Update Packets
//-----------------------------------------------------
//-----------------------------------------------------------------------------
/*!
  \brief    void setStateIdle(void)
  Idle State
  74 BE FE FF FF FF 8F 7C
  74 BE FE FF FF FF 8F 7C
  ...
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
void VWCDC::setStateIdle(void)
{
  playing = FALSE;
  BIDIstate = StateIdle;
}
void VWCDC::setStateTP(void)
{
  playing = FALSE;
  BIDIstate = StateTP;
}
//-----------------------------------------------------------------------------
/*!
  \brief     setStateIdleThenPlay(void)
  Real CD Changer doesn't really do this, but we're gonna do it to try
  and make sure we unmute the audio even if the user didn't connect
  the PW-TX pin properly.
  \author     Koelling
  \date       27.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::setStateIdleThenPlay(void)
{
  playing = 0;
  BIDIstate = StateIdleThenPlay;
  BIDIcount = -20;
}
//-----------------------------------------------------------------------------
/*!
  \brief    void (void)
  set state to play mode
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
void VWCDC::setStatePlay(void)
{
  playing = TRUE;
  BIDIstate = StatePlay;
}
//-----------------------------------------------------------------------------
/*!
  \brief    void setStateInitPlay(void)
  Initiate Playing
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
void VWCDC::setStateInitPlay(void)
{
  playing = TRUE;
  BIDIstate = StateInitPlay;
  discload = 0xD1; //0xFF - 0x2E
  BIDIcount = -24;
}
//-----------------------------------------------------------------------------
/*!
  \brief    void setStatePlayLeadIn(void)
  34 BE FE FF FF FF AE 3C (play lead-in)
  34 2E ED DE AF B7 FF 3C
  34 BE FE FF FF FF AE 3C
  34 2E ED DE AF B7 FF 3C
  34 BE FE FF FF FF AE 3C
  34 2E ED DE AF B7 FF 3C
  34 BE FE FF FF FF AE 3C
  34 2E ED DE AF B7 FF 3C
  34 BE FE FF FF FF AE 3C
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
void VWCDC::setStatePlayLeadIn(void)
{
  playing = TRUE;
  BIDIstate = StatePlayLeadIn;
  BIDIcount = -10;
}
//-----------------------------------------------------------------------------
/*!
  \brief    void setStateTrackLeadIn(void)
  34BEFEFFEEFFCF3C (playing)
  n34BEFEFFEEFFCF3C
  14BEFDFFFFFFCF1C (ack)
  14BEFDFFFFFFAE1C (track lead in)
  14BEFDFFFFFFAE1C
  34BEFDFFFFFFAE3C
  34BEFDFFFFFFAE3C
  34BEFDFFFFFFAE3C
  34BEFDFFFFFFAE3C
  34BEFDFFFFFFAE3C
  34BEFDFFFFFFAE3C
  34BEFDFFFFFFAE3C
  34BEFDFFFFFFAE3C
  34BEFDFFFFFFAE3C
  34BEFDFFFFFFAE3C
  34BEFDFFFFFFCF3C (playing)
  34BEFDFFFFFFCF3C
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
void VWCDC::setStateTrackLeadIn(void)
{
  playing = TRUE;
  BIDIstate = StateTrackLeadIn;
  BIDIcount = -12;
}
// TODO: We might implement one more state machine for
// the CHANGECD/INQUIRY command. (mute byte goes 0x6F and 0xFF cd load
// while changer is busy motoring next CD into position). Then
// again, maybe we don't need to implement any busy states since
// we are instantly ready (no motoring here!).
// =========================================================================
// SEND DISPLAY UPDATE PACKETS
// =========================================================================
//-----------------------------------------------------------------------------
/*!
  \brief    void sendDisplayBytes(void)
  send display bytes to head unit
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
void VWCDC::sendDisplayBytes(void)
{
  sendByte(disc); // disc display value
  sendDisplayBytesNoCD();
}
//-----------------------------------------------------------------------------
/*!
  \brief    void sendDisplayBytesNoCD(void)
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
void VWCDC::sendDisplayBytesNoCD(void)
{
  uint8_t send_byte_u8 = 0;
  sendByte(track);
  sendByte(minute);
  sendByte(second);
  // D4 - scan on, mix on
  // D0 - scan on, mix off
  // 04 - scan off, mix on
  // 00 - scan off, mix off
  if (mix == TRUE) // mode (scan/mix)
  {
    send_byte_u8 |= 0x20; // turn on mix light
  }
  if (scan == TRUE)
  {
    send_byte_u8 |= 0x10; // turn on scan display
  }
  sendByte(send_byte_u8);
}
//-----------------------------------------------------------------------------
/*!
  \brief    void sendDisplayBytesInitCD(void)
  When sending an "init cd" packet, we need to send it the number of
  tracks and whatnot available on the CD. Required on Audi Concert II so
  that track up/dn buttons work.
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
void VWCDC::sendDisplayBytesInitCD(void)
{
  sendByte(0x99); // number of tracks total (99)?
  sendByte(0x99); // total minutes?
  sendByte(0x59); // total seconds?
  sendByte(0x49);//0xFF - 0xB7 = 48, 53, 31, 25, and 37 seen from real CDC,
  // no idea what it really means.
}
//-----------------------------------------------------------------------------
/*!
  \brief    void sendFrameByte(uint8_t byte_u8)
  sendFrameByte - sends a framing byte to head unit (first and last bytes).
  If the ACK flag is set, we modify the send byte to send an
  acknowledgement.
  \author     Koelling
  \date       06.10.2007
  \param[in]  byte_u8 -> byte to send
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
void VWCDC::sendFrameByte(uint8_t byte_u8)
{
  if (ACKcount == 0)
  {
    sendByte(byte_u8);
  }
  else
  {
    byte_u8 |= 0x20; // flag acknowledgement
    ACKcount++;
    sendByte(byte_u8);
  }
}
//-----------------------------------------------------------------------------
/*!
  \brief    void sendFrameByte(uint8_t byte_u8)
  sendByte - sends a byte to head unit.
  \author     Koelling
  \date       06.10.2007
  \param[in]  byte_u8 -> byte to send
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
void VWCDC::sendByte(uint8_t byte_u8)
{
  static uint8_t display_byte_counter_u8 = 0;
  // wait for head unit to store sent byte
  // 335us didn't work so good on late 2003 wolfsburg double din,
  // so we now wait 700us instead.
  display_byte_buffer_mau8[display_byte_counter_u8] = byte_u8;
  display_byte_counter_u8++;
  if (display_byte_counter_u8 == 8)
  {
    display_byte_counter_u8 = 0;
  }
}
//-----------------------------------------------------------------------------
/*!
  \brief     void enqueueString(const uint8_t addr PROGMEM)
  enqueueString - Adds a new string pointer into the outgoing serial string
  queue.
  \author     Koelling
  \date       02.10.2007
  \param[in]  const uint8_t addr PROGMEM -> start address of string to display
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::enqueueString(const uint8_t *addr)
{
#ifndef JUST_HEX_TO_SERIAL
  txbuffer[txinptr] = addr;
  txinptr++;
  if (txinptr == TX_BUFFER_END)
  {
    txinptr = 0;
  }
#endif
}
//-----------------------------------------------------------------------------
/*!
  \brief     void enqueueHex(uint8_t hexbyte_u8)
  The byte is converted to a two byte ASCII hexidecimal string
  \author     Koelling
  \date       05.10.2007
  \param[in]  uint8_t hexbyte -> hexbyte to display
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::enqueueHex(uint8_t hexbyte_u8)
{
  uint8_t nibble_u8;
  nibble_u8 = hexbyte_u8 >> 4; // send high nibble first
  nibble_u8 <<= 1; // multiply high nibble by 2
  enqueueString(&sHEX[nibble_u8]);
  nibble_u8 = hexbyte_u8 & 0x0F; // prepare low nibble
  nibble_u8 <<= 1; // multiply low nibble by 2
  enqueueString(&sHEX[nibble_u8]);
}
//-----------------------------------------------------------------------------
/*!
  \brief     resetTime(void)
  reset time information
  \author     Koelling
  \date       27.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::resetTime(void)
{
  secondcount = SECONDWAIT;
  second = 0;
  minute = 0;
}
//-----------------------------------------------------------------------------
/*!
  \brief     void sendStateIdle(void)
  send data for idle state
  \author     Koelling
  \date       29.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::sendStateIdle(void)
{
  secondcount = SECONDWAIT; // stop display from ticking time
  sendFrameByte(0x8B);//FF - 0x74
  sendDisplayBytes();
  sendByte(0x70);//FF - 0x8F, mutes audio on Monsoon head units
  sendFrameByte(0x83);//FF - 0x7C
}
void VWCDC::sendStateTP(void)
{ //B4 BE EF FE DB FF DF BC
  secondcount = SECONDWAIT; // stop display from ticking time
  sendFrameByte(0x4B);//FF - 0x4b
  sendDisplayBytes();
  sendByte(0x20);
  sendFrameByte(0x43);//FF - 0x7C
}
//-----------------------------------------------------------------------------
/*!
  \brief     sendStatePlayLeadInEnd(void)
  send data for state PlayLeadInEnd
  \author     Koelling
  \date       29.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::sendStatePlayLeadInEnd(void)
{
  sendFrameByte(0xC3);//FF - 0x3C
  BIDIcount++;
  if (BIDIcount == 0)
  {
    setStatePlay();
  }
}
//-----------------------------------------------------------------------------
/*!
  \brief     sendStateInitPlayEnd(void)
  send data for state StateInitPlayEnd
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::sendStateInitPlayEnd(void)
{
  sendFrameByte(0xC3);//FF - 0x3C
  BIDIcount++;
  if (BIDIcount == 0)
  {
    setStatePlayLeadIn();
  }
}
//-----------------------------------------------------------------------------
/*!
  \brief     sendStateInitPlayAnnounceCD(void)
  send data for state StateInitPlayAnnounceCD
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::sendStateInitPlayAnnounceCD(void)
{
  // 0xF6..0xF0: CD-ROM Loaded (seen on changer)
  // 0xE6..0xE0: CD-ROM Loaded. (made up)
  // 0x96..0x90: Slot Empty (seen on changer)
  // 0x86..0x80: Slot Empty (made up)
  // 0xD6..0xD0: AUDIO CD Loaded. (seen on changer)
  sendByte(discload);
  if (discload == 0xD6)
  {
    discload = 0xD1;
  }
  else
  {
    discload++;
  }
  sendDisplayBytesInitCD();
  sendByte(0x00);//0xFF - 0xFF
  sendStateInitPlayEnd();
}
//-----------------------------------------------------------------------------
/*!
  \brief     sendStatePlayLeadInAnnounceCD(void)
  send data for state StatePlayLeadInAnnounceCD
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::sendStatePlayLeadInAnnounceCD(void)
{
  sendByte((disc & 0x0F) | 0xD0);
  sendDisplayBytesInitCD();
  sendByte(0x00);//0xFF - 0xFF
  sendStatePlayLeadInEnd();
}
//-----------------------------------------------------------------------------
/*!
  \brief     void sendPacket(void)
  depending on BIDIstate data packet will be sent
  \author     Koelling
  \date       27.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
void VWCDC::sendPacket(void)
{
  switch (BIDIstate) {
    case StateTP:
      sendStateTP();
      break;
    case StateIdle:
      sendStateIdle();
      break;
    case StateIdleThenPlay:
      BIDIcount++;
      if (BIDIcount == 0)
      {
        setStateInitPlay();
        sendStateIdle();
      }
      else
      {
        sendStateIdle();
      }
      break;
    // 34 2E ED DE AF B7 FF 3C
    // 34 BE FE FF FF FF EF 3C
    // 34 2D EB BE AB AC FF 3C
    // 34 BE FE FF FF FF EF 3C
    // 34 2C EC CE AA CE FF 3C
    // 34 BE FE FF FF FF EF 3C
    // 34 2B EE EE B7 DA FF 3C
    // 34 BE FE FF FF FF EF 3C
    // 34 2A EB BE A6 C8 FF 3C
    // 34 BE FE FF FF FF EF 3C
    // 34 69 00 FF FF FF FF 3C
    // 34 BE FE FF FF FF EF 3C
    case StateInitPlay:
      secondcount = SECONDWAIT; // stop display from ticking time
      sendFrameByte(0xCB);//0xFF - 0x34
      if ((BIDIcount & 0x01) == 0)
      {
        sendStateInitPlayAnnounceCD();
        break;
      }
      sendDisplayBytes();
      sendByte(0x10);//0xFF - 0xEF
//no break here!
    case StateInitPlayEnd:
      sendStateInitPlayEnd();
      break;
    case StateInitPlayAnnounceCD:
      sendStateInitPlayAnnounceCD();
      break;
    case StatePlayLeadIn:
      // 34 BE FE FF FF FF AE 3C (play lead-in)
      // 34 2E ED DE AF B7 FF 3C
      // 34 BE FE FF FF FF AE 3C
      // 34 2E ED DE AF B7 FF 3C
      // 34 BE FE FF FF FF AE 3C
      // 34 2E ED DE AF B7 FF 3C
      // 34 BE FE FF FF FF AE 3C
      // 34 2E ED DE AF B7 FF 3C
      // 34 BE FE FF FF FF AE 3C
      secondcount = SECONDWAIT; // stop display from ticking time
      sendFrameByte(0xCB);//0xFF - 0x34
      if ((BIDIcount & 0x01) == 0)
      {
        sendStatePlayLeadInAnnounceCD();
        break;
      }
      sendDisplayBytes();
      sendByte(0x51);//0xFF - 0xAE
//no break here!
    case StatePlayLeadInEnd:
      sendStatePlayLeadInEnd();
      break;
    case StatePlayLeadInAnnounceCD:
      sendStatePlayLeadInAnnounceCD();
      break;
    case StateTrackLeadIn:
      secondcount = SECONDWAIT; // stop display from ticking time
      sendFrameByte(0xCB);//0xFF - 0x34
      sendDisplayBytes();
      sendByte(0x51);//0xFF - 0xAE
      sendFrameByte(0xC3);//0xFF - 0x3C
      BIDIcount++;
      if (BIDIcount == 0)
      {
        break;
      }
      setStatePlay();
      break;
    case StatePlay:
      sendFrameByte(0xCB);//0xFF - 0x34
      sendDisplayBytes();
      sendByte(0x30);//0xFF - 0xCF
      sendFrameByte(0xC3);//FF - 0x3C
      break;
    default:
      break;
  }
}
void VWCDC::androidButtons() {
#ifdef ANDROID_HEADPHONES
  //android headphone control, this is fired every 50ms
  //play button
  if (play_count > 0) {
    _serial.println("play");
    _serial.println(play_count);
    _serial.println(next_count);
    _serial.println(prev_count);
    play_count--;
    ANDROID_PLAY_PORT |= (1 << ANDROID_PLAY); //high
  } else {
    ANDROID_PLAY_PORT &= ~_BV(ANDROID_PLAY); //low
#ifdef ANDROID_HEADPHONES_ONE_BUTTON
    if (play_count_delay > 0 ) { //counting delay low
      play_count_delay--;
      if (play_count_delay == 0 && play_count_push > 0) {
        play_count = ANDROID_PUSH_COUNT;
        play_count_delay = ANDROID_DELAY_COUNT;
        if (play_count_push > 0) play_count_push--;
      }
    }
#endif
  }
#ifndef ANDROID_HEADPHONES_ONE_BUTTON
  //next button
  if (next_count > 0) {
    _serial.println("next");
    _serial.println(play_count);
    _serial.println(next_count);
    _serial.println(prev_count);
    next_count--;
    ANDROID_NEXT_PORT |= _BV(ANDROID_NEXT); //high
  } else {
    ANDROID_NEXT_PORT &= ~_BV(ANDROID_NEXT); //low
  }
  //prev button, double push, head unit goes to previous song, no to start of the song
  if (prev_count > 0) {
    _serial.println("prev");
    _serial.println(play_count);
    _serial.println(next_count);
    _serial.println(prev_count);
    prev_count--;
    ANDROID_PREV_PORT |= _BV(ANDROID_PREV); //high
  } else {
    //wait between pushes
    ANDROID_PREV_PORT &= ~_BV(ANDROID_PREV); //low
    if (prev_count_delay > 0 ) { //we ended first push and do not finished second push
      prev_count_delay--;
      if (prev_count_delay == 0) //we are at last run of delay loop
        prev_count = ANDROID_PUSH_COUNT;
    }
  }
#endif
#endif
}
void VWCDC::printstrP(const char *s)
{
  char c;
  for (c = pgm_read_byte(s); c; ++s, c = pgm_read_byte(s))
  {
    _serial.print(c);
    if (c == '\n')
      break;
  }
}

//-----------------------------------------------------------------------------
/*!
  \brief    outputByte //was: ISR(TIMER2_COMPA_vect)
  runs every 700us Timer2 ensures 700µs timing between display package bytes
  Shift bytes out to head unit
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------

void VWCDC::outputByte(void)
{
  static uint8_t display_byte_counter_u8 = 0;
  uint8_t byte_u8;
  if (display_byte_counter_u8 < 8)
  {
    byte_u8 = display_byte_buffer_mau8[display_byte_counter_u8];

#ifdef DUMPMODE2
    _serial.print("|");
    _serial.print(byte_u8, HEX);
    _serial.print("|");
#endif

    for (sendbitcount = -8; sendbitcount != 0; sendbitcount++)
    {
      RADIO_CLOCK_PORT |= _BV(RADIO_CLOCK); // SCLK high
      //_delay_loop_1(CLK_DELAY);
      _1us_delay=1;
      while(_1us_delay);
      if ((byte_u8 & 0x80) == 0) // mask highest bit and test if set
      {
        RADIO_DATA_PORT |= _BV(RADIO_DATA); // DATA high
      }
      else
      {
        RADIO_DATA_PORT &= ~_BV(RADIO_DATA); // DATA low
      }
      byte_u8 <<= 1; // load the next bit
      RADIO_CLOCK_PORT &= ~_BV(RADIO_CLOCK); // SCLK low
      //_delay_loop_1(CLK_DELAY);
      _1us_delay=1;
      while(_1us_delay);      
    }
  }
  counter_to_send_packet = _700US;
  display_byte_counter_u8++;
  if (display_byte_counter_u8 == 8)
  { //wait 50ms
    display_byte_counter_u8 = 0;
    counter_to_send_packet = _50MS;
  }
}

//-----------------------------------------------------------------------------
/*!
  \brief     initVWCDC
  initialization for cdc protocol
  \author     Koelling
  \date       26.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------


void VWCDC::initVWCDC(void)
{
#ifdef DIGISPARK
  clock_prescale_set(clock_div_2);
#endif

  cli();

  RADIO_CLOCK_DDR |= _BV(RADIO_CLOCK);
  RADIO_DATA_DDR  |= _BV(RADIO_DATA);
  RADIO_COMMAND_DDR &= ~_BV(RADIO_COMMAND); // input capture as input
  RADIO_COMMAND_PORT |= _BV(RADIO_COMMAND); // enable pull up

#ifdef ANDROID_HEADPHONES
  ANDROID_PLAY_DDR |= _BV(ANDROID_PLAY);
  ANDROID_NEXT_DDR |= _BV(ANDROID_NEXT);
  ANDROID_PREV_DDR |= _BV(ANDROID_PREV);
  ANDROID_PREV_PORT &= ~_BV(ANDROID_PREV);
  ANDROID_PLAY_PORT &= ~_BV(ANDROID_PLAY);
  ANDROID_NEXT_PORT &= ~_BV(ANDROID_NEXT);
#endif

  setupTimers();

  capptr = 0; // indirect pointer to capture buffer
  scanptr = 0;
  capbit = -8;
  txinptr = 0; // queue pointers
  txoutptr = 0;

  capbusy = FALSE; // reset flags
  mix = FALSE;
  scan = FALSE;
  playing = FALSE;
  overflow = FALSE;
  dataerr = FALSE;

#ifdef DUMPMODE
  startbit = FALSE;
#endif
  ACKcount = 0;

  // these values can be set depending on the state of mp3
  // it has to be evaluated wether CD number can be grater than 6
  disc = 0x41; // CD 1
  track = 0x01 ; // track 1
  poweridentcount = POWERIDENTWAIT;

  resetTime();
  setStateIdleThenPlay();
  enqueueString(sIDENTIFY);
  enqueueString(sVERSION);
  enqueueString(sNEWLINE);
  enqueueString(sRING);
  sendPacket(); // force first display update packet
  sei();
}



//-----------------------------------------------------------------------------
/*!
  \brief     cdcProtocol(void)
  cyclic called main program for cdc protocol (50ms?)
  \author     Koelling
  \date       26.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

void VWCDC::cdcProtocol(void)
{
  uint8_t decimal_adjust_u8;
  if (flag_50ms == TRUE)
  {
    flag_50ms = FALSE;
    sendPacket();
    scancount++;
    if (scancount == 0)
    {
      scancount = SCANWAIT;
      scan = FALSE; // turn off scan display
    }
    secondcount++;
    if (secondcount == 0)
    {
      secondcount = SECONDWAIT;
      poweridentcount++;
      if (poweridentcount == 0)
      {
        poweridentcount = POWERIDENTWAIT;
        enqueueString(sIDENTIFY);
        enqueueString(sVERSION);
        enqueueString(sNEWLINE);
      }
      second++; // increment the time display
      decimal_adjust_u8 = second & 0x0F; // skip past hexidecimal codes
      if (decimal_adjust_u8 == 0x0A) // are with at xA?
      {
        second += 6; // yes, add 6 and we'll be at x0 instead
      }
      if (second == 0x60)
      {
        second = 0;
        minute++;
        decimal_adjust_u8 = minute & 0x0F; // skip past hexidecimal codes
        if (decimal_adjust_u8 == 0x0A) // are with at xA?
        {
          minute += 6; // yes, add 6 and we'll be at x0 instead
        }
        if (minute == 0xA0) // have we gone beyond 99 minutes?
        {
          minute = 0;
        }
      }
    }
  }
  if (overflow == TRUE) // has the command receive code detected
  { // an overflow error?
    overflow = FALSE; // clear error flag
    enqueueString(sOVERFLOW);
  }
  if (dataerr == TRUE) // has the command receive code detected
  { // a framing type data error?
    dataerr = FALSE; // clear error flag
    enqueueString(sDATAERR);
  }
#ifndef DUMPMODE
  scanCommandBytes();
#else
  if (startbit == TRUE) // have we just recieved a start bit?
  {
    startbit = FALSE;
    enqueueString(sNEWLINE); // yes, start a new line
  }
  fsr = scanptr;
  while (getCaptureByte() == TRUE)
  {
    scanptr = fsr;
    enqueueHex(scanbyte);
  }
#endif

  while (txoutptr != txinptr)
  {
    printstrP((char*) txbuffer[txoutptr]);
    txoutptr++;
    if (txoutptr == TX_BUFFER_END)
    {
      txoutptr = 0;
    }
  }
}

//-----------------------------------------------------------------------------
/*!
  \brief    void decodeCommand(void)
  decode cmdcode and do required actions
  ;--------------------------------------------------------------------------
  ; Button Push Packets
  ;--------------------------------------------------------------------------
  ; 532C609F Mix 1
  ; 532CE01F Mix 6
  ; 532CA05F Scan
  ;     Note: Blaupunkt Gamma V head unit will continue to send scan key code
  ;       unless display is switched into scan mode.
  ;       (reported by tony.gilbert@orange.co.uk)
  ; 532C10EF Head Unit mode change. Emitted at power up, power down, and
  ;        any mode change. (disable playing)
  ; 532C58A7 Seek Back Pressed
  ; 532CD827 Seek Forward Pressed
  ; 532C7887 Dn
  ; 532CA857 Dn on Mk3 premium (Adam Yellen <adam@yellen.com>)
  ; 532CF807 Up
  ; 532C6897 Up on Mk3 premium (Adam Yellen)
  ; 532C38C7 CD Change (third packet)
  ; 532CE41B Seek Forward Released (enable playing)
  ; 532CE41B Seek Back Released (enable playing)
  ; 532CE41B CD Mode selected. Emitted at power up (if starting in CD
  ;            mode), change to CD mode. (enable playing)
  ; 532C14EB CD Change (second packet)
  ; 532C0CF3 CD 1 (first packet)
  ; 532C8C73 CD 2 (first packet)
  ; 532C4CB3 CD 3 (first packet)
  ; 532CCC33 CD 4 (first packet)
  ; 532C2CD3 CD 5 (first packet)
  ; 532CAC53 CD 6 (first packet)
  ;
  ; Monsoon State Changes:
  ; 532CE41B enable playing (transition to State 2)
  ; 532C38C7 disc loaded inquiry (transition to State 5)
  ; 532C10EF disable playing (transition to State 1)
  ;--------------------------------------------------------------------------
  \author     Koelling
  \date       05.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

void VWCDC::decodeCommand(void)
{
  uint8_t decimal_adjust_u8 = 0;
#ifdef JUST_HEX_TO_SERIAL
  _serial.write(cmdcode);
#endif
  switch (cmdcode) {
    case Do_CHANGECD:
      // Head unit seems to send this after each CDx number change
      // but the CD Changer seems to completely ignore (doesn't even ACK it).
      ACKcount = 0; // do not ack this command
#ifdef PJRC
      enqueueString(sRANDOM);
#endif
      break;

    case Do_ENABLE:
    case Do_ENABLE_MK:
      mix = FALSE;
      if (playing == FALSE)
      {
        setStateInitPlay(); // skip this if already playing
      }
      if (!mix_button)
        enqueueString(sMENABLE);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_LOADCD:
      if (playing == TRUE)
      {
        setStateInitPlay(); // skip this if we're in idle mode
      }
      resetTime();
      enqueueString(sMINQUIRY);
      break;

    case Do_DISABLE:
      setStateIdle(); // skip this if we're already in idle mode
      enqueueString(sMDISABLE);
      break;

    case Do_SEEKBACK:
    case Do_PREVCD:
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc--;
      track = 1;
      resetTime();
      if ((disc & 0x0F) == 0)
      {
        disc = 0x46; // set back to CD 1
      }
#endif
      enqueueString(sPRV_LIST);
      break;

    case Do_SEEKFORWARD:
    case Do_SEEKFORWARD_MK:
      if (cd_button == FALSE) // mk don't increment when previous command was a cd button
      {
        enqueueString(sNXT_LIST);
#ifndef DISC_TRACK_NUMBER_FROM_MPD
        resetTime();
        track = 1;
        disc++;
        if (disc > 0x46)
        {
          disc = 0x41;
        }
#endif
        // Going beyond CD9 displays hex codes on premium head unit.
        // Examples: "CD A"
        //           "CD B"
        //           "CD C" etc...
        //
        // However, going beyond CD6 mutes audio on monsoon head unit, so we
        // definitely don't want to do that.
      }
      else
      {
        cd_button = FALSE; // mk clear cd button flag
      }
      break;

    case Do_MIX:
    case Do_MIX_CD:
      mix_button = 1;
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      if (mix == FALSE)
      {
        mix = TRUE;
      }
      else
      {
        mix = FALSE;
      }
#endif
      enqueueString(sRANDOM);
      break;

    case Do_PLAY:
      enqueueString(sPLAY); // this will make the PJRC play/pause
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_SCAN:
      scancount = SCANWAIT;
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      if (scan == FALSE)
      {
        scan = TRUE;
      }
      else
      {
        scan = FALSE;
      }
#endif
#ifdef PJRC
      enqueueString(sPLAY); // this will make the PJRC play/pause
#else
      enqueueString(sSCAN); //
#endif
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_UP:
    case Do_UP_MK3:
      if (playing == TRUE) // skip track lead-in if not in play mode
      {
        setStateTrackLeadIn();
      }
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      track++;
      resetTime();
      decimal_adjust_u8 = track & 0x0F; // skip past hexidecimal codes
      if (decimal_adjust_u8 == 0x0A) // are with at xA?
      {
        track += 6; // yes, add 6 and we'll be at x0 instead
      }
      if (track == 0xA0) // have we gone beyond Track 99?
      { // yes, rollover to Track 01 so that jog wheels
        track = 1; // can continue rolling (Audi Concert II)
      }
#endif
      enqueueString(sNEXT);
#ifdef ANDROID_HEADPHONES
#ifdef ANDROID_HEADPHONES_ONE_BUTTON
      play_count_delay = ANDROID_DELAY_COUNT;
      play_count_push = ANDROID_NEXT_COUNT;
#else
      next_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_NEXT pin
#endif
#endif
      break;

    case Do_DOWN:
    case Do_DOWN_MK3:
      if (playing == TRUE) // skip track lead-in if not in play mode
      {
        setStateTrackLeadIn();
      }
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      decimal_adjust_u8 = track & 0x0F; // skip past hexidecimal codes
      if (decimal_adjust_u8 == 0) // are we at x0?
      {
        track -= 6; // yes, subtract 6 and we'll be at x9 instead
      }
      track--;
      resetTime();
      if (track == 0) // have we gone below Track 1?
      { // yes, rollover to Track 99 so that jog wheels
        track = 0x99; // can continue rolling (Audi Concert II)
      }
#endif
      enqueueString(sPREVIOUS);
#ifdef ANDROID_HEADPHONES
#ifdef ANDROID_HEADPHONES_ONE_BUTTON
      play_count_delay = ANDROID_DELAY_COUNT;
      play_count_push = ANDROID_PREV_COUNT;
#else
      prev_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PREV pin
      prev_count_delay = ANDROID_DELAY_COUNT; // 50ms high on ANDROID_PREV pin
#endif
#endif
      break;

    case Do_CD1:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x41; // set CD 1
      resetTime();
#endif
      enqueueString(sLIST1);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_CD2:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x42; // set CD 2
      resetTime();
#endif
      enqueueString(sLIST2);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_CD3:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x43; // set CD 3
      resetTime();
#endif
      enqueueString(sLIST3);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_CD4:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x44; // set CD 4
      resetTime();
#endif
      enqueueString(sLIST4);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_CD5:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x45; // set CD 5
      resetTime();
#endif
      enqueueString(sLIST5);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_CD6:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x46; // set CD 6
      resetTime();
#endif
      enqueueString(sLIST6);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_TP:
      if (playing == TRUE) {
        setStateTP();
      } else {
        setStateInitPlay();
      }
      enqueueString(sTP);
      break;

    default:
      /* if execution reaches here, we have verified that we got
         a valid command packet, but the command code received is not
         one that we understand.
         Dump the unknown command code for the user to view.
      */
      enqueueString(sDASH);
      enqueueHex(cmdcode);
      enqueueString(sNEWLINE);
      break;
  }
}

// --- ISR wrappers ---

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny167__)
ISR(TIMER0_COMPA_vect) {
#else
ISR(TIMER2_COMPA_vect) {
#endif
  if (VWCDC::_instance) VWCDC::_instance->handleTimer10us();
}

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny167__)
ISR(TIMER1_OVF_vect) {
#else
ISR(TIMER0_OVF_vect) {
#endif
  if (VWCDC::_instance) VWCDC::_instance->handleTimerOVF();
}

#if defined(__AVR_ATtiny167__)
ISR(TIMER1_CAPT_vect) {
#elif defined(__AVR_ATmega324__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__)
ISR(INT2_vect) {
#else
ISR(INT0_vect) {
#endif
  if (VWCDC::_instance) VWCDC::_instance->handleINT();
}
