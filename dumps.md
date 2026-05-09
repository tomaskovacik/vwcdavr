## Dump pattern notes

Source files:
- `dumps/dump_cdc_real_changer.txt`
- `dumps/dump_cdc_real_changer_real_radio_audi_concert1.txt`

## Protocol encoding

CDC status packets are 8 bytes sent from the changer to the head unit over a clocked serial
bus.  The data line uses negative/inverted NRZ logic: a code bit of 0 drives the wire HIGH
and a code bit of 1 drives it LOW.  The sniffer reads pin-HIGH as 1, so the captured
(dump) byte equals `code_byte XOR 0xFF`.  Equivalently: `code_byte = dump_byte XOR 0xFF`.

### 8-byte packet layout (wire/dump values)

| Byte | Field          | Notes |
|------|----------------|-------|
| 0    | Frame start    | Encodes state & ACK; emulator varies this by state |
| 1    | Disc           | `disc_number = dump_byte XOR 0xBF`  (CD1=0x01, CD2=0x02 …) |
| 2    | Track          | `track_number = dump_byte XOR 0xFF` |
| 3    | Minute         | `minute = dump_byte XOR 0xFF` |
| 4    | Second         | `second = dump_byte XOR 0xFF` |
| 5    | Scan/Mix flags | bit 5 (code) = Mix, bit 4 (code) = Scan; 0x00→wire 0xFF = no flags |
| 6    | Mute/State     | Controls audio mute and player state (see below) |
| 7    | Frame end      | Mirrors frame start state encoding |

### 4-byte radio command layout

| Byte | Value  | Notes |
|------|--------|-------|
| 1    | `0x53` | fixed header |
| 2    | `0x2C` | fixed header |
| 3    | cmd    | command code, always a multiple of 4 |
| 4    | ~cmd   | bitwise complement of byte 3 |

## Packet structure patterns

- Head unit command packets follow:
  - byte1 `0x53`
  - byte2 `0x2C`
  - byte3 command code
  - byte4 bitwise complement of byte3
- Frequent trailer/control command:
  - `532C14EB` appears very often (end command / next-cd follow-up behavior).

## Common command patterns from dumps

- Enable/disable:
  - `532CE41B` (ENABLE, common in one dump)
  - `532C08F7` (ENABLE MK, very common with Audi Concert 1 dump)
  - `532C10EF` (DISABLE)
- Navigation and mode:
  - `532C38C7` (SEEKFORWARD MK / inquiry-like transition)
  - `532CD827` (seek forward)
  - `532C58A7` (seek back)
  - `532C609F` / `532CE01F` (mix variants)
  - `532CA05F` (scan)
  - `532C30CF` (TP)

## State packets: emulator vs real changer comparison

The second dump (`dump_cdc_real_changer_real_radio_audi_concert1.txt`) contains captures
from both a real 6-disc changer and from the Arduino emulator (captured in different
sessions and concatenated).  Patterns with frame byte `0x34`/`0x3C` originate from the
emulator; patterns with `0xB4`/`0xBC` originate from the real changer.

### Idle state

| Source        | Wire packet                          | Mute byte |
|---------------|--------------------------------------|-----------|
| Emulator      | `74 BE FE FF FF FF 8F 7C`            | `0x8F`    |
| Real changer  | `B4 BF FF FF FF FF 8F BC`            | `0x8F`    |

**Mute byte matches (`0x8F`, code `0x70`).**  The frame bytes differ — the real changer
uses frame `0xB4`/`0xBC` for all states (constant), while the emulator changes the frame
byte per state.  Both are accepted by tested head units.

Disc byte: real changer starts at disc `0` (`0xBF`); emulator starts at disc `1` (`0xBE`).
This difference is cosmetic on head units that accept the first valid PLAY command.

### Play state

| Source        | Wire packet                          | Mute byte | Frame    |
|---------------|--------------------------------------|-----------|----------|
| Emulator      | `34 BE FE FF FF FF CF 3C`            | `0xCF`    | `34`/`3C`|
| Real changer  | `B4 BE FE FF FF FF 6F BC`            | `0x6F`    | `B4`/`BC`|

Differences:
- **Frame byte**: emulator `0x34`/`0x3C` (code `0xCB`/`0xC3`), real `0xB4`/`0xBC`
  (code `0x4B`/`0x43`).
- **Mute byte**: emulator `0xCF` (code `0x30`), real `0x6F` (code `0x90`).
  Bit 7 of the code differs: code `0x90 = 1001 0000` vs `0x30 = 0011 0000`.
  The comment in the code notes code `0x70` (wire `0x8F`) "mutes audio on Monsoon head
  units" during idle.  The real changer uses code `0x90` for play, which differs from both
  the emulator's play value (`0x30`) and the idle mute value (`0x70`).  The emulator is
  accepted by most tested head units with code `0x30`, but code `0x90` may be more
  compatible with Monsoon-type head units that use this byte for mute control.

### Disc announcement packets (during init/load)

During `StateInitPlay` the emulator sends disc-slot info bytes cycling from `0xD1` to
`0xD6` (wire `0x2E` to `0x29`).  These match the real changer's announce packets seen in
the dumps (e.g. `B4 2E 0 FF FF FF FF BC`, `B4 2D 0 FF FF ...`).  Encoding is correct.

## Response/frame behavior patterns

- Real changer streams repeated 8-byte status/display frames between command events.
- Around SCAN events, mode/status bytes often move to values consistent with scan flag
  enabled (for example, lines in the dump files that include `... EF ...` inside the
  8-byte response frame text).
- Dumps also show many interleaved, partially corrupted/noisy rows (capture timing/noise),
  so stable interpretation depends on repeated motifs rather than single lines.

## Code issues found during analysis

### Fixed bugs

1. **`cdButtonPushed` case 6 (functions.ino)**: case 6 incorrectly reset and returned
   `cd5pushed` instead of `cd6pushed`.  Fixed in the main variant.

2. **`Do_CD2` calls wrong argument (main .ino)**: `Do_CD2` called `cdButtonPushed(1)`
   instead of `cdButtonPushed(2)`, silently counting CD1 presses when CD2 is pressed.
   Fixed in the main variant.

3. **ATmega8 ISR assignment instead of comparison (main .ino, BK3254 variant)**: the
   ATmega8 `TIMER0_OVF` ISR used `=` (assignment) instead of `==` (comparison):
   ```c
   // BUG (before fix):
   if (counter_timer0_overflows = _TIMER0_OVERFLOW_COUNTS)
   // FIXED:
   if (counter_timer0_overflows == _TIMER0_OVERFLOW_COUNTS)
   ```
   With the assignment, `flag_50ms` fired on every overflow instead of every 50 ms,
   making the 50 ms timer fire ~50× too fast on ATmega8/ATmega128 targets.

### Noted discrepancies (not changed)

- **Play state mute byte**: emulator uses code `0x30` (wire `0xCF`); real changer uses
  code `0x90` (wire `0x6F`).  Most head units accept `0xCF`, but if audio stays muted
  during playback on a Monsoon-type head unit, changing `SendByte(0x30)` to
  `SendByte(0x90)` in `StatePlay` may help.
- **Frame bytes**: emulator encodes state in the frame byte (idle `0x8B`/`0x83`, play
  `0xCB`/`0xC3`); real changer uses a fixed frame byte (`0x4B`/`0x43`) for all states.
  This difference has not caused compatibility problems with tested radios.

## Practical implication for emulator tuning

- The strongest repeatable pattern tied to audible issues is SCAN-mode signaling.
- Suppressing SCAN indicator in emulator status packets is a reasonable first mitigation to test while keeping normal command parsing/ACK behavior.
