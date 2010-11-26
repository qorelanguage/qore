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

my T $obj7(7);
$obj7.a = $obj7;
$obj7.b = $obj7;

printf("x\n");

my T $obj8(8);
my T $obj9(9);

printf("8=%s\n9=%s\n", dbg_node_info($obj8), dbg_node_info($obj9));

$obj8.a = ("a": $obj9, "b": $obj9);
$obj9.b = $obj8;
$obj9.c = $obj8;
