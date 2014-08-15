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
%requires qore >= 0.8.4

# for Mime tests
%requires Mime

# for Mapper tests
%requires Mapper

# for CsvUtil tests
%requires CsvUtil

# global variables needed for tests
our Test $to("program-test.q");
our Test $ro("readonly");
our (hash $o, int $errors);
our hash $thash;

sub usage() {
    printf(
"usage: %s [options] <iterations>
  -h,--help         shows this help text
  -b,--backquote    include backquote tests (slow)
  -t,--threads=ARG  runs tests in ARG threads
  -v,--verbose=ARG  sets verbosity level to ARG
", 
	   get_script_name());
    exit(1);
}

const opts = 
    ( "verbose" : "verbose,v:i+",
      "help"    : "help,h",
      "bq"      : "backquote,b",
      "threads" : "threads,t=i" );

sub parse_command_line() {
    my GetOpt $g(opts);
    $o = $g.parse(\$ARGV);
    if (exists $o."_ERRORS_") {
        printf("%s\n", $o."_ERRORS_"[0]);
        exit(1);
    }
    if ($o.help)
	usage();

    $o.iters = shift $ARGV;
    if (elements $ARGV) {
	printf("error, excess arguments on command-line\n");
	usage();
    }

    if (!$o.iters)
	$o.iters = 1;
    if (!$o.threads)
	$o.threads = 1;
}

sub test_value(any $v1, any $v2, string $msg) {
    if ($v1 === $v2) {
	if ($o.verbose)
	    printf("OK: %s test\n", $msg);
    }
    else {
	printf("ERROR: %s test failed! (%N != %N)\n", $msg, $v1, $v2);
	#printf("%s%s", dbg_node_info($v1), dbg_node_info($v2));
	$errors++;
    }
    $thash.$msg = True;
}

sub test_xrange(list $correct, RangeIterator $testing, string $message) {
    my list $l;
    foreach my int $i in ($testing)
        push $l, $i;

    if ($correct === $l) {
        if ($o.verbose)
            printf("OK: %s test\n", $message);
    }
    else {
        printf("ERROR: $s test failed! (%N != %N)\n", $message, $correct, $l);
        $errors++;
    }
    $thash.$message= True;
}

int sub test1() { return 1;} int sub test2() { return 2; } 
list sub test3() { return (1, 2, 3); }

sub array_helper(list $a) {
    $a[1][1] = 2;
    test_value($a[1][1], 2, "passed local array variable assignment");    
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

    if ($o.verbose)
	print("%%%% array tests\n");
    $a = 1, 2, 3, 4, 5;
    test_value(elements $a, 5, "elements operator");
    test_value($a[1], 2, "single-dimensional list dereference");
    $b = 1, 2, (3, 4, 5), 6, 7;
    test_value($b[2][1], 4, "multi-dimensional list dereference");
    delete $b;
    test_value($b[2][1], NOTHING, "multi-dimensional list dereference after delete operator");
    $b = $a;
    $a[1] = "hello";
    test_value($a[1], "hello", "list dereference after list assignment and element reassignment");
    test_value($b[1], 2, "list dereference of source list");
    $a[0][1] = "hello";
    $c[10]{"testing"} = "well then";
    test_value($a[0][1], "hello", "second multi-dimensional list dereference");
    test_value($a[1][500], NOTHING, "non-existent element deference");
    test_value(int($c[10].testing), 0, "hash list element dereference");
    test_value($c[10]{"testing"}, "well then", "hash element in list dereference");
    $d = test1(), test2();
    test_value($d[1], 2, "list element dereference with evaluation");
    $b = $a = 1, 2, 3;
    delete $a[2];
    test_value($a[2] != $b[2], True, "shared list element comparison after delete");
    $a[1][1] = 3;
    test_value($a[1][1], 3, "array variable assignment before copy");
    array_helper($a);
    test_value($a[1][1], 3, "array variable assignment after copy");
    array_helper($a);
    test_value(list_return()[0], 1, "simple list return and deref(e)");
    test_value(list_return()[1], 2, "list return with function element result and deref(e)");
    test_value(list_return("gee")[2], "gee", "list return with local variable result and deref(e)");
    $a = 1, 2, 3;
    $a += 4, 5, 6;
    test_value($a[3], 4, "first list list plus-equals concatenation");
    $a += 7;
    test_value($a[6], 7, "list element plus-equals concatenation");
    $a += list(8);
    test_value($a[7], 8, "second list list plus-equals concatenation");
    $a = (1, 2, 3) + (4, 5, 6);
    test_value($a[3], 4, "first list list plus operator concatenation");
    $a = 1, 2, 3;
    $b = 4, 5, 6;
    $c = $a + $b;
    test_value($c[4], 5, "second list list plus operator concatenation");
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

    test_value(sort($l1), (1,2,3,4,5,6), "first sort()");
    test_value(sort($l2), ("five", "four", "one", "six", "three", "two"), "second sort()");
    test_value(sort($hl, \hash_sort_callback()), $sorted_hl, "sort() with function call reference callback");
    test_value(sort($hl, \$s.hash()), $sorted_hl, "sort() with object method callback");
    test_value(sort($hl, "hash_sort_callback"), $sorted_hl, "sort() with string function name callback");
    test_value(sort($hl, $hash_compare), $sorted_hl, "sort() with closure callback");

    my list $r_sorted_hl = reverse($sorted_hl);
    test_value(sortDescending($l1), (6,5,4,3,2,1), "first sortDescending()");
    test_value(sortDescending($l2), ("two", "three", "six", "one", "four", "five"), "second sortDescending()");
    test_value(sortDescending($hl, \SC::hash_sort_callback()), $r_sorted_hl, "first sortDescending() with callback");
    test_value(sortDescending($hl, \$s.hash()), $r_sorted_hl, "second sortDescending() with callback");
    test_value(sortDescending($hl, "hash_sort_callback"), $r_sorted_hl, "third sortDescending() with callback");
    test_value(sortDescending($hl, $hash_compare), $r_sorted_hl, "sortDescending() with closure callback");

    $hl += ( "key1" : 3, "key2" : "five-o" );
    test_value(sortStable($hl, \hash_sort_callback()), $stable_sorted_hl, "first sortStable() with callback");
    test_value(sortStable($hl, \$s.hash()), $stable_sorted_hl, "second sortStable() with callback");
    test_value(sortStable($hl, "hash_sort_callback"), $stable_sorted_hl, "third sortStable() with callback");
    test_value(sortStable($hl, $hash_compare), $stable_sorted_hl, "sortStable() with closure callback");

    my list $r_stable_sorted_hl = reverse($stable_sorted_hl);
    test_value(sortDescendingStable($hl, \SC::hash_sort_callback()), $r_stable_sorted_hl, "first sortDescendingStable() with callback");
    test_value(sortDescendingStable($hl, \$s.hash()), $r_stable_sorted_hl, "second sortDescendingStable() with callback");
    test_value(sortDescendingStable($hl, "hash_sort_callback"), $r_stable_sorted_hl, "third sortDescendingStable() with callback");
    test_value(sortDescendingStable($hl, $hash_compare), $r_stable_sorted_hl, "sortDescendingStable() with closure callback");

    test_value(min($l1), 1, "simple min()");
    test_value(max($l1), 6, "simple max()");
    test_value(min($hl, \hash_sort_callback()), ( "key1" : 1, "key2" : "eight" ), "first min() with callback");
    test_value(min($hl, \$s.hash()), ( "key1" : 1, "key2" : "eight" ), "second min() with callback");
    test_value(min($hl, "hash_sort_callback"), ( "key1" : 1, "key2" : "eight" ), "third min() with callback");
    test_value(max($hl, \SC::hash_sort_callback()), ( "key1" : 9, "key2" : "three" ), "first max() with callback");
    test_value(max($hl, \$s.hash()), ( "key1" : 9, "key2" : "three" ), "second max() with callback");
    test_value(max($hl, "hash_sort_callback"), ( "key1" : 9, "key2" : "three" ), "third max() with callback");
    my string $v = shift $l2;
    test_value($l2, ("two","three","four","five","six"), "array shift");
    unshift $l2, $v;
    test_value($l2, ("one","two","three","four","five","six"), "array unshift");
    # list assignment tests
    my list $l[1] = "boo";
    ($l[0], $l[1]) = "hi1";
    test_value($l, ("hi1", NOTHING), "first list assigment");
    ($l[0], $l[1]) = ("hi2", "shoo1");
    test_value($l, ("hi2", "shoo1"), "second list assigment");
    ($l[0], $l[1]) = ("hi3", "shoo2", "bean1");
    test_value($l, ("hi3", "shoo2"), "third list assigment");
    my int $v2 = pop $l1;
    test_value($v2, 5, "first pop");
    test_value($l1, (3,2,4,1,6), "second pop");
    push $l1, "hi";

    # splice tests
    test_value($l1, (3,2,4,1,6,"hi"), "push");
    splice $l1, 5;
    test_value($l1, (3,2,4,1,6), "first list splice");
    splice $l1, 3, 1;
    test_value($l1, (3,2,4,6), "second list splice");
    splice $l1, 1, 2, (4, 5, 5.5);
    test_value($l1, (3,4,5,5.5,6), "third list splice");
    splice $l1, 0, 4, (10, 11, 12);
    test_value($l1, (10, 11, 12, 6), "third list splice");
    splice $l1, 0, 1;
    test_value($l1, (11, 12, 6), "fourth list splice");
    splice $l1, 5, 2, (1, 2, 3);
    test_value($l1, (11, 12, 6, 1, 2, 3), "fifth list splice");
    splice $l1, -4, 2, 9;
    test_value($l1, (11, 12, 9, 2, 3), "sixth list splice");
    splice $l1, -4, -2, (21, 22, 23);
    test_value($l1, (11, 21, 22, 23, 2, 3), "seventh list splice");

    # extract tests
    test_value((extract $l1, 5), list(3), "first list extract");
    test_value((extract $l1, 2, 2), (22, 23), "second list extract");
    test_value((extract $l1, 1, 2, 4), (21, 2), "second list extract");
    test_value($l1, (11, 4), "final list extract");

    my string $astr = "hello";
    test_value($astr[2], "l", "string element dereference");
    my binary $bin = binary($astr);
    test_value($bin[4], ord("o"), "binary byte dereference");
    
    # range tests
    test_value(range(1), (0, 1,), "range - basic test");
    test_value(range(2, 5), (2, 3, 4, 5), "range - boundaries test");
    test_value(range(2, -2), (2, 1, 0, -1, -2), "range - descending test");
    test_value(range(1, 10, 5), (1, 6), "range - step test");
    test_value(range(0, 10, 5), (0, 5, 10), "range - step from 0");
    test_value(range(-10, 10, 5), (-10, -5, 0, 5, 10), "range - asc test");
    test_value(range(10, -10, 5), (10, 5, 0, -5, -10), "range - descending step test");
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
    test_value($pseudoList.typeCode(), NT_LIST, "<list>::typeCode");
    test_value($pseudoList.size(), 5, "<list>::size");
    test_value($pseudoList.empty(), False, "<list>::empty");
    test_value($pseudoList.val(), True, "<list>::val");
    test_value($pseudoList.first(), 1, "<list>::first");
    test_value($pseudoList.last(), 'a', "<list>::last");
    test_value($pseudoList.join('-'), '1-2-3-4-a', "<list>::join");
    test_value($pseudoList.lsize(), 5, "<list>::lsize");
    test_value($pseudoList.contains(2), True, "<list>::contains");
    #$pseudoList.append(6);
    #test_value($pseudoList, (1, 2, 3, 4, 'a', 6), "<list>::append");
}

sub hash_tests() {
    if ($o.verbose)
	print("%%%% hash tests\n");
    # hash tests
    my hash $b = ( "test" : 1, "gee" : 2, "well" : "string" );
    test_value($b.gee, 2, "object dereference");
    test_value(elements $b, 3, "elements operator on hash before delete");
    delete $b{"gee"};
    test_value(elements $b, 2, "elements operator on hash after delete");
    $b{"test"} = "there";
    my hash $d{"gee"}[25] = "I hope it works";
    test_value($b.test, "there", "hash dereference after assignment");
    test_value($b.test, "there", "object dereference after assignment");
    test_value($b{"geez"}, NOTHING, "non-existent object dereference");
    test_value(int($d.gee[25]), 0, "hash dereference of list element");
    test_value($d{"gee"}[25], "I hope it works", "dereference of list member of hash");
    my hash $c = ( "hi" : "there", "gee" : "whillakers" );
    $d = $c;
    test_value($d == $c, True, "hash comparison");
    test_value($d.gee, "whillakers", "hash dereference after entire hash assignment");
    $c{"gee"} = "roo";
    test_value($c{"gee"}, "roo", "original hash dereference after assignment to member of copied hash");
    test_value($d.gee, "whillakers", "hash dereference of member of copied hash");
    $d = ( "gee" : test1(), "howdy" : test2());
    test_value($d.gee, 1, "hash dereference with evaluation");
    test_value(hash_return(){"gee"}, "whiz", "simple hash return and dereference");
    test_value(hash_return(){"num"}, 1, "hash return with function element result and dereference");
    test_value(hash_return("hi there"){"var"}, "hi there", "hash return with local variable result and dereference");
    my hash $a = ( "key" : 1, "unique" : 100, "asd" : "dasd" );
    $b = ( "key" : 3, "new" : 45, "barn" : "door" );
    $c = $a + $b;
    test_value($c.key, 3, "hash plus operator element override");
    test_value($c."new", 45, "hash plus operator new element");
    test_value($c.unique, 100, "hash plus operator unchanged element");
    $a += $b;
    test_value($a.key, 3, "hash plus equals operator element override");
    test_value($a."new", 45, "hash plus equals operator new element");
    test_value($a.unique, 100, "hash plus equals operator unchanged element");

    # test hash slice creation
    test_value($a.("unique", "new"), ("unique" : 100, "new" : 45), "hash slice creation");

    my Test $ot(1, "two", 3.0);
    $ot += $a;
    test_value($ot.("unique", "new"), ("unique" : 100, "new" : 45), "hash slice creation from object");

    # use the foreach ... in (keys <hash>) specialization
    my int $cnt = 0;
    foreach my string $k in (keys $c) {
        # to avoid unused local var warning
        delete $k;
        ++$cnt;
    }
    test_value($cnt, 5, "foreach hash keys specialization");
    # do pseudo-method tests
    test_value($c.firstKey(), "key", "<hash>.firstKey()");
    test_value($c.lastKey(), "barn", "<hash>.lastKey()");
    test_value($c.size(), 5, "<hash>.size()");

    my hash $nch = $c.("key", "barn");
    foreach my hash $hi in ($nch.pairIterator()) {
        if (!$#)
            test_value($hi.key, "key", "HashIterator::first()");
        else if ($# == 4)
            test_value($hi.key, "barn", "HashIterator::last()");
    }

    my HashPairReverseIterator $hi($nch);
    foreach my hash $hiv in ($hi) {
        if ($# == 4)
            test_value($hiv.key, "key", "HashReverseIterator::last()");
        else if (!$#)
            test_value($hiv.key, "barn", "HashReverseIterator::first()");
    }
    test_value($hi.valid(), False, "HashReverseIterator::valid()");
    # restart iterator
    test_value($hi.next(), True, "HashReverseIterator::next()");
    test_value($hi.getKey(), "barn", "HashReverseIterator::getKey()");
    $hi.reset();
    test_value($hi.valid(), False, "HashReverseIterator::valid() after reset");

    # delete 3 keys from the $c hash
    $b = $c - "new" - "barn" - "asd";
    test_value($b, ( "key" : 3, "unique" : 100 ), "hash minus operator"); 
    $b = $c - ("new", "barn", "asd");
    test_value($b, ( "key" : 3, "unique" : 100 ), "hash minus operator with list argument"); 
    $b -= "unique";
    test_value($b, ( "key" : 3 ), "hash minus-equals operator"); 
    $c -= ( "new", "barn" );
    test_value($c, ( "key": 3, "unique" : 100, "asd" : "dasd" ), "hash minus-equals operator with list argument");
    my hash $nh += ( "new-hash" : 1 );
    test_value($nh, ( "new-hash" : 1 ), "hash plus-equals, lhs NOTHING");
}

sub global_variable_testa() {
    printf("user=%s\n", $ENV{"USER"});
}

code sub map_closure(any $v) { return any sub(any $v1) { return $v * $v1; }; }

# operator tests
sub operator_test() {
    if ($o.verbose)
	print("%%%% operator tests\n");
    my int $a = 1;
    test_value($a, 1, "variable assignment");
    $a += 3;
    test_value($a, 4, "integer += operator");
    $a -= 2;
    test_value($a, 2, "integer -= operator");
    $a |= 1;
    test_value($a, 3, "|= operator");
    $a &= 1;
    test_value($a, 1, "&= operator");
    $a *= 10;
    test_value($a, 10, "integer *= operator");
    my float $f = $a;
    $f *= 2.2;
    test_value($f, 22.0, "first float *= operator");
    $f *= 2;
    test_value($f, 44.0, "second float *= operator");
    $f /= 4.4;
    test_value($f, 10.0, "float /= operator");
    $a = 10;
    $a /= 2;
    test_value($a, 5, "integer /= operator");
    test_value(4 / 2, 2, "first / operator");
    $a = 0xfdb4902a;
    $a ^= 0xbf40e848;
    test_value($a, 0x42f47862, "^= xor equals operator");
    $a <<= 2;
    test_value($a, 0x10bd1e188, "<<= shift-left-equals operator");
    $a >>= 3;
    test_value($a, 0x217a3c31, ">>= shift-right-equals operator");
    $a = 1;
    test_value($a++, 1, "pre post-increment (++) operator");
    test_value($a, 2, "post post-increment (++) operator");
    test_value($a--, 2, "pre post-decrement (--) operator");
    test_value($a, 1, "post post-decrement (--) operator");
    test_value(++$a, 2, "pre-increment (++) operator");
    test_value(--$a, 1, "pre-decrement (--) operator");

    my string $astr = "hello" + " there";
    test_value($astr, "hello there", "string concatenation");
    $astr += " gee";
    test_value($astr, "hello there gee", "string plus equals");

    $f = 1.0;
    $f += 1.2;
    test_value($f, 2.2, "float += operator");
    $f -= 1.1;
    test_value($f, 1.1, "float -= operator");
    $f = 5.5 * 2.0;
    test_value($f, 11.0, "float * operator");

    test_value(now() > (now() - 1D), True, "date > operator");
    test_value(now() >= (now() - 1h), True, "date >= operator");
    test_value((now() - 1m) < now(), True, "date < operator");
    test_value((now() - 1M) <= now(), True, "date <= operator");

    my date $bt = my date $at = now();
    test_value($at, $bt, "date == operator");
    $at = 2004-02-28-12:00:00;
    $at += 1D;
    test_value($at, 2004-02-29-12:00:00, "first date += operator");
    $at -= (3h + 5m);
    test_value($at, 2004-02-29-08:55:00, "second date += operator");

    my any $ni += 3.2;
    test_value($ni, 3.2, "float +=, lhs NOTHING");
    delete $ni;
    $ni += "hello";
    test_value($ni, "hello", "string +=, lhs NOTHING");
    delete $ni;
    $ni -= 4.5;
    test_value($ni, -4.5, "float -=, lhs NOTHING");
    delete $ni;
    $ni -= 4;
    test_value($ni, -4, "integer -=, lhs NOTHING");
    # some array and hash tests in separate functions

    # get function closure with bound local variable (multiply by 2)
    my code $c = map_closure(2);

    # map function to list
    test_value((map $c($1), (1, 2, 3)), (2, 4, 6), "map operator using closure");

    # map immediate expression to list
    test_value((map $1 * 2, (1, 2, 3)), (2, 4, 6), "map operator using expression");

    # map function to list with optional select code as expression
    test_value((map $c($1), (1, 2, 3), $1 > 1), (4, 6), "map operator using closure with optional select expression");

    # select all elements from list greater than 1 with expression
    test_value((select (1, 2, 3), $1 > 1), (2, 3), "select operator with expression");

    # create a sinple closure to subtract the second argument from the first
    $c = any sub(any $x, any $y) { return $x - $y; };

    # left fold function on list using closure
    test_value((foldl $c($1, $2), (2, 3, 4)), -5, "foldl operator with closure");

    # left fold function on list using expression
    test_value((foldl $1 - $2, (2, 3, 4)), -5, "foldl operator with expression");

    # right fold function on list using immediate closure
    test_value((foldr $c($1, $2), (2, 3, 4)), -1, "foldr operator with closure");

    # right fold function on list using expression and implicit arguments
    test_value((foldr $1 - $2, (2, 3, 4)), -1, "foldr operator with expression");

    my hash $h = ("test" : 1, "two" : 2.0, "three" : "three", "four" : False );
    test_value(remove $h.two, 2.0, "first remove operator");
}

sub no_parameter_test(any $p) {
    test_value($p, NOTHING, "non-existent parameter");
}

sub parameter_and_shift_test(int $p) {
    test_value($p, 1, "parameter before shift");
    test_value(shift $argv, 2, "shift on second parameter");
}

sub one_parameter_shift_test() {
    test_value(shift $argv, 1, "one parameter shift");
}

sub shift_test() {
    my list $var = (1, 2, 3, 4, "hello");
    foreach my any $v in ($var)
	test_value($v, shift $argv, ("shift " + string($v) + " parameter"));
}

sub parameter_tests() {
    no_parameter_test();
    parameter_and_shift_test(1, 2);
    shift_test(1, test3()[1], 3, 4, "hello");
    one_parameter_shift_test(1);
}

bool sub short_circuit_test(string $op) {
    print("ERROR: %n logic short-circuiting is not working!\n", $op);
    $errors++;
    return False;
}

sub logic_message(string $op) {
    if ($o.verbose)
	printf("OK: %s logic test\n", $op);
}

# logic short-circuiting test
sub logic_tests() {
    my any $a = 1;
    my any $b = 0;
    my int $c;

    if ($o.verbose)
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
    test_value($b ? 0 : 1, 1, "first question-colon");
    test_value($a ? 1 : 0, 1, "second question-colon");
    $a = 1;
    $b = "1";
    test_value($a == $b, True, "comparison with type conversion");
    test_value($a === $b, False, "absolute comparison");
    $a = 1, 2, 3, 4;
    $b = 1, 2, 3, 4;
    test_value($a == $b, True, "list comparison");
    delete $b[3];
    test_value($a == $b, False, "list comparison after delete");
    $a[3] = ("gee" : 1, "whillakers" : 2, "list" : ( 1, 2, "three" ));
    $b[3] = $a[3];
    test_value($a == $b, True, "complex list comparison");
    test_value($a[3] == $b[3], True, "hash comparison");
    test_value($a[3] != $b[3], False, "negative hash unequal comparison");
    $a[3].chello = "hi";
    test_value($a[3] == $b[3], False, "negative hash comparison");
    test_value($a[3] != $b[3], True, "hash unequal comparison");
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
    if ($o.verbose)
	print("%%%% statement tests\n");
    # while test
    my int $a = 0;
    while ($a < 3)
	$a++;
    test_value($a, 3, "while");
    # do while test
    $a = 0;
    do {
	$a++;
    } while ($a < 3);
    test_value($a, 3, "do while");
    # for test
    my int $b = 0;
    for (my int $i = 0; $i < 3; $i++)
	$b++;
    test_value($a, 3, "for");
    test_value($b, 3, "for exec");    
    # foreach tests
    $b = 0;
    my int $v;
    foreach $v in (1, 2, 3)
	$b++;
    test_value($v, 3, "foreach");
    test_value($b, 3, "foreach exec");

    my any $list = my list $x;
    test_value($x, NOTHING, "unassigned typed variable");
    foreach my string $y in (\$list) $y = "test";
    test_value($list, NOTHING, "first foreach reference");
    
    $list = (1, 2, 3);
    foreach my any $y in (\$list) $y = "test";
    test_value($list, ("test", "test", "test"), "second foreach reference");
    
    $list = 1;
    foreach my any $y in (\$list) $y = "test";
    test_value($list, "test", "third foreach reference");

    # switch tests
    test_value(switch_test(1), "case 1", "first switch");
    test_value(switch_test(2), "default", "second switch");
    test_value(switch_test(3), "default", "third switch");
    test_value(switch_test(0), "case 1", "fourth switch");
    test_value(switch_test("hello"), "case 1", "fifth switch");
    test_value(switch_test("testing"), "default", "sixth switch");
    # switch with operators
    test_value(switch_with_relation_test(-2), "first switch", "first operator switch");
    test_value(switch_with_relation_test(2), "second switch", "second operator switch");
    test_value(switch_with_relation_test(-1.0), "third switch", "third operator switch");
    test_value(switch_with_relation_test(1.0), "fourth switch", "fourth operator switch");
    test_value(switch_with_relation_test(0), "fifth switch", "fifth operator switch");
    # regex switch
    test_value(regex_switch_test("abc"), "case 1", "first regex switch");
    test_value(regex_switch_test(), "case 3", "second regex switch");
    test_value(regex_switch_test("BOOM"), "case 3", "third regex switch");
    test_value(regex_switch_test("dinosaur"), "case 2", "fourth regex switch");
    test_value(regex_switch_test("barney"), "case 1", "fifth regex switch");
    test_value(regex_switch_test("canada"), "default", "sixth regex switch");

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
    test_value($a, 2, "first on_exit");
    test_value($b, 5, "second on_exit");
    test_value($v, 99, "third on_exit");
    test_value($err, True, "on_error");
    test_value($success, False, "on_success");
}

int sub fibonacci(int $num) {
    if ($num == 2)
        return 2;
    return $num * fibonacci($num - 1);
}

# recursive function test
sub recursive_function_test() {
    test_value(fibonacci(10), 3628800, "recursive function");
}

sub backquote_tests() {
    test_value(`echo -n 1`, "1", "backquote");
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

    test_value($d1,                              date(int($d)),    "date conversion " + $i);
    test_value(getISOWeekString($d),             $str,             "getISOWeekString() " + $i);
    test_value(getISOWeekHash($d),               $h,               "getISOWeekHash() " + $i);
    test_value(getISODayOfWeek($d),              $day,             "getDayOfWeek() " + $i);
    test_value(getDayNumber($d),                 $n,               "getDayNumber() " + $i);
    test_value(getDateFromISOWeek($y, $w, $day), get_midnight($d), "getDateFromISOWeek() " + $i);
    # not all architectures support the timegm() system call
    #if ($d >= 1970-01-01 && $d < 2038-01-19)
	#test_value(timegm($d), int($d), "qore epoch conversion " + $i);
    $i++;
}

sub date_time_tests() {
    # here are the two formats for directly specifying date/time values:
    # ISO-8601 format (without timezone specification, currently qore does not support time zones)
    my date $date  = 2004-02-01T12:30:00;
    # qore-specific date/time specification format ('-' instead of 'T' - more readable but non-standard)
    my date $ndate = 2004-03-02-12:30:00;
    test_value(format_date("YYYY-MM-DD HH:mm:SS", $date), "2004-02-01 12:30:00", "format_date()");
    test_value($date - 5Y,                1999-02-01T12:30:00, "first date year subtraction");
    test_value($date - 5M,                2003-09-01T12:30:00, "first date month subtraction");
    test_value($date - 10D,               2004-01-22T12:30:00, "first date day subtraction");
    test_value($date - 2h,                2004-02-01T10:30:00, "first date hour subtraction");
    test_value($date - 25m,               2004-02-01T12:05:00, "first date minute subtraction");
    test_value($date - 11s,               2004-02-01T12:29:49, "first date second subtraction");
    test_value($date - 251ms,             2004-02-01T12:29:59.749, "first date millisecond subtraction");

    test_value($date + 2Y,                2006-02-01T12:30:00, "first date year addition");
    test_value($date + 5M,                2004-07-01T12:30:00, "first date month addition");
    test_value($date + 10D,               2004-02-11T12:30:00, "first date day addition");
    test_value($date + 2h,                2004-02-01T14:30:00, "first date hour addition");
    test_value($date + 25m,               2004-02-01T12:55:00, "first date minute addition");
    test_value($date + 11s,               2004-02-01T12:30:11, "first date second addition");
    test_value($date + 251ms,             2004-02-01T12:30:00.251, "first date millisecond addition");

    test_value($date - years(5),          1999-02-01-12:30:00, "second date year subtraction");
    test_value($date - months(5),         2003-09-01-12:30:00, "second date month subtraction");
    test_value($date - days(10),          2004-01-22-12:30:00, "second date day subtraction");
    test_value($date - hours(2),          2004-02-01-10:30:00, "second date hour subtraction");
    test_value($date - minutes(25),       2004-02-01-12:05:00, "second date minute subtraction");
    test_value($date - seconds(11),       2004-02-01-12:29:49, "second date second subtraction");
    test_value($date - milliseconds(500), 2004-02-01-12:29:59.5, "second date millisecond subtraction");

    test_value($date + years(2),          2006-02-01-12:30:00, "second date year addition");
    test_value($date + months(5),         2004-07-01-12:30:00, "second date month addition");
    test_value($date + days(10),          2004-02-11-12:30:00, "second date day addition");
    test_value($date + hours(2),          2004-02-01-14:30:00, "second date hour addition");
    test_value($date + minutes(25),       2004-02-01-12:55:00, "second date minute addition");
    test_value($date + seconds(11),       2004-02-01-12:30:11, "second date second addition");
    test_value($date + milliseconds(578), 2004-02-01-12:30:00.578, "second date millisecond addition");

    # testing ISO-8601 alternate period syntax (which is not very readable... :-( )
    # date periods
    test_value($date - P0001-00-00T00:00:00, 2003-02-01T12:30:00, "third date year subtraction");
    test_value($date - P1M,          2004-01-01T12:30:00, "third date month subtraction");
    test_value($date - P0000-00-01,          2004-01-31T12:30:00, "third date day subtraction");
    test_value($date + P1Y,          2005-02-01T12:30:00, "third date year addition");
    test_value($date + P0000-01-00,          2004-03-01T12:30:00, "third date month addition");
    test_value($date + P0000-00-01,          2004-02-02T12:30:00, "third date day addition");

    # time periods
    test_value($date - P0000-00-00T01:00:00, 2004-02-01T11:30:00, "third date hour subtraction");
    test_value($date - P00:01:00,            2004-02-01T12:29:00, "third date minute subtraction");
    test_value($date - PT00:00:01,           2004-02-01T12:29:59, "third date second subtraction");
    test_value($date + P01:00:00,            2004-02-01T13:30:00, "third date hour addition");
    test_value($date + PT00:01:00,           2004-02-01T12:31:00, "third date minute addition");
    test_value($date + P00:00:01,            2004-02-01T12:30:01, "third date second addition");

    # arithmetic on dates with ms overflow
    test_value(2006-01-02T00:00:00.112, 2006-01-01T23:59:59.800 + 312ms, "third millisecond addition");
    test_value(2006-01-01T23:59:59.800, 2006-01-02T00:00:00.112 - 312ms, "third millisecond subtraction");

    test_value($date,        localtime(mktime($date)), "localtime() and mktime()");
    test_value($date - PT1H, 2004-02-01T11:30:00, "fourth date hour subtraction");
    test_value($date + 30D,  $ndate,                   "fourth date day addition");
    test_value($ndate - 30D, $date,                    "fourth date day subtraction");
    test_value($date + 23M,  2006-01-01T12:30:00,      "fourth date month addition");
    test_value($date - 4M,   2003-10-01T12:30:00,      "fourth date month subtraction");
    test_value($date,        date("20040201123000"),   "date function");

    test_value(2001-01-01,   date("2001-01", "YYYY-MM-DD"), "first date mask function");
    test_value(2001-01-01,   date("2001 Jan xx", "YYYY Mon DD"), "second date mask function");
    test_value(2001-01-01T13:01,   date("2001 JAN 01 13:01", "YYYY MON DD HH:mm"), "second date mask function");

    # times without a date are assumed to be on Jan 1, 1970
    test_value(11:25:27, 1970-01-01T11:25:27.000, "direct hour");

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

    test_value(date("2012-03-02", "YYYY-MM-DD"), 2012-03-02, "date() format parsing test");

    # absolute date difference tests
    test_value(2006-01-02T11:34:28.344 - 2006-01-01,              35h + 34m + 28s +344ms,       "date difference 1");
%ifndef Windows
    # this test fails on Windows due to different DST application for dates outside the UNIX epoch
    test_value(2099-04-21T19:20:02.106 - 1804-03-04T20:45:19.956, 2587078h + 34m + 42s + 150ms, "date difference 2");
%endif

    my SingleValueIterator $svi(2012-01-01);
    test_value($svi.next(), True, "1st SingleValueIterator::next()");
    test_value($svi.next(), False, "2nd SingleValueIterator::next()");
    test_value($svi.next(), True, "3rd SingleValueIterator::next()");
    test_value($svi.getValue(), 2012-01-01, "SingleValueIterator::getValue()");
    test_value($svi.valid(), True, "SingleValueIterator::valid()");
    my SingleValueIterator $ni = $svi.copy();
    test_value($ni.getValue(), 2012-01-01, "SingleValueIterator::getValue() (copy)");
    test_value($ni.next(), False, "SingleValueIterator::next() (copy)");
    test_value($ni.valid(), False, "SingleValueIterator::valid() (copy)");
}

sub binary_tests() {
    my binary $b = binary("this is a test");
    test_value(get_byte($b, 3), ord("s"), "get_byte()");
    test_value($b, binary("this is a test"), "binary comparison");
    test_value($b != binary("this is a test"), False, "binary negative comparison");
}

sub string_tests() {
    my string $str = "Hi there, you there, pal";
    my string $big = "GEE WHIZ";
    test_value(strlen($str), 24, "strlen()");
    test_value($str.strlen(), 24, "<string>::strlen()");
    test_value($str.size(), 24, "<string::size()");
    test_value(toupper($str), "HI THERE, YOU THERE, PAL", "toupper()");
    test_value($str.upr(), "HI THERE, YOU THERE, PAL", "<string>::upr()");
    test_value($big.lwr(), "gee whiz", "<string>::lwr()");
    test_value(reverse($big), "ZIHW EEG", "reverse()");
    # strmul
    test_value(strmul($big, 2), "GEE WHIZGEE WHIZ", "strmul() basic");
    test_value(strmul("%v, ", 3, 2), "%v, %v, %v", "strmul() extended");
    test_value(strmul(123, 2), "123123", "strmul() type conversion");
    
    # set up a string with UTF-8 multi-byte characters
    $str = "Über die Wolken läßt sich die Höhe begrüßen";
    test_value(strlen($str), 49, "UTF-8 strlen()");
    test_value($str.strlen(), 49, "UTF-8 <string>::strlen()");
    test_value(length($str), 43, "UTF-8 length()");
    test_value($str.length(), 43, "UTF-8 <string>::length()");
    test_value(substr($str, 30), "Höhe begrüßen", "first UTF-8 substr()");
    test_value(substr($str, -8), "begrüßen", "second UTF-8 substr()");
    test_value(substr($str, 0, -39), "Über", "third UTF-8 substr()");
    test_value(index($str, "läßt"), 16, "first UTF-8 index()");
    test_value(index($str, "läßt", 1), 16, "second UTF-8 index()");
    test_value(rindex($str, "ß"), 40, "first UTF-8 rindex()");
    test_value(rindex($str, "ß", 25), 18, "second UTF-8 rindex()"); 
    test_value(bindex($str, "läßt"), 17, "first UTF-8 bindex()");
    test_value(bindex($str, "läßt", 1), 17, "second UTF-8 bindex()");
    test_value(brindex($str, "ß"), 45, "first UTF-8 brindex()");
    test_value(brindex($str, "ß", 25), 20, "second UTF-8 brindex()"); 
    test_value(reverse($str), "neßürgeb ehöH eid hcis tßäl nekloW eid rebÜ", "UTF-8 reverse()");
    test_value(index($str, "==="), -1, "negative index()");
    test_value(rindex($str, "==="), -1, "negative rindex()");
    test_value(bindex($str, "==="), -1, "negative bindex()");

    test_value($str[31], "ö", "first UTF-8 string index dereference");
    test_value($str[39], "ü", "second UTF-8 string index dereference");

    # save string
    my string $ostr = $str;
    # convert the string to single-byte ISO-8859-1 characters and retest
    $str = convert_encoding($str, "ISO-8859-1");
    test_value($str != $ostr, False, "string != operator with same text with different encodings");
    test_value(strlen($str), 43, "ISO-8859-1 strlen()");
    test_value($str.strlen(), 43, "ISO-8859-1 <string>::strlen()");
    test_value(length($str), 43, "ISO-8859-1 length()");
    test_value($str.length(), 43, "ISO-8859-1 <string>::length()");
    test_value(substr($str, 30), convert_encoding("Höhe begrüßen", "ISO-8859-1"), "first ISO-8859-1 substr()");
    test_value(substr($str, -8), convert_encoding("begrüßen", "ISO-8859-1"), "second ISO-8859-1 substr()");
    test_value(substr($str, 0, -39), convert_encoding("Über", "ISO-8859-1"), "third ISO-8859-1 substr()");
    test_value(index($str, convert_encoding("läßt", "ISO-8859-1")), 16, "first ISO-8859-1 index()");
    test_value(index($str, convert_encoding("läßt", "ISO-8859-1"), 1), 16, "second ISO-8859-1 index()");
    test_value(rindex($str, convert_encoding("ß", "ISO-8859-1")), 40, "first ISO-8859-1 rindex()");
    test_value(rindex($str, convert_encoding("ß", "ISO-8859-1"), 25), 18, "second ISO-8859-1 rindex()"); 
    test_value(ord($str, 1), 98, "ord()");

    test_value(chr(104), "h", "chr()");

    $str = "gee this is a long string";
    my list $a = split(" ", $str);
    test_value($a[2], "is", "first string split()");
    test_value($a[5], "string", "second string split()");

    $a = split(binary(" "), binary($str));
    test_value($a[2], binary("is"), "first binary split()");
    test_value($a[5], binary("string"), "second binary split()");

    $str = "äüößÄÖÜ";
    # test length() with UTF-8 multi-byte characters
    test_value(length($str), 7, "length() with UTF-8 multi-byte characters");
    test_value(strlen($str), 14, "strlen() with UTF-8 multi-byte characters");
    # test charset encoding conversions
    my string $nstr = convert_encoding($str, "ISO-8859-1");
    test_value(length($nstr), 7, "length() with ISO-8859-1 special characters");
    test_value(strlen($nstr), 7, "strlen() with ISO-8859-1 special characters");
    test_value($str, convert_encoding($nstr, "UTF-8"), "convert_encoding()");
    # assign binary object
    my binary $x = <0abf83e8ca72d32c>;
    my string $b64 = makeBase64String($x);
    test_value($x, parseBase64String($b64), "first base64");
    test_value("aGVsbG8=", makeBase64String("hello"), "makeBase64String()");
    my string $hex = makeHexString($x);
    test_value($x, parseHexString($hex), "first hex");

    # UTF-8 string splice tests
    $str = "äbcdéf";
    splice $str, 5;
    test_value($str, "äbcdé", "first UTF-8 string splice");
    splice $str, 3, 1;
    test_value($str, "äbcé", "second UTF-8 string splice");
    splice $str, 1, 2, "GHI";
    test_value($str, "äGHIé", "third UTF-8 string splice");
    splice $str, 0, 4, "jkl";
    test_value($str, "jklé", "fourth UTF-8 string splice");
    splice $str, 0, 1;
    test_value($str, "klé", "fifth UTF-8 string splice");
    splice $str, 5, 2, "MNO";
    test_value($str, "kléMNO", "sixth UTF-8 string splice");
    splice $str, -4, 2, "p";
    test_value($str, "klpNO", "seventh UTF-8 string splice");
    splice $str, -4, -2, "QRS";
    test_value($str, "kQRSNO", "eighth UTF-8 string splice");

    # UTF-8 string extract tests
    $str = "äbcdéf";
    test_value((extract $str, 4), "éf", "first UTF-8 string extract");
    test_value((extract $str, 1, 2), "bc", "second UTF-8 string extract");
    test_value((extract $str, 1, 1, "bcdef"), "d", "third UTF-8 string extract");
    test_value($str, "äbcdef", "final UTF-8 string extract");

    # single-byte string splice tests
    $str = convert_encoding("äbcdéf", "ISO-8859-1");
    splice $str, 5;
    test_value($str == "äbcdé", True, "first ISO-8859-1 string splice");
    splice $str, 3, 1;
    test_value($str == "äbcé", True, "second ISO-8859-1 string splice");
    splice $str, 1, 2, "GHI";
    test_value($str == "äGHIé", True, "third ISO-8859-1 string splice");
    splice $str, 0, 4, "jkl";
    test_value($str == "jklé", True, "fouth ISO-8859-1 string splice");
    splice $str, 0, 1;
    test_value($str == "klé", True, "fifth ISO-8859-1 string splice");
    splice $str, 5, 2, "MNO";
    test_value($str == "kléMNO", True, "sixth ISO-8859-1 string splice");
    splice $str, -4, 2, "p";
    test_value($str == "klpNO", True, "seventh ISO-8859-1 string splice");
    splice $str, -4, -2, "QRS";
    test_value($str == "kQRSNO", True, "eighth ISO-8859-1 string splice");

    # UTF-8 string extract tests
    $str = convert_encoding("äbcdéf", "ISO-8859-1");
    my string $val = extract $str, 4;
    test_value($val == "éf", True, "first UTF-8 string extract");
    $val = extract $str, 1, 2;
    test_value($val == "bc", True, "second UTF-8 string extract");
    $val = extract $str, 1, 1, "bcdef";
    test_value($val == "d", True, "third UTF-8 string extract");
    test_value($str == "äbcdef", True, "final UTF-8 string extract");

    # join tests
    $str = join(":", "login","passwd",1,"gid","gcos","home","shell");
    test_value($str, "login:passwd:1:gid:gcos:home:shell", "first join");
    my list $l = ("login","passwd","uid","gid","gcos","home","shell");
    $str = join(":", $l);
    test_value($str, "login:passwd:uid:gid:gcos:home:shell", "second join");

    # transliteration tests
    $str = "Hello There";
    test_value($str =~ tr/def/123/, "H2llo Th2r2", "first transliteration");
    test_value($str =~ tr/a-z/0-9/, "H2999 T7292", "first range transliteration");
    $str = "Hello There";
    test_value($str =~ tr/a-z/A-Z/, "HELLO THERE", "second range transliteration");
    test_value($str =~ tr/A-Z/a-z/, "hello there", "third range transliteration");

    # regex subpattern extraction operator tests
    test_value("xmlns:wsdl" =~ x/(\w+):(\w+)/, ("xmlns", "wsdl"), "regex subpattern extraction");
    test_value("xmlns-wsdl" =~ x/(\w+):(\w+)/, NOTHING, "negative regex subpattern extraction");
    test_value(regex_extract("xmlns:wsdl", "(\\w+):(\\w+)"), ("xmlns", "wsdl"), "regex_extract()");

    # regex operator tests
    test_value("hello" =~ /^hel*/, True, "regular expression positive match");
    test_value("hello" =~ m/123*/, False, "regular expression negative match");
    test_value("hello" =~ m/^HEL*/i, True, "regular expression case-insensitive positive match");
    test_value("hello" =~ /^HEL*/, False, "regular expression case-insensitive negative match");
    test_value("test\nx" =~ /test.x/s, True, "regular expression newline positive match");
    test_value("test\nx" =~ /test.x/, False, "regular expression newline negative match");
    test_value("hello" =~ /^  hel* #comment/x, True, "extended regular expression positive match");
    test_value("hello" =~ /^  hel* #comment/, False, "extended regular expression negative match");
    test_value("test\nx123" =~ /^x123/m, True, "multiline regular expression positive match");
    test_value("test\nx123" =~ /^x123/, False, "multiline regular expression negative match");
    # NOTE that escaping UTF-8 characters (\ä) doesn't work on PPC for some reason
    test_value("testäöüß" =~ /äöüß/, True, "regular expression UTF-8 match");
    test_value("testäöüß" =~ /aouB/, False, "regular expression UTF-8 negative match");
    test_value("hello" !~ /hel*/, False, "negative regular expression negative match");
    test_value("hello" !~ /123*/, True, "negative regular expression positive match");

    # regex substitution operator tests
    $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
    test_value($l[0] =~ s/bar/foo/, "hello foo hi bar", "first non-global regular expression substitution");
    test_value($l[1] =~ s/bar/foo/, "foo hello bar hi bar", "second non-global regular expression substitution");
    test_value($l[2] =~ s/BAR/foo/i, "hello foo hi", "case-insensitive non-global regular expression substitution");
    $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
    test_value($l[0] =~ s/bar/foo/g, "hello foo hi foo", "first global regular expression substitution");
    test_value($l[1] =~ s/bar/foo/g, "foo hello foo hi foo", "second global regular expression substitution");
    test_value($l[2] =~ s/BAR/foo/ig, "hello foo hi", "case-insensitive global regular expression substitution");

    my string $astr= "abc def";
    $astr =~ s/(\w+) +(\w+)/$2, $1/; 
    test_value($astr, "def, abc", "regular expression subpattern substitution");

    # regex() tests
    test_value(regex("hello", "^hel*"), True, "regex() positive match");
    test_value(regex("hello", "123*"), False, "regex() negative match");
    test_value(regex("hello", "^HEL*", RE_Caseless), True, "regex() case-insensitive positive match");
    test_value(regex("hello", "^HEL*"), False, "regex() case-insensitive negative match");
    test_value(regex("test\nx", "test.x", RE_DotAll), True, "regex() newline positive match");
    test_value(regex("test\nx", "test.x/"), False, "regex() newline negative match");
    test_value(regex("hello", "^  hel* #comment", RE_Extended), True, "regex() extended positive match");
    test_value(regex("hello", "^  hel* #comment"), False, "regex() extended negative match");
    test_value(regex("test\nx123", "^x123", RE_MultiLine), True, "regex() multiline positive match");
    test_value(regex("test\nx123", "^x123/"), False, "regex() multiline negative match");
    test_value(regex("testäöüß", "\äöüß"), True, "regex() UTF-8 match");
    test_value(regex("testäöüß", "aouB"), False, "regex() UTF-8 negative match");

    # regex_subst() tests
    $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
    test_value(regex_subst($l[0], "bar", "foo"), "hello foo hi bar", "first non-global regex_subst()");
    test_value(regex_subst($l[1], "bar", "foo"), "foo hello bar hi bar", "second non-global regex_subst()");
    test_value(regex_subst($l[2], "BAR", "foo", RE_Caseless), "hello foo hi", "case-insensitive non-global regex_subst()");
    $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
    test_value(regex_subst($l[0], "bar", "foo", RE_Global), "hello foo hi foo", "first global regex_subst()");
    test_value(regex_subst($l[1], "bar", "foo", RE_Global), "foo hello foo hi foo", "second global regex_subst()");
    test_value(regex_subst($l[2], "BAR", "foo", RE_Global|RE_Caseless), "hello foo hi", "case-insensitive global regex_subst()");

    $astr = "abc def";
    # note that the escape characters have to be escaped in the following pattern string
    test_value(regex_subst($astr, "(\\w+) +(\\w+)", "$2, $1"), "def, abc", "first subpattern regex_subst()");
    # here we use single-quotes, so the escape characters do not have to be escaped
    test_value(regex_subst($astr, '(\w+) +(\w+)', "$2, $1"), "def, abc", "second subpattern regex_subst()");

    # chomp tests
    $str = "hello\n";
    chomp $str;
    test_value($str, "hello", "first string chomp");
    $str += "\r\n";
    chomp $str;
    test_value($str, "hello", "second string chomp");
    $l = ( 1, "hello\n", 3.0, True, "test\r\n" );
    chomp $l;
    test_value($l, ( 1, "hello", 3.0, True, "test" ), "list chomp");
    my hash $h = ( "key1" : "hello\n", "key2" : 2045, "key3": "test\r\n", "key4" : 302.223 );
    chomp $h;
    test_value($h, ( "key1" : "hello", "key2" : 2045, "key3": "test", "key4" : 302.223 ), "hash chomp");
    $str = "hello\n";
    chomp(\$str);
    test_value($str, "hello", "string reference chomp()");
    $str = "  \t\n  hello  \n   \r \t \0 ";
    trim $str;
    test_value($str, "hello", "trim string operator test");
    $str = "  \t\n  hello  \n   \r \t \0 ";
    trim(\$str);
    test_value($str, "hello", "trim string reference test");

    $l = ( 1, "   \r \t hello  \n  \r \v \t", 3.0, True, "    test\r\n  " );
    trim $l;
    test_value($l, ( 1, "hello", 3.0, True, "test" ), "list trim");

    $h = ( "key1" : "    hello\n \r  ", "key2" : 2045, "key3": "     test\r   \n \t\v   ", "key4" : 302.223 );
    trim $h;
    test_value($h, ( "key1" : "hello", "key2" : 2045, "key3": "test", "key4" : 302.223 ), "hash trim");    

    # make sure strings containing floating-point numbers between -1.0 and 1.0 exclusive return True when evaluated in a boolean context
    test_value(True, boolean("0.1"), "first string fp boolean");
    test_value(True, boolean("-0.1"), "second string fp boolean");

    $str = "příliš žluťoučký kůň úpěl ďábelské ódy";
    test_value($str.unaccent(), "prilis zlutoucky kun upel dabelske ody", "<string>::unaccent()");
    my string $ustr = $str.upr();
    test_value($ustr, "PŘÍLIŠ ŽLUŤOUČKÝ KŮŇ ÚPĚL ĎÁBELSKÉ ÓDY", "<string>::upr()");
    test_value($ustr.lwr(), "příliš žluťoučký kůň úpěl ďábelské ódy", "<string>::lwr()");

    # regression tests for floating-point formatting bugs
    test_value(sprintf("%f", 1.5), "1.500000", "%f float");    
    test_value(sprintf("%f", 1.5n), "1.500000", "%f number");
    test_value(sprintf("%g", 1.5), "1.5", "%f float");    
    test_value(sprintf("%g", 1.5n), "1.5", "%f number");
}

sub pwd_tests() {
    # getpwuid(0).pw_name may not always be "root"
    # skip the test on windows
    if (Option::HAVE_UNIX_USERMGT) {
        test_value(getpwuid(0).pw_uid, 0, "getpwuid()");
        my hash $h;
        # try to get passwd entry for uid 0, ignore exceptions
        try $h = getpwuid2(0); catch () {}
        test_value($h.pw_uid, 0, "getpwuid2()");
        test_value(getpwnam("root").pw_uid, 0, "getpwnam()");
        # try to get passwd entry for root, ignore exceptions
        try $h = getpwnam2("root"); catch () {}
        test_value($h.pw_uid, 0, "getpwnam2()");
        test_value(getgrgid(0).gr_gid, 0, "getgrgid()");
        # try to get group entry for gid 0, ignore exceptions
        try $h = getgrgid2(0); catch () {}
        test_value($h.gr_gid, 0, "getgrgid2()");
        test_value(getgrnam($h.gr_name).gr_gid, 0, "getgrnam()");
        # try to get group entry for root, ignore exceptions
        try $h = getgrnam2($h.gr_name); catch () {}
        test_value($h.gr_gid, 0, "getgrnam2()");
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
    test_value($dh, parseDatasource($ds), "first parseDatasource()"); 
    test_value((1, 2), simple_shift((1, 2)), "list arg function call");

    test_value(call_function("simple_shift", 1), 1, "call_function()");
    test_value(call_builtin_function("type", 1), Type::Int, "call_builtin_function()");
    test_value(existsFunction("simple_shift"), True, "existsFunction()");
    test_value(functionType("simple_shift"), "user", "functionType() user");
    test_value(functionType("printf"), "builtin", "functionType() builtin");
    test_value(type(1), "integer", "type()");
    my string $str1 = "&<>\"";
    my string $str2 = "&amp;&lt;&gt;&quot;";
    test_value(html_encode($str1), $str2, "html_encode()");
    test_value(html_decode($str2), $str1, "html_decode()");

    # note that '@' signs are legal in the password field as with datasources
    my string $url = "https://username:passw@rd@hostname:1044/path/is/here";
    my hash $uh = ( "protocol" : "https",
		    "username" : "username",
		    "password" : "passw@rd",
		    "host" : "hostname",
		    "port" : 1044,
		    "path" : "/path/is/here" );

    test_value(parseURL($url), $uh, "parseURL()");

    # test gzip
    my string $str = "This is a long string xxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    my binary $bstr = binary($str);
    my binary $c = compress($str);
    test_value($str, uncompress_to_string($c), "compress() and uncompress_to_string()");
    test_value($bstr, uncompress_to_binary($c), "compress() and uncompress_to_binary()");
    my binary $gz = gzip($str);
    test_value($str, gunzip_to_string($gz), "gzip() and gunzip_to_string()");
    test_value($bstr, gunzip_to_binary($gz), "gzip() and gunzip_to_binary()");
    
    # test bzip2
    my binary $bz = bzip2($str);
    test_value($str, bunzip2_to_string($bz), "bzip2 and bunzip2_to_string");
    test_value($bstr, bunzip2_to_binary($bz), "bzip2 and bunzip2_to_binary");
}

sub math_tests() {
    test_value(ceil(2.7), 3.0, "ceil()");
    test_value(floor(3.7), 3.0, "fllor()");
    test_value(format_number(".,3", -48392093894.2349), "-48.392.093.894,235", "format_number()");
}

sub lib_tests() {
    my string $pn = get_script_path();
    if (PlatformOS != "Windows") {
        test_value(stat($pn)[1], hstat($pn).inode, "stat() and hstat()");
        test_value(hstat($pn).type, "REGULAR", "hstat()");
    }
    #my string $h = gethostname();
    #test_value($h, gethostbyaddr(gethostbyname($h)), "host functions");
}

sub file_tests() {
    test_value(is_file($ENV."_"), True, "is_file()");
    test_value(is_executable($ENV."_"), True, "is_executable()");
    test_value(is_dir("/"), True, "is_dir()");
    test_value(is_writeable($ENV.HOME), True, "is_writable()");
    test_value(is_readable($ENV.HOME), False, "is_readable()");
    test_value(is_dev("/dev/null"), True, "is_dev()");
    test_value(is_cdev("/dev/null"), True, "is_cdev()");
    test_value(is_bdev("/dev/null"), False, "is_bdev()");
    test_value(is_link("/"), False, "is_link()");
    test_value(is_socket("/"), False, "is_socket()");
    test_value(is_pipe("/"), False, "is_pipe()");
}

sub io_tests() {
    test_value(sprintf("%04d-%.2f", 25, 101.239), "0025-101.24", "sprintf()");
    test_value(vsprintf("%04d-%.2f", (25, 101.239)), "0025-101.24", "vsprintf()");
    # check multi-byte character set support for f_*printf()
    test_value(f_sprintf("%3s", "niña"), "niñ", "UTF-8 f_sprintf()");
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
    test_value(f_test(1), "integer", "first overload partial match");
    test_value(f_test(1.1), "float", "second overload partial match");
    test_value(f1_test(1), "float", "third overload partial match");
    test_value(f1_test(1.1), "float", "fourth overload partial match");
    test_value(f1_test("str"), "string", "fifth overload partial match");
    my int $i = 1;
    test_value(f_test($i), "integer", "first runtime overload partial match");
    test_value(f1_test($i), "float", "second runtime overload partial match");
    my float $fi = 1.1;
    test_value(f_test($fi), "float", "third runtime overload partial match");
    test_value(f1_test($fi), "float", "fourth runtime overload partial match");
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

    test_value($a.callFunction("newfunc"), True, "Program::parsePending()");
    test_value($a.callFunction("t2", 1), 3, "Program::parse()");
    test_value($a.callFunctionArgs("t2", list(int(2))), 4, "program imported function");
    test_value($a.callFunction("et", 1), 2, "program imported function");
    test_value($a.callFunction("tot"), "Test", "program imported object variable");
    test_value($to.member, "memberGate-member", "program imported object member gate");
    test_value($to.method(), "method", "program imported object method gate");
    try
	$a.callFunction("deleteException");
    catch ($ex)
	test_value($ex.err, "ACCESS-ERROR", "Program::importGlobalVariable() readonly");

    test_value($a.callFunction("check_ro"), True, "delete read-only");
    
    my Queue $o = $a.callFunction("getObject");
    my object $ox = $a.callFunction("get_x");
    delete $a;
    test_value(getClassName($o), "Queue", "builtin class returned from deleted subprogram object");
    test_value(getClassName($ox), "X", "user class returned from deleted subprogram object");

    # test for incorrect parse location when processing constants after a commit
    $a = new Program();
    $a.parse("sub x() {}", "lib");
    my *hash $h = $a.parse("const X1 = 'a'; const X2 = 'a'; const h = (X1: 1, X2: 2);", "warn", WARN_ALL);
    test_value($h.file, "<run-time-loaded: warn>", "constant parse location");

    # make sure recursive constant definitions are handled
    try {
        $a.parse("const A = B; const B = A; any a = A;", "rec");
    }
    catch (hash $ex) {
        test_value($ex.err, "PARSE-EXCEPTION", "recursive constant ref");
    }

    my string $pstr = "class T { private { int i = 1; static *int j = 4; const X = 2; } int get() { return i; } static other (int x) {} } T sub getT() { return new T(); } int sub checkT(T t) { return t.get(); }";

    my Program $p1(PO_NEW_STYLE);
    my Program $p2(PO_NEW_STYLE);

    $p1.parse($pstr, "p");
    $p2.parse($pstr, "p");

    my object $o2 = $p1.callFunction("getT");
    test_value(1, $p1.callFunction("checkT", $o2), "first cross-Program class");
    test_value(1, $p2.callFunction("checkT", $o2), "second cross-Program class");

    my Program $p3();
    $p3.parse("class X { private $.a; }", "p");

    my Program $p4();
    try {
        $p4.parse("error", "error", 0, "source", 10);
        test_value(True, False, "exception source & offset");
    }
    catch (hash $ex) {
        test_value($ex.source, "source", "exception source");
        test_value($ex.offset, 10, "exception offset");
    }
}

sub class_test_File() {
    # File test
    my File $f("iso-8859-1");
    $f.open(get_script_path());
    test_value($f.getEncoding(), "ISO-8859-1", "file encoding");
/*
    my string $l = $f.readLine();
    my int $p = $f.getPos();
    $f.setPos(0);
    test_value($l, $f.readLine(), "File::readLine() and File::setPos()");
    test_value($p, $f.getPos(), "File::getPos()");
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
    test_value($cert.getSignature(), cert_sig, "SSLCertificate::getSignature()");
    test_value($cert.getInfo().subject.emailAddress, "david@qore.org", "SSLCertificate::getInfo()");
}

sub class_test_SSLPrivateKey() {
    my SSLPrivateKey $key(key_pem, key_pass);
    test_value($key.getVersion(), 1, "SSLPrivateKey::getVersion()");
    test_value($key.getBitLength(), 512, "SSLPrivateKey::getBitLength()");
    test_value($key.getType(), "RSA", "SSLPrivateKey::getType()");
    test_value($key.getInfo().type, "RSA", "SSLPrivateKey::getInfo()");
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
    test_value(True, False, $test);
}

sub check(string $err, string $test) {
    test_value($err, "PRIVATE-MEMBER", $test);
}

class Test2 { private { any $.a; } }

sub class_library_tests() {
    my Test $t(1, "gee", 2);
    test_value($t.getData(1), "gee", "first object");
    test_value(exists $t.testing, True, "memberGate() existence");
    test_value($t.testing, "memberGate-testing", "memberGate() value");
    test_value($t.test(), "test", "methodGate() value");
    test_value($t instanceof Test, True, "first instanceof");
    test_value($t instanceof Qore::Socket, True, "second instanceof");

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
    test_value($t.a, "memberGate-a", "object memberGate() methods");

    # test memberNotification()
    $t.x = 1;
    # test object closure
    my code $c = $t.closure(1);
    test_value($c(2), "gee-1-2-2", "first object closure");
    test_value($c(2), "gee-1-2-3", "second object closure");
    test_value($t.t.x, 1, "memberNotification() method");

    # test callObjectMethod*()
    test_value(callObjectMethod($t1, "argTest", 1, 2), (1, 2), "callObjectMethod()");
    test_value(callObjectMethodArgs($t1, "argTest"), NOTHING, "first callObjectMethodArgs()");
    test_value(callObjectMethodArgs($t1, "argTest", 1), list(1), "second callObjectMethodArgs()");
    test_value(callObjectMethodArgs($t1, "argTest", (1, 2)), (1, 2), "third callObjectMethodArgs()");

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

    test_value($h, $i, "context");

    my int $age = find %age in $q where (%name == "david");
    test_value($age, 37, "find");

    my list $ages = find %age in $q where (%name == "david" || %name == "isabella");
    test_value($ages, (37, 1), "list find"); 
    context ($q) {
	test_value(%%, ("name" : "david", "age" : 37), "context row");
        test_value(cx_first(), True, "cx_first()");
        test_value(cx_last(), False, "cx_last()");
        test_value(cx_pos(), 0, "cx_pos()");
        test_value(cx_total(), 5, "cx_total()");
        test_value(cx_value("name"), "david", "cx_value()");
	break;
    }

    my HashListIterator $qi($q);
    while ($qi.next()) {
	test_value($qi.getRow(), ("name" : "david", "age" : 37), "HashListIterator::getRow()");
        test_value($qi.first(), True, "HashListIterator::first()");
        test_value($qi.last(), False, "HashListIterator::last()");
        test_value($qi.index(), 0, "HashListIterator::index()");
        test_value($qi.max(), 5, "HashListIterator::max()");
        test_value($qi.name, "david", "HashListIterator::memberGate()");
	break;
    }

    my HashListReverseIterator $rqi($q);
    while ($rqi.next()) {
	test_value($rqi.getRow(), ("name" : "isabella", "age" : 1), "HashListReverseIterator::getRow()");
        test_value($rqi.first(), True, "HashListReverseIterator::first()");
        test_value($rqi.last(), False, "HashListReverseIterator::last()");
        test_value($rqi.index(), 4, "HashListReverseIterator::index()");
        test_value($rqi.max(), 5, "HashListReverseIterator::max()");
        test_value($rqi.name, "isabella", "HashListReverseIterator::memberGate()");
	break;
    }

    # convert the hash of lists to a list of hashes
    $qi.set(-1);
    my list $l = map $qi.getRow(), $qi;

    my ListHashIterator $lqi($l);
    while ($lqi.next()) {
	test_value($lqi.getRow(), ("name" : "david", "age" : 37), "ListHashIterator::getRow()");
        test_value($lqi.first(), True, "ListHashIterator::first()");
        test_value($lqi.last(), False, "ListHashIterator::last()");
        test_value($lqi.index(), 0, "ListHashIterator::index()");
        test_value($lqi.max(), 5, "ListHashIterator::max()");
        test_value($lqi.name, "david", "ListHashIterator::memberGate()");

        my ListHashIterator $ni = $lqi.copy();
	test_value($ni.getRow(), ("name" : "david", "age" : 37), "ListHashIterator::getRow() (copy)");
        test_value($ni.first(), True, "ListHashIterator::first() (copy)");
        test_value($ni.index(), 0, "ListHashIterator::index() (copy)");
	break;
    }

    my ListHashReverseIterator $lrqi($l);
    while ($lrqi.next()) {
	test_value($lrqi.getRow(), ("name" : "isabella", "age" : 1), "ListHashReverseIterator::getRow()");
        test_value($lrqi.first(), True, "ListHashReverseIterator::first()");
        test_value($lrqi.last(), False, "ListHashReverseIterator::last()");
        test_value($lrqi.index(), 4, "ListHashReverseIterator::index()");
        test_value($lrqi.max(), 5, "ListHashReverseIterator::max()");
        test_value($lrqi.name, "isabella", "ListHashReverseIterator::memberGate()");

        my ListHashReverseIterator $ni = $lrqi.copy();
	test_value($ni.getRow(), ("name" : "isabella", "age" : 1), "ListHashReverseIterator::getRow() (copy)");
        test_value($ni.first(), True, "ListHashReverseIterator::first() (copy)");
        test_value($ni.index(), 4, "ListHashReverseIterator::index() (copy)");
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
        test_value(t1, "hello", "1st namespace constant resolution");
        test_value(Type::i, 1, "2nd namespace constant resolution");
        test_value(Type::hithere, 1, "3rd namespace constant resolution");
        test_value(C::a, 1, "class constant resolution in namespace context");
        test_value(C::t1(), 1, "static method resolution in namespace context");
        test_value(C::t2(), 1, "constant resolution in namespace context in class code");
        test_value(d1(), 1, "first function resolution in namespace context");
        test_value(d2(), 1, "second function resolution in namespace context");
        test_value(H.a, 1, "hash key constant resolution in namespace context");
    }
}

namespace NTest {
    const t2 = 2;
}

const NTest::Type::val1 = 1;

const Qore::myconst = 1;

sub constant_tests() {
    test_value(i, 1, "simple constant");
    test_value(type(Type::val1), "integer", "first namespace constant");
    test_value(Qore::myconst, NTest::Type::val1, "second namespace constant");
    test_value(NTest::Type::i, 1, "third namespace constant"); 
    test_value(chash{b}, (1, 2, 3), "indirect constant");
    test_value(exp, 3, "evaluated constant");
    test_value(hexp2, (1, 2, 3), "evaluated constant hash");
    NTest::test();
}

sub hmac_tests() {
    my string $str = "Hello There This is a Test - 1234567890";
    my string $key = "a key";

    if (HAVE_MD2)
        test_value(MD2_hmac($str, $key),    "27f5f17500b408e97643403ea8ef1413", "MD2 hmac");
    test_value(MD4_hmac($str, $key),        "053d084f321a3886e60166ebd9609ce1", "MD4 hmac");
    test_value(MD5_hmac($str, $key),        "87505c6164aaf6ca6315233902a01ef4", "MD5 hmac");
    test_value(DSS_hmac($str, $key),        "37a3cc73159aa129b0eb22bbdf4b9309d389f629", "DSS hmac");
    test_value(DSS1_hmac($str, $key),       "37a3cc73159aa129b0eb22bbdf4b9309d389f629", "DSS1 hmac");
    test_value(SHA_hmac($str, $key),        "0ad47c8d36dc4606d52f7e4cbd144ef2fda492a0", "SHA hmac");
    test_value(SHA1_hmac($str, $key),       "37a3cc73159aa129b0eb22bbdf4b9309d389f629", "SHA1 hmac");
    test_value(RIPEMD160_hmac($str, $key),  "4bca70bca1601aba57624eeb2606535cb12f2079", "RIPEMD-160 hmac");
    if (HAVE_MDC2)
        test_value(MDC2_hmac($str, $key),       "e0ef6a6803e58807c5db395e180a999c", "MDC2 hmac");
    if (HAVE_SHA224)
        test_value(SHA224_hmac($str, $key),     "fad5667fa5aa412044555b7e077fced62372fe9c6ce20815609da12c", "SHA224 hmac");
    if (HAVE_SHA256)
        test_value(SHA256_hmac($str, $key),     "1c90c21e227712b62019ff831f34cba22c2e70f1a902651ef69a70705ee0f754", "SHA256 hmac");
    if (HAVE_SHA384)
        test_value(SHA384_hmac($str, $key),     "e2c253c6dcb050990b4da3cee95cd7b227f43388fa8116f476f59395af295d0d3bb7156ab2fcd0663b0500249a7a0865", "SHA384 hmac");
    if (HAVE_SHA512)
       test_value(SHA512_hmac($str, $key),     "8dcefd7ea3f90ff1c822b5e9547fc36edf78c3e4ce13d47510a212a406bdda1a4094e7ea5ade90e1c736e204d331a814520eba49f3d074e2c261208de07264f6", "SHA512 hmac");
}

sub digest_tests() {
    my string $str = "Hello There This is a Test - 1234567890";

    if (HAVE_MD2)
        test_value(MD2($str), "349ea9f6c9681278cf86955dabd72d31", "MD2 digest");
    test_value(MD4($str), "675d84fbf5d63e0d68c04577c3298fdc", "MD4 digest");
    test_value(MD5($str), "bcbece19c1fe41d8c9e2e6134665ba5b", "MD5 digest");
    test_value(DSS($str), "f4bc2c85698aae8961d626e2c590852b2d081199", "DSS digest");
    test_value(DSS1($str), "f4bc2c85698aae8961d626e2c590852b2d081199", "DSS1 digest");
    test_value(SHA($str), "99910d63f10286e8dda3c4a57801996f48f25b4b", "SHA digest");
    test_value(SHA1($str), "f4bc2c85698aae8961d626e2c590852b2d081199", "SHA1 digest");
    test_value(RIPEMD160($str), "8f32702e0146d5db6145f36271a4ddf249c087ae", "RIPEMD-160 digest");
}

sub crypto_tests() {
    my string $str = "Hello There This is a Test - 1234567890";

    my string $key = "1234567812345abcabcdefgh";
    my binary $x = des_ede_encrypt_cbc($str, $key);
    my string $xstr = des_ede_decrypt_cbc_to_string($x, $key);
    test_value($str, $xstr, "triple DES 2 key encrypt-decrypt");

    $x = des_ede3_encrypt_cbc($str, $key);
    $xstr = des_ede3_decrypt_cbc_to_string($x, $key);
    test_value($str, $xstr, "triple DES 3 key encrypt-decrypt");

    $x = desx_encrypt_cbc($str, $key);
    $xstr = desx_decrypt_cbc_to_string($x, $key);
    test_value($str, $xstr, "DESX encrypt-decrypt");

    $x = blowfish_encrypt_cbc($str, $key);
    $xstr = blowfish_decrypt_cbc_to_string($x, $key);
    test_value($str, $xstr, "blowfish encrypt-decrypt");

    $x = rc4_encrypt($str, $key);
    $xstr = rc4_decrypt_to_string($x, $key);
    test_value($str, $xstr, "rc4 encrypt-decrypt");

    $x = rc2_encrypt_cbc($str, $key);
    $xstr = rc2_decrypt_cbc_to_string($x, $key);
    test_value($str, $xstr, "rc2 encrypt-decrypt");

    $x = cast5_encrypt_cbc($str, $key);
    $xstr = cast5_decrypt_cbc_to_string($x, $key);
    test_value($str, $xstr, "CAST5 encrypt-decrypt");

    my binary $bkey = des_random_key();
    $x = des_encrypt_cbc($str, $bkey);
    $xstr = des_decrypt_cbc_to_string($x, $bkey);
    test_value($str, $xstr, "DES random single key encrypt-decrypt");
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
    test_value($inc(5), "test-5-2", "first closure");
    test_value($inc(7), "test-7-3", "second closure");
    test_value($dec(3), "test-3-2", "third closure");

    my code $c = sub (*reference $r) {
        $r = "hi";
    };
    my string $str;
    $c(\$str);
    test_value($str, "hi", "closure with reference arg");
}

sub format_date_tests() {
    my date $d = 2005-04-01T08:02:05.001;

    test_value(format_date("YY", $d), "05", "last two digits of year");
    test_value(format_date("YYYY", $d), "2005", "four-digit year");
    test_value(format_date("M", $d), "4", "non zero-padded month number (1-12)");
    test_value(format_date("MM", $d), "04", "zero-padded month number (01-12)");
    test_value(format_date("Month", $d), "April", "long month string (ex: \"January\")");
    test_value(format_date("MONTH", $d), "APRIL", "long month string capitalized (ex: \"JANUARY\")");
    test_value(format_date("Mon", $d), "Apr", "abbreviated month (ex: \"Jan\")");
    test_value(format_date("MON", $d), "APR", "abbreviated month, capitalized (ex: \"JAN\")");
    test_value(format_date("D", $d), "1", "non zero-padded day number (1 - 31)");
    test_value(format_date("DD", $d), "01", "zero-padded day number (01 - 31)");
    test_value(format_date("Day", $d), "Friday", "long day of week string (ex: \"Monday\")");
    test_value(format_date("DAY", $d), "FRIDAY", "long day of week string, capitalized (ex: \"MONDAY\")");
    test_value(format_date("Dy", $d), "Fri", "abbreviated day of week string (ex: \"Mon\")");
    test_value(format_date("DY", $d), "FRI", "abbreviated day of week string capitalized (ex: \"MON\")");
    test_value(format_date("H", $d), "8", "non zero-padded hour number (0 - 23)");
    test_value(format_date("HH", $d), "08", "zero-padded hour number (00 - 23)");
    test_value(format_date("h", $d), "8", "non zero-padded hour number, 12-hour clock (1 - 12)");
    test_value(format_date("hh", $d), "08", "zero-padded hour number, 12-hour clock (01 - 12)");
    test_value(format_date("m", $d), "2", "non zero-padded minute number (0 - 59)");
    test_value(format_date("mm", $d), "02", "zero-padded minute number (00 - 59)");
    test_value(format_date("S", $d), "5", "non zero-padded second number (0 - 59)");
    test_value(format_date("SS", $d), "05", "zero-padded second number (00 - 59)");
    test_value(format_date("P", $d), "AM", "\"AM\" or \"PM\" (upper-case)");
    test_value(format_date("p", $d), "am", "\"am\" or \"pm\" (lower-case)");
    test_value(format_date("u", $d), "1", "non zero-padded millisecond number (0 - 999)");
    test_value(format_date("uu", $d), "001", "zero-padded millisecond number (000 - 999)");
    test_value(format_date("ms", $d), "001", "zero-padded millisecond number (000 - 999)");
    test_value(format_date("x", $d), "1000", "non zero-padded microsecond number (0 - 999999)");
    test_value(format_date("xx", $d), "001000", "zero-padded microsecond number (000000 - 999999)");
    test_value(format_date("y", $d), "001", "microseconds, with trailing zeros removed (suitable for use after the '.')");

    # commented out tests that only work when run in European CET time zone
    #test_value(format_date("z", $d), "CEST", "local time zone name (ex: \"EST\") if available, otherwise the UTC offset (ex: \"+01:00\")");
    #test_value(format_date("Z", $d), "+02:00", "time zone UTC offset like +HH:mm[:SS] (ex: \"+01:00\"), seconds are only included if non-zero");
}

sub number_tests() {
    test_value(string(10.2n), "10.2", "first number"); 
    test_value(string(-10.2n), "-10.2", "second number"); 
    test_value(string(1.000000000099999999n), "1.000000000099999999", "third number"); 
    test_value(10.245n.toString(NF_Scientific), "1.0245e+01", "fourth number"); 
    test_value((-10.245n).toString(NF_Scientific), "-1.0245e+01", "fifth number"); 
    test_value(0.10245n.toString(NF_Scientific), "1.0245e-01", "sixth number"); 
    test_value((-0.10245n).toString(NF_Scientific), "-1.0245e-01", "seventh number"); 
    test_value(1.0245n.toString(NF_Scientific), "1.0245e+00", "sixth number"); 
    test_value((-1.0245n).toString(NF_Scientific), "-1.0245e+00", "seventh number"); 
    test_value(10.245n.toString(), "10.245", "eighth number"); 
    test_value((-10.245n).toString(), "-10.245", "ninth number"); 
    test_value(0.10245n.toString(), "0.10245", "tenth number"); 
    test_value((-0.10245n).toString(), "-0.10245", "eleventh number"); 
    test_value(1.0245n.toString(), "1.0245", "twelfth number"); 
    test_value((-1.0245n).toString(), "-1.0245", "thirteenth number"); 
    test_value(10.001999999999n.toString(), "10.001999999999", "fourteenth number"); 
    test_value((-10.001999999999n).toString(), "-10.001999999999", "fifteenth number"); 
    test_value(0.10001999999999n.toString(), "0.10001999999999", "sixteenth number"); 
    test_value((-0.10001999999999n).toString(), "-0.10001999999999", "seventeenth number"); 
    test_value(1.0001999999999n.toString(), "1.0001999999999", "eighteenth number"); 
    test_value((-1.0001999999999n).toString(), "-1.0001999999999", "nineteenth number"); 
    test_value(0.8n.toString(), "0.8", "number rounding 1");
    test_value(0.8n.toString(NF_Scientific), "8e-01", "number rounding 2");
    test_value((-0.8n).toString(), "-0.8", "number rounding 3");
    test_value((-0.8n).toString(NF_Scientific), "-8e-01", "number rounding 4");
    test_value((34.9n * 100).toString(), "3490", "number rounding 5");
    test_value(1e50n.toString(), "100000000000000000000000000000000000000000000000000", "number rounding 5");
    test_value((-1e50n).toString(), "-100000000000000000000000000000000000000000000000000", "number rounding 6");
}

sub background_tests() {
    my int $i = 0;
    background delete $i;
    background remove $i;

    # do negative tests
    my Program $p();
    try {
        $p.parse("my int $i; background ($i *= 10);background ($i /= 10);background ($i -= 10);background ($i += 10);background ($i %= 10);background ($i >>= 10);background ($i <<= 10);background ++$i;background $i++;background --$i;background $i--;my string $str;background splice $str, 0;background extract $str, 0;", "bg");
        test_value(False, True, "background negative");
    }
    catch (*hash $ex) {
        # count exceptions
        while ($ex) {
            ++$i;
            $ex = $ex.next;
        }
        test_value($i, 13, "background negative");
    }
}

sub type_code_test() {
    test_value(True.typeCode(), NT_BOOLEAN, "typeCode() bool");
    test_value("foo".typeCode(), NT_STRING, "typeCode() string");
    test_value(1.typeCode(), NT_INT, "typeCode() int");
    test_value(1n.typeCode(), NT_NUMBER, "typeCode() number");
    test_value(now().typeCode(), NT_DATE, "typeCode() date");
    test_value(1.2.typeCode(), NT_FLOAT, "typeCode() float");
    test_value((1,2,).typeCode(), NT_LIST, "typeCode() list");
    test_value(("foo":1).typeCode(), NT_HASH, "typeCode() bool");
    test_value(NULL.typeCode(), NT_NULL, "typeCode() NULL");
    test_value(NOTHING.typeCode(), NT_NOTHING, "typeCode() NOTHING");
}

sub mime_tests() {
    my string $str = "This is a test: æéìœü";
    test_value($str, mime_decode_quoted_printable(mime_encode_quoted_printable($str)), "MIME: quoted printable");
    test_value($str, mime_decode_base64_to_string(mime_encode_base64($str)), "MIME: base64");
    test_value($str, mime_decode_header(mime_encode_header_word_q($str)), "MIME: header word q");
    test_value($str, mime_decode_header(mime_encode_header_word_b($str)), "MIME: header word b");
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
    # returns a constant value
    "target_sys": ("constant": "Orders"),
);

const MapInput = ((
    "^attributes^": ("Id": 1),
    "name": "John Smith",
    "Count": 1,
    "OrderDate": "02.01.2014 10:37:45.103948",
    "Products": ((
        "ProductName": "Widget 1",
        "Quantity": 1,
        ),
    )), (
    "^attributes^": ("Id": 2),
    "name": "Steve Austin",
    "Count": 2,
    "OrderDate": "04.01.2014 19:21:08.882634",
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
    "target_sys": "Orders",
    ), (
    "id": 2,
    "name": "Steve Austin",
    "explicit_count": 2,
    "implicit_count": 2,
    "order_date": 2014-01-04T19:21:08.882634,
    "target_sys": "Orders",
    ),
);

sub mapper_tests() {
    my Mapper $map(DataMap);
    my list $l = $map.mapAll(MapInput);
    test_value($l, MapOutput, "Mapper::mapAll()");
    test_value($map.getCount(), 2, "1:Mapper::getCount()");
    $l = map $map.mapData($1), MapInput;
    test_value($l, MapOutput, "map Mapper::mapData()");
    test_value($map.getCount(), 4, "1:Mapper::getCount()");
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
    test_value($l, CsvRecords, "CsvDataIterator 1");

    # test with empty data and header lines
    $i = new CsvDataIterator("", ("header-lines": 1));
    test_value($i.next(), False, "CsvDataIterator 2");
}

sub module_tests() {
    mime_tests();
    mapper_tests();
    csvutil_tests();
}

sub do_tests() {
    on_exit $counter.dec();
    try {
	for (my int $i = 0; $i < $o.iters; $i++) {
	    if ($o.verbose)
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
	    if ($o.bq)
		backquote_tests();
	}
    }
    catch () {
	++$errors;
	rethrow;	
    }
}

sub main() {
    parse_command_line();
    printf("QORE v%s Test Script (%d thread%s, %d iteration%s per thread)\n", Qore::VersionString, 
	   $o.threads, $o.threads == 1 ? "" : "s", $o.iters, $o.iters == 1 ? "" : "s");

    # run regression background tests
    background_tests();

    our Counter $counter();
    while ($o.threads--) {
	$counter.inc();
	background do_tests();
    }

    $counter.waitForZero();

    my int $ntests = elements $thash;
    printf("%d error%s encountered in %d test%s.\n",
	   $errors, $errors == 1 ? "" : "s", 
	   $ntests, $ntests == 1 ? "" : "s");

    # add exit status as # of errors
    exit($errors);
}

main();
