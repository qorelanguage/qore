# Exact scalar conversion and temporary ownership

Copyright (C) 2026 Qore Technologies, s.r.o.

Non-scientific `number.toString(NF_Raw)` preserves every digit of an integral MPFR
value. MPFR's automatic decimal digit count depends on the source significand
precision. It suffices to round back to that precision, but can denote a different
integer when a consumer parses the text at greater precision. For example, a
128-bit significand with a 333-bit exponent requires more decimal digits than
the automatic count supplies.

For raw integral output, `qore_number_private::getAsString()` requests as many
digits as the binary exponent. A nonzero integer has a positive exponent, and
that exponent bounds the digit count in every supported base. The existing
trailing-zero removal and radix placement then produce the exact integer text.
Zero, nonintegral values, special values, default formatting and scientific
formatting retain their prior paths. No precision or numeric conversion policy
is changed. The MPFR buffer retains its block-exit `mpfr_free_str` ownership.

The reference is the MPFR conversion API:
https://www.mpfr.org/mpfr-current/mpfr.html#Conversion-Functions.
`examples/test/qore/vars/number-raw-integer.qtest` includes a value independently
computed with integer arithmetic, signs, bounds, fractions and special values.
The AOT constant writer also uses raw formatting, so an executable constant
round-trip is included in validation.

`number.toStringRoundTrip(scientific = False)` is an explicit alternative for wire
formats whose numeric binding uses a short decimal spelling. It finds the fewest
significant decimal digits that reconstruct the same MPFR value when parsed with
round-to-nearest at the source's `prec()` bits. Among candidates of that length,
it chooses the nearest decimal, with ties to an even significand. For example,
both `123.45n` and `number("1.00000000000000000001")` retain those short spellings.
The latter's meaningful digits would be removed by the existing display heuristic.
Neither the display heuristic nor arithmetic precision is changed.

MPFR's automatic decimal digit count supplies a guaranteed upper bound. A binary
search finds the smallest digit count with a round-tripping decimal neighbor.
Each step tests nearest, downward and upward rounding; checking nearest alone
would be insufficient at asymmetric binary rounding intervals around powers of
two. The existence predicate is monotone because successive decimal grids are
nested. All temporary MPFR values and strings have scoped ownership.

`float.toStringRoundTrip()` uses C++17 `to_chars` in scientific mode for the
shortest binary64 significand. Plain output expands that same decimal exponent;
it does not ask fixed-format `to_chars` for a different, potentially much longer
integer spelling. The common formatter emits `NaN`, `INF`, `-INF`, and preserves
negative zero. Numeric formatting is locale independent. Decimal exponent
expansion checks cancellation between 64 KiB chunks and commits a completed
result to the caller's destination only on success.

The guarantee is reconstruction of the source binary value at its source
precision. Parsing at a higher precision can produce a different binary value;
these functions are not exact decimal arithmetic and do not recover arbitrary
original decimal text. Exact decimal protocols must retain source decimal text
when available, and apply schema facets to the chosen decimal value without
rounding merely to satisfy a constraint.

`number-round-trip.qtest` covers the public pseudo-methods, meaningful zero/nine
runs, signed zero, special values, IEEE boundaries, exponent expansion and
unchanged formatting. Build `qore-number-round-trip-test` and run
`examples/test/qore/vars/number_round_trip.py --native build-debug/qore-number-round-trip-test
--qore build-debug/qore` with the local Debug library path. This checks an independent
exact rational rounding-interval oracle across 128–8192 bits and a Python binary64
oracle with fixed seeds. The native test also checks destination append semantics,
deterministic pending cancellation, unchanged output on errors and recovery.

`QoreLValue::assignInitial(QoreValue)` can return a temporary node to release
even when the destination has no fixed type. NaN boxing represents large
integers with `QoreBigIntNode`; assignment into a generic lvalue stores the
integer inline and returns the redundant box. `LValueRemoveHelper` discards that
returned node for every initial-assignment path. The former assumption that it
could only return a node for fixed-type destinations caused Debug assertions
and Release leaks during hash/member removal. The stored numeric value remains
owned by the result; cleanup releases only the redundant temporary node.

`examples/test/qore/operators/remove-boxed-integer.qtest` covers hash members,
list indices/slices, object/self members, locals and references. Its explicit
AST Program also exercises ignored map-removal results, so the test reaches
the native helper when the surrounding suite uses the IR interpreter.

Typed integer/float foreach instructions can also allocate numeric fallback
nodes when extracting a list entry. They use `setOwnedValueSlot()` to release
the prior iteration's value and register the new value for cleanup on normal,
return and exception exits. Boolean entries remain inline and string entries
retain their borrowed ownership. `foreach-boxed-numeric.qtest` exercises mixed
inline/boxed integers, negative NaNs, empty lists and control-flow exits in both
IR and AST execution modes; Valgrind detects the former per-iteration leaks.
