#!/usr/bin/env qore 
# -*- mode: qore; indent-tabs-mode: nil -*-

# require all global variables to be declared with "our"
%require-our
# enable all warnings
%enable-all-warnings
# child programs do not inherit parent's restrictions
%no-child-restrictions
# require types to be declared
%require-types

# make sure we have the right version of qore
%requires qore >= 0.8.11

%append-module-path ../../qlib
%requires UnitTest

# for Mime tests
%requires Mime

# for Mapper tests
%requires Mapper

# for CsvUtil tests
%requires CsvUtil

# global variables needed for tests
our Test $to("program-test.q");
our Test $ro("readonly");


our UnitTest $unit();


sub test_xrange(list $correct, RangeIterator $testing, string $message) {
    my list $l;
    foreach my int $i in ($testing)
        push $l, $i;

    $unit.cmp($correct, $l, $message);
}

int sub test1() { return 1;} int sub test2() { return 2; } 
list sub test3() { return (1, 2, 3); }

sub array_helper(list $a) {
    $a[1][1] = 2;
    $unit.cmp($a[1][1], 2, "passed local array variable assignment");
}

list sub list_return(any $var) {
    return (1, test2(), $var);
}

hash sub hash_return(any $var) {
    return ( "gee" : "whiz", 
	     "num" : test1(),
	     "var" : $var );
}

class Sort {
    int hash(hash $l, hash $r) {
	return $l.key1 <=> $r.key1;
    }
}
int sub hash_sort_callback(hash $l, hash $r) {
    return $l.key1 <=> $r.key1;
}

class SC;
static int SC::hash_sort_callback(hash $l, hash $r) {
    return $l.key1 <=> $r.key1;
}

# array tests
sub array_tests() {
    my (list $a, list $b, list $c, list $d);

    if ($unit.verbose())
	print("%%%% array tests\n");
    $a = 1, 2, 3, 4, 5;
    $unit.cmp(elements $a, 5, "elements operator");
    $unit.cmp($a[1], 2, "single-dimensional list dereference");
    $b = 1, 2, (3, 4, 5), 6, 7;
    $unit.cmp($b[2][1], 4, "multi-dimensional list dereference");
    delete $b;
    $unit.cmp($b[2][1], NOTHING, "multi-dimensional list dereference after delete operator");
    $b = $a;
    $a[1] = "hello";
    $unit.cmp($a[1], "hello", "list dereference after list assignment and element reassignment");
    $unit.cmp($b[1], 2, "list dereference of source list");
    $a[0][1] = "hello";
    $c[10]{"testing"} = "well then";
    $unit.cmp($a[0][1], "hello", "second multi-dimensional list dereference");
    $unit.cmp($a[1][500], NOTHING, "non-existent element deference");
    $unit.cmp(int($c[10].testing), 0, "hash list element dereference");
    $unit.cmp($c[10]{"testing"}, "well then", "hash element in list dereference");
    $d = test1(), test2();
    $unit.cmp($d[1], 2, "list element dereference with evaluation");
    $b = $a = 1, 2, 3;
    delete $a[2];
    $unit.cmp($a[2] != $b[2], True, "shared list element comparison after delete");
    $a[1][1] = 3;
    $unit.cmp($a[1][1], 3, "array variable assignment before copy");
    array_helper($a);
    $unit.cmp($a[1][1], 3, "array variable assignment after copy");
    array_helper($a);
    $unit.cmp(list_return()[0], 1, "simple list return and deref(e)");
    $unit.cmp(list_return()[1], 2, "list return with function element result and deref(e)");
    $unit.cmp(list_return("gee")[2], "gee", "list return with local variable result and deref(e)");
    $a = 1, 2, 3;
    $a += 4, 5, 6;
    $unit.cmp($a[3], 4, "first list list plus-equals concatenation");
    $a += 7;
    $unit.cmp($a[6], 7, "list element plus-equals concatenation");
    $a += list(8);
    $unit.cmp($a[7], 8, "second list list plus-equals concatenation");
    $a = (1, 2, 3) + (4, 5, 6);
    $unit.cmp($a[3], 4, "first list list plus operator concatenation");
    $a = 1, 2, 3;
    $b = 4, 5, 6;
    $c = $a + $b;
    $unit.cmp($c[4], 5, "second list list plus operator concatenation");
    my list $l1 = ( 3, 2, 4, 1, 6, 5 );
    my list $l2 = ( "one", "two", "three", "four", "five", "six" );
    my list $hl = 
	( ( "key1" : 8, "key2" : "two" ),
	  ( "key1" : 2, "key2" : "seven" ),
	  ( "key1" : 7, "key2" : "six" ),
	  ( "key1" : 1, "key2" : "eight" ),
	  ( "key1" : 6, "key2" : "four" ),
	  ( "key1" : 9, "key2" : "three" ),
	  ( "key1" : 3, "key2" : "five" ),
	  ( "key1" : 5, "key2" : "nine" ),
	  ( "key1" : 4, "key2" : "one" ) );
    my list $sorted_hl = 
	( ( "key1" : 1, "key2" : "eight" ),
	  ( "key1" : 2, "key2" : "seven" ),
	  ( "key1" : 3, "key2" : "five" ),
	  ( "key1" : 4, "key2" : "one" ),
	  ( "key1" : 5, "key2" : "nine" ),
	  ( "key1" : 6, "key2" : "four" ),
	  ( "key1" : 7, "key2" : "six" ),
	  ( "key1" : 8, "key2" : "two" ),
	  ( "key1" : 9, "key2" : "three" ) );
    my list $stable_sorted_hl = 
	( ( "key1" : 1, "key2" : "eight" ),
	  ( "key1" : 2, "key2" : "seven" ),
	  ( "key1" : 3, "key2" : "five" ),
	  ( "key1" : 3, "key2" : "five-o" ),
	  ( "key1" : 4, "key2" : "one" ),
	  ( "key1" : 5, "key2" : "nine" ),
	  ( "key1" : 6, "key2" : "four" ),
	  ( "key1" : 7, "key2" : "six" ),
	  ( "key1" : 8, "key2" : "two" ),
	  ( "key1" : 9, "key2" : "three" ) );
    my Sort $s();

    my code $hash_compare = int sub (hash $l, hash $r) { return $l.key1 <=> $r.key1; };

    $unit.cmp(sort($l1), (1,2,3,4,5,6), "first sort()");
    $unit.cmp(sort($l2), ("five", "four", "one", "six", "three", "two"), "second sort()");
    $unit.cmp(sort($hl, \hash_sort_callback()), $sorted_hl, "sort() with function call reference callback");
    $unit.cmp(sort($hl, \$s.hash()), $sorted_hl, "sort() with object method callback");
    $unit.cmp(sort($hl, "hash_sort_callback"), $sorted_hl, "sort() with string function name callback");
    $unit.cmp(sort($hl, $hash_compare), $sorted_hl, "sort() with closure callback");

    my list $r_sorted_hl = reverse($sorted_hl);
    $unit.cmp(sortDescending($l1), (6,5,4,3,2,1), "first sortDescending()");
    $unit.cmp(sortDescending($l2), ("two", "three", "six", "one", "four", "five"), "second sortDescending()");
    $unit.cmp(sortDescending($hl, \SC::hash_sort_callback()), $r_sorted_hl, "first sortDescending() with callback");
    $unit.cmp(sortDescending($hl, \$s.hash()), $r_sorted_hl, "second sortDescending() with callback");
    $unit.cmp(sortDescending($hl, "hash_sort_callback"), $r_sorted_hl, "third sortDescending() with callback");
    $unit.cmp(sortDescending($hl, $hash_compare), $r_sorted_hl, "sortDescending() with closure callback");

    $hl += ( "key1" : 3, "key2" : "five-o" );
    $unit.cmp(sortStable($hl, \hash_sort_callback()), $stable_sorted_hl, "first sortStable() with callback");
    $unit.cmp(sortStable($hl, \$s.hash()), $stable_sorted_hl, "second sortStable() with callback");
    $unit.cmp(sortStable($hl, "hash_sort_callback"), $stable_sorted_hl, "third sortStable() with callback");
    $unit.cmp(sortStable($hl, $hash_compare), $stable_sorted_hl, "sortStable() with closure callback");

    my list $r_stable_sorted_hl = reverse($stable_sorted_hl);
    $unit.cmp(sortDescendingStable($hl, \SC::hash_sort_callback()), $r_stable_sorted_hl, "first sortDescendingStable() with callback");
    $unit.cmp(sortDescendingStable($hl, \$s.hash()), $r_stable_sorted_hl, "second sortDescendingStable() with callback");
    $unit.cmp(sortDescendingStable($hl, "hash_sort_callback"), $r_stable_sorted_hl, "third sortDescendingStable() with callback");
    $unit.cmp(sortDescendingStable($hl, $hash_compare), $r_stable_sorted_hl, "sortDescendingStable() with closure callback");

    $unit.cmp(min($l1), 1, "simple min()");
    $unit.cmp(max($l1), 6, "simple max()");
    $unit.cmp(min($hl, \hash_sort_callback()), ( "key1" : 1, "key2" : "eight" ), "first min() with callback");
    $unit.cmp(min($hl, \$s.hash()), ( "key1" : 1, "key2" : "eight" ), "second min() with callback");
    $unit.cmp(min($hl, "hash_sort_callback"), ( "key1" : 1, "key2" : "eight" ), "third min() with callback");
    $unit.cmp(max($hl, \SC::hash_sort_callback()), ( "key1" : 9, "key2" : "three" ), "first max() with callback");
    $unit.cmp(max($hl, \$s.hash()), ( "key1" : 9, "key2" : "three" ), "second max() with callback");
    $unit.cmp(max($hl, "hash_sort_callback"), ( "key1" : 9, "key2" : "three" ), "third max() with callback");
    my string $v = shift $l2;
    $unit.cmp($l2, ("two","three","four","five","six"), "array shift");
    unshift $l2, $v;
    $unit.cmp($l2, ("one","two","three","four","five","six"), "array unshift");
    # list assignment tests
    my list $l[1] = "boo";
    ($l[0], $l[1]) = "hi1";
    $unit.cmp($l, ("hi1", NOTHING), "first list assigment");
    ($l[0], $l[1]) = ("hi2", "shoo1");
    $unit.cmp($l, ("hi2", "shoo1"), "second list assigment");
    ($l[0], $l[1]) = ("hi3", "shoo2", "bean1");
    $unit.cmp($l, ("hi3", "shoo2"), "third list assigment");
    my int $v2 = pop $l1;
    $unit.cmp($v2, 5, "first pop");
    $unit.cmp($l1, (3,2,4,1,6), "second pop");
    push $l1, "hi";

    # splice tests
    $unit.cmp($l1, (3,2,4,1,6,"hi"), "push");
    splice $l1, 5;
    $unit.cmp($l1, (3,2,4,1,6), "first list splice");
    splice $l1, 3, 1;
    $unit.cmp($l1, (3,2,4,6), "second list splice");
    splice $l1, 1, 2, (4, 5, 5.5);
    $unit.cmp($l1, (3,4,5,5.5,6), "third list splice");
    splice $l1, 0, 4, (10, 11, 12);
    $unit.cmp($l1, (10, 11, 12, 6), "third list splice");
    splice $l1, 0, 1;
    $unit.cmp($l1, (11, 12, 6), "fourth list splice");
    splice $l1, 5, 2, (1, 2, 3);
    $unit.cmp($l1, (11, 12, 6, 1, 2, 3), "fifth list splice");
    splice $l1, -4, 2, 9;
    $unit.cmp($l1, (11, 12, 9, 2, 3), "sixth list splice");
    splice $l1, -4, -2, (21, 22, 23);
    $unit.cmp($l1, (11, 21, 22, 23, 2, 3), "seventh list splice");

    # extract tests
    $unit.cmp((extract $l1, 5), list(3), "first list extract");
    $unit.cmp((extract $l1, 2, 2), (22, 23), "second list extract");
    $unit.cmp((extract $l1, 1, 2, 4), (21, 2), "second list extract");
    $unit.cmp($l1, (11, 4), "final list extract");

    my string $astr = "hello";
    $unit.cmp($astr[2], "l", "string element dereference");
    my binary $bin = binary($astr);
    $unit.cmp($bin[4], ord("o"), "binary byte dereference");
    
    # range tests
    $unit.cmp(range(1), (0, 1,), "range - basic test");
    $unit.cmp(range(2, 5), (2, 3, 4, 5), "range - boundaries test");
    $unit.cmp(range(2, -2), (2, 1, 0, -1, -2), "range - descending test");
    $unit.cmp(range(1, 10, 5), (1, 6), "range - step test");
    $unit.cmp(range(0, 10, 5), (0, 5, 10), "range - step from 0");
    $unit.cmp(range(-10, 10, 5), (-10, -5, 0, 5, 10), "range - asc test");
    $unit.cmp(range(10, -10, 5), (10, 5, 0, -5, -10), "range - descending step test");
    # xrange tests
    test_xrange(range(1), xrange(1), "xrange - basic test");
    test_xrange(range(2, 5), xrange(2, 5), "xrange - boundaries test");
    test_xrange(range(2, -2), xrange(2, -2), "xrange - descending test");
    test_xrange(range(1, 10, 5), xrange(1, 10, 5), "xrange - step test");
    test_xrange(range(0, 10, 5), xrange(0, 10, 5), "xrange - step from 0");
    test_xrange(range(-10, 10, 5), xrange(-10, 10, 5), "xrange - asc test");
    test_xrange(range(10, -10, 5), xrange(10, -10, 5), "xrange - descending step test");

    # pseudomethods
    my list $pseudoList = (1, 2, 3, 4, 'a');
    $unit.cmp($pseudoList.typeCode(), NT_LIST, "<list>::typeCode");
    $unit.cmp($pseudoList.size(), 5, "<list>::size");
    $unit.cmp($pseudoList.empty(), False, "<list>::empty");
    $unit.cmp($pseudoList.val(), True, "<list>::val");
    $unit.cmp($pseudoList.first(), 1, "<list>::first");
    $unit.cmp($pseudoList.last(), 'a', "<list>::last");
    $unit.cmp($pseudoList.join('-'), '1-2-3-4-a', "<list>::join");
    $unit.cmp($pseudoList.lsize(), 5, "<list>::lsize");
    $unit.cmp($pseudoList.contains(2), True, "<list>::contains");
    #$pseudoList.append(6);
    #$unit.cmp($pseudoList, (1, 2, 3, 4, 'a', 6), "<list>::append");
}

sub hash_tests() {
    if ($unit.verbose())
	print("%%%% hash tests\n");
    # hash tests
    my hash $b = ( "test" : 1, "gee" : 2, "well" : "string" );
    $unit.cmp($b.gee, 2, "object dereference");
    $unit.cmp(elements $b, 3, "elements operator on hash before delete");
    delete $b{"gee"};
    $unit.cmp(elements $b, 2, "elements operator on hash after delete");
    $b{"test"} = "there";
    my hash $d{"gee"}[25] = "I hope it works";
    $unit.cmp($b.test, "there", "hash dereference after assignment");
    $unit.cmp($b.test, "there", "object dereference after assignment");
    $unit.cmp($b{"geez"}, NOTHING, "non-existent object dereference");
    $unit.cmp(int($d.gee[25]), 0, "hash dereference of list element");
    $unit.cmp($d{"gee"}[25], "I hope it works", "dereference of list member of hash");
    my hash $c = ( "hi" : "there", "gee" : "whillakers" );
    $d = $c;
    $unit.cmp($d == $c, True, "hash comparison");
    $unit.cmp($d.gee, "whillakers", "hash dereference after entire hash assignment");
    $c{"gee"} = "roo";
    $unit.cmp($c{"gee"}, "roo", "original hash dereference after assignment to member of copied hash");
    $unit.cmp($d.gee, "whillakers", "hash dereference of member of copied hash");
    $d = ( "gee" : test1(), "howdy" : test2());
    $unit.cmp($d.gee, 1, "hash dereference with evaluation");
    $unit.cmp(hash_return(){"gee"}, "whiz", "simple hash return and dereference");
    $unit.cmp(hash_return(){"num"}, 1, "hash return with function element result and dereference");
    $unit.cmp(hash_return("hi there"){"var"}, "hi there", "hash return with local variable result and dereference");
    my hash $a = ( "key" : 1, "unique" : 100, "asd" : "dasd" );
    $b = ( "key" : 3, "new" : 45, "barn" : "door" );
    $c = $a + $b;
    $unit.cmp($c.key, 3, "hash plus operator element override");
    $unit.cmp($c."new", 45, "hash plus operator new element");
    $unit.cmp($c.unique, 100, "hash plus operator unchanged element");
    $a += $b;
    $unit.cmp($a.key, 3, "hash plus equals operator element override");
    $unit.cmp($a."new", 45, "hash plus equals operator new element");
    $unit.cmp($a.unique, 100, "hash plus equals operator unchanged element");

    # test hash slice creation
    $unit.cmp($a.("unique", "new"), ("unique" : 100, "new" : 45), "hash slice creation");

    my Test $ot(1, "two", 3.0);
    $ot += $a;
    $unit.cmp($ot.("unique", "new"), ("unique" : 100, "new" : 45), "hash slice creation from object");

    # use the foreach ... in (keys <hash>) specialization
    my int $cnt = 0;
    foreach my string $k in (keys $c) {
        # to avoid unused local var warning
        delete $k;
        ++$cnt;
    }
    $unit.cmp($cnt, 5, "foreach hash keys specialization");
    # do pseudo-method tests
    $unit.cmp($c.firstKey(), "key", "<hash>.firstKey()");
    $unit.cmp($c.lastKey(), "barn", "<hash>.lastKey()");
    $unit.cmp($c.size(), 5, "<hash>.size()");

    my hash $nch = $c.("key", "barn");
    foreach my hash $hi in ($nch.pairIterator()) {
        if (!$#)
            $unit.cmp($hi.key, "key", "HashIterator::first()");
        else if ($# == 4)
            $unit.cmp($hi.key, "barn", "HashIterator::last()");
    }

    my HashPairReverseIterator $hi($nch);
    foreach my hash $hiv in ($hi) {
        if ($# == 4)
            $unit.cmp($hiv.key, "key", "HashReverseIterator::last()");
        else if (!$#)
            $unit.cmp($hiv.key, "barn", "HashReverseIterator::first()");
    }
    $unit.cmp($hi.valid(), False, "HashReverseIterator::valid()");
    # restart iterator
    $unit.cmp($hi.next(), True, "HashReverseIterator::next()");
    $unit.cmp($hi.getKey(), "barn", "HashReverseIterator::getKey()");
    $hi.reset();
    $unit.cmp($hi.valid(), False, "HashReverseIterator::valid() after reset");

    # delete 3 keys from the $c hash
    $b = $c - "new" - "barn" - "asd";
    $unit.cmp($b, ( "key" : 3, "unique" : 100 ), "hash minus operator"); 
    $b = $c - ("new", "barn", "asd");
    $unit.cmp($b, ( "key" : 3, "unique" : 100 ), "hash minus operator with list argument"); 
    $b -= "unique";
    $unit.cmp($b, ( "key" : 3 ), "hash minus-equals operator"); 
    $c -= ( "new", "barn" );
    $unit.cmp($c, ( "key": 3, "unique" : 100, "asd" : "dasd" ), "hash minus-equals operator with list argument");
    my hash $nh += ( "new-hash" : 1 );
    $unit.cmp($nh, ( "new-hash" : 1 ), "hash plus-equals, lhs NOTHING");
}

sub global_variable_testa() {
    printf("user=%s\n", $ENV{"USER"});
}

code sub map_closure(any $v) { return any sub(any $v1) { return $v * $v1; }; }

# operator tests
sub operator_test() {
    if ($unit.verbose())
	print("%%%% operator tests\n");
    my int $a = 1;
    $unit.cmp($a, 1, "variable assignment");
    $a += 3;
    $unit.cmp($a, 4, "integer += operator");
    $a -= 2;
    $unit.cmp($a, 2, "integer -= operator");
    $a |= 1;
    $unit.cmp($a, 3, "|= operator");
    $a &= 1;
    $unit.cmp($a, 1, "&= operator");
    $a *= 10;
    $unit.cmp($a, 10, "integer *= operator");
    my float $f = $a;
    $f *= 2.2;
    $unit.cmp($f, 22.0, "first float *= operator");
    $f *= 2;
    $unit.cmp($f, 44.0, "second float *= operator");
    $f /= 4.4;
    $unit.cmp($f, 10.0, "float /= operator");
    $a = 10;
    $a /= 2;
    $unit.cmp($a, 5, "integer /= operator");
    $unit.cmp(4 / 2, 2, "first / operator");
    $a = 0xfdb4902a;
    $a ^= 0xbf40e848;
    $unit.cmp($a, 0x42f47862, "^= xor equals operator");
    $a <<= 2;
    $unit.cmp($a, 0x10bd1e188, "<<= shift-left-equals operator");
    $a >>= 3;
    $unit.cmp($a, 0x217a3c31, ">>= shift-right-equals operator");
    $a = 1;
    $unit.cmp($a++, 1, "pre post-increment (++) operator");
    $unit.cmp($a, 2, "post post-increment (++) operator");
    $unit.cmp($a--, 2, "pre post-decrement (--) operator");
    $unit.cmp($a, 1, "post post-decrement (--) operator");
    $unit.cmp(++$a, 2, "pre-increment (++) operator");
    $unit.cmp(--$a, 1, "pre-decrement (--) operator");

    my string $astr = "hello" + " there";
    $unit.cmp($astr, "hello there", "string concatenation");
    $astr += " gee";
    $unit.cmp($astr, "hello there gee", "string plus equals");

    $f = 1.0;
    $f += 1.2;
    $unit.cmp($f, 2.2, "float += operator");
    $f -= 1.1;
    $unit.cmp($f, 1.1, "float -= operator");
    $f = 5.5 * 2.0;
    $unit.cmp($f, 11.0, "float * operator");

    $unit.cmp(now() > (now() - 1D), True, "date > operator");
    $unit.cmp(now() >= (now() - 1h), True, "date >= operator");
    $unit.cmp((now() - 1m) < now(), True, "date < operator");
    $unit.cmp((now() - 1M) <= now(), True, "date <= operator");

    my date $bt = my date $at = now();
    $unit.cmp($at, $bt, "date == operator");
    $at = 2004-02-28-12:00:00;
    $at += 1D;
    $unit.cmp($at, 2004-02-29-12:00:00, "first date += operator");
    $at -= (3h + 5m);
    $unit.cmp($at, 2004-02-29-08:55:00, "second date += operator");

    my any $ni += 3.2;
    $unit.cmp($ni, 3.2, "float +=, lhs NOTHING");
    delete $ni;
    $ni += "hello";
    $unit.cmp($ni, "hello", "string +=, lhs NOTHING");
    delete $ni;
    $ni -= 4.5;
    $unit.cmp($ni, -4.5, "float -=, lhs NOTHING");
    delete $ni;
    $ni -= 4;
    $unit.cmp($ni, -4, "integer -=, lhs NOTHING");
    # some array and hash tests in separate functions

    # get function closure with bound local variable (multiply by 2)
    my code $c = map_closure(2);

    # map function to list
    $unit.cmp((map $c($1), (1, 2, 3)), (2, 4, 6), "map operator using closure");

    # map immediate expression to list
    $unit.cmp((map $1 * 2, (1, 2, 3)), (2, 4, 6), "map operator using expression");

    # map function to list with optional select code as expression
    $unit.cmp((map $c($1), (1, 2, 3), $1 > 1), (4, 6), "map operator using closure with optional select expression");

    # select all elements from list greater than 1 with expression
    $unit.cmp((select (1, 2, 3), $1 > 1), (2, 3), "select operator with expression");

    # create a sinple closure to subtract the second argument from the first
    $c = any sub(any $x, any $y) { return $x - $y; };

    # left fold function on list using closure
    $unit.cmp((foldl $c($1, $2), (2, 3, 4)), -5, "foldl operator with closure");

    # left fold function on list using expression
    $unit.cmp((foldl $1 - $2, (2, 3, 4)), -5, "foldl operator with expression");

    # right fold function on list using immediate closure
    $unit.cmp((foldr $c($1, $2), (2, 3, 4)), -1, "foldr operator with closure");

    # right fold function on list using expression and implicit arguments
    $unit.cmp((foldr $1 - $2, (2, 3, 4)), -1, "foldr operator with expression");

    my hash $h = ("test" : 1, "two" : 2.0, "three" : "three", "four" : False );
    $unit.cmp(remove $h.two, 2.0, "first remove operator");
}

sub no_parameter_test(any $p) {
    $unit.cmp($p, NOTHING, "non-existent parameter");
}

sub parameter_and_shift_test(int $p) {
    $unit.cmp($p, 1, "parameter before shift");
    $unit.cmp(shift $argv, 2, "shift on second parameter");
}

sub one_parameter_shift_test() {
    $unit.cmp(shift $argv, 1, "one parameter shift");
}

sub shift_test() {
    my list $var = (1, 2, 3, 4, "hello");
    foreach my any $v in ($var)
	$unit.cmp($v, shift $argv, ("shift " + string($v) + " parameter"));
}

sub parameter_tests() {
    no_parameter_test();
    parameter_and_shift_test(1, 2);
    shift_test(1, test3()[1], 3, 4, "hello");
    one_parameter_shift_test(1);
}

bool sub short_circuit_test(string $op) {
    print("ERROR: %n logic short-circuiting is not working!\n", $op);
    $unit.errorInc();
    return False;
}

sub logic_message(string $op) {
    if ($unit.verbose())
	printf("OK: %s logic test\n", $op);
}

# logic short-circuiting test
sub logic_tests() {
    my any $a = 1;
    my any $b = 0;
    my int $c;

    if ($unit.verbose())
	print("%%%% logic tests\n");
    if ($a || short_circuit_test("or"))
	logic_message("short-circuit or");
    if ($b && short_circuit_test("and"))
	$c = 1;
    else
	logic_message("short-circuit and");
    if ($a && 1)
	logic_message("and");
    if ($b || 1)
	logic_message("or");
    $unit.cmp($b ? 0 : 1, 1, "first question-colon");
    $unit.cmp($a ? 1 : 0, 1, "second question-colon");
    $a = 1;
    $b = "1";
    $unit.cmp($a == $b, True, "comparison with type conversion");
    $unit.cmp($a === $b, False, "absolute comparison");
    $a = 1, 2, 3, 4;
    $b = 1, 2, 3, 4;
    $unit.cmp($a == $b, True, "list comparison");
    delete $b[3];
    $unit.cmp($a == $b, False, "list comparison after delete");
    $a[3] = ("gee" : 1, "whillakers" : 2, "list" : ( 1, 2, "three" ));
    $b[3] = $a[3];
    $unit.cmp($a == $b, True, "complex list comparison");
    $unit.cmp($a[3] == $b[3], True, "hash comparison");
    $unit.cmp($a[3] != $b[3], False, "negative hash unequal comparison");
    $a[3].chello = "hi";
    $unit.cmp($a[3] == $b[3], False, "negative hash comparison");
    $unit.cmp($a[3] != $b[3], True, "hash unequal comparison");
}

sub printf_tests() {
    # some printf tests
    printf("field tests\n");
    f_printf("f_printf: 5 character field with 7 char arg: %5s\n", "freddy1");
    printf(  "  printf: 5 character field with 7 char arg: %5s\n", "freddy1");
    printf("printf alignment tests\n");
    f_printf("f_printf: 3 char arg left  in 5 char field: %-5s\n", "abc"); 
    printf(  "  printf: 3 char arg left  in 5 char field: %-5s\n", "abc"); 
    f_printf("f_printf: 3 char arg right in 5 char field: %5s\n", "abc"); 
    printf(  "  printf: 3 char arg right in 5 char field: %5s\n", "abc"); 
}

const TH = ("a":1);

string sub switch_test(any $val) {
    my string $rv;

    switch ($val) {
	case 0:
	case "hello":
	
	case TH.a:
	    $rv = "case 1";
            break;

	case 2:
	    $rv = "case 2";

        default:
	    return "default";
    }
    return $rv;
}

string sub regex_switch_test(any $val) {
    my string $rv;

    switch ($val) {
	case /abc/:
	case /def/:
	
	case /barney/:
	    $rv = "case 1";
            break;

	case =~ /dinosaur/:
	    $rv = "case 2";
	    break;

	case !~ /can/:
	    $rv = "case 3";
	    break;

        default:
	    return "default";
    }
    return $rv;
}

string sub switch_with_relation_test(float $val) {
  my string $rv;
  switch ($val) {
  case < -1.0: $rv = "first switch"; break;
  case > 1.0: $rv = "second switch"; break;
  case <= -1.0: $rv = "third switch"; break;
  case >= 1.0: $rv = "fourth switch"; break;
  case 0.0: $rv = "fifth switch"; break;
  }
  return $rv;
}

sub statement_tests() {
    if ($unit.verbose())
	print("%%%% statement tests\n");
    # while test
    my int $a = 0;
    while ($a < 3)
	$a++;
    $unit.cmp($a, 3, "while");
    # do while test
    $a = 0;
    do {
	$a++;
    } while ($a < 3);
    $unit.cmp($a, 3, "do while");
    # for test
    my int $b = 0;
    for (my int $i = 0; $i < 3; $i++)
	$b++;
    $unit.cmp($a, 3, "for");
    $unit.cmp($b, 3, "for exec");    
    # foreach tests
    $b = 0;
    my int $v;
    foreach $v in (1, 2, 3)
	$b++;
    $unit.cmp($v, 3, "foreach");
    $unit.cmp($b, 3, "foreach exec");

    my any $list = my list $x;
    $unit.cmp($x, NOTHING, "unassigned typed variable");
    foreach my string $y in (\$list) $y = "test";
    $unit.cmp($list, NOTHING, "first foreach reference");
    
    $list = (1, 2, 3);
    foreach my any $y in (\$list) $y = "test";
    $unit.cmp($list, ("test", "test", "test"), "second foreach reference");
    
    $list = 1;
    foreach my any $y in (\$list) $y = "test";
    $unit.cmp($list, "test", "third foreach reference");

    # switch tests
    $unit.cmp(switch_test(1), "case 1", "first switch");
    $unit.cmp(switch_test(2), "default", "second switch");
    $unit.cmp(switch_test(3), "default", "third switch");
    $unit.cmp(switch_test(0), "case 1", "fourth switch");
    $unit.cmp(switch_test("hello"), "case 1", "fifth switch");
    $unit.cmp(switch_test("testing"), "default", "sixth switch");
    # switch with operators
    $unit.cmp(switch_with_relation_test(-2), "first switch", "first operator switch");
    $unit.cmp(switch_with_relation_test(2), "second switch", "second operator switch");
    $unit.cmp(switch_with_relation_test(-1.0), "third switch", "third operator switch");
    $unit.cmp(switch_with_relation_test(1.0), "fourth switch", "fourth operator switch");
    $unit.cmp(switch_with_relation_test(0), "fifth switch", "fifth operator switch");
    # regex switch
    $unit.cmp(regex_switch_test("abc"), "case 1", "first regex switch");
    $unit.cmp(regex_switch_test(), "case 3", "second regex switch");
    $unit.cmp(regex_switch_test("BOOM"), "case 3", "third regex switch");
    $unit.cmp(regex_switch_test("dinosaur"), "case 2", "fourth regex switch");
    $unit.cmp(regex_switch_test("barney"), "case 1", "fifth regex switch");
    $unit.cmp(regex_switch_test("canada"), "default", "sixth regex switch");

    # on_exit tests
    try {
	$a = 1;
	on_exit
	    $a = 2;
	$a = 3;
	throw False;
    }
    catch() {
    }
    my bool $err;
    my bool $success = False;
    try {
	$b = 100;
	on_exit {
	    $b = 2;
	    on_exit
		$b = 5;
	    on_error
		$err = True;
	    on_success
		$success = True;
	    # we use "if (True)..." so we don't get an "unreachable-code" warning
	    if (True)
		throw False;
	    $b = -1;
	}
	$v = 100;
	on_exit
	    $v = 99;
	
	# we use "if (True)..." so we don't get an "unreachable-code" warning
	if (True)
	    throw False;
	on_exit
	    $v = 101;
    }
    catch() {
    }
    $unit.cmp($a, 2, "first on_exit");
    $unit.cmp($b, 5, "second on_exit");
    $unit.cmp($v, 99, "third on_exit");
    $unit.cmp($err, True, "on_error");
    $unit.cmp($success, False, "on_success");
}

int sub fibonacci(int $num) {
    if ($num == 2)
        return 2;
    return $num * fibonacci($num - 1);
}

# recursive function test
sub recursive_function_test() {
    $unit.cmp(fibonacci(10), 3628800, "recursive function");
}

sub backquote_tests() {
    $unit.cmp(`echo -n 1`, "1", "backquote");
}

string sub sd(date $d) {
    return format_date("YYYY-MM-DD HH:mm:SS", $d);
}

sub test_date(date $d, int $y, int $w, int $day, int $n, reference $i) {
    my string $str = sprintf("%04d-W%02d-%d", $y, $w, $day);
    my hash $h = ( "year" : $y, "week" : $w, "day" : $day );
    my date $d1;
    # subtract milliseconds from date to compare with timegm value
    if (my int $ms = get_milliseconds($d))
	$d1 = $d - milliseconds($ms);
    else
	$d1 = $d;

    $unit.cmp($d1,                              date(int($d)),    "date conversion " + $i);
    $unit.cmp(getISOWeekString($d),             $str,             "getISOWeekString() " + $i);
    $unit.cmp(getISOWeekHash($d),               $h,               "getISOWeekHash() " + $i);
    $unit.cmp(getISODayOfWeek($d),              $day,             "getDayOfWeek() " + $i);
    $unit.cmp(getDayNumber($d),                 $n,               "getDayNumber() " + $i);
    $unit.cmp(getDateFromISOWeek($y, $w, $day), get_midnight($d), "getDateFromISOWeek() " + $i);
    # not all architectures support the timegm() system call
    #if ($d >= 1970-01-01 && $d < 2038-01-19)
	#$unit.cmp(timegm($d), int($d), "qore epoch conversion " + $i);
    $i++;
}

sub date_time_tests() {
    # here are the two formats for directly specifying date/time values:
    # ISO-8601 format (without timezone specification, currently qore does not support time zones)
    my date $date  = 2004-02-01T12:30:00;
    # qore-specific date/time specification format ('-' instead of 'T' - more readable but non-standard)
    my date $ndate = 2004-03-02-12:30:00;
    $unit.cmp(format_date("YYYY-MM-DD HH:mm:SS", $date), "2004-02-01 12:30:00", "format_date()");
    $unit.cmp($date - 5Y,                1999-02-01T12:30:00, "first date year subtraction");
    $unit.cmp($date - 5M,                2003-09-01T12:30:00, "first date month subtraction");
    $unit.cmp($date - 10D,               2004-01-22T12:30:00, "first date day subtraction");
    $unit.cmp($date - 2h,                2004-02-01T10:30:00, "first date hour subtraction");
    $unit.cmp($date - 25m,               2004-02-01T12:05:00, "first date minute subtraction");
    $unit.cmp($date - 11s,               2004-02-01T12:29:49, "first date second subtraction");
    $unit.cmp($date - 251ms,             2004-02-01T12:29:59.749, "first date millisecond subtraction");

    $unit.cmp($date + 2Y,                2006-02-01T12:30:00, "first date year addition");
    $unit.cmp($date + 5M,                2004-07-01T12:30:00, "first date month addition");
    $unit.cmp($date + 10D,               2004-02-11T12:30:00, "first date day addition");
    $unit.cmp($date + 2h,                2004-02-01T14:30:00, "first date hour addition");
    $unit.cmp($date + 25m,               2004-02-01T12:55:00, "first date minute addition");
    $unit.cmp($date + 11s,               2004-02-01T12:30:11, "first date second addition");
    $unit.cmp($date + 251ms,             2004-02-01T12:30:00.251, "first date millisecond addition");

    $unit.cmp($date - years(5),          1999-02-01-12:30:00, "second date year subtraction");
    $unit.cmp($date - months(5),         2003-09-01-12:30:00, "second date month subtraction");
    $unit.cmp($date - days(10),          2004-01-22-12:30:00, "second date day subtraction");
    $unit.cmp($date - hours(2),          2004-02-01-10:30:00, "second date hour subtraction");
    $unit.cmp($date - minutes(25),       2004-02-01-12:05:00, "second date minute subtraction");
    $unit.cmp($date - seconds(11),       2004-02-01-12:29:49, "second date second subtraction");
    $unit.cmp($date - milliseconds(500), 2004-02-01-12:29:59.5, "second date millisecond subtraction");

    $unit.cmp($date + years(2),          2006-02-01-12:30:00, "second date year addition");
    $unit.cmp($date + months(5),         2004-07-01-12:30:00, "second date month addition");
    $unit.cmp($date + days(10),          2004-02-11-12:30:00, "second date day addition");
    $unit.cmp($date + hours(2),          2004-02-01-14:30:00, "second date hour addition");
    $unit.cmp($date + minutes(25),       2004-02-01-12:55:00, "second date minute addition");
    $unit.cmp($date + seconds(11),       2004-02-01-12:30:11, "second date second addition");
    $unit.cmp($date + milliseconds(578), 2004-02-01-12:30:00.578, "second date millisecond addition");

    # testing ISO-8601 alternate period syntax (which is not very readable... :-( )
    # date periods
    $unit.cmp($date - P0001-00-00T00:00:00, 2003-02-01T12:30:00, "third date year subtraction");
    $unit.cmp($date - P1M,          2004-01-01T12:30:00, "third date month subtraction");
    $unit.cmp($date - P0000-00-01,          2004-01-31T12:30:00, "third date day subtraction");
    $unit.cmp($date + P1Y,          2005-02-01T12:30:00, "third date year addition");
    $unit.cmp($date + P0000-01-00,          2004-03-01T12:30:00, "third date month addition");
    $unit.cmp($date + P0000-00-01,          2004-02-02T12:30:00, "third date day addition");

    # time periods
    $unit.cmp($date - P0000-00-00T01:00:00, 2004-02-01T11:30:00, "third date hour subtraction");
    $unit.cmp($date - P00:01:00,            2004-02-01T12:29:00, "third date minute subtraction");
    $unit.cmp($date - PT00:00:01,           2004-02-01T12:29:59, "third date second subtraction");
    $unit.cmp($date + P01:00:00,            2004-02-01T13:30:00, "third date hour addition");
    $unit.cmp($date + PT00:01:00,           2004-02-01T12:31:00, "third date minute addition");
    $unit.cmp($date + P00:00:01,            2004-02-01T12:30:01, "third date second addition");

    # arithmetic on dates with ms overflow
    $unit.cmp(2006-01-02T00:00:00.112, 2006-01-01T23:59:59.800 + 312ms, "third millisecond addition");
    $unit.cmp(2006-01-01T23:59:59.800, 2006-01-02T00:00:00.112 - 312ms, "third millisecond subtraction");

    $unit.cmp($date,        localtime(mktime($date)), "localtime() and mktime()");
    $unit.cmp($date - PT1H, 2004-02-01T11:30:00, "fourth date hour subtraction");
    $unit.cmp($date + 30D,  $ndate,                   "fourth date day addition");
    $unit.cmp($ndate - 30D, $date,                    "fourth date day subtraction");
    $unit.cmp($date + 23M,  2006-01-01T12:30:00,      "fourth date month addition");
    $unit.cmp($date - 4M,   2003-10-01T12:30:00,      "fourth date month subtraction");
    $unit.cmp($date,        date("20040201123000"),   "date function");

    $unit.cmp(2001-01-01,   date("2001-01", "YYYY-MM-DD"), "first date mask function");
    $unit.cmp(2001-01-01,   date("2001 Jan xx", "YYYY Mon DD"), "second date mask function");
    $unit.cmp(2001-01-01T13:01,   date("2001 JAN 01 13:01", "YYYY MON DD HH:mm"), "second date mask function");

    # times without a date are assumed to be on Jan 1, 1970
    $unit.cmp(11:25:27, 1970-01-01T11:25:27.000, "direct hour");

    # test date conversion/calculation functions against known values
    my int $i = 1;
    test_date(1068-01-01,              1068, 1, 3, 1,    \$i);
    test_date(1783-09-18,              1783, 38, 4, 261, \$i);
    test_date(1864-02-29,              1864, 9,  1, 60,  \$i);
    test_date(1864-03-01,              1864, 9,  2, 61,  \$i);
    test_date(1968-01-01T11:01:20,     1968, 1,  1, 1,   \$i);
    test_date(1968-02-29,              1968, 9,  4, 60,  \$i);
    test_date(1968-03-01,              1968, 9,  5, 61,  \$i);
    test_date(1969-12-31T23:59:59.999, 1970, 1,  3, 365, \$i);
    test_date(1969-12-31T00:00:00.100, 1970, 1,  3, 365, \$i);
    test_date(1969-01-01T17:25:31.380, 1969, 1,  3, 1,   \$i); # 10

    test_date(1970-01-01,              1970, 1,  4, 1,   \$i);
    test_date(1970-12-01T00:00:00,     1970, 49, 2, 335, \$i);
    test_date(1972-01-01,              1971, 52, 6, 1,   \$i);
    test_date(1972-12-30,              1972, 52, 6, 365, \$i);
    test_date(1972-12-31,              1972, 52, 7, 366, \$i);
    test_date(2004-02-28,              2004, 9,  6, 59,  \$i);
    test_date(2004-02-29,              2004, 9,  7, 60,  \$i);
    test_date(2004-03-01,              2004, 10, 1, 61,  \$i);
    test_date(2004-03-28,              2004, 13, 7, 88,  \$i);
    test_date(2006-01-01,              2005, 52, 7, 1,   \$i); # 20

    test_date(2006-09-01,              2006, 35, 5, 244, \$i);
    test_date(2006-12-01,              2006, 48, 5, 335, \$i);
    test_date(2007-12-30,              2007, 52, 7, 364, \$i);
    test_date(2007-12-31,              2008, 1,  1, 365, \$i);
    test_date(2008-01-01,              2008, 1,  2, 1,   \$i);
    test_date(2008-01-06,              2008, 1,  7, 6,   \$i);
    test_date(2008-01-07,              2008, 2,  1, 7,   \$i);
    test_date(2008-01-08,              2008, 2,  2, 8,   \$i);
    test_date(2008-01-09,              2008, 2,  3, 9,   \$i);
    test_date(2008-01-10,              2008, 2,  4, 10,  \$i); # 30

    test_date(2008-12-28,              2008, 52, 7, 363, \$i);
    test_date(2008-12-29,              2009, 1,  1, 364, \$i);
    test_date(2008-12-30,              2009, 1,  2, 365, \$i);
    test_date(2010-01-03,              2009, 53, 7, 3,   \$i);
    test_date(2010-01-04,              2010, 1,  1, 4,   \$i);
    test_date(2010-01-09,              2010, 1,  6, 9,   \$i);
    test_date(2010-01-10,              2010, 1,  7, 10,  \$i);
    test_date(2010-01-11,              2010, 2,  1, 11,  \$i);
    test_date(2016-12-01,              2016, 48, 4, 336, \$i);
    test_date(2026-08-22,              2026, 34, 6, 234, \$i); # 40

    test_date(2036-04-30,              2036, 18, 3, 121, \$i);
    test_date(2054-06-19,              2054, 25, 5, 170, \$i);
    test_date(2400-12-01,              2400, 48, 5, 336, \$i);
    test_date(2970-01-01,              2970, 1,  1, 1,   \$i);
    test_date(9999-12-31,              9999, 52, 5, 365, \$i);
    test_date(9999-12-31T23:59:59.999, 9999, 52, 5, 365, \$i);

    $unit.cmp(date("2012-03-02", "YYYY-MM-DD"), 2012-03-02, "date() format parsing test");

    # absolute date difference tests
    $unit.cmp(2006-01-02T11:34:28.344 - 2006-01-01,              35h + 34m + 28s +344ms,       "date difference 1");
%ifndef Windows
    # this test fails on Windows due to different DST application for dates outside the UNIX epoch
    $unit.cmp(2099-04-21T19:20:02.106 - 1804-03-04T20:45:19.956, 2587078h + 34m + 42s + 150ms, "date difference 2");
%endif

    my SingleValueIterator $svi(2012-01-01);
    $unit.cmp($svi.next(), True, "1st SingleValueIterator::next()");
    $unit.cmp($svi.next(), False, "2nd SingleValueIterator::next()");
    $unit.cmp($svi.next(), True, "3rd SingleValueIterator::next()");
    $unit.cmp($svi.getValue(), 2012-01-01, "SingleValueIterator::getValue()");
    $unit.cmp($svi.valid(), True, "SingleValueIterator::valid()");
    my SingleValueIterator $ni = $svi.copy();
    $unit.cmp($ni.getValue(), 2012-01-01, "SingleValueIterator::getValue() (copy)");
    $unit.cmp($ni.next(), False, "SingleValueIterator::next() (copy)");
    $unit.cmp($ni.valid(), False, "SingleValueIterator::valid() (copy)");
}

sub binary_tests() {
    my binary $b = binary("this is a test");
    $unit.cmp(get_byte($b, 3), ord("s"), "get_byte()");
    $unit.cmp($b, binary("this is a test"), "binary comparison");
    $unit.cmp($b != binary("this is a test"), False, "binary negative comparison");
}

sub string_tests() {
    my string $str = "Hi there, you there, pal";
    my string $big = "GEE WHIZ";
    $unit.cmp(strlen($str), 24, "strlen()");
    $unit.cmp($str.strlen(), 24, "<string>::strlen()");
    $unit.cmp($str.size(), 24, "<string::size()");
    $unit.cmp(toupper($str), "HI THERE, YOU THERE, PAL", "toupper()");
    $unit.cmp($str.upr(), "HI THERE, YOU THERE, PAL", "<string>::upr()");
    $unit.cmp($big.lwr(), "gee whiz", "<string>::lwr()");
    $unit.cmp(reverse($big), "ZIHW EEG", "reverse()");
    # strmul
    $unit.cmp(strmul($big, 2), "GEE WHIZGEE WHIZ", "strmul() basic");
    $unit.cmp(strmul("%v, ", 3, 2), "%v, %v, %v", "strmul() extended");
    $unit.cmp(strmul(123, 2), "123123", "strmul() type conversion");
    
    # set up a string with UTF-8 multi-byte characters
    $str = "Über die Wolken läßt sich die Höhe begrüßen";
    $unit.cmp(strlen($str), 49, "UTF-8 strlen()");
    $unit.cmp($str.strlen(), 49, "UTF-8 <string>::strlen()");
    $unit.cmp(length($str), 43, "UTF-8 length()");
    $unit.cmp($str.length(), 43, "UTF-8 <string>::length()");
    $unit.cmp(substr($str, 30), "Höhe begrüßen", "first UTF-8 substr()");
    $unit.cmp(substr($str, -8), "begrüßen", "second UTF-8 substr()");
    $unit.cmp(substr($str, 0, -39), "Über", "third UTF-8 substr()");
    $unit.cmp(index($str, "läßt"), 16, "first UTF-8 index()");
    $unit.cmp(index($str, "läßt", 1), 16, "second UTF-8 index()");
    $unit.cmp(rindex($str, "ß"), 40, "first UTF-8 rindex()");
    $unit.cmp(rindex($str, "ß", 25), 18, "second UTF-8 rindex()"); 
    $unit.cmp(bindex($str, "läßt"), 17, "first UTF-8 bindex()");
    $unit.cmp(bindex($str, "läßt", 1), 17, "second UTF-8 bindex()");
    $unit.cmp(brindex($str, "ß"), 45, "first UTF-8 brindex()");
    $unit.cmp(brindex($str, "ß", 25), 20, "second UTF-8 brindex()"); 
    $unit.cmp(reverse($str), "neßürgeb ehöH eid hcis tßäl nekloW eid rebÜ", "UTF-8 reverse()");
    $unit.cmp(index($str, "==="), -1, "negative index()");
    $unit.cmp(rindex($str, "==="), -1, "negative rindex()");
    $unit.cmp(bindex($str, "==="), -1, "negative bindex()");

    $unit.cmp($str[31], "ö", "first UTF-8 string index dereference");
    $unit.cmp($str[39], "ü", "second UTF-8 string index dereference");

    # save string
    my string $ostr = $str;
    # convert the string to single-byte ISO-8859-1 characters and retest
    $str = convert_encoding($str, "ISO-8859-1");
    $unit.cmp($str != $ostr, False, "string != operator with same text with different encodings");
    $unit.cmp(strlen($str), 43, "ISO-8859-1 strlen()");
    $unit.cmp($str.strlen(), 43, "ISO-8859-1 <string>::strlen()");
    $unit.cmp(length($str), 43, "ISO-8859-1 length()");
    $unit.cmp($str.length(), 43, "ISO-8859-1 <string>::length()");
    $unit.cmp(substr($str, 30), convert_encoding("Höhe begrüßen", "ISO-8859-1"), "first ISO-8859-1 substr()");
    $unit.cmp(substr($str, -8), convert_encoding("begrüßen", "ISO-8859-1"), "second ISO-8859-1 substr()");
    $unit.cmp(substr($str, 0, -39), convert_encoding("Über", "ISO-8859-1"), "third ISO-8859-1 substr()");
    $unit.cmp(index($str, convert_encoding("läßt", "ISO-8859-1")), 16, "first ISO-8859-1 index()");
    $unit.cmp(index($str, convert_encoding("läßt", "ISO-8859-1"), 1), 16, "second ISO-8859-1 index()");
    $unit.cmp(rindex($str, convert_encoding("ß", "ISO-8859-1")), 40, "first ISO-8859-1 rindex()");
    $unit.cmp(rindex($str, convert_encoding("ß", "ISO-8859-1"), 25), 18, "second ISO-8859-1 rindex()"); 
    $unit.cmp(ord($str, 1), 98, "ord()");

    $unit.cmp(chr(104), "h", "chr()");

    $str = "gee this is a long string";
    my list $a = split(" ", $str);
    $unit.cmp($a[2], "is", "first string split()");
    $unit.cmp($a[5], "string", "second string split()");

    $a = split(binary(" "), binary($str));
    $unit.cmp($a[2], binary("is"), "first binary split()");
    $unit.cmp($a[5], binary("string"), "second binary split()");

    $str = "äüößÄÖÜ";
    # test length() with UTF-8 multi-byte characters
    $unit.cmp(length($str), 7, "length() with UTF-8 multi-byte characters");
    $unit.cmp(strlen($str), 14, "strlen() with UTF-8 multi-byte characters");
    # test charset encoding conversions
    my string $nstr = convert_encoding($str, "ISO-8859-1");
    $unit.cmp(length($nstr), 7, "length() with ISO-8859-1 special characters");
    $unit.cmp(strlen($nstr), 7, "strlen() with ISO-8859-1 special characters");
    $unit.cmp($str, convert_encoding($nstr, "UTF-8"), "convert_encoding()");
    # assign binary object
    my binary $x = <0abf83e8ca72d32c>;
    my string $b64 = makeBase64String($x);
    $unit.cmp($x, parseBase64String($b64), "first base64");
    $unit.cmp("aGVsbG8=", makeBase64String("hello"), "makeBase64String()");
    my string $hex = makeHexString($x);
    $unit.cmp($x, parseHexString($hex), "first hex");

    # UTF-8 string splice tests
    $str = "äbcdéf";
    splice $str, 5;
    $unit.cmp($str, "äbcdé", "first UTF-8 string splice");
    splice $str, 3, 1;
    $unit.cmp($str, "äbcé", "second UTF-8 string splice");
    splice $str, 1, 2, "GHI";
    $unit.cmp($str, "äGHIé", "third UTF-8 string splice");
    splice $str, 0, 4, "jkl";
    $unit.cmp($str, "jklé", "fourth UTF-8 string splice");
    splice $str, 0, 1;
    $unit.cmp($str, "klé", "fifth UTF-8 string splice");
    splice $str, 5, 2, "MNO";
    $unit.cmp($str, "kléMNO", "sixth UTF-8 string splice");
    splice $str, -4, 2, "p";
    $unit.cmp($str, "klpNO", "seventh UTF-8 string splice");
    splice $str, -4, -2, "QRS";
    $unit.cmp($str, "kQRSNO", "eighth UTF-8 string splice");

    # UTF-8 string extract tests
    $str = "äbcdéf";
    $unit.cmp((extract $str, 4), "éf", "first UTF-8 string extract");
    $unit.cmp((extract $str, 1, 2), "bc", "second UTF-8 string extract");
    $unit.cmp((extract $str, 1, 1, "bcdef"), "d", "third UTF-8 string extract");
    $unit.cmp($str, "äbcdef", "final UTF-8 string extract");

    # single-byte string splice tests
    $str = convert_encoding("äbcdéf", "ISO-8859-1");
    splice $str, 5;
    $unit.cmp($str == "äbcdé", True, "first ISO-8859-1 string splice");
    splice $str, 3, 1;
    $unit.cmp($str == "äbcé", True, "second ISO-8859-1 string splice");
    splice $str, 1, 2, "GHI";
    $unit.cmp($str == "äGHIé", True, "third ISO-8859-1 string splice");
    splice $str, 0, 4, "jkl";
    $unit.cmp($str == "jklé", True, "fouth ISO-8859-1 string splice");
    splice $str, 0, 1;
    $unit.cmp($str == "klé", True, "fifth ISO-8859-1 string splice");
    splice $str, 5, 2, "MNO";
    $unit.cmp($str == "kléMNO", True, "sixth ISO-8859-1 string splice");
    splice $str, -4, 2, "p";
    $unit.cmp($str == "klpNO", True, "seventh ISO-8859-1 string splice");
    splice $str, -4, -2, "QRS";
    $unit.cmp($str == "kQRSNO", True, "eighth ISO-8859-1 string splice");

    # UTF-8 string extract tests
    $str = convert_encoding("äbcdéf", "ISO-8859-1");
    my string $val = extract $str, 4;
    $unit.cmp($val == "éf", True, "first UTF-8 string extract");
    $val = extract $str, 1, 2;
    $unit.cmp($val == "bc", True, "second UTF-8 string extract");
    $val = extract $str, 1, 1, "bcdef";
    $unit.cmp($val == "d", True, "third UTF-8 string extract");
    $unit.cmp($str == "äbcdef", True, "final UTF-8 string extract");

    # join tests
    $str = join(":", "login","passwd",1,"gid","gcos","home","shell");
    $unit.cmp($str, "login:passwd:1:gid:gcos:home:shell", "first join");
    my list $l = ("login","passwd","uid","gid","gcos","home","shell");
    $str = join(":", $l);
    $unit.cmp($str, "login:passwd:uid:gid:gcos:home:shell", "second join");

    # transliteration tests
    $str = "Hello There";
    $unit.cmp($str =~ tr/def/123/, "H2llo Th2r2", "first transliteration");
    $unit.cmp($str =~ tr/a-z/0-9/, "H2999 T7292", "first range transliteration");
    $str = "Hello There";
    $unit.cmp($str =~ tr/a-z/A-Z/, "HELLO THERE", "second range transliteration");
    $unit.cmp($str =~ tr/A-Z/a-z/, "hello there", "third range transliteration");

    # regex subpattern extraction operator tests
    $unit.cmp("xmlns:wsdl" =~ x/(\w+):(\w+)/, ("xmlns", "wsdl"), "regex subpattern extraction");
    $unit.cmp("xmlns-wsdl" =~ x/(\w+):(\w+)/, NOTHING, "negative regex subpattern extraction");
    $unit.cmp(regex_extract("xmlns:wsdl", "(\\w+):(\\w+)"), ("xmlns", "wsdl"), "regex_extract()");

    # regex operator tests
    $unit.cmp("hello" =~ /^hel*/, True, "regular expression positive match");
    $unit.cmp("hello" =~ m/123*/, False, "regular expression negative match");
    $unit.cmp("hello" =~ m/^HEL*/i, True, "regular expression case-insensitive positive match");
    $unit.cmp("hello" =~ /^HEL*/, False, "regular expression case-insensitive negative match");
    $unit.cmp("test\nx" =~ /test.x/s, True, "regular expression newline positive match");
    $unit.cmp("test\nx" =~ /test.x/, False, "regular expression newline negative match");
    $unit.cmp("hello" =~ /^  hel* #comment/x, True, "extended regular expression positive match");
    $unit.cmp("hello" =~ /^  hel* #comment/, False, "extended regular expression negative match");
    $unit.cmp("test\nx123" =~ /^x123/m, True, "multiline regular expression positive match");
    $unit.cmp("test\nx123" =~ /^x123/, False, "multiline regular expression negative match");
    # NOTE that escaping UTF-8 characters (\ä) doesn't work on PPC for some reason
    $unit.cmp("testäöüß" =~ /äöüß/, True, "regular expression UTF-8 match");
    $unit.cmp("testäöüß" =~ /aouB/, False, "regular expression UTF-8 negative match");
    $unit.cmp("hello" !~ /hel*/, False, "negative regular expression negative match");
    $unit.cmp("hello" !~ /123*/, True, "negative regular expression positive match");

    # regex substitution operator tests
    $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
    $unit.cmp($l[0] =~ s/bar/foo/, "hello foo hi bar", "first non-global regular expression substitution");
    $unit.cmp($l[1] =~ s/bar/foo/, "foo hello bar hi bar", "second non-global regular expression substitution");
    $unit.cmp($l[2] =~ s/BAR/foo/i, "hello foo hi", "case-insensitive non-global regular expression substitution");
    $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
    $unit.cmp($l[0] =~ s/bar/foo/g, "hello foo hi foo", "first global regular expression substitution");
    $unit.cmp($l[1] =~ s/bar/foo/g, "foo hello foo hi foo", "second global regular expression substitution");
    $unit.cmp($l[2] =~ s/BAR/foo/ig, "hello foo hi", "case-insensitive global regular expression substitution");

    my string $astr= "abc def";
    $astr =~ s/(\w+) +(\w+)/$2, $1/; 
    $unit.cmp($astr, "def, abc", "regular expression subpattern substitution");

    # regex() tests
    $unit.cmp(regex("hello", "^hel*"), True, "regex() positive match");
    $unit.cmp(regex("hello", "123*"), False, "regex() negative match");
    $unit.cmp(regex("hello", "^HEL*", RE_Caseless), True, "regex() case-insensitive positive match");
    $unit.cmp(regex("hello", "^HEL*"), False, "regex() case-insensitive negative match");
    $unit.cmp(regex("test\nx", "test.x", RE_DotAll), True, "regex() newline positive match");
    $unit.cmp(regex("test\nx", "test.x/"), False, "regex() newline negative match");
    $unit.cmp(regex("hello", "^  hel* #comment", RE_Extended), True, "regex() extended positive match");
    $unit.cmp(regex("hello", "^  hel* #comment"), False, "regex() extended negative match");
    $unit.cmp(regex("test\nx123", "^x123", RE_MultiLine), True, "regex() multiline positive match");
    $unit.cmp(regex("test\nx123", "^x123/"), False, "regex() multiline negative match");
    $unit.cmp(regex("testäöüß", "\äöüß"), True, "regex() UTF-8 match");
    $unit.cmp(regex("testäöüß", "aouB"), False, "regex() UTF-8 negative match");

    # regex_subst() tests
    $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
    $unit.cmp(regex_subst($l[0], "bar", "foo"), "hello foo hi bar", "first non-global regex_subst()");
    $unit.cmp(regex_subst($l[1], "bar", "foo"), "foo hello bar hi bar", "second non-global regex_subst()");
    $unit.cmp(regex_subst($l[2], "BAR", "foo", RE_Caseless), "hello foo hi", "case-insensitive non-global regex_subst()");
    $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
    $unit.cmp(regex_subst($l[0], "bar", "foo", RE_Global), "hello foo hi foo", "first global regex_subst()");
    $unit.cmp(regex_subst($l[1], "bar", "foo", RE_Global), "foo hello foo hi foo", "second global regex_subst()");
    $unit.cmp(regex_subst($l[2], "BAR", "foo", RE_Global|RE_Caseless), "hello foo hi", "case-insensitive global regex_subst()");

    $astr = "abc def";
    # note that the escape characters have to be escaped in the following pattern string
    $unit.cmp(regex_subst($astr, "(\\w+) +(\\w+)", "$2, $1"), "def, abc", "first subpattern regex_subst()");
    # here we use single-quotes, so the escape characters do not have to be escaped
    $unit.cmp(regex_subst($astr, '(\w+) +(\w+)', "$2, $1"), "def, abc", "second subpattern regex_subst()");

    # chomp tests
    $str = "hello\n";
    chomp $str;
    $unit.cmp($str, "hello", "first string chomp");
    $str += "\r\n";
    chomp $str;
    $unit.cmp($str, "hello", "second string chomp");
    $l = ( 1, "hello\n", 3.0, True, "test\r\n" );
    chomp $l;
    $unit.cmp($l, ( 1, "hello", 3.0, True, "test" ), "list chomp");
    my hash $h = ( "key1" : "hello\n", "key2" : 2045, "key3": "test\r\n", "key4" : 302.223 );
    chomp $h;
    $unit.cmp($h, ( "key1" : "hello", "key2" : 2045, "key3": "test", "key4" : 302.223 ), "hash chomp");
    $str = "hello\n";
    chomp(\$str);
    $unit.cmp($str, "hello", "string reference chomp()");
    $str = "  \t\n  hello  \n   \r \t \0 ";
    trim $str;
    $unit.cmp($str, "hello", "trim string operator test");
    $str = "  \t\n  hello  \n   \r \t \0 ";
    trim(\$str);
    $unit.cmp($str, "hello", "trim string reference test");

    $l = ( 1, "   \r \t hello  \n  \r \v \t", 3.0, True, "    test\r\n  " );
    trim $l;
    $unit.cmp($l, ( 1, "hello", 3.0, True, "test" ), "list trim");

    $h = ( "key1" : "    hello\n \r  ", "key2" : 2045, "key3": "     test\r   \n \t\v   ", "key4" : 302.223 );
    trim $h;
    $unit.cmp($h, ( "key1" : "hello", "key2" : 2045, "key3": "test", "key4" : 302.223 ), "hash trim");    

    # make sure strings containing floating-point numbers between -1.0 and 1.0 exclusive return True when evaluated in a boolean context
    $unit.cmp(True, boolean("0.1"), "first string fp boolean");
    $unit.cmp(True, boolean("-0.1"), "second string fp boolean");

    $str = "příliš žluťoučký kůň úpěl ďábelské ódy";
    $unit.cmp($str.unaccent(), "prilis zlutoucky kun upel dabelske ody", "<string>::unaccent()");
    my string $ustr = $str.upr();
    $unit.cmp($ustr, "PŘÍLIŠ ŽLUŤOUČKÝ KŮŇ ÚPĚL ĎÁBELSKÉ ÓDY", "<string>::upr()");
    $unit.cmp($ustr.lwr(), "příliš žluťoučký kůň úpěl ďábelské ódy", "<string>::lwr()");

    # regression tests for floating-point formatting bugs
    $unit.cmp(sprintf("%f", 1.5), "1.500000", "%f float");    
    $unit.cmp(sprintf("%f", 1.5n), "1.500000", "%f number");
    $unit.cmp(sprintf("%g", 1.5), "1.5", "%f float");    
    $unit.cmp(sprintf("%g", 1.5n), "1.5", "%f number");
}

sub pwd_tests() {
    # getpwuid(0).pw_name may not always be "root"
    # skip the test on windows
    if (Option::HAVE_UNIX_USERMGT) {
        $unit.cmp(getpwuid(0).pw_uid, 0, "getpwuid()");
        my hash $h;
        # try to get passwd entry for uid 0, ignore exceptions
        try $h = getpwuid2(0); catch () {}
        $unit.cmp($h.pw_uid, 0, "getpwuid2()");
        $unit.cmp(getpwnam("root").pw_uid, 0, "getpwnam()");
        # try to get passwd entry for root, ignore exceptions
        try $h = getpwnam2("root"); catch () {}
        $unit.cmp($h.pw_uid, 0, "getpwnam2()");
        $unit.cmp(getgrgid(0).gr_gid, 0, "getgrgid()");
        # try to get group entry for gid 0, ignore exceptions
        try $h = getgrgid2(0); catch () {}
        $unit.cmp($h.gr_gid, 0, "getgrgid2()");
        $unit.cmp(getgrnam($h.gr_name).gr_gid, 0, "getgrnam()");
        # try to get group entry for root, ignore exceptions
        try $h = getgrnam2($h.gr_name); catch () {}
        $unit.cmp($h.gr_gid, 0, "getgrnam2()");
    }
}

any sub simple_shift() {
    return shift $argv;
}

sub misc_tests() {
    my hash $dh = ( "user"    : "user",
		    "pass"    : "123pass@word",
		    "db"      : "dbname",
		    "charset" : "utf8",
		    "host"    : "hostname" );
    my string $ds = "user/123pass@word@dbname(utf8)%hostname";
    $unit.cmp($dh, parseDatasource($ds), "first parseDatasource()"); 
    $unit.cmp((1, 2), simple_shift((1, 2)), "list arg function call");

    $unit.cmp(call_function("simple_shift", 1), 1, "call_function()");
    $unit.cmp(call_builtin_function("type", 1), Type::Int, "call_builtin_function()");
    $unit.cmp(existsFunction("simple_shift"), True, "existsFunction()");
    $unit.cmp(functionType("simple_shift"), "user", "functionType() user");
    $unit.cmp(functionType("printf"), "builtin", "functionType() builtin");
    $unit.cmp(type(1), "integer", "type()");
    my string $str1 = "&<>\"";
    my string $str2 = "&amp;&lt;&gt;&quot;";
    $unit.cmp(html_encode($str1), $str2, "html_encode()");
    $unit.cmp(html_decode($str2), $str1, "html_decode()");

    # note that '@' signs are legal in the password field as with datasources
    my string $url = "https://username:passw@rd@hostname:1044/path/is/here";
    my hash $uh = ( "protocol" : "https",
		    "username" : "username",
		    "password" : "passw@rd",
		    "host" : "hostname",
		    "port" : 1044,
		    "path" : "/path/is/here" );

    $unit.cmp(parseURL($url), $uh, "parseURL()");

    # test gzip
    my string $str = "This is a long string xxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    my binary $bstr = binary($str);
    my binary $c = compress($str);
    $unit.cmp($str, uncompress_to_string($c), "compress() and uncompress_to_string()");
    $unit.cmp($bstr, uncompress_to_binary($c), "compress() and uncompress_to_binary()");
    my binary $gz = gzip($str);
    $unit.cmp($str, gunzip_to_string($gz), "gzip() and gunzip_to_string()");
    $unit.cmp($bstr, gunzip_to_binary($gz), "gzip() and gunzip_to_binary()");
    
    # test bzip2
    my binary $bz = bzip2($str);
    $unit.cmp($str, bunzip2_to_string($bz), "bzip2 and bunzip2_to_string");
    $unit.cmp($bstr, bunzip2_to_binary($bz), "bzip2 and bunzip2_to_binary");
}

sub math_tests() {
    $unit.cmp(ceil(2.7), 3.0, "ceil()");
    $unit.cmp(floor(3.7), 3.0, "fllor()");
    $unit.cmp(format_number(".,3", -48392093894.2349), "-48.392.093.894,235", "format_number()");
}

sub lib_tests() {
    my string $pn = get_script_path();
    if (PlatformOS != "Windows") {
        $unit.cmp(stat($pn)[1], hstat($pn).inode, "stat() and hstat()");
        $unit.cmp(hstat($pn).type, "REGULAR", "hstat()");
    }
    #my string $h = gethostname();
    #$unit.cmp($h, gethostbyaddr(gethostbyname($h)), "host functions");
}

sub file_tests() {
    $unit.cmp(is_file($ENV."_"), True, "is_file()");
    $unit.cmp(is_executable($ENV."_"), True, "is_executable()");
    $unit.cmp(is_dir("/"), True, "is_dir()");
    $unit.cmp(is_writeable($ENV.HOME), True, "is_writable()");
    $unit.cmp(is_readable($ENV.HOME), False, "is_readable()");
    $unit.cmp(is_dev("/dev/null"), True, "is_dev()");
    $unit.cmp(is_cdev("/dev/null"), True, "is_cdev()");
    $unit.cmp(is_bdev("/dev/null"), False, "is_bdev()");
    $unit.cmp(is_link("/"), False, "is_link()");
    $unit.cmp(is_socket("/"), False, "is_socket()");
    $unit.cmp(is_pipe("/"), False, "is_pipe()");
}

sub io_tests() {
    $unit.cmp(sprintf("%04d-%.2f", 25, 101.239), "0025-101.24", "sprintf()");
    $unit.cmp(vsprintf("%04d-%.2f", (25, 101.239)), "0025-101.24", "vsprintf()");
    # check multi-byte character set support for f_*printf()
    $unit.cmp(f_sprintf("%3s", "niña"), "niñ", "UTF-8 f_sprintf()");
}

string sub f1_test(string $x) {
    return type($x);
}

string sub f1_test(float $x) {
    return type($x);
}

string sub f_test(int $x) {
    return type($x);
}

string sub f_test(float $x) {
    return type($x);
}

sub overload_tests() {
    $unit.cmp(f_test(1), "integer", "first overload partial match");
    $unit.cmp(f_test(1.1), "float", "second overload partial match");
    $unit.cmp(f1_test(1), "float", "third overload partial match");
    $unit.cmp(f1_test(1.1), "float", "fourth overload partial match");
    $unit.cmp(f1_test("str"), "string", "fifth overload partial match");
    my int $i = 1;
    $unit.cmp(f_test($i), "integer", "first runtime overload partial match");
    $unit.cmp(f1_test($i), "float", "second runtime overload partial match");
    my float $fi = 1.1;
    $unit.cmp(f_test($fi), "float", "third runtime overload partial match");
    $unit.cmp(f1_test($fi), "float", "fourth runtime overload partial match");
}

sub function_tests() {
    date_time_tests();
    binary_tests();
    string_tests();  
    pwd_tests();
    misc_tests();
    math_tests();
    lib_tests();
    io_tests();
    overload_tests();
}

int sub t(any $a) {
    return $a + 1;
}

class Test inherits Socket {
    private {
        int $.a;
        int $.b;
    }
    public {
	list $.data;
	hash $.t;
	int $.x;
        any $.key;
        any $.unique;
        any $.new;
        any $.barn;
        any $.asd;
    }

    constructor(any $a, any $b, any $c) {
	$.a = 1;
	$.b = 2;
        $.data = ($a, $b, $c);
    }
    any getData(int $elem) {
	if (exists $elem)
	    return $.data[$elem];
        return $.data;
    }
    string methodGate(string $m) {
        return $m;
    }
    string memberGate(string $m) {
        return "memberGate-" + $m;
    }
    memberNotification(string $m) {
	$.t.$m = $self.$m;
    }
    code closure(any $x) {
	my int $a = 1;
	# return a closure encapsulating the state of the object
	return string sub (any $y) {
	    return sprintf("%s-%n-%n-%n", $.data[1], $x, $y, ++$a);
	};
    }
    any argTest() {
        return $argv;
    }
}

sub class_test_Program() {
    my string $func = "namespace ITest { const val = 1.0; class X; } $gv2 = 123; int sub t2(int $a) { return $a + 2; } int sub et(int $a) { return t($a); } string sub tot() { return getClassName($to); } Queue sub getObject() { return new Queue(); } X sub get_x() { return new X(); } sub deleteException() { $ro.getData(0); delete $ro; } bool sub check_ro() { return exists $ro; }";

    my string $pf = "newfunc();";
    my string $nf = "sub newfunc() { return True; }";

    my Program $a();
    $a.parsePending($pf, "pending test part1", 0);
    $a.parsePending($nf, "pending test part2", 0);
    $a.parseCommit();    
    $a.importFunction("t");
    $a.importGlobalVariable("to");
    $a.importGlobalVariable("ro", True);
    $a.parse($func, "test");

    $unit.cmp($a.callFunction("newfunc"), True, "Program::parsePending()");
    $unit.cmp($a.callFunction("t2", 1), 3, "Program::parse()");
    $unit.cmp($a.callFunctionArgs("t2", list(int(2))), 4, "program imported function");
    $unit.cmp($a.callFunction("et", 1), 2, "program imported function");
    $unit.cmp($a.callFunction("tot"), "Test", "program imported object variable");
    $unit.cmp($to.member, "memberGate-member", "program imported object member gate");
    $unit.cmp($to.method(), "method", "program imported object method gate");
    try
	$a.callFunction("deleteException");
    catch ($ex)
	$unit.cmp($ex.err, "ACCESS-ERROR", "Program::importGlobalVariable() readonly");

    $unit.cmp($a.callFunction("check_ro"), True, "delete read-only");
    
    my Queue $o = $a.callFunction("getObject");
    my object $ox = $a.callFunction("get_x");
    delete $a;
    $unit.cmp(getClassName($o), "Queue", "builtin class returned from deleted subprogram object");
    $unit.cmp(getClassName($ox), "X", "user class returned from deleted subprogram object");

    # test for incorrect parse location when processing constants after a commit
    $a = new Program();
    $a.parse("sub x() {}", "lib");
    my *hash $h = $a.parse("const X1 = 'a'; const X2 = 'a'; const h = (X1: 1, X2: 2);", "warn", WARN_ALL);
    $unit.cmp($h.file, "<run-time-loaded: warn>", "constant parse location");

    # make sure recursive constant definitions are handled
    try {
        $a.parse("const A = B; const B = A; any a = A;", "rec");
    }
    catch (hash $ex) {
        $unit.cmp($ex.err, "PARSE-EXCEPTION", "recursive constant ref");
    }

    my string $pstr = "class T { private { int i = 1; static *int j = 4; const X = 2; } int get() { return i; } static other (int x) {} } T sub getT() { return new T(); } int sub checkT(T t) { return t.get(); }";

    my Program $p1(PO_NEW_STYLE);
    my Program $p2(PO_NEW_STYLE);

    $p1.parse($pstr, "p");
    $p2.parse($pstr, "p");

    my object $o2 = $p1.callFunction("getT");
    $unit.cmp(1, $p1.callFunction("checkT", $o2), "first cross-Program class");
    $unit.cmp(1, $p2.callFunction("checkT", $o2), "second cross-Program class");

    my Program $p3();
    $p3.parse("class X { private $.a; }", "p");

    my Program $p4();
    try {
        $p4.parse("error", "error", 0, "source", 10);
        $unit.cmp(True, False, "exception source & offset");
    }
    catch (hash $ex) {
        $unit.cmp($ex.source, "source", "exception source");
        $unit.cmp($ex.offset, 10, "exception offset");
    }
}

sub class_test_File() {
    # File test
    my File $f("iso-8859-1");
    $f.open(get_script_path());
    $unit.cmp($f.getEncoding(), "ISO-8859-1", "file encoding");
/*
    my string $l = $f.readLine();
    my int $p = $f.getPos();
    $f.setPos(0);
    $unit.cmp($l, $f.readLine(), "File::readLine() and File::setPos()");
    $unit.cmp($p, $f.getPos(), "File::getPos()");
*/
}

const cert_pem = "-----BEGIN CERTIFICATE-----
MIIDAjCCAqygAwIBAgIJALLMpB2Hc61YMA0GCSqGSIb3DQEBBQUAMIGKMQswCQYD
VQQGEwJDWjEPMA0GA1UECBMGUHJhZ3VlMQ8wDQYDVQQHEwZQcmFndWUxIjAgBgNV
BAoTGVFvcmUgUHJvZ3JhbW1pbmcgTGFuZ3VhZ2UxFjAUBgNVBAMTDURhdmlkIE5p
Y2hvbHMxHTAbBgkqhkiG9w0BCQEWDmRhdmlkQHFvcmUub3JnMB4XDTEwMDMxMDE0
MjcwN1oXDTExMDMxMDE0MjcwN1owgYoxCzAJBgNVBAYTAkNaMQ8wDQYDVQQIEwZQ
cmFndWUxDzANBgNVBAcTBlByYWd1ZTEiMCAGA1UEChMZUW9yZSBQcm9ncmFtbWlu
ZyBMYW5ndWFnZTEWMBQGA1UEAxMNRGF2aWQgTmljaG9sczEdMBsGCSqGSIb3DQEJ
ARYOZGF2aWRAcW9yZS5vcmcwXDANBgkqhkiG9w0BAQEFAANLADBIAkEAxvP3j5yN
/7BxHxSCaJLYAAeGFo93jVtulzIPu3ULH9rzSiO3EPYeUOEQtpe3ks0tUu75BVDY
OxiRSD3iy99/pQIDAQABo4HyMIHvMB0GA1UdDgQWBBSV/JWX0QUgmL+5885yMjh8
dS4T8DCBvwYDVR0jBIG3MIG0gBSV/JWX0QUgmL+5885yMjh8dS4T8KGBkKSBjTCB
ijELMAkGA1UEBhMCQ1oxDzANBgNVBAgTBlByYWd1ZTEPMA0GA1UEBxMGUHJhZ3Vl
MSIwIAYDVQQKExlRb3JlIFByb2dyYW1taW5nIExhbmd1YWdlMRYwFAYDVQQDEw1E
YXZpZCBOaWNob2xzMR0wGwYJKoZIhvcNAQkBFg5kYXZpZEBxb3JlLm9yZ4IJALLM
pB2Hc61YMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEFBQADQQAMZ6N0cjzTiaNx
1Jyrp3Agcc71xt47xSle5j3kDb0D7dn+HvgaNfDIW7cmDJIsiYnhxdMyezct06WS
IcewTtsR
-----END CERTIFICATE-----";

# certificate signature (literal binary value)
const cert_sig = <0c67a374723cd389a371d49caba7702071cef5c6de3bc5295ee63de40dbd03edd9fe1ef81a35f0c85bb7260c922c8989e1c5d3327b372dd3a59221c7b04edb11>;

const key_pem = "-----BEGIN RSA PRIVATE KEY-----
Proc-Type: 4,ENCRYPTED
DEK-Info: BF-CBC,C73CE02812F598E4

TTpTQq7DR2GUFrpjMVh1QzQDNp2lut/tEJlcPlX0qo7JxS8vm3N4+9Wmq3GCGdGZ
3hs/bZ/aAZRxzDguxEV03Bxy+eqP2G/FyfpzesJL4m7bdr35P8ZKXn75PJbvDzhC
7uZOH1UaLP/8OHJ8u2gK8skRF0kCtnOMLKBJYmVQFMgnFmiIMtYEtd4UitMTcxVo
Ax1m2IeIo5j3FxQ58zo2SG15p/qj470pKQD/fiLLjhFv30L4jQdWidDido4SkL1f
dFVXOpkauGI4IjM2+yAqaV1LFqV0FeRyaGxyyPC9HJow5idZ4wZQyplwA0bV9GuS
cu/KPgDZez9wrlFeb+MGLQE7tw+jKum8OVSFAjF5NfQLF2mRHlccOImuy5RBXIVq
fL7VyL/oGoUX4w4wwpUZlMlx3VqnXAoyf7NLQ50RD0M=
-----END RSA PRIVATE KEY-----";

const key_pass = "qore";

sub class_test_SSLCertificate() {
    my SSLCertificate $cert(cert_pem);
    $unit.cmp($cert.getSignature(), cert_sig, "SSLCertificate::getSignature()");
    $unit.cmp($cert.getInfo().subject.emailAddress, "david@qore.org", "SSLCertificate::getInfo()");
}

sub class_test_SSLPrivateKey() {
    my SSLPrivateKey $key(key_pem, key_pass);
    $unit.cmp($key.getVersion(), 1, "SSLPrivateKey::getVersion()");
    $unit.cmp($key.getBitLength(), 512, "SSLPrivateKey::getBitLength()");
    $unit.cmp($key.getType(), "RSA", "SSLPrivateKey::getType()");
    $unit.cmp($key.getInfo().type, "RSA", "SSLPrivateKey::getInfo()");
}

sub class_test_Condition() {
    my Mutex $m(); 
    $m.lock(); 
    on_exit $m.unlock();
    my Condition $c();
    my code $dc = sub() { while (True) { if (!$c.wait_count($m)) continue; $c.signal(); break; } };
    background $dc();
    $c.wait($m, 2191769000);
}

sub err(string $test) {
    $unit.cmp(True, False, $test);
}

sub check(string $err, string $test) {
    $unit.cmp($err, "PRIVATE-MEMBER", $test);
}

class Test2 { private { any $.a; } }

sub class_library_tests() {
    my Test $t(1, "gee", 2);
    $unit.cmp($t.getData(1), "gee", "first object");
    $unit.cmp(exists $t.testing, True, "memberGate() existence");
    $unit.cmp($t.testing, "memberGate-testing", "memberGate() value");
    $unit.cmp($t.test(), "test", "methodGate() value");
    $unit.cmp($t instanceof Test, True, "first instanceof");
    $unit.cmp($t instanceof Qore::Socket, True, "second instanceof");

    # verify private member access protection
    my string $test = "object -= private member";
    try { $t -= "a"; err($test); } catch($ex) { check($ex.err, $test); }
    $test = "object -= list of private members";
    try { $t -= ("a", "b"); err($test); } catch($ex) { check($ex.err, $test); }

    my any $t1 = new Test(1, "gee", 2);
    $test = "delete object's private member";
    try { delete $t1.a; err($test); } catch($ex) { check($ex.err, $test); }
    $test = "reassign object's private member";
    try { $t1.a = 3; err($test); } catch($ex) { check($ex.err, $test); }

    my any $t2 = new Test2();
    $test = "read object's private member";
    try { my any $x = $t2.a + $x; err($test); } catch($ex) { check($ex.err, $test); }

    # test memberGate
    $unit.cmp($t.a, "memberGate-a", "object memberGate() methods");

    # test memberNotification()
    $t.x = 1;
    # test object closure
    my code $c = $t.closure(1);
    $unit.cmp($c(2), "gee-1-2-2", "first object closure");
    $unit.cmp($c(2), "gee-1-2-3", "second object closure");
    $unit.cmp($t.t.x, 1, "memberNotification() method");

    # test callObjectMethod*()
    $unit.cmp(callObjectMethod($t1, "argTest", 1, 2), (1, 2), "callObjectMethod()");
    $unit.cmp(callObjectMethodArgs($t1, "argTest"), NOTHING, "first callObjectMethodArgs()");
    $unit.cmp(callObjectMethodArgs($t1, "argTest", 1), list(1), "second callObjectMethodArgs()");
    $unit.cmp(callObjectMethodArgs($t1, "argTest", (1, 2)), (1, 2), "third callObjectMethodArgs()");

    class_test_File();
    class_test_Program();
    class_test_SSLCertificate();
    class_test_SSLPrivateKey();
    class_test_Condition();
}

# find and context tests
sub context_tests() {
    my hash $q = ( "name" : ("david", "renata", "laura", "camilla", "isabella"),
		   "age"  : (37, 30, 7, 4, 1 ) );

    # initial matrix
    my hash $t = ( "name" : ("david", "renata", "laura", "camilla", "isabella"),
		   "key1" : (1, 2, 3, 4, 5),
		   "key2" : (4, 5, 6, 7, 8),
		   "key3" : (7, 8, 9, 0, 1),
		   "key4" : (2, 3, 4, 5, 6),
		   "key5" : (3, 4, 5, 6, 7) );

    # resulting transposed matrix
    my hash $i = ( "david"    : (1, 4, 7, 2, 3),
		   "renata"   : (2, 5, 8, 3, 4),
		   "laura"    : (3, 6, 9, 4, 5),
		   "camilla"  : (4, 7, 0, 5, 6),
		   "isabella" : (5, 8, 1, 6, 7) );

    my hash $h;
    context q ($q) sortBy (%name)
	context t ($t) where (%q:name == %name) sortBy (%key2)
	    $h.%q:name = (%key1, %t:key2, %key3, %key4, %key5);

    $unit.cmp($h, $i, "context");

    my int $age = find %age in $q where (%name == "david");
    $unit.cmp($age, 37, "find");

    my list $ages = find %age in $q where (%name == "david" || %name == "isabella");
    $unit.cmp($ages, (37, 1), "list find"); 
    context ($q) {
	$unit.cmp(%%, ("name" : "david", "age" : 37), "context row");
        $unit.cmp(cx_first(), True, "cx_first()");
        $unit.cmp(cx_last(), False, "cx_last()");
        $unit.cmp(cx_pos(), 0, "cx_pos()");
        $unit.cmp(cx_total(), 5, "cx_total()");
        $unit.cmp(cx_value("name"), "david", "cx_value()");
	break;
    }

    my HashListIterator $qi($q);
    while ($qi.next()) {
	$unit.cmp($qi.getRow(), ("name" : "david", "age" : 37), "HashListIterator::getRow()");
        $unit.cmp($qi.first(), True, "HashListIterator::first()");
        $unit.cmp($qi.last(), False, "HashListIterator::last()");
        $unit.cmp($qi.index(), 0, "HashListIterator::index()");
        $unit.cmp($qi.max(), 5, "HashListIterator::max()");
        $unit.cmp($qi.name, "david", "HashListIterator::memberGate()");
	break;
    }

    my HashListReverseIterator $rqi($q);
    while ($rqi.next()) {
	$unit.cmp($rqi.getRow(), ("name" : "isabella", "age" : 1), "HashListReverseIterator::getRow()");
        $unit.cmp($rqi.first(), True, "HashListReverseIterator::first()");
        $unit.cmp($rqi.last(), False, "HashListReverseIterator::last()");
        $unit.cmp($rqi.index(), 4, "HashListReverseIterator::index()");
        $unit.cmp($rqi.max(), 5, "HashListReverseIterator::max()");
        $unit.cmp($rqi.name, "isabella", "HashListReverseIterator::memberGate()");
	break;
    }

    # convert the hash of lists to a list of hashes
    $qi.set(-1);
    my list $l = map $qi.getRow(), $qi;

    my ListHashIterator $lqi($l);
    while ($lqi.next()) {
	$unit.cmp($lqi.getRow(), ("name" : "david", "age" : 37), "ListHashIterator::getRow()");
        $unit.cmp($lqi.first(), True, "ListHashIterator::first()");
        $unit.cmp($lqi.last(), False, "ListHashIterator::last()");
        $unit.cmp($lqi.index(), 0, "ListHashIterator::index()");
        $unit.cmp($lqi.max(), 5, "ListHashIterator::max()");
        $unit.cmp($lqi.name, "david", "ListHashIterator::memberGate()");

        my ListHashIterator $ni = $lqi.copy();
	$unit.cmp($ni.getRow(), ("name" : "david", "age" : 37), "ListHashIterator::getRow() (copy)");
        $unit.cmp($ni.first(), True, "ListHashIterator::first() (copy)");
        $unit.cmp($ni.index(), 0, "ListHashIterator::index() (copy)");
	break;
    }

    my ListHashReverseIterator $lrqi($l);
    while ($lrqi.next()) {
	$unit.cmp($lrqi.getRow(), ("name" : "isabella", "age" : 1), "ListHashReverseIterator::getRow()");
        $unit.cmp($lrqi.first(), True, "ListHashReverseIterator::first()");
        $unit.cmp($lrqi.last(), False, "ListHashReverseIterator::last()");
        $unit.cmp($lrqi.index(), 4, "ListHashReverseIterator::index()");
        $unit.cmp($lrqi.max(), 5, "ListHashReverseIterator::max()");
        $unit.cmp($lrqi.name, "isabella", "ListHashReverseIterator::memberGate()");

        my ListHashReverseIterator $ni = $lrqi.copy();
	$unit.cmp($ni.getRow(), ("name" : "isabella", "age" : 1), "ListHashReverseIterator::getRow() (copy)");
        $unit.cmp($ni.first(), True, "ListHashReverseIterator::first() (copy)");
        $unit.cmp($ni.index(), 4, "ListHashReverseIterator::index() (copy)");
	break;
    }
}

const a = "key";
const b = 1.0;
const i = 1;
const l = (1, 2, 3);
const chash = ( a : "one", b : l );

const exp   = elements l;
const hexp2 = chash{b};
const t1 = "goodbye";

class C {
    const a = 2;
    static int t1() { return 2; }
    static int t2() { return Type::i; }
}

int sub d1() { return 2; }
int sub d2() { return Type::i; }
const K1 = "b";
const K2 = "a";

namespace Type {
    const i = 2;
    const hithere = 2;
}

namespace NTest {
    const t1 = "hello";

    namespace Type {
        const i = 1;
    }

    const Type::hithere = 1;

    class C {
	const a = 1;
	static int t1() { return 1; }
	static int t2() { return Type::i; }
    }

    int sub d1() { return 1; }
    int sub d2() { return Type::i; }

    const K1 = "a";
    const K2 = "b";
    const H = (K1: 1, K2: 2);

    class T1;

    sub test() {
        $unit.cmp(t1, "hello", "1st namespace constant resolution");
        $unit.cmp(Type::i, 1, "2nd namespace constant resolution");
        $unit.cmp(Type::hithere, 1, "3rd namespace constant resolution");
        $unit.cmp(C::a, 1, "class constant resolution in namespace context");
        $unit.cmp(C::t1(), 1, "static method resolution in namespace context");
        $unit.cmp(C::t2(), 1, "constant resolution in namespace context in class code");
        $unit.cmp(d1(), 1, "first function resolution in namespace context");
        $unit.cmp(d2(), 1, "second function resolution in namespace context");
        $unit.cmp(H.a, 1, "hash key constant resolution in namespace context");
    }
}

namespace NTest {
    const t2 = 2;
}

const NTest::Type::val1 = 1;

const Qore::myconst = 1;

sub constant_tests() {
    $unit.cmp(i, 1, "simple constant");
    $unit.cmp(type(Type::val1), "integer", "first namespace constant");
    $unit.cmp(Qore::myconst, NTest::Type::val1, "second namespace constant");
    $unit.cmp(NTest::Type::i, 1, "third namespace constant"); 
    $unit.cmp(chash{b}, (1, 2, 3), "indirect constant");
    $unit.cmp(exp, 3, "evaluated constant");
    $unit.cmp(hexp2, (1, 2, 3), "evaluated constant hash");
    NTest::test();
}

sub hmac_tests() {
    my string $str = "Hello There This is a Test - 1234567890";
    my string $key = "a key";

    if (HAVE_MD2)
        $unit.cmp(MD2_hmac($str, $key),    "27f5f17500b408e97643403ea8ef1413", "MD2 hmac");
    $unit.cmp(MD4_hmac($str, $key),        "053d084f321a3886e60166ebd9609ce1", "MD4 hmac");
    $unit.cmp(MD5_hmac($str, $key),        "87505c6164aaf6ca6315233902a01ef4", "MD5 hmac");
    $unit.cmp(DSS_hmac($str, $key),        "37a3cc73159aa129b0eb22bbdf4b9309d389f629", "DSS hmac");
    $unit.cmp(DSS1_hmac($str, $key),       "37a3cc73159aa129b0eb22bbdf4b9309d389f629", "DSS1 hmac");
    $unit.cmp(SHA_hmac($str, $key),        "0ad47c8d36dc4606d52f7e4cbd144ef2fda492a0", "SHA hmac");
    $unit.cmp(SHA1_hmac($str, $key),       "37a3cc73159aa129b0eb22bbdf4b9309d389f629", "SHA1 hmac");
    $unit.cmp(RIPEMD160_hmac($str, $key),  "4bca70bca1601aba57624eeb2606535cb12f2079", "RIPEMD-160 hmac");
    if (HAVE_MDC2)
        $unit.cmp(MDC2_hmac($str, $key),       "e0ef6a6803e58807c5db395e180a999c", "MDC2 hmac");
    if (HAVE_SHA224)
        $unit.cmp(SHA224_hmac($str, $key),     "fad5667fa5aa412044555b7e077fced62372fe9c6ce20815609da12c", "SHA224 hmac");
    if (HAVE_SHA256)
        $unit.cmp(SHA256_hmac($str, $key),     "1c90c21e227712b62019ff831f34cba22c2e70f1a902651ef69a70705ee0f754", "SHA256 hmac");
    if (HAVE_SHA384)
        $unit.cmp(SHA384_hmac($str, $key),     "e2c253c6dcb050990b4da3cee95cd7b227f43388fa8116f476f59395af295d0d3bb7156ab2fcd0663b0500249a7a0865", "SHA384 hmac");
    if (HAVE_SHA512)
       $unit.cmp(SHA512_hmac($str, $key),     "8dcefd7ea3f90ff1c822b5e9547fc36edf78c3e4ce13d47510a212a406bdda1a4094e7ea5ade90e1c736e204d331a814520eba49f3d074e2c261208de07264f6", "SHA512 hmac");
}

sub digest_tests() {
    my string $str = "Hello There This is a Test - 1234567890";

    if (HAVE_MD2)
        $unit.cmp(MD2($str), "349ea9f6c9681278cf86955dabd72d31", "MD2 digest");
    $unit.cmp(MD4($str), "675d84fbf5d63e0d68c04577c3298fdc", "MD4 digest");
    $unit.cmp(MD5($str), "bcbece19c1fe41d8c9e2e6134665ba5b", "MD5 digest");
    $unit.cmp(DSS($str), "f4bc2c85698aae8961d626e2c590852b2d081199", "DSS digest");
    $unit.cmp(DSS1($str), "f4bc2c85698aae8961d626e2c590852b2d081199", "DSS1 digest");
    $unit.cmp(SHA($str), "99910d63f10286e8dda3c4a57801996f48f25b4b", "SHA digest");
    $unit.cmp(SHA1($str), "f4bc2c85698aae8961d626e2c590852b2d081199", "SHA1 digest");
    $unit.cmp(RIPEMD160($str), "8f32702e0146d5db6145f36271a4ddf249c087ae", "RIPEMD-160 digest");
}

sub crypto_tests() {
    my string $str = "Hello There This is a Test - 1234567890";

    my string $key = "1234567812345abcabcdefgh";
    my binary $x = des_ede_encrypt_cbc($str, $key);
    my string $xstr = des_ede_decrypt_cbc_to_string($x, $key);
    $unit.cmp($str, $xstr, "triple DES 2 key encrypt-decrypt");

    $x = des_ede3_encrypt_cbc($str, $key);
    $xstr = des_ede3_decrypt_cbc_to_string($x, $key);
    $unit.cmp($str, $xstr, "triple DES 3 key encrypt-decrypt");

    $x = desx_encrypt_cbc($str, $key);
    $xstr = desx_decrypt_cbc_to_string($x, $key);
    $unit.cmp($str, $xstr, "DESX encrypt-decrypt");

    $x = blowfish_encrypt_cbc($str, $key);
    $xstr = blowfish_decrypt_cbc_to_string($x, $key);
    $unit.cmp($str, $xstr, "blowfish encrypt-decrypt");

    $x = rc4_encrypt($str, $key);
    $xstr = rc4_decrypt_to_string($x, $key);
    $unit.cmp($str, $xstr, "rc4 encrypt-decrypt");

    $x = rc2_encrypt_cbc($str, $key);
    $xstr = rc2_decrypt_cbc_to_string($x, $key);
    $unit.cmp($str, $xstr, "rc2 encrypt-decrypt");

    $x = cast5_encrypt_cbc($str, $key);
    $xstr = cast5_decrypt_cbc_to_string($x, $key);
    $unit.cmp($str, $xstr, "CAST5 encrypt-decrypt");

    my binary $bkey = des_random_key();
    $x = des_encrypt_cbc($str, $bkey);
    $xstr = des_decrypt_cbc_to_string($x, $bkey);
    $unit.cmp($str, $xstr, "DES random single key encrypt-decrypt");
}

list sub closures(string $x) {
    my int $a = 1;
    
    my code $inc = string sub (any $y) {
	return sprintf("%s-%n-%n", $x, $y, ++$a);
    };

    my code $dec = string sub (any $y) {
	return sprintf("%s-%n-%n", $x, $y, --$a);
    };

    return ($inc, $dec);
}

sub closure_tests() {
    my (code $inc, code $dec) = closures("test");
    $unit.cmp($inc(5), "test-5-2", "first closure");
    $unit.cmp($inc(7), "test-7-3", "second closure");
    $unit.cmp($dec(3), "test-3-2", "third closure");

    my code $c = sub (*reference $r) {
        $r = "hi";
    };
    my string $str;
    $c(\$str);
    $unit.cmp($str, "hi", "closure with reference arg");
}

sub format_date_tests() {
    my date $d = 2005-04-01T08:02:05.001;

    $unit.cmp(format_date("YY", $d), "05", "last two digits of year");
    $unit.cmp(format_date("YYYY", $d), "2005", "four-digit year");
    $unit.cmp(format_date("M", $d), "4", "non zero-padded month number (1-12)");
    $unit.cmp(format_date("MM", $d), "04", "zero-padded month number (01-12)");
    $unit.cmp(format_date("Month", $d), "April", "long month string (ex: \"January\")");
    $unit.cmp(format_date("MONTH", $d), "APRIL", "long month string capitalized (ex: \"JANUARY\")");
    $unit.cmp(format_date("Mon", $d), "Apr", "abbreviated month (ex: \"Jan\")");
    $unit.cmp(format_date("MON", $d), "APR", "abbreviated month, capitalized (ex: \"JAN\")");
    $unit.cmp(format_date("D", $d), "1", "non zero-padded day number (1 - 31)");
    $unit.cmp(format_date("DD", $d), "01", "zero-padded day number (01 - 31)");
    $unit.cmp(format_date("Day", $d), "Friday", "long day of week string (ex: \"Monday\")");
    $unit.cmp(format_date("DAY", $d), "FRIDAY", "long day of week string, capitalized (ex: \"MONDAY\")");
    $unit.cmp(format_date("Dy", $d), "Fri", "abbreviated day of week string (ex: \"Mon\")");
    $unit.cmp(format_date("DY", $d), "FRI", "abbreviated day of week string capitalized (ex: \"MON\")");
    $unit.cmp(format_date("H", $d), "8", "non zero-padded hour number (0 - 23)");
    $unit.cmp(format_date("HH", $d), "08", "zero-padded hour number (00 - 23)");
    $unit.cmp(format_date("h", $d), "8", "non zero-padded hour number, 12-hour clock (1 - 12)");
    $unit.cmp(format_date("hh", $d), "08", "zero-padded hour number, 12-hour clock (01 - 12)");
    $unit.cmp(format_date("m", $d), "2", "non zero-padded minute number (0 - 59)");
    $unit.cmp(format_date("mm", $d), "02", "zero-padded minute number (00 - 59)");
    $unit.cmp(format_date("S", $d), "5", "non zero-padded second number (0 - 59)");
    $unit.cmp(format_date("SS", $d), "05", "zero-padded second number (00 - 59)");
    $unit.cmp(format_date("P", $d), "AM", "\"AM\" or \"PM\" (upper-case)");
    $unit.cmp(format_date("p", $d), "am", "\"am\" or \"pm\" (lower-case)");
    $unit.cmp(format_date("u", $d), "1", "non zero-padded millisecond number (0 - 999)");
    $unit.cmp(format_date("uu", $d), "001", "zero-padded millisecond number (000 - 999)");
    $unit.cmp(format_date("ms", $d), "001", "zero-padded millisecond number (000 - 999)");
    $unit.cmp(format_date("x", $d), "1000", "non zero-padded microsecond number (0 - 999999)");
    $unit.cmp(format_date("xx", $d), "001000", "zero-padded microsecond number (000000 - 999999)");
    $unit.cmp(format_date("y", $d), "001", "microseconds, with trailing zeros removed (suitable for use after the '.')");

    # commented out tests that only work when run in European CET time zone
    #$unit.cmp(format_date("z", $d), "CEST", "local time zone name (ex: \"EST\") if available, otherwise the UTC offset (ex: \"+01:00\")");
    #$unit.cmp(format_date("Z", $d), "+02:00", "time zone UTC offset like +HH:mm[:SS] (ex: \"+01:00\"), seconds are only included if non-zero");
}

sub number_tests() {
    $unit.cmp(string(10.2n), "10.2", "first number"); 
    $unit.cmp(string(-10.2n), "-10.2", "second number"); 
    $unit.cmp(string(1.000000000099999999n), "1.000000000099999999", "third number"); 
    $unit.cmp(10.245n.toString(NF_Scientific), "1.0245e+01", "fourth number"); 
    $unit.cmp((-10.245n).toString(NF_Scientific), "-1.0245e+01", "fifth number"); 
    $unit.cmp(0.10245n.toString(NF_Scientific), "1.0245e-01", "sixth number"); 
    $unit.cmp((-0.10245n).toString(NF_Scientific), "-1.0245e-01", "seventh number"); 
    $unit.cmp(1.0245n.toString(NF_Scientific), "1.0245e+00", "sixth number"); 
    $unit.cmp((-1.0245n).toString(NF_Scientific), "-1.0245e+00", "seventh number"); 
    $unit.cmp(10.245n.toString(), "10.245", "eighth number"); 
    $unit.cmp((-10.245n).toString(), "-10.245", "ninth number"); 
    $unit.cmp(0.10245n.toString(), "0.10245", "tenth number"); 
    $unit.cmp((-0.10245n).toString(), "-0.10245", "eleventh number"); 
    $unit.cmp(1.0245n.toString(), "1.0245", "twelfth number"); 
    $unit.cmp((-1.0245n).toString(), "-1.0245", "thirteenth number"); 
    $unit.cmp(10.001999999999n.toString(), "10.001999999999", "fourteenth number"); 
    $unit.cmp((-10.001999999999n).toString(), "-10.001999999999", "fifteenth number"); 
    $unit.cmp(0.10001999999999n.toString(), "0.10001999999999", "sixteenth number"); 
    $unit.cmp((-0.10001999999999n).toString(), "-0.10001999999999", "seventeenth number"); 
    $unit.cmp(1.0001999999999n.toString(), "1.0001999999999", "eighteenth number"); 
    $unit.cmp((-1.0001999999999n).toString(), "-1.0001999999999", "nineteenth number"); 
    $unit.cmp(0.8n.toString(), "0.8", "number rounding 1");
    $unit.cmp(0.8n.toString(NF_Scientific), "8e-01", "number rounding 2");
    $unit.cmp((-0.8n).toString(), "-0.8", "number rounding 3");
    $unit.cmp((-0.8n).toString(NF_Scientific), "-8e-01", "number rounding 4");
    $unit.cmp((34.9n * 100).toString(), "3490", "number rounding 5");
    $unit.cmp(1e50n.toString(), "100000000000000000000000000000000000000000000000000", "number rounding 5");
    $unit.cmp((-1e50n).toString(), "-100000000000000000000000000000000000000000000000000", "number rounding 6");
}

class GcTest { 
    public {
        code $.inc;
        any $.a;
        *GcTest $.b;
        *GcTest $.c;
    }

    private {
        *GcTest $.o;
    }

    constructor(code $i, *GcTest $obj) {
        $.inc = $i;
        $.o = $obj;
    }
    
    destructor() {
        # increment static counter in destructor
        call_function($.inc);
    } 
    
    set(*GcTest $obj) {
        $.o = $obj;
    }
}

sub gc_tests() {
    if (!HAVE_DETERMINISTIC_GC)
        return;

    my int $cnt = 0;
    my code $inc = sub () { ++$cnt; };

    # make circular references
    {
        my GcTest $obj1($inc);
        $obj1.a = $obj1;
    }
    $unit.cmp($cnt, 1, "recursive gc 1");

    {
        my GcTest $obj2($inc);
        $obj2.a = $obj2;
    }
    $unit.cmp($cnt, 2, "recursive gc 2");

    {
        my GcTest $obj3($inc);
        $obj3.a.a = $obj3;
    }
    $unit.cmp($cnt, 3, "recursive gc 3");

    {
        my GcTest $obj4($inc);
        $obj4.a = list($obj4);
    }
    $unit.cmp($cnt, 4, "recursive gc 4");

    {
        my GcTest $obj5($inc);
        my GcTest $obj6($inc);
        $obj5.a = $obj6;
        $obj6.b = $obj5;
    }
    $unit.cmp($cnt, 6, "recursive gc 6");

    {
        my GcTest $obj7($inc);
        $obj7.a = $obj7;
        $obj7.b = $obj7;
    }
    $unit.cmp($cnt, 7, "recursive gc 7");

    {
        my GcTest $obj8($inc);
        my GcTest $obj9($inc);
        
        $obj8.a = ("a": $obj9, "b": $obj9);
        $obj9.b = $obj8;
        $obj9.c = $obj8;
    }
    $unit.cmp($cnt, 9, "recursive gc 9");

    {
        my GcTest $obj10($inc);
        my GcTest $obj11($inc);
        $obj10.set($obj11);
        $obj11.set($obj10);
    }
    $unit.cmp($cnt, 11, "recursive gc 11");

    {
        my GcTest $obj12($inc);
        {
            my GcTest $obj13($inc);
            
            $obj12.a = $obj13;
            $obj13.a = $obj12;
        }
    }
    $unit.cmp($cnt, 13, "recursive gc 13-1");

    {
        my GcTest $t1($inc);
        my GcTest $t2($inc);
        $t1.set($t2);
        $t2.set($t1);
        $t1.set();

        $unit.cmp($cnt, 13, "recursive gc 13-2");
    }
    $unit.cmp($cnt, 15, "recursive gc 15-1");

    {
        my GcTest $t1($inc);
        $t1.set($t1);
        $t1.set();

        $unit.cmp($cnt, 15, "recursive gc 15-2");
    }
    $unit.cmp($cnt, 16, "recursive gc 16-1");

    {
        my GcTest $t1($inc);
        {
            my GcTest $t2($inc);
            $t1.set($t2);
            $t2.b = $t1;
            {
                my GcTest $t3($inc);
                $t2.set($t3);
                $t2.b = $t1;
                {
                    my GcTest $t4($inc);
                    $t3.set($t4);
                    $t4.set($t1);
                    $t3.b = $t2;
                    $t4.b = $t3;
                }
            }
        }
        $unit.cmp($cnt, 16, "recursive gc 16-2");
    }
    $unit.cmp($cnt, 20, "recursive gc 20");

    {
        my GcTest $t1($inc);
        {
            my GcTest $t2($inc);
            $t1.set($t2);
            $t2.b = $t1;
            {
                my GcTest $t3($inc);
                $t2.set($t3);
                $t3.b = $t2;
                $t3.c = $t1;
                $t1.b = $t3;
                {
                    my GcTest $t4($inc);
                    $t3.set($t4);
                    $t4.set($t1);
                    $t4.b = $t2;
                    $t4.c = $t3;
                    $t2.c = $t4;
                    $t1.c = $t4;
                }
            }
        }

        $unit.cmp($cnt, 20, "recursive gc 20-2");

    }
    $unit.cmp($cnt, 24, "recursive gc 24");
}

sub background_tests() {
    my int $i = 0;
    background delete $i;
    background remove $i;

    # do negative tests
    my Program $p();
    try {
        $p.parse("my int $i; background ($i *= 10);background ($i /= 10);background ($i -= 10);background ($i += 10);background ($i %= 10);background ($i >>= 10);background ($i <<= 10);background ++$i;background $i++;background --$i;background $i--;my string $str;background splice $str, 0;background extract $str, 0;", "bg");
        $unit.cmp(False, True, "background negative");
    }
    catch (*hash $ex) {
        # count exceptions
        while ($ex) {
            ++$i;
            $ex = $ex.next;
        }
        $unit.cmp($i, 13, "background negative");
    }
}

sub type_code_test() {
    $unit.cmp(True.typeCode(), NT_BOOLEAN, "typeCode() bool");
    $unit.cmp("foo".typeCode(), NT_STRING, "typeCode() string");
    $unit.cmp(1.typeCode(), NT_INT, "typeCode() int");
    $unit.cmp(1n.typeCode(), NT_NUMBER, "typeCode() number");
    $unit.cmp(now().typeCode(), NT_DATE, "typeCode() date");
    $unit.cmp(1.2.typeCode(), NT_FLOAT, "typeCode() float");
    $unit.cmp((1,2,).typeCode(), NT_LIST, "typeCode() list");
    $unit.cmp(("foo":1).typeCode(), NT_HASH, "typeCode() bool");
    $unit.cmp(NULL.typeCode(), NT_NULL, "typeCode() NULL");
    $unit.cmp(NOTHING.typeCode(), NT_NOTHING, "typeCode() NOTHING");
}

sub mime_tests() {
    my string $str = "This is a test: æéìœü";
    $unit.cmp($str, mime_decode_quoted_printable(mime_encode_quoted_printable($str)), "MIME: quoted printable");
    $unit.cmp($str, mime_decode_base64_to_string(mime_encode_base64($str)), "MIME: base64");
    $unit.cmp($str, mime_decode_header(mime_encode_header_word_q($str)), "MIME: header word q");
    $unit.cmp($str, mime_decode_header(mime_encode_header_word_b($str)), "MIME: header word b");
}

const DataMap = (
    # this will take the "Id" element of any "^attributes^" hash in the input record
    "id": "^attributes^.Id",
    # this maps input "name" -> output "name"
    "name": True,
    # this marks "explicit_count" as an integer field mapped from the input "Count" field
    "explicit_count": ("type": "int", "name": "Count"),
    # runs the given code on the input record and retuns the result - the number of "Products" sub-records
    "implicit_count": int sub (any $ignored, hash $rec) { return $rec.Products.size(); },
    # converts the given field to a date in the specified format
    "order_date": ("name": "OrderDate", "date_format": "DD.MM.YYYY HH:mm:SS.us"),
    # converts the given field to a number in the specified format
    "unit_price": ("name": "UnitPrice", "number_format": ".,"),
    # returns a constant value
    "target_sys": ("constant": "Orders"),
    # returns structured output
    "sr0.sr1.key0": ("constant": "key0"),
    "sr0.sr1.^attributes^.key0": ("constant": "key0"),
    "sr0.sr1.^attributes^.key1": ("constant": "key1"),
    "sr0.sr1.^attributes^.type": ("name": "Type", "code": *string sub (*string $v, hash $rec) { return $v ? $v.lwr() : NOTHING;}, "default": "unknown"),
    "sr0.sr1.key1": ("constant": "key1"),
    "sr0.store_name": "StoreInfo.StoreName",
);

const MapInput = ((
    "^attributes^": ("Id": 1),
    "name": "John Smith",
    "Count": 1,
    "OrderDate": "02.01.2014 10:37:45.103948",
    "UnitPrice": "1.543,50",
    "StoreInfo": ("StoreName": "Store1"),
    "Products": ((
        "ProductName": "Widget 1",
        "Quantity": 1,
         ),                 
    )), (
    "^attributes^": ("Id": 2),
    "name": "Steve Austin",
    "Type": "Retail",
    "Count": 2,
    "OrderDate": "04.01.2014 19:21:08.882634",
    "UnitPrice": "9,95",
    "StoreInfo": ("StoreName": "Store2"),
    "Products": ((
        "ProductName": "Widget X",
        "Quantity": 4,
        ), (
        "ProductName": "Widget 2",
        "Quantity": 2,
        ),
    )),
);

const MapOutput = ((
    "id": 1,
    "name": "John Smith",
    "explicit_count": 1,
    "implicit_count": 1,
    "order_date": 2014-01-02T10:37:45.103948,
    "unit_price": 1543.50n,
    "target_sys": "Orders",
    "sr0": (
        "sr1": (
            "key0": "key0",
            "^attributes^": (
                "key0": "key0",
                "key1": "key1",
                "type": "unknown",
            ),
            "key1": "key1",
        ),
        "store_name": "Store1",
    ),
    ), (
    "id": 2,
    "name": "Steve Austin",
    "explicit_count": 2,
    "implicit_count": 2,
    "order_date": 2014-01-04T19:21:08.882634,
    "unit_price": 9.95n,
    "target_sys": "Orders",
    "sr0": (
        "sr1": (
            "key0": "key0",
            "^attributes^": (
                "key0": "key0",
                "key1": "key1",
                "type": "retail",
            ),
            "key1": "key1",
        ),
        "store_name": "Store2",
    ),
    ),
);

sub mapper_tests() {
    my Mapper $map(DataMap);
    my list $l = $map.mapAll(MapInput);
    #printf("%N\n", $l);
    $unit.cmp($l, MapOutput, "Mapper::mapAll()");
    $unit.cmp($map.getCount(), 2, "1:Mapper::getCount()");
    $l = map $map.mapData($1), MapInput;
    $unit.cmp($l, MapOutput, "map Mapper::mapData()");
    $unit.cmp($map.getCount(), 4, "1:Mapper::getCount()");
}

const CsvInput = "UK,1234567890,\"Sony, Xperia S\",31052012
UK,1234567891,\"Sony, Xperia S\",31052012
UK,1234567892,\"Sony, Xperia S\",31052012
UK,1234567893,\"Sony, Xperia S\",31052012";

const CsvRecords = (
    ("cc": "UK", "serno": 1234567890, "desc": "Sony, Xperia S", "received": 2012-05-31),
    ("cc": "UK", "serno": 1234567891, "desc": "Sony, Xperia S", "received": 2012-05-31),
    ("cc": "UK", "serno": 1234567892, "desc": "Sony, Xperia S", "received": 2012-05-31),
    ("cc": "UK", "serno": 1234567893, "desc": "Sony, Xperia S", "received": 2012-05-31),
);

sub csvutil_tests() {
    my hash $opts = (
        "fields": ("cc": "string", "serno": "int", "desc": "string", "received": ("type": "date", "format": "DDMMYYYY")),
        );

    my CsvDataIterator $i(CsvInput, $opts);
    my list $l = map $1, $i;
    $unit.cmp($l, CsvRecords, "CsvDataIterator 1");

    # test with empty data and header lines
    $i = new CsvDataIterator("", ("header-lines": 1));
    $unit.cmp($i.next(), False, "CsvDataIterator 2");
}

sub module_tests() {
    mime_tests();
    mapper_tests();
    csvutil_tests();
}

sub do_tests() {
    on_exit $counter.dec();
    try {
	for (my int $i = 0; $i < $unit.option("iters"); $i++) {
	    if ($unit.verbose())
		printf("TID %d: iteration %d\n", gettid(), $i);
            number_tests();
	    operator_test();
	    array_tests();
	    hash_tests();
	    logic_tests();
	    statement_tests();
	    recursive_function_test();
	    parameter_tests();
	    class_library_tests();
	    function_tests();
	    context_tests();
	    constant_tests();	
	    crypto_tests();
	    digest_tests();
            hmac_tests();
	    closure_tests();
	    format_date_tests();
            module_tests();
            type_code_test();
            gc_tests();
	    if ($unit.option("bq"))
		backquote_tests();
	}
    }
    catch () {
        $unit.errorInc();
	rethrow;	
    }
}

sub main() {
    # run regression background tests
    background_tests();

    our Counter $counter();
    my int $threads = $unit.option("threads");
    while ($threads--) {
	$counter.inc();
	background do_tests();
    }

    $counter.waitForZero();
}

main();
