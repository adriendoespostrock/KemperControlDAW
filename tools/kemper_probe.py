#!/usr/bin/env python3
"""Probe a Kemper Profiler's bidirectional SysEx support.

Sends read-only requests (rig name, effect slot type/state) and a beacon,
then logs every incoming message so we can see what the Profiler reports.

Usage: python kemper_probe.py [--port Profiler] [--flags 0x03] [--seconds 30]
Requires: pip install python-rtmidi
"""
import argparse
import time

import rtmidi

HEADER = [0xF0, 0x00, 0x20, 0x33, 0x02, 0x7F]  # Kemper, Profiler, device OMNI
SLOTS = {"A": 0x32, "B": 0x33, "C": 0x34, "D": 0x35,
         "X": 0x38, "MOD": 0x3A, "DLY": 0x3C, "REV": 0x3D}


def sysex(*body):
    return HEADER + list(body) + [0xF7]


def request_string(page, param):
    return sysex(0x43, 0x00, page, param)


def request_param(page, param):
    return sysex(0x41, 0x00, page, param)


def beacon(flags, lease):
    return sysex(0x7E, 0x00, 0x40, 0x02, flags, lease)


def describe(msg):
    hexs = " ".join(f"{b:02X}" for b in msg)
    # Replies carry product type/device 00 00 rather than the 02 7F we send.
    if msg[:4] != HEADER[:4] or len(msg) < 8 or msg[-1] != 0xF7:
        return hexs
    fn, body = msg[6], msg[7:-1]
    if fn == 0x03 and len(body) >= 3:  # string response
        text = bytes(body[3:]).split(b"\0")[0].decode("ascii", "replace")
        return f"{hexs}\n    -> STRING page={body[1]:02X} param={body[2]:02X} '{text}'"
    if fn == 0x01 and len(body) >= 5:  # single parameter response
        value = (body[3] << 7) | body[4]
        slot = next((k for k, v in SLOTS.items() if v == body[1]), None)
        where = f" ({slot})" if slot else ""
        return f"{hexs}\n    -> PARAM page={body[1]:02X}{where} param={body[2]:02X} value={value}"
    return f"{hexs}\n    -> function {fn:02X}"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", default="Profiler")
    ap.add_argument("--flags", type=lambda s: int(s, 0), default=0x03)
    ap.add_argument("--lease", type=int, default=5)
    ap.add_argument("--seconds", type=float, default=30)
    ap.add_argument("--no-beacon", action="store_true")
    args = ap.parse_args()

    mi, mo = rtmidi.MidiIn(), rtmidi.MidiOut()
    mi.ignore_types(sysex=False, timing=True, active_sense=True)
    mi.open_port(mi.get_ports().index(args.port))
    mo.open_port(mo.get_ports().index(args.port))

    start = time.monotonic()

    def on_message(event, _):
        msg, _delta = event
        print(f"[{time.monotonic() - start:6.2f}s] IN  {describe(msg)}", flush=True)

    mi.set_callback(on_message)

    def send(label, msg):
        print(f"[{time.monotonic() - start:6.2f}s] OUT {label}: "
              + " ".join(f"{b:02X}" for b in msg), flush=True)
        mo.send_message(msg)
        time.sleep(0.05)

    send("rig name", request_string(0x00, 0x01))
    for name, page in SLOTS.items():
        send(f"{name} type", request_param(page, 0x00))
        send(f"{name} on/off", request_param(page, 0x03))

    if args.no_beacon:
        time.sleep(args.seconds)
    else:
        print("\nBeacon active. Change rigs/effects on the Profiler now.\n", flush=True)
        next_beacon = 0.0
        flags = args.flags
        while time.monotonic() - start < args.seconds:
            if time.monotonic() >= next_beacon:
                send(f"beacon flags={flags:#04x}", beacon(flags, args.lease))
                # Bit 0 (INIT) triggers a full state dump; only ask for it once.
                flags &= ~0x01
                next_beacon = time.monotonic() + args.lease / 2
            time.sleep(0.05)

    mi.close_port()
    mo.close_port()


if __name__ == "__main__":
    main()
