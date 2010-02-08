#!/usr/bin/env qore

%require-our

%requires qore >= 0.8

class T { private copy() {} private p() {} }

my T $t = new T();
# parse error: private copy method
my T $x = $t.copy();

