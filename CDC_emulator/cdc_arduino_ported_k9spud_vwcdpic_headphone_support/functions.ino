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
  \brief    SetStatePlay(void)
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
  34BEFEFFEEFFCF3C
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
    send_byte_u8 |= 0x10; // turn on scan display //this probably cose mute for few microsec.
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
  SendByte(0x49); //0xFF - 0xB7 = 48, 53, 31, 25, and 37 seen from real CDC,
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
#ifdef USETIMER2
    TCCR2B |= _BV(CS22); // prescaler = 64 -> 1 timer clock tick is 4us long
    TIMSK2 |= _BV(OCIE2A); // enable output compare interrupt on timer2
#else
#ifdef USETIMER3
    TCCR3B |= _BV(CS31) | _BV(CS30); // prescaler = 64 -> 1 timer clock tick is 4us long
    TIMSK3 |= _BV(OCIE3A); // enable output compare interrupt on timer3
#endif
#endif
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
    //no break here!!

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
    //no break here!!

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

static uint8_t cdButtonPushed(uint8_t cdnumber) {
  static uint8_t cd1pushed, cd2pushed, cd3pushed, cd4pushed, cd5pushed, cd6pushed = 0; //variable to cound how many times we pressed button for spectial actions
  switch (cdnumber) {
    case 1:
      cd2pushed = cd3pushed = cd4pushed = cd5pushed = cd6pushed;
      if (++cd1pushed == 6)
        cd1pushed = 0;
      return cd1pushed;

    case 2:
      cd1pushed = cd3pushed = cd4pushed = cd5pushed = cd6pushed;
      if (++cd2pushed == 6)
        cd2pushed = 0;
      return cd2pushed;

    case 3:
      cd1pushed = cd2pushed = cd4pushed = cd5pushed = cd6pushed;
      if (++cd3pushed == 6)
        cd3pushed = 0;
      return cd3pushed;

    case 4:
      cd1pushed = cd2pushed = cd3pushed = cd5pushed = cd6pushed;
      if (++cd4pushed == 6)
        cd4pushed = 0;
      return cd4pushed;

    case 5:
      cd1pushed = cd2pushed = cd3pushed = cd4pushed = cd6pushed;
      if (++cd5pushed == 6)
        cd5pushed = 0;
      return cd5pushed;

    case 6:
      cd1pushed = cd2pushed = cd3pushed = cd4pushed = cd5pushed;
      if (++cd6pushed == 6)
        cd5pushed = 0;
      return cd5pushed;
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

#if defined(BUTTONS_ACTIVE_HIGH)
    ANDROID_PLAY_PORT |= (1 << ANDROID_PLAY); //high //PORTD |= B01000000; //digitalWrite(6, HIGH);
#elif defined(BUTTONS_ACTIVE_LOW)
    ANDROID_PLAY_PORT &= ~_BV(ANDROID_PLAY); //low   //PORTD &= B10111111; //digitalWrite(6, LOW);
#endif
  }

  else {
#if defined(BUTTONS_ACTIVE_HIGH)
    ANDROID_PLAY_PORT &= ~_BV(ANDROID_PLAY); //low   //PORTD &= B10111111; //digitalWrite(6, LOW);
#elif defined(BUTTONS_ACTIVE_LOW)
    ANDROID_PLAY_PORT |= (1 << ANDROID_PLAY); //high //PORTD |= B01000000; //digitalWrite(6, HIGH);
#endif
  }

  //next button
  if (next_count > 0) {
    Serial.println("next");
    Serial.println(play_count);
    Serial.println(next_count);
    Serial.println(prev_count);
    next_count--;

#if defined(BUTTONS_ACTIVE_HIGH)
    ANDROID_NEXT_PORT |= _BV(ANDROID_NEXT); //high //PORTD |= B00100000; //digitalWrite(5, HIGH);
#elif defined(BUTTONS_ACTIVE_LOW)
    ANDROID_NEXT_PORT &= ~_BV(ANDROID_NEXT); //low //PORTD &= B11011111; //digitalWrite(5, LOW);
#endif
  }

  else {
#if defined(BUTTONS_ACTIVE_HIGH)
    ANDROID_NEXT_PORT &= ~_BV(ANDROID_NEXT); //low //PORTD &= B11011111; //digitalWrite(5, LOW);
#elif defined(BUTTONS_ACTIVE_LOW)
    ANDROID_NEXT_PORT |= _BV(ANDROID_NEXT); //high //PORTD |= B00100000; //digitalWrite(5, HIGH)
#endif
  }

  //prev button
  if (prev_count > 0) {
    Serial.println("prev");
    Serial.println(play_count);
    Serial.println(next_count);
    Serial.println(prev_count);
    prev_count--;

#if defined(BUTTONS_ACTIVE_HIGH)
    ANDROID_PREV_PORT |= _BV(ANDROID_PREV); //high //PORTD |= B10000000; //digitalWrite(7, HIGH);
#elif defined(BUTTONS_ACTIVE_LOW)
    ANDROID_PREV_PORT &= ~_BV(ANDROID_PREV); //low //PORTD &= B01111111; //digitalWrite(7, LOW);
#endif
  }
  else {
#if defined(BUTTONS_ACTIVE_HIGH)
    ANDROID_PREV_PORT &= ~_BV(ANDROID_PREV); //low //PORTD &= B01111111; //digitalWrite(7, LOW);
#elif defined(BUTTONS_ACTIVE_LOW)
    ANDROID_PREV_PORT |= _BV(ANDROID_PREV); //high //PORTD |= B10000000; //digitalWrite(7, HIGH);
#endif

    //uncomment in case smartphone/MP3 player requires double press to go to previous song
    /*
      if (prev_count_delay > 0 ) { //we ended first push and do not finished second push
      prev_count_delay--;
      if (prev_count_delay == 0) //we are at last run of delay loop
        prev_count = ANDROID_PUSH_COUNT;
      }
    */
  }
#endif
}
