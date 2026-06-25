#!/usr/bin/env python3
"""Behavioral oracle (known-answer test) for icalendar.

Exercises the SAME parsing pipeline the fuzzer drives — Calendar.from_ical -> component tree ->
property values -> to_ical round-trip — and ASSERTS specific decoded values. A no-op / neutered
program (which prints nothing) FAILS test.sh, because the SELFTEST_PASS marker and its asserted
values are only printed when every assertion holds.
"""
from icalendar import Calendar

ICS = (
    "BEGIN:VCALENDAR\r\n"
    "VERSION:2.0\r\n"
    "PRODID:-//Mayhem//icalendar oracle//EN\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:oracle-event-1\r\n"
    "SUMMARY:Team Meeting\r\n"
    "DTSTART:20240115T100000Z\r\n"
    "DTEND:20240115T110000Z\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n"
)

# 1) Parse a known calendar and assert top-level + nested values.
cal = Calendar.from_ical(ICS)
assert str(cal["VERSION"]) == "2.0", cal["VERSION"]
assert str(cal["PRODID"]) == "-//Mayhem//icalendar oracle//EN", cal["PRODID"]

events = list(cal.walk("VEVENT"))
assert len(events) == 1, len(events)
ev = events[0]
assert str(ev["SUMMARY"]) == "Team Meeting", ev["SUMMARY"]
assert str(ev["UID"]) == "oracle-event-1", ev["UID"]
assert ev["DTSTART"].dt.year == 2024, ev["DTSTART"].dt
assert ev["DTSTART"].dt.hour == 10, ev["DTSTART"].dt

# 2) Round-trip: re-serialize and confirm the payload survives.
out = cal.to_ical()
assert isinstance(out, bytes), type(out)
assert b"SUMMARY:Team Meeting" in out, out
assert b"UID:oracle-event-1" in out, out

# 3) Reject clearly malformed input (a content line that cannot be parsed).
try:
    Calendar.from_ical("THIS IS NOT A CALENDAR LINE\r\n")
    raise SystemExit("BUG: malformed input was accepted by from_ical")
except ValueError:
    pass

print(
    "SELFTEST_PASS version=%s summary=%s uid=%s"
    % (str(cal["VERSION"]), str(ev["SUMMARY"]), str(ev["UID"]))
)
