# Checked date parsing with an explicit time zone

Copyright (C) 2026 Qore Technologies, s.r.o.

`TimeZone::date(string)` passes its exception sink to the existing absolute-date
parser. Invalid calendar fields and malformed inputs detected by that parser
raise `INVALID-DATE`. The method owns the newly allocated date with
`ReferenceHolder` until parsing succeeds, so an error cannot publish a fallback
date or leak the allocation.

The additive C++ constructor
`DateTimeNode(const AbstractQoreZoneInfo*, const char*, ExceptionSink*)` supplies
both the assumed zone and the error sink in a single parse. Unzoned input uses
the supplied zone, including region daylight saving rules. Explicit ISO offsets
override that zone. Supported HTTP and email formats produce UTC dates.
Existing constructor signatures remain available for binary compatibility.

For example, an appointment imported without an offset can use the business's
configured zone:

```qore
TimeZone office("Europe/Prague");
date appointment = office.date(date_string: "2026-07-01T12:00:00");
# The instant is 2026-07-01T10:00:00Z.
```

The method retains Qore's flexible date syntax. Applications implementing a
specific wire format must still validate that format's lexical grammar and
precision requirements.

`examples/test/qore/vars/timezone-date-errors.qtest` covers invalid input and
recovery, fixed offsets, explicit zones, leap years, region transitions,
HTTP/email formats and the other date overloads. The existing date, date-format,
duration and relative-date suites cover neighboring runtime behavior.
