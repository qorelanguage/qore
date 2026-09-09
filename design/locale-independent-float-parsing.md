# Locale-independent string-to-float parsing

Copyright (C) 2026 Qore Technologies, s.r.o.

`q_strtod()` in `lib/QoreLib.cpp` is the single conversion every string-to-float path uses:
`QoreStringNode::getAsFloat()` and therefore `float()` and `softfloat` argument coercion,
`parse_float()`, `QoreGetOpt` float options, parse-define values, and the lexer's float
literals.

It converts the longest prefix of its argument that forms a valid floating-point number
exactly as `strtod()` does in the classic locale, and returns `0.0` when there is no such
prefix, which is also the result for a null pointer and for empty and whitespace-only input.
Leading classic-locale whitespace is skipped and trailing characters are ignored, so `"1x"`
is `1.0` and `" -2.5tail"` is `-2.5`. The hexadecimal, `inf` and `nan` forms `strtod()`
accepts are accepted too, so a stringified infinite or NaN value converts back to the value
it came from.

The conversion must not be written as a `std::istringstream` extraction. A stream's `num_get`
facet accumulates an implementation-defined set of characters before converting and fails the
whole extraction when the accumulated text does not convert completely, and libstdc++ and
libc++ do not agree on that set: libc++ includes the hexadecimal and `inf`/`nan` letters and
libstdc++ does not. The same program therefore produced different numbers per platform —
`"1x"` converted to `1.0` on Linux and `0.0` on macOS, `"0x10"` to `0.0` and `16.0` — and the
`"1e"` and `NAN` forms converted to `0.0` on both even though `strtod()` accepts them.

Locale independence comes from POSIX.1-2008 per-thread locales rather than from the stream:
`q_strtod()` installs a process-wide classic `locale_t` with `uselocale()` for the duration of
the call. `setlocale()` is not usable here because it is process-wide and would race other
threads; `uselocale()` affects only the calling thread. This is the input-direction counterpart
of `q_fix_decimal()`, which repairs the decimal point on the output side (issue 1556: an
embedding application or an external module can call `setlocale()` at any time and change the
decimal point character from `.` to `,`).

The classic locale is created once by a function-local static and is deliberately never freed:
`q_strtod()` is reachable from static destructors and process-teardown paths that run after a
destructible singleton would already be gone, and using a freed `locale_t` is undefined
behavior. The operating system reclaims it at process exit.

`HAVE_USELOCALE` gates the whole mechanism. It is detected by compiling a probe rather than by
symbol lookup, because `newlocale()`/`uselocale()` are declared in `<xlocale.h>` on Darwin and
in `<locale.h>` elsewhere. Where the probe fails, and where `newlocale()` fails at runtime, the
conversion falls back to `strtod()` in the process locale: still deterministic in the parsing
rules, only no longer immune to `setlocale()`.

`examples/test/qore/vars/float-empty-conversion.qtest` pins the contract: empty and every
classic-locale whitespace kind, inputs with no valid prefix, numeric prefixes with trailing
text, the hexadecimal, `inf` and `nan` forms, infinite and NaN round-trips, and signed zero.
Every case is asserted through both `float()` and `softfloat` argument coercion.
