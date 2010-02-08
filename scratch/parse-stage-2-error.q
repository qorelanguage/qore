#!/usr/bin/env qore

%require-our

%requires qore >= 0.8

class T { private constructor() {} private copy() {} private p() { $.copy(1); } }

# parse error: private constructor
my T $t = new T();
# parse error: private copy method
my T $x = $t.copy();

