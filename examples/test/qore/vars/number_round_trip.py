#!/usr/bin/env python3
"""Independent exact-rational and Python binary64 oracles for round-trip formatting.

Build qore-number-round-trip-test, then run this script with --native and --qore
pointing to the local Debug executables. No third-party Python packages are used.
Copyright (C) 2026 Qore Technologies, s.r.o.
"""
import argparse
from fractions import Fraction
import math
import random
import struct
import subprocess
import sys


def power10(exponent):
    return Fraction(10**exponent) if exponent >= 0 else Fraction(1, 10**-exponent)


def plain(value):
    """Exact finite decimal text; denominator must contain only factors two/five."""
    sign = "-" if value < 0 else ""
    numerator, denominator = abs(value).as_integer_ratio()
    twos = fives = 0
    while denominator % 2 == 0:
        denominator //= 2
        twos += 1
    while denominator % 5 == 0:
        denominator //= 5
        fives += 1
    assert denominator == 1
    places = max(twos, fives)
    digits = str(numerator * 2**(places - twos) * 5**(places - fives))
    if not places:
        return sign + digits
    digits = digits.rjust(places + 1, "0")
    return sign + (digits[:-places] + "." + digits[-places:]).rstrip("0").rstrip(".")


def shortest(mantissa, precision, exponent):
    """Solve the exact binary rounding interval on nested decimal integer grids.

    This oracle uses only integers/Fraction: no MPFR, binary-to-decimal library,
    formatter output, or conversion back into the implementation under test.
    """
    gap = Fraction(2) ** (exponent - precision + 1)
    value = mantissa * gap
    lower = value - gap / (4 if mantissa == 1 << (precision - 1) else 2)
    upper = value + gap / 2
    closed = not mantissa % 2
    decade = len(str(value.numerator)) - len(str(value.denominator))
    if value < power10(decade):
        decade -= 1
    assert power10(decade) <= value < power10(decade + 1)

    def interval(digits):
        unit = power10(decade - digits + 1)
        low, high = lower / unit, upper / unit
        first = -(-low.numerator // low.denominator)
        last = high.numerator // high.denominator
        if not closed:
            first += low.denominator == 1
            last -= high.denominator == 1
        return first, last, unit

    lo, hi = 1, precision * 30103 // 100000 + 3
    while lo < hi:
        mid = (lo + hi) // 2
        first, last, _ = interval(mid)
        if first <= last:
            hi = mid
        else:
            lo = mid + 1
    first, last, unit = interval(lo)
    assert first <= last
    if lo > 1:
        a, b, _ = interval(lo - 1)
        assert a > b
    scaled = value / unit
    floor = scaled.numerator // scaled.denominator
    remainder = scaled - floor
    nearest = floor + (remainder > Fraction(1, 2) or (remainder == Fraction(1, 2) and floor % 2))
    chosen = min(last, max(first, nearest)) * unit
    assert lower <= chosen <= upper
    if not closed:
        assert lower < chosen < upper
    return value, chosen


def native_cases():
    rng = random.Random(0xDEC1A12026)
    cases = []
    for precision in (128, 129, 200, 512, 8192):
        low = 1 << (precision - 1)
        for exponent in (-1000, -100, -1, 0, 1, 10, 100, 1000):
            for mantissa in (low, low + 1, 2 * low - 1, low | rng.getrandbits(precision - 1)):
                source, expected = shortest(mantissa, precision, exponent)
                for sign in (1, -1):
                    cases.append((precision, plain(sign * source), plain(sign * expected)))
    return cases


def run_native(executable):
    cases = native_cases()
    data = "".join(f"{precision} {source}\n" for precision, source, _ in cases)
    result = subprocess.run([executable], input=data, text=True, capture_output=True, timeout=180)
    assert result.returncode == 0, (result.returncode, result.stderr)
    assert not result.stderr, result.stderr
    rows = result.stdout.splitlines()
    assert len(rows) == len(cases), (len(rows), len(cases))
    for index, (row, (precision, source, expected)) in enumerate(zip(rows, cases)):
        fixed, scientific = row.split("\t")
        assert fixed == expected, (index, precision, source, expected, fixed)
        assert Fraction(scientific) == Fraction(expected), (index, scientific, expected)
        assert "e" not in fixed and "e" in scientific
    print(f"Exact rational oracle: {len(cases)} numbers, two formats, precision 128–8192 bits; native cancellation passed")


def run_floats(qore):
    rng = random.Random(0xB1642026)
    values = [0.1, 123.45, math.nextafter(0.0, 1.0), sys.float_info.min, sys.float_info.max]
    for exponent in (-1022, -100, -1, 0, 1, 100, 1023):
        value = math.ldexp(1.0, exponent)
        values.extend((value, math.nextafter(value, 0.0), math.nextafter(value, math.inf)))
    for _ in range(1024):
        value = struct.unpack("!d", rng.getrandbits(64).to_bytes(8, "big"))[0]
        if math.isfinite(value) and value:
            values.append(value)
    # Qore receives already pinned decimal spellings, never executable input.
    script = '''sub() { while (*string line = stdin.readLine()) {
        float value = float(line);
        printf("%s\\t%s\\n", value.toStringRoundTrip(), value.toStringRoundTrip(True));
    } }()'''
    result = subprocess.run([qore, "-b", "--enable-debug", "-e", "%modern\n" + script + ";"],
                            input="".join(repr(value) + "\n" for value in values), text=True,
                            capture_output=True, timeout=60)
    assert result.returncode == 0, (result.returncode, result.stderr)
    assert not result.stderr, result.stderr
    rows = result.stdout.splitlines()
    assert len(rows) == len(values), (len(rows), len(values))
    for row, value in zip(rows, values):
        fixed, scientific = row.split("\t")
        expected = Fraction(repr(value))
        assert Fraction(fixed) == expected, (value, fixed, expected)
        assert Fraction(scientific) == expected, (value, scientific, expected)
        assert float(fixed) == value == float(scientific)
        assert "e" not in fixed
    print(f"Python binary64 oracle: {len(values)} floats, two formats; seeds 0xDEC1A12026 and 0xB1642026")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--native", required=True)
    parser.add_argument("--qore", required=True)
    args = parser.parse_args()
    # The finite 8192-bit dyadics require up to 9200 fractional decimal places.
    if hasattr(sys, "set_int_max_str_digits"):
        sys.set_int_max_str_digits(20000)
    run_native(args.native)
    run_floats(args.qore)


if __name__ == "__main__":
    main()
