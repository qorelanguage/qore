#!/usr/bin/env qore

%require-our
%requires qore >= 0.8

class T;

# make a circular reference
my T $t = new T();
my $l = list($t);
$t."one" = $l;
