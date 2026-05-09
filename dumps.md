## Dump pattern notes

Source files:
- `dumps/dump_cdc_real_changer.txt`
- `dumps/dump_cdc_real_changer_real_radio_audi_concert1.txt`

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

## Response/frame behavior patterns

- Real changer streams repeated 8-byte status/display frames between command events.
- Around SCAN events, mode/status bytes often move to values consistent with scan flag enabled (examples with `... EF ...` sequences in decoded rows).
- Dumps also show many interleaved, partially corrupted/noisy rows (capture timing/noise), so stable interpretation depends on repeated motifs rather than single lines.

## Practical implication for emulator tuning

- The strongest repeatable pattern tied to audible issues is SCAN-mode signaling.
- Suppressing SCAN indicator in emulator status packets is a reasonable first mitigation to test while keeping normal command parsing/ACK behavior.
