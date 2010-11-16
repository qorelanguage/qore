#!/usr/bin/env qore

%require-our
%requires qore >= 0.8

class T { private { int $.id; } constructor(int $id) { $.id = $id; } destructor() { printf("deleting %d\n", $.id); } }

# make circular references
my T $obj1(1);
$obj1.a = $obj1;

my T $obj2(2);
$obj2.a = $obj2;

my T $obj3(3);
$obj3.a.a = $obj3;

my T $obj4(4);
my list $l = list($obj4);
$obj4.one = $l;

my T $obj5(5);
my T $obj6(6);
$obj5.a = $obj6;
$obj6.b = $obj5;
