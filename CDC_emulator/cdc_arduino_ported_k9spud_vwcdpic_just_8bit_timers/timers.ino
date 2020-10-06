void setup_timers(void){
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

//-----------------------------------------------------------------------------
/*!
  \brief     ISR(TIMER0_COMP_vect)
  on atmega8 overflow is used
  timer0 output compare interrupt service routine for cdc protocol
  radio display update
  \author     Koelling
  \date       04.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny167__)
ISR(TIMER0_COMPA_vect)
{
  TCNT0 = 0;
#else
ISR(TIMER2_COMPA_vect) //10us
{
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
    android_buttons();
#endif
  }
  if (counter_to_send_packet == 0)
  {
    counter_to_send_packet--; //make it -1 to stop counting..
    OutputByte();
  }
}

//-----------------------------------------------------------------------------
/*!
  \brief     ISR(TIMER1_OVF_vect)
  timer1 overflow interrupt service routine for cdc protocol
  \author     Koelling
  \date       26.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny167__)
ISR(TIMER1_OVF_vect)
#else
ISR(TIMER0_OVF_vect)
#endif
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

//-----------------------------------------------------------------------------
/*!
  \brief     ISR(INT0_vect)
  input capture interrupt service routine for cdc protocol
  \author     Koelling
  \date       26.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

#if defined(__AVR_ATtiny167__)
ISR(TIMER1_CAPT_vect)
#elif defined(__AVR_ATmega324__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__)
ISR(INT2_vect)
#else
ISR(INT0_vect)
#endif
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
