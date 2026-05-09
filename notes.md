## CDC emulator scan/mute test notes

### Context
- Investigated `CDC_emulator/cdc_arduino_ported_k9spud_vwcdpic` protocol handling and dumps in `/home/runner/work/vwcdavr/vwcdavr/dumps`.
- Login/command framing looks correct (`53 2C <cmd> <~cmd>` and command byte multiple-of-4 checks).

### Observed issue
- Some radios intermittently enter CD scanning behavior and mute audio when emulator is active.
- In code, SCAN command (`Do_SCAN`, `0xA0`) previously toggled the SCAN indicator bit (`0x10`) in outgoing display/status mode byte.

### Change in this PR
- For `Do_SCAN`, keep `scan = FALSE` so outgoing packets do not set SCAN indicator bit.
- Command is still accepted and acknowledged; only scan-indicator signaling is suppressed.

### Why
- Dump patterns show real radios issuing SCAN-related commands, and historical comments in code already suggest scan signaling can cause mute on some units.
- This is a minimal risk change focused on avoiding the known mute path while preserving normal protocol flow.

### How to test
1. Flash updated sketch from `CDC_emulator/cdc_arduino_ported_k9spud_vwcdpic`.
2. Start playback and trigger operations that previously led to scan/mute (SCAN button, next/seek cycles, CD change).
3. Verify:
   - audio remains unmuted,
   - head unit still stays in CDC mode,
   - track/disc updates still work.
4. Compare with previous firmware if needed.
