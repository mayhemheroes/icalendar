#!/usr/bin/env python3
import random

import atheris
import sys
import fuzz_helpers
import random

with atheris.instrument_imports(include=['icalendar']):
    from icalendar import Calendar


def TestOneInput(data):
    fdp = fuzz_helpers.EnhancedFuzzedDataProvider(data)
    try:
        Calendar.from_ical(fdp.ConsumeRemainingString())
    except ValueError:
        return -1
    except (TypeError, IndexError):
        if random.random() > 0.50:
            raise

def main():
    atheris.Setup(sys.argv, TestOneInput)
    atheris.Fuzz()


if __name__ == "__main__":
    main()
