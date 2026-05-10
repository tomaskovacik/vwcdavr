# NEC IR Protocol vs VW CD-changer Protocol: Comparison

## Background

Investigation into whether an Arduino NEC IR library (e.g. IRremote, IRLib2) can replace the custom software used for the VW CD-changer (CDC) ↔ radio communication in this project.

## What the Custom SW Does

### TX (radio_emulator: Arduino pretends to be the radio, sends commands to CDC)

The `send_cmd()` / `shiftOutPulse()` functions in `radio_emulator.ino` manually bit-bang the DataOut line:

- 9 ms HIGH leader, 4.5 ms LOW space
- Each bit: 550 µs HIGH mark, then 540 µs (logic '0') or 1650 µs (logic '1') LOW space
- 4 bytes total per packet: `0x53 0x2C [cmd] [0xFF ^ cmd]`
- **Bit order: MSB first**

### RX (CDC emulator / sniffer: Arduino receives commands from real radio, responds as CDC)

The ISR in `cdc_arduino_ported_k9spud_vwcdpic.ino` uses Timer1 Input Capture to time LOW pulses on DataOut:

- Start bit: LOW > 3,200 µs
- Bit '1': LOW > 1,248 µs
- Bit '0': LOW > 256 µs
- Bits accumulated **MSB first** into a capture buffer

---

## Protocol Comparison Table

| Feature | NEC IR Library | VW Protocol | Compatible? |
|---|---|---|---|
| Leader mark | 9 ms | 9 ms | ✅ |
| Leader space | 4.5 ms | 4.5 ms | ✅ |
| Bit mark | 562.5 µs | ~550 µs (~2% off) | ✅ |
| Bit '0' space | 562.5 µs | ~540 µs (~4% off) | ✅ |
| Bit '1' space | 1687.5 µs | ~1650 µs (~2% off) | ✅ |
| Packet length | 32 bits | 32 bits | ✅ |
| **Bit order** | **LSB first** | **MSB first** | ❌ |
| Address validation | addr + ~addr = 0xFF | fixed 0x53 + 0x2C (0x53 + 0x2C ≠ 0xFF) | ❌ |
| Command validation | cmd + ~cmd = 0xFF | cmd + (0xFF ^ cmd) = 0xFF | ✅ |
| Physical medium | Modulated IR 38 kHz carrier → demodulator inverts signal | Direct wire (no carrier, no demodulation) | ❌ |
| Idle wire state | Library assumes demodulated: idle = HIGH | DataOut idle = LOW | ❌ |

---

## Analysis by Direction

### RX (receiving radio commands) — Feasible with adaptation

The timing tolerances of NEC libraries are typically ±20–30%, so the VW timing fits within NEC decoders. However, the following issues must be resolved:

1. **Polarity**: The VW DataOut wire is active-HIGH (mark = HIGH pulse), while an NEC demodulator output is active-LOW (mark = LOW = carrier present). The signal must be inverted — either via a transistor/NOT gate in hardware, or by configuring the library to use the opposite edge.
2. **Bit order**: After decoding, every received byte must be bit-reversed (`reverse_bits(byte)`) before use, since the library decodes LSB-first while the VW protocol is MSB-first.
3. **Address validation**: The library would decode `0x53 0x2C` as address + ~address, but `~0x53 = 0xAC ≠ 0x2C`, so **NEC address validation would reject all packets**. This check must be disabled or bypassed in the library.

### TX (sending radio commands) — Feasible with adaptation

Libraries like IRremote can send raw NEC timing. However:

1. **No IR carrier needed**: The 38 kHz carrier modulation must be disabled; the DataOut line must be driven directly.
2. **Bit order**: Each of the 4 bytes must be bit-reversed before passing to the library, or `sendRaw()` must be used with a manually constructed pulse array.
3. **Fixed address bytes**: The `0x53 0x2C` sync word must be sent as-is; a standard `sendNEC(address, command)` call would not produce the correct bit pattern without byte reversal.

---

## Practical Verdict

Using a standard NEC library **as-is will not work**. The protocol is close enough that it could be adapted **with the following modifications**:

1. **Hardware invert** the DataOut signal (one transistor or logic inverter between the Arduino pin and the VW bus).
2. **Bit-reverse** every byte before TX or after RX.
3. **Disable** NEC address-complement validation in the library's RX path.
4. For TX, use `sendRaw()` instead of `sendNEC()` to avoid the library imposing its own packet structure.

### Why the custom SW is preferred

The existing custom ISR-based approach is:
- Lean (~200 lines), no external dependencies
- Already verified against real hardware
- Handles the exact polarity, bit order, and sync-word format natively

A NEC library would add ~2–5 KB of flash, introduce 38 kHz carrier overhead that must be disabled, and still require all of the workarounds listed above. **The custom SW is the simpler and more reliable path for this use case.**
