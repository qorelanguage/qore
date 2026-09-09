# Date offsets at the represented instant

Copyright (C) 2026 Qore Technologies, s.r.o.

`date.getUtcOffset()` returns the offset in seconds east of UTC at the date's
represented instant, including the region's daylight-saving transitions. It uses
the date's epoch when looking up the offset. A relative date returns `-1` as
specified by the existing API; `date.relative()` distinguishes that case from an
absolute date with a fixed offset of minus one second.

The timezone manager stores the fallback standard offset as zero until it finds
a non-DST type. Its reverse scan selects the latest such type, including a valid
minus-one-second offset. It no longer reserves an in-band offset value as an
unset marker. Fixed timezone getters and both TimeZone/date serialization thus
retain all integral offsets. No public signature, object layout or serialized
format changes.

For example, a Prague appointment in summer has a two-hour UTC offset:

```qore
TimeZone office("Europe/Prague");
date appointment = office.date("2026-07-01T09:00:00");
@assert(appointment.getUtcOffset() == 7200);
@assert(1h.getUtcOffset() == -1);
```

`examples/test/qore/vars/date-utc-offset.qtest` covers fixed offsets down to
individual seconds, dates before/at/after the epoch, direct and serialized
copies, both DST transitions in Prague and New York, relative values, and an
authored TZif file with a minus-one-second standard offset. The 54-byte fixture
has no transitions or leap records and contains one non-DST `QOR` type. It is
a synthetic regression fixture, not a timezone-database extract.
