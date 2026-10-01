# Kemper Profiler bidirectional SysEx — observed behaviour

Captured with `tools/kemper_probe.py` against a Profiler Rack over USB (port "Profiler").
Request formats come from Kemper's *MIDI Parameter Documentation*; the beacon and
everything marked *observed* come from probing real hardware.

## Framing

| Direction | Header |
|---|---|
| Requests we send | `F0 00 20 33 02 7F <fn> 00 <page> <param> F7` (product 02 = Profiler, device 7F = omni) |
| Most replies | `F0 00 20 33 00 00 <fn> …` (*observed*: product/device 00 00) |
| Function 3C / 06 / 7E replies | `F0 00 20 33 02 7F …` or `02 00 …` |

Accept any message starting `F0 00 20 33` and dispatch on byte 6 (function).

## Requests (documented)

| Purpose | Send | Reply |
|---|---|---|
| Rig name | `43 00 00 01` | `03 00 00 01 <ascii> 00` |
| Single param | `41 00 <page> <param>` | `01 00 <page> <param> <msb> <lsb>` (value = msb<<7 \| lsb) |

## Beacon (observed)

`F0 00 20 33 02 7F 7E 00 40 02 <flags> <lease> F7`, re-sent every lease/2 seconds (lease 5 used).

- Flags `0x03` work. Bit 0 (INIT) triggers a full state dump — send it on the first beacon only,
  otherwise every beacon repeats the dump.
- Profiler acks each beacon with `F0 00 20 33 02 7F 7E 00 7F <counter> F7`.
- Meaning of the other flag bits is undocumented; not yet tested.

## Pushed messages while beacon is active (observed)

| Event | Messages |
|---|---|
| Rig change | `03 00 00 01 <rig name>`, then **only the changed** slot params, then `B0 00 00`, `B0 20 00`, `C0 <program>` |
| Program number | `program = (performance - 1) * 5 + (slot - 1)` (e.g. `C0 1C` = perf 6, slot 4) |
| Performance browse | `B0 2F <n>` while scrolling, `B0 2F 7F` when loaded |
| Performance loaded | `07 00 00 00 01 00 00 <perf name>`, `07 … 00 0N <slot N name>` (N = 1–5, only changed ones) |
| Slot enabled flag | `06 00 00 00 01 00 0N 00 00 00 00 <0/1>` (only changed ones) |
| Effect slot type | `01 00 <slot page> 00 <msb> <lsb>` + `3C 00 <slot page> 00 <msb> <lsb> <short name> 00` |
| Effect on/off | `01 00 <slot page> 03 00 <0/1>` (pushed immediately on footswitch toggle) |
| Tuner / level | `01 00 7C 0F …`, `01 00 7C 51 …`, `01 00 7D 54 …`, ~16 msgs/s — ignore |

Slot pages: A `32`, B `33`, C `34`, D `35`, X `38`, MOD `3A`, DLY `3C`, REV `3D`.

## Still unknown

- Morph state: pressing the active slot again re-sends its program change and toggles
  `01 00 7F 7E` (seen 1 → 3) / `7F 7D`, but this needs a dedicated test.
- `01 00 04 02`, `4A 02`, `4B 02` appear on rig changes; meaning unknown.

## Implementation notes

- JUCE 8.0.12 sends SysEx through CoreMIDI's UMP API (`MIDISendEventList`); on macOS 26 it never
  reaches the Profiler, although CC/PC do. The link therefore uses CoreMIDI's MIDI 1.0 packet API
  directly (`Source/KemperPort.cpp`), like rtmidi does.
- JUCE 8's MIDI device list is message-thread only; open ports on the message thread.
