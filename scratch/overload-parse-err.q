#!/usr/bin/env qore

%require-our

%requires qore >= 0.8

sub test(int $x) {}
sub test(string $x) {}
# parse error: duplicate signature
sub test(int $x) {}
sub test(Mutex $x) {}
# parse error: ambiguous duplicate signature
sub test(int $x, object $y = new Mutex()) {}

# parse error: cannot overload constructor in builtin class
Mutex::constructor(int $x) {}
# parse error: cannot overload destructor in builtin class
Mutex::destructor() {}
# parse error: cannot overload copy method in builtin class
Mutex::copy() {}
# parse error: duplicate signature
Mutex::lock() {}
# parse error: ambiguous signature
File::write(string $fn, int $len = 0) returns int {}
