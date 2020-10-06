//-----------------------------------------------------------------------------
/*!
  \brief    void ScanCommandBytes(void)
  ScanCommandBytes - Looks in the command receive buffer and tries
  to identify valid command codes.
  \author     Koelling
  \date       05.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void ScanCommandBytes(void)
{
  fsr = scanptr;
FirstByteLoop:
  //printstr_p(PSTR("1"),DEBUG);
  if (GetCaptureByte() == FALSE)
  {
    return;
  }
FirstByteTest:
  //printstr_p(PSTR("2"),DEBUG);
  if (scanbyte == 0x53)
  {
    goto SecondByte;
  }
  // this byte doesn't match the beginning of a normal command packet,
  EnqueueHex(scanbyte);
  scanptr = fsr; // save scanptr, won't look at this byte again
  goto FirstByteLoop;
SecondByte:
  //printstr_p(PSTR("3"),DEBUG);
  if (GetCaptureByte() == FALSE)
  {
    return;
  }
  if (scanbyte == 0x2C) // verify that byte 2 is 0x2C)
  {
    goto ThirdByte;
  }
  // the first byte was a match, but the second byte failed.
  // dump first byte and then see if this one is the real first byte.
  EnqueueHex(0x53);
  goto FirstByteTest;
ThirdByte:
  //printstr_p(PSTR("4"),DEBUG);
  if (GetCaptureByte() == FALSE)
  {
    return;
  }
  cmdcode = scanbyte; // save command code for later use.
FourthByte:
  //printstr_p(PSTR("5"),DEBUG);
  if (GetCaptureByte() == FALSE)
  {
    return;
  }
  // if execution reaches here, we have already verified that
  // bytes 1 and 2 are valid for a command packet.
  // verify that (Byte 3 + Byte 4) = 0xFF
  if ((cmdcode + scanbyte) == 0xFF)
  {
    //printstr_p(PSTR("6"),DEBUG);
    if ((cmdcode & 0x03) == 0) // verify that Byte 3 is a multiple of 4
    {
      //printstr_p(PSTR("7"),DEBUG);
      ACKcount = -4; // acknowledge command
      scanptr = fsr; // save scanptr, won't look at this byte again
      // Now, let's jump to the section of code that handles the
      // command we just received.
      DecodeCommand();
      //printstr_p(PSTR("\n"),DEBUG);
    }
    else
    {
      DumpFullCommand(); // ABORT: dump invalid packet for display
    }
  }
  else
  {
    DumpFullCommand(); // ABORT: dump invalid packet for display
  }
}
//-----------------------------------------------------------------------------
/*!
  \brief    void DumpFullCommand(void)
  dump all received command bytes
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void DumpFullCommand(void)
{
  fsr = scanptr; // restart back at the beginning of the packet
  if (GetCaptureByte() == TRUE) // send byte 1
  {
    EnqueueHex(scanbyte);
  }
  if (GetCaptureByte() == TRUE) // send byte 2
  {
    EnqueueHex(scanbyte);
  }
  if (GetCaptureByte() == TRUE) // send byte 3
  {
    EnqueueHex(scanbyte);
  }
  if (GetCaptureByte() == TRUE) // send byte 4
  {
    EnqueueHex(scanbyte);
  }
  EnqueueString(sNEWLINE);
  scanptr = fsr; // save scanptr, won't look at this byte again
}
//-----------------------------------------------------------------------------
/*!
  \brief    uint8_t GetCaptureByte(void)
  checks wether a command byte is still in queue
  \author     Koelling
  \date       05.10.2007
  \param[in]  none
  \param[out] none
  \return     FALSE ->	no more bytes to collect
  TRUE  -> scanbyte contains next byte
*/
//-----------------------------------------------------------------------------
static uint8_t GetCaptureByte(void)
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
  \brief    void SetStateIdle(void)
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
static void SetStateIdle(void)
{
  playing = FALSE;
  BIDIstate = StateIdle;
}
static void SetStateTP(void)
{
  playing = FALSE;
  BIDIstate = StateTP;
}
//-----------------------------------------------------------------------------
/*!
  \brief     SetStateIdleThenPlay(void)
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
static void SetStateIdleThenPlay(void)
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
static void SetStatePlay(void)
{
  playing = TRUE;
  BIDIstate = StatePlay;
}
//-----------------------------------------------------------------------------
/*!
  \brief    void SetStateInitPlay(void)
  Initiate Playing
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
static void SetStateInitPlay(void)
{
  playing = TRUE;
  BIDIstate = StateInitPlay;
  discload = 0xD1; //0xFF - 0x2E
  BIDIcount = -24;
}
//-----------------------------------------------------------------------------
/*!
  \brief    void SetStatePlayLeadIn(void)
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
static void SetStatePlayLeadIn(void)
{
  playing = TRUE;
  BIDIstate = StatePlayLeadIn;
  BIDIcount = -10;
}
//-----------------------------------------------------------------------------
/*!
  \brief    void SetStateTrackLeadIn(void)
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
static void SetStateTrackLeadIn(void)
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
  \brief    void SendDisplayBytes(void)
  send display bytes to head unit
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
static void SendDisplayBytes(void)
{
  SendByte(disc); // disc display value
  SendDisplayBytesNoCD();
}
//-----------------------------------------------------------------------------
/*!
  \brief    void SendDisplayBytesNoCD(void)
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
static void SendDisplayBytesNoCD(void)
{
  uint8_t send_byte_u8 = 0;
  SendByte(track);
  SendByte(minute);
  SendByte(second);
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
  SendByte(send_byte_u8);
}
//-----------------------------------------------------------------------------
/*!
  \brief    void SendDisplayBytesInitCD(void)
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
static void SendDisplayBytesInitCD(void)
{
  SendByte(0x99); // number of tracks total (99)?
  SendByte(0x99); // total minutes?
  SendByte(0x59); // total seconds?
  SendByte(0x49);//0xFF - 0xB7 = 48, 53, 31, 25, and 37 seen from real CDC,
  // no idea what it really means.
}
//-----------------------------------------------------------------------------
/*!
  \brief    void SendFrameByte(uint8_t byte_u8)
  SendFrameByte - sends a framing byte to head unit (first and last bytes).
  If the ACK flag is set, we modify the send byte to send an
  acknowledgement.
  \author     Koelling
  \date       06.10.2007
  \param[in]  byte_u8 -> byte to send
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
static void SendFrameByte(uint8_t byte_u8)
{
  if (ACKcount == 0)
  {
    SendByte(byte_u8);
  }
  else
  {
    byte_u8 |= 0x20; // flag acknowledgement
    ACKcount++;
    SendByte(byte_u8);
  }
}
//-----------------------------------------------------------------------------
/*!
  \brief    void SendFrameByte(uint8_t byte_u8)
  SendByte - sends a byte to head unit.
  \author     Koelling
  \date       06.10.2007
  \param[in]  byte_u8 -> byte to send
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------
static void SendByte(uint8_t byte_u8)
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
  \brief     void EnqueueString(const uint8_t addr PROGMEM)
  EnqueueString - Adds a new string pointer into the outgoing serial string
  queue.
  \author     Koelling
  \date       02.10.2007
  \param[in]  const uint8_t addr PROGMEM -> start address of string to display
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void EnqueueString(const uint8_t *addr PROGMEM)
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
  \brief     void EnqueueHex(uint8_t hexbyte_u8)
  The byte is converted to a two byte ASCII hexidecimal string
  \author     Koelling
  \date       05.10.2007
  \param[in]  uint8_t hexbyte -> hexbyte to display
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void EnqueueHex(uint8_t hexbyte_u8)
{
  uint8_t nibble_u8;
  nibble_u8 = hexbyte_u8 >> 4; // send high nibble first
  nibble_u8 <<= 1; // multiply high nibble by 2
  EnqueueString(&sHEX[nibble_u8]);
  nibble_u8 = hexbyte_u8 & 0x0F; // prepare low nibble
  nibble_u8 <<= 1; // multiply low nibble by 2
  EnqueueString(&sHEX[nibble_u8]);
}
//-----------------------------------------------------------------------------
/*!
  \brief     ResetTime(void)
  reset time information
  \author     Koelling
  \date       27.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void ResetTime(void)
{
  secondcount = SECONDWAIT;
  second = 0;
  minute = 0;
}
//-----------------------------------------------------------------------------
/*!
  \brief     void SendStateIdle(void)
  send data for idle state
  \author     Koelling
  \date       29.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void SendStateIdle(void)
{
  secondcount = SECONDWAIT; // stop display from ticking time
  SendFrameByte(0x8B);//FF - 0x74
  SendDisplayBytes();
  SendByte(0x70);//FF - 0x8F, mutes audio on Monsoon head units
  SendFrameByte(0x83);//FF - 0x7C
}
static void SendStateTP(void)
{ //B4 BE EF FE DB FF DF BC
  secondcount = SECONDWAIT; // stop display from ticking time
  SendFrameByte(0x4B);//FF - 0x4b
  SendDisplayBytes();
  SendByte(0x20);
  SendFrameByte(0x43);//FF - 0x7C
}
//-----------------------------------------------------------------------------
/*!
  \brief     SendStatePlayLeadInEnd(void)
  send data for state PlayLeadInEnd
  \author     Koelling
  \date       29.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void SendStatePlayLeadInEnd(void)
{
  SendFrameByte(0xC3);//FF - 0x3C
  BIDIcount++;
  if (BIDIcount == 0)
  {
    SetStatePlay();
  }
}
//-----------------------------------------------------------------------------
/*!
  \brief     SendStateInitPlayEnd(void)
  send data for state StateInitPlayEnd
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void SendStateInitPlayEnd(void)
{
  SendFrameByte(0xC3);//FF - 0x3C
  BIDIcount++;
  if (BIDIcount == 0)
  {
    SetStatePlayLeadIn();
  }
}
//-----------------------------------------------------------------------------
/*!
  \brief     SendStateInitPlayAnnounceCD(void)
  send data for state StateInitPlayAnnounceCD
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void SendStateInitPlayAnnounceCD(void)
{
  // 0xF6..0xF0: CD-ROM Loaded (seen on changer)
  // 0xE6..0xE0: CD-ROM Loaded. (made up)
  // 0x96..0x90: Slot Empty (seen on changer)
  // 0x86..0x80: Slot Empty (made up)
  // 0xD6..0xD0: AUDIO CD Loaded. (seen on changer)
  SendByte(discload);
  if (discload == 0xD6)
  {
    discload = 0xD1;
  }
  else
  {
    discload++;
  }
  SendDisplayBytesInitCD();
  SendByte(0x00);//0xFF - 0xFF
  SendStateInitPlayEnd();
}
//-----------------------------------------------------------------------------
/*!
  \brief     SendStatePlayLeadInAnnounceCD(void)
  send data for state StatePlayLeadInAnnounceCD
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void SendStatePlayLeadInAnnounceCD(void)
{
  SendByte((disc & 0x0F) | 0xD0);
  SendDisplayBytesInitCD();
  SendByte(0x00);//0xFF - 0xFF
  SendStatePlayLeadInEnd();
}
//-----------------------------------------------------------------------------
/*!
  \brief     void SendPacket(void)
  depending on BIDIstate data packet will be sent
  \author     Koelling
  \date       27.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------
static void SendPacket(void)
{
  switch (BIDIstate) {
    case StateTP:
      SendStateTP();
      break;
    case StateIdle:
      SendStateIdle();
      break;
    case StateIdleThenPlay:
      BIDIcount++;
      if (BIDIcount == 0)
      {
        SetStateInitPlay();
        SendStateIdle();
      }
      else
      {
        SendStateIdle();
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
      SendFrameByte(0xCB);//0xFF - 0x34
      if ((BIDIcount & 0x01) == 0)
      {
        SendStateInitPlayAnnounceCD();
        break;
      }
      SendDisplayBytes();
      SendByte(0x10);//0xFF - 0xEF
//no break here!
    case StateInitPlayEnd:
      SendStateInitPlayEnd();
      break;
    case StateInitPlayAnnounceCD:
      SendStateInitPlayAnnounceCD();
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
      SendFrameByte(0xCB);//0xFF - 0x34
      if ((BIDIcount & 0x01) == 0)
      {
        SendStatePlayLeadInAnnounceCD();
        break;
      }
      SendDisplayBytes();
      SendByte(0x51);//0xFF - 0xAE
//no break here!
    case StatePlayLeadInEnd:
      SendStatePlayLeadInEnd();
      break;
    case StatePlayLeadInAnnounceCD:
      SendStatePlayLeadInAnnounceCD();
      break;
    case StateTrackLeadIn:
      secondcount = SECONDWAIT; // stop display from ticking time
      SendFrameByte(0xCB);//0xFF - 0x34
      SendDisplayBytes();
      SendByte(0x51);//0xFF - 0xAE
      SendFrameByte(0xC3);//0xFF - 0x3C
      BIDIcount++;
      if (BIDIcount == 0)
      {
        break;
      }
      SetStatePlay();
      break;
    case StatePlay:
      SendFrameByte(0xCB);//0xFF - 0x34
      SendDisplayBytes();
      SendByte(0x30);//0xFF - 0xCF
      SendFrameByte(0xC3);//FF - 0x3C
      break;
    default:
      break;
  }
}
static void android_buttons() {
#ifdef ANDROID_HEADPHONES
  //android headphone control, this is fired every 50ms
  //play button
  if (play_count > 0) {
    Serial.println("play");
    Serial.println(play_count);
    Serial.println(next_count);
    Serial.println(prev_count);
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
    Serial.println("next");
    Serial.println(play_count);
    Serial.println(next_count);
    Serial.println(prev_count);
    next_count--;
    ANDROID_NEXT_PORT |= _BV(ANDROID_NEXT); //high
  } else {
    ANDROID_NEXT_PORT &= ~_BV(ANDROID_NEXT); //low
  }
  //prev button, double push, head unit goes to previous song, no to start of the song
  if (prev_count > 0) {
    Serial.println("prev");
    Serial.println(play_count);
    Serial.println(next_count);
    Serial.println(prev_count);
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
static void printstr_p(const char *s)
{
  char c;
  for (c = pgm_read_byte(s); c; ++s, c = pgm_read_byte(s))
  {
    Serial.print(c);
    if (c == '\n')
      break;
  }
}

//-----------------------------------------------------------------------------
/*!
  \brief    OutputByte //was: ISR(TIMER2_COMPA_vect)
  runs every 700us Timer2 ensures 700µs timing between display package bytes
  Shift bytes out to head unit
  \author     Koelling
  \date       06.10.2007
  \param[in]  none
  \param[out] none
  \return     none
*/
//-----------------------------------------------------------------------------

void OutputByte(void)
{
  static uint8_t display_byte_counter_u8 = 0;
  uint8_t byte_u8;
  if (display_byte_counter_u8 < 8)
  {
    byte_u8 = display_byte_buffer_mau8[display_byte_counter_u8];

#ifdef DUMPMODE2
    Serial.print("|");
    Serial.print(byte_u8, HEX);
    Serial.print("|");
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
  \brief     Init_VWCDC
  initialization for cdc protocol
  \author     Koelling
  \date       26.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------


void Init_VWCDC(void)
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

  setup_timers();

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

  ResetTime();
  SetStateIdleThenPlay();
  EnqueueString(sIDENTIFY);
  EnqueueString(sVERSION);
  EnqueueString(sNEWLINE);
  EnqueueString(sRING);
  SendPacket(); // force first display update packet
  sei();
}



//-----------------------------------------------------------------------------
/*!
  \brief     CDC_Protocol(void)
  cyclic called main program for cdc protocol (50ms?)
  \author     Koelling
  \date       26.09.2007
  \param[in]  none
  \param[out] none
  \return     void
*/
//-----------------------------------------------------------------------------

void CDC_Protocol(void)
{
  uint8_t decimal_adjust_u8;
  if (flag_50ms == TRUE)
  {
    flag_50ms = FALSE;
    SendPacket();
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
        EnqueueString(sIDENTIFY);
        EnqueueString(sVERSION);
        EnqueueString(sNEWLINE);
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
    EnqueueString(sOVERFLOW);
  }
  if (dataerr == TRUE) // has the command receive code detected
  { // a framing type data error?
    dataerr = FALSE; // clear error flag
    EnqueueString(sDATAERR);
  }
#ifndef DUMPMODE
  ScanCommandBytes();
#else
  if (startbit == TRUE) // have we just recieved a start bit?
  {
    startbit = FALSE;
    EnqueueString(sNEWLINE); // yes, start a new line
  }
  fsr = scanptr;
  while (GetCaptureByte() == TRUE)
  {
    scanptr = fsr;
    EnqueueHex(scanbyte);
  }
#endif

  while (txoutptr != txinptr)
  {
    printstr_p((char*) txbuffer[txoutptr]);
    txoutptr++;
    if (txoutptr == TX_BUFFER_END)
    {
      txoutptr = 0;
    }
  }
}

//-----------------------------------------------------------------------------
/*!
  \brief    void DecodeCommand(void)
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

static void DecodeCommand(void)
{
  uint8_t decimal_adjust_u8 = 0;
#ifdef JUST_HEX_TO_SERIAL
  Serial.write(cmdcode);
#endif
  switch (cmdcode) {
    case Do_CHANGECD:
      // Head unit seems to send this after each CDx number change
      // but the CD Changer seems to completely ignore (doesn't even ACK it).
      ACKcount = 0; // do not ack this command
#ifdef PJRC
      EnqueueString(sRANDOM);
#endif
      break;

    case Do_ENABLE:
    case Do_ENABLE_MK:
      mix = FALSE;
      if (playing == FALSE)
      {
        SetStateInitPlay(); // skip this if already playing
      }
      if (!mix_button)
        EnqueueString(sMENABLE);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_LOADCD:
      if (playing == TRUE)
      {
        SetStateInitPlay(); // skip this if we're in idle mode
      }
      ResetTime();
      EnqueueString(sMINQUIRY);
      break;

    case Do_DISABLE:
      SetStateIdle(); // skip this if we're already in idle mode
      EnqueueString(sMDISABLE);
      break;

    case Do_SEEKBACK:
    case Do_PREVCD:
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc--;
      track = 1;
      ResetTime();
      if ((disc & 0x0F) == 0)
      {
        disc = 0x46; // set back to CD 1
      }
#endif
      EnqueueString(sPRV_LIST);
      break;

    case Do_SEEKFORWARD:
    case Do_SEEKFORWARD_MK:
      if (cd_button == FALSE) // mk don't increment when previous command was a cd button
      {
        EnqueueString(sNXT_LIST);
#ifndef DISC_TRACK_NUMBER_FROM_MPD
        ResetTime();
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
      EnqueueString(sRANDOM);
      break;

    case Do_PLAY:
      EnqueueString(sPLAY); // this will make the PJRC play/pause
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
      EnqueueString(sPLAY); // this will make the PJRC play/pause
#else
      EnqueueString(sSCAN); //
#endif
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_UP:
    case Do_UP_MK3:
      if (playing == TRUE) // skip track lead-in if not in play mode
      {
        SetStateTrackLeadIn();
      }
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      track++;
      ResetTime();
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
      EnqueueString(sNEXT);
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
        SetStateTrackLeadIn();
      }
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      decimal_adjust_u8 = track & 0x0F; // skip past hexidecimal codes
      if (decimal_adjust_u8 == 0) // are we at x0?
      {
        track -= 6; // yes, subtract 6 and we'll be at x9 instead
      }
      track--;
      ResetTime();
      if (track == 0) // have we gone below Track 1?
      { // yes, rollover to Track 99 so that jog wheels
        track = 0x99; // can continue rolling (Audi Concert II)
      }
#endif
      EnqueueString(sPREVIOUS);
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
      ResetTime();
#endif
      EnqueueString(sLIST1);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_CD2:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x42; // set CD 2
      ResetTime();
#endif
      EnqueueString(sLIST2);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_CD3:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x43; // set CD 3
      ResetTime();
#endif
      EnqueueString(sLIST3);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_CD4:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x44; // set CD 4
      ResetTime();
#endif
      EnqueueString(sLIST4);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_CD5:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x45; // set CD 5
      ResetTime();
#endif
      EnqueueString(sLIST5);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_CD6:
      cd_button = TRUE; // mk store cd button pressed
#ifndef DISC_TRACK_NUMBER_FROM_MPD
      disc = 0x46; // set CD 6
      ResetTime();
#endif
      EnqueueString(sLIST6);
#ifdef ANDROID_HEADPHONES
      play_count = ANDROID_PUSH_COUNT; // 100ms high on ANDROID_PLAY pin
#endif
      break;

    case Do_TP:
      if (playing == TRUE) {
        SetStateTP();
      } else {
        SetStateInitPlay();
      }
      EnqueueString(sTP);
      break;

    default:
      /* if execution reaches here, we have verified that we got
         a valid command packet, but the command code received is not
         one that we understand.
         Dump the unknown command code for the user to view.
      */
      EnqueueString(sDASH);
      EnqueueHex(cmdcode);
      EnqueueString(sNEWLINE);
      break;
  }
}
