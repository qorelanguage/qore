#!/usr/bin/env qore

class T;

# make a circular reference
my T $t = new T();
my $l = list($t);
$t."one" = $l;

