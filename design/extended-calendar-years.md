# Extended years in native date strings

Copyright (C) 2026 Qore Technologies, s.r.o.

The absolute-date string parser recognizes a separated signed or extended year
before consuming month and day. It accumulates the magnitude with a checked
signed-integer limit and checks cooperative cancellation every 100 digits.
Leading zeros do not impose an arbitrary input-length limit. Invalid years
report `INVALID-DATE` through the checked parser; cancellation retains its own
exception and cannot publish a parsed value through `TimeZone::date()`.

Unsigned basic `YYYYMMDD` and compact time forms keep their four-digit year.
A following hyphen can introduce a time or UTC offset, so a longer unsigned
year is distinguished by the following `-MM-DD` fields. This preserves basic
dates such as `20260909-000000` and `20260909000000-03:30`.

Native calendar components remain signed integers and absolute dates remain
64-bit epoch seconds plus microseconds and a timezone. Leap-day counting uses
floor division for negative years. The epoch calculation widens before year
subtraction and leap-day multiplication. Reverse conversion restores the
January/February year before narrowing its March-based intermediate.
Weekdays use an equivalent positive year in the 400-year Gregorian cycle,
avoiding both negative array indexes and overflow at native year boundaries.
The shared week-year rule permits 53 weeks whenever January 1 is Thursday,
including leap years, as well as leap years beginning on Wednesday. Both ISO
week extraction and date construction apply that rule.
The ISO week year can extend one year beyond a native calendar boundary.
An additive `DateTime::getISOWeek(int64&, int&, int&)` overload retains that
result; Qore's week accessors and formatters use the wide year. The legacy
integer-reference overload remains available for existing binary callers.

Formatting `YYYY`, `IF` and ordinary date text emits the sign separately from
the year magnitude, with at least four digits after the sign. Qore's
`get_years()` and `date.years()` obtain the full year through the existing
`DateTime::getInfo()` API. The legacy C++ `getYear()` signature remains `short`
for binary compatibility; C++ callers needing the full component use
`qore_tm::year` from `getInfo()`.

The compact `QoreString(DateTime*)` constructor delegates to the growing string
implementation rather than writing into a fixed 15-byte buffer. Compact date
concatenation and its ISO variant also preserve four magnitude digits for
negative years. Native regressions cover construction and both concatenation
paths at the calendar boundaries.

For example, a historical timestamp can retain its negative year and offset:

```qore
%modern
TimeZone utc(0);
date historical = utc.date("-0004-02-29T12:30:45.123456Z");
@assert(historical.years() == -4);
@assert(historical.format("IF") == "-0004-02-29T12:30:45.123456+00:00");
```

This is Qore's proleptic Gregorian calendar with astronomical year zero.
It does not make the flexible native parser an XSD 1.0 lexical validator.
XML year-zero rules, missing timezones, larger calendar components and
arbitrary fractional precision require the XML layer's separate value contract.

The regressions in `examples/test/qore/vars/extended-years.qtest` compare
extended and negative years against contemporary dates separated by exact
400-year cycles, test malformed/range/calendar rejection and recovery, and
cover both full-year Qore accessors. `extended_year_cancellation.cpp` requests
cancellation before entering the native parser, without scheduling races or
polling, then verifies successful recovery and object cleanup.
