#!/usr/bin/env qore

%require-our

%requires qore >= 0.8

class T { private copy() {} }

my T $t = new T();
# parse error: private copy method
my T $x = $t.copy();

/*
sub test(int $x) {}
sub test(string $x) {}
# parse error: duplicate signature
sub test(int $x) {}
sub test(Mutex $x) {}
# parse error: ambiguous duplicate signature
sub test(object $x) {}

# parse error: cannot overload constructor in builtin class
Mutex::constructor(int $x) {}
# parse error: cannot overload destructor in builtin class
Mutex::destructor() {}
# parse error: cannot overload copy method in builtin class
Mutex::copy() {}
# parse error: duplicate signature
Mutex::lock() {}
*/
