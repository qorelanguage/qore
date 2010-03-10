#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-

%require-our
%require-types

%requires qore >= 0.8

# return value but no return type declared
sub rt() { return 1; }

class T { private constructor() {} private copy() {} private p() { $.copy(1); } }

# parse error: private constructor
my T $t = new T();
# parse error: private copy method
my T $x = $t.copy();

