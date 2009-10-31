#!/usr/bin/env qore 

# require all global variables to be declared with "our"
%require-our
# enable all warnings
%enable-all-warnings
# child programs do not inherit parent's restrictions
%no-child-restrictions

# make sure we have the right version of qore
%requires qore >= 0.7.6

# global variables needed for tests
our $to = new Test("program-test.q");
our $ro = new Test("readonly");
our ($o, $errors, $counter);
our $thash;

sub get_program_name() {
    my $l = split("/", $ENV."_");
    return $l[elements $l - 1];
}

sub usage() {
    printf(
"usage: %s [options] <iterations>
  -h,--help         shows this help text
  -b,--backquote    include backquote tests (slow)
  -t,--threads=ARG  runs tests in ARG threads
  -v,--verbose=ARG  sets verbosity level to ARG
", 
	   get_program_name());
    exit(1);
}

const opts = 
    ( "verbose" : "verbose,v:i+",
      "help"    : "help,h",
      "bq"      : "backquote,b",
      "threads" : "threads,t=i" );

sub parse_command_line() {
    my $g = new GetOpt(opts);
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

sub test_value($v1, $v2, $msg) {
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

sub test1() { return 1;} sub test2() { return 2; } 
sub test3() { return (1, 2, 3); }

sub array_helper($a) {
    $a[1][1] = 2;
    test_value($a[1][1], 2, "passed local array variable assignment");    
}

sub list_return($var) {
    return (1, test2(), $var);
}

sub hash_return($var) {
    return ( "gee" : "whiz", 
	     "num" : test1(),
	     "var" : $var );
}

class Sort {
    hash($l, $r) {
	return $l.key1 <=> $r.key1;
    }
}
sub hash_sort_callback($l, $r) {
    return $l.key1 <=> $r.key1;
}

# array tests
sub array_tests() {
    my ($a, $b, $c, $d);

    if ($o.verbose)
	print("%%%% array tests\n");
    $a = 1, 2, 3, 4, 5;
    test_value(elements $a, 5, "elements operator");
    test_value($a[1], 2, "single-dimentional list dereference");
    $b = 1, 2, (3, 4, 5), 6, 7;
    test_value($b[2][1], 4, "multi-dimentional list dereference");
    delete $b;
    test_value($b[2][1], NOTHING, "multi-dimentional list dereference after delete operator");
    $b = $a;
    $a[1] = "hello";
    test_value($a[1], "hello", "list dereference after list assignment and element reassignment");
    test_value($b[1], 2, "list dereference of source list");
    $a[0][1] = "hello";
    $c[10]{"testing"} = "well then";
    test_value($a[0][1], "hello", "second multi-dimentional list dereference");
    test_value($a[1][500], NOTHING, "non-existant element deference");
    test_value(int($c[10]), 0, "hash list element dereference");
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
    my $l1 = ( 3, 2, 4, 1, 6, 5 );
    my $l2 = ( "one", "two", "three", "four", "five", "six" );
    my $hl = 
	( ( "key1" : 8, "key2" : "two" ),
	  ( "key1" : 2, "key2" : "seven" ),
	  ( "key1" : 7, "key2" : "six" ),
	  ( "key1" : 1, "key2" : "eight" ),
	  ( "key1" : 6, "key2" : "four" ),
	  ( "key1" : 9, "key2" : "three" ),
	  ( "key1" : 3, "key2" : "five" ),
	  ( "key1" : 5, "key2" : "nine" ),
	  ( "key1" : 4, "key2" : "one" ) );
    my $sorted_hl = 
	( ( "key1" : 1, "key2" : "eight" ),
	  ( "key1" : 2, "key2" : "seven" ),
	  ( "key1" : 3, "key2" : "five" ),
	  ( "key1" : 4, "key2" : "one" ),
	  ( "key1" : 5, "key2" : "nine" ),
	  ( "key1" : 6, "key2" : "four" ),
	  ( "key1" : 7, "key2" : "six" ),
	  ( "key1" : 8, "key2" : "two" ),
	  ( "key1" : 9, "key2" : "three" ) );
    my $stable_sorted_hl = 
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
    my $s = new Sort();
    test_value(sort($l1), (1,2,3,4,5,6), "first sort()");
    test_value(sort($l2), ("five", "four", "one", "six", "three", "two"), "second sort()");
    test_value(sort($hl, \hash_sort_callback()), $sorted_hl, "sort() with function call reference callback");
    test_value(sort($hl, \$s.hash()), $sorted_hl, "sort() with object method callback");
    test_value(sort($hl, "hash_sort_callback"), $sorted_hl, "sort() with string function name callback");
    test_value(sort($hl, sub ($l, $r) { return $l.key1 <=> $r.key1; }), $sorted_hl, "sort() with closure callback");

    my $r_sorted_hl = reverse($sorted_hl);
    test_value(sortDescending($l1), (6,5,4,3,2,1), "first sortDescending()");
    test_value(sortDescending($l2), ("two", "three", "six", "one", "four", "five"), "second sortDescending()");
    test_value(sortDescending($hl, \hash_sort_callback()), $r_sorted_hl, "first sortDescending() with callback");
    test_value(sortDescending($hl, \$s.hash()), $r_sorted_hl, "second sortDescending() with callback");
    test_value(sortDescending($hl, "hash_sort_callback"), $r_sorted_hl, "third sortDescending() with callback");
    test_value(sortDescending($hl, sub ($l, $r) { return $l.key1 <=> $r.key1; }), $r_sorted_hl, "sortDescending() with closure callback");

    $hl += ( "key1" : 3, "key2" : "five-o" );
    test_value(sortStable($hl, \hash_sort_callback()), $stable_sorted_hl, "first sortStable() with callback");
    test_value(sortStable($hl, \$s.hash()), $stable_sorted_hl, "second sortStable() with callback");
    test_value(sortStable($hl, "hash_sort_callback"), $stable_sorted_hl, "third sortStable() with callback");
    test_value(sortStable($hl, sub ($l, $r) { return $l.key1 <=> $r.key1; }), $stable_sorted_hl, "sortStable() with closure callback");

    my $r_stable_sorted_hl = reverse($stable_sorted_hl);
    test_value(sortDescendingStable($hl, \hash_sort_callback()), $r_stable_sorted_hl, "first sortDescendingStable() with callback");
    test_value(sortDescendingStable($hl, \$s.hash()), $r_stable_sorted_hl, "second sortDescendingStable() with callback");
    test_value(sortDescendingStable($hl, "hash_sort_callback"), $r_stable_sorted_hl, "third sortDescendingStable() with callback");
    test_value(sortDescendingStable($hl, sub ($l, $r) { return $l.key1 <=> $r.key1; }), $r_stable_sorted_hl, "sortDescendingStable() with closure callback");

    test_value(min($l1), 1, "simple min()");
    test_value(max($l1), 6, "simple max()");
    test_value(min($hl, \hash_sort_callback()), ( "key1" : 1, "key2" : "eight" ), "first min() with callback");
    test_value(min($hl, \$s.hash()), ( "key1" : 1, "key2" : "eight" ), "second min() with callback");
    test_value(min($hl, "hash_sort_callback"), ( "key1" : 1, "key2" : "eight" ), "third min() with callback");
    test_value(max($hl, \hash_sort_callback()), ( "key1" : 9, "key2" : "three" ), "first max() with callback");
    test_value(max($hl, \$s.hash()), ( "key1" : 9, "key2" : "three" ), "second max() with callback");
    test_value(max($hl, "hash_sort_callback"), ( "key1" : 9, "key2" : "three" ), "third max() with callback");
    my $v = shift $l2;
    test_value($l2, ("two","three","four","five","six"), "array shift");
    unshift $l2, $v;
    test_value($l2, ("one","two","three","four","five","six"), "array unshift");
    # list assignment tests
    my $l[1] = "boo";
    ($l[0], $l[1]) = "hi1";
    test_value($l, ("hi1", NOTHING), "first list assigment");
    ($l[0], $l[1]) = ("hi2", "shoo1");
    test_value($l, ("hi2", "shoo1"), "second list assigment");
    ($l[0], $l[1]) = ("hi3", "shoo2", "bean1");
    test_value($l, ("hi3", "shoo2"), "third list assigment");
    $v = pop $l1;
    test_value($l1, (3,2,4,1,6), "pop");
    push $l1, "hi";
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

    $a = "hello";
    test_value($a[2], "l", "string element dereference");
    $a = binary($a);
    test_value($a[4], ord("o"), "binary byte dereference");
}

sub hash_tests() {
    if ($o.verbose)
	print("%%%% hash tests\n");
    # hash tests
    my $b = ( "test" : 1, "gee" : 2, "well" : "string" );
    test_value($b.gee, 2, "object dereference");
    test_value(elements $b, 3, "elements operator on hash before delete");
    delete $b{"gee"};
    test_value(elements $b, 2, "elements operator on hash after delete");
    $b{"test"} = "there";
    my $d{"gee"}[25] = "I hope it works";
    test_value($b.test, "there", "hash dereference after assignment");
    test_value($b.test, "there", "object dereference after assignment");
    test_value($b{"geez"}, NOTHING, "non-existant object dereference");
    test_value(int($d.gee), 0, "hash dereference of entire list member");
    test_value($d{"gee"}[25], "I hope it works", "dereference of list member of hash");
    my $c = ( "hi" : "there", "gee" : "whillakers" );
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
    my $a = ( "key" : 1, "unique" : 100, "asd" : "dasd" );
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

    my $ot = new Test(1, "two", 3.0);
    $ot += $a;
    test_value($ot.("unique", "new"), ("unique" : 100, "new" : 45), "hash slice creation from object");

    # delete 3 keys from the $c hash
    $b = $c - "new" - "barn" - "asd";
    test_value($b, ( "key" : 3, "unique" : 100 ), "hash minus operator"); 
    $b = $c - ("new", "barn", "asd");
    test_value($b, ( "key" : 3, "unique" : 100 ), "hash minus operator with list argument"); 
    $b -= "unique";
    test_value($b, ( "key" : 3 ), "hash minus-equals operator"); 
    $c -= ( "new", "barn" );
    test_value($c, ( "key": 3, "unique" : 100, "asd" : "dasd" ), "hash minus-equals operator with list argument");
    my $nh += ( "new-hash" : 1 );
    test_value($nh, ( "new-hash" : 1 ), "hash plus-equals, lhs NOTHING");
}

sub global_variable_testa() {
    printf("user=%s\n", $ENV{"USER"});
}

sub map_closure($v) { return sub($v1) { return $v * $v1; }; }

# operator tests
sub operator_test() {
    if ($o.verbose)
	print("%%%% operator tests\n");
    my $a = 1;
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
    $a *= 2.2;
    test_value($a, 22.0, "first float *= operator");
    $a *= 2;
    test_value($a, 44.0, "second float *= operator");
    $a /= 4.4;
    test_value($a, 10.0, "float /= operator");
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
    $a = "hello" + " there";
    test_value($a, "hello there", "string concatenation");
    $a += " gee";
    test_value($a, "hello there gee", "string plus equals");
    $a = 1.0;
    $a += 1.2;
    test_value($a, 2.2, "float += operator");
    $a -= 1.1;
    test_value($a, 1.1, "float -= operator");
    $a = 5.5 * 2.0;
    test_value($a, 11.0, "float * operator");
    test_value(now() > (now() - 1D), True, "date > operator");
    test_value(now() >= (now() - 1h), True, "date >= operator");
    test_value((now() - 1m) < now(), True, "date < operator");
    test_value((now() - 1M) <= now(), True, "date <= operator");
    my $b = $a = now();
    #test_value($a == $b, True, "date == operator");
    test_value($a, $b, "date == operator");
    $a = 2004-02-28-12:00:00;
    $a += 1D;
    test_value($a, 2004-02-29-12:00:00, "first date += operator");
    $a -= (3h + 5m);
    test_value($a, 2004-02-29-08:55:00, "second date += operator");
    my $ni += 3.2;
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
    my $c = map_closure(2);

    # map function to list
    test_value((map $c($1), (1, 2, 3)), (2, 4, 6), "map operator using closure");

    # map immediate expression to list
    test_value((map $1 * 2, (1, 2, 3)), (2, 4, 6), "map operator using expression");

    # map function to list with optional select code as expression
    test_value((map $c($1), (1, 2, 3), $1 > 1), (4, 6), "map operator using closure with optional select expression");

    # select all elements from list greater than 1 with expression
    test_value((select (1, 2, 3), $1 > 1), (2, 3), "select operator with expression");

    # create a sinple closure to subtract the second argument from the first
    $c = sub($x, $y) { return $x - $y; };

    # left fold function on list using closure
    test_value((foldl $c($1, $2), (2, 3, 4)), -5, "foldl operator with closure");

    # left fold function on list using expression
    test_value((foldl $1 - $2, (2, 3, 4)), -5, "foldl operator with expression");

    # right fold function on list using immediate closure
    test_value((foldr $c($1, $2), (2, 3, 4)), -1, "foldr operator with closure");

    # right fold function on list using expression and implicit arguments
    test_value((foldr $1 - $2, (2, 3, 4)), -1, "foldr operator with expression");
}

sub no_parameter_test($p) {
    test_value($p, NOTHING, "non-existant parameter");
}

sub parameter_and_shift_test($p) {
    test_value($p, 1, "parameter before shift");
    test_value(shift $argv, 2, "shift on second parameter");
}

sub one_parameter_shift_test() {
    test_value(shift $argv, 1, "one parameter shift");
}

sub shift_test() {
    my $var = (1, 2, 3, 4, "hello");
    foreach my $v in ($var)
	test_value($v, shift $argv, ("shift " + string($v) + " parameter"));
}

sub parameter_tests() {
    no_parameter_test();
    parameter_and_shift_test(1, 2);
    shift_test(1, test3()[1], 3, 4, "hello");
    one_parameter_shift_test(1);
}

sub short_circuit_test($op) {
    print("ERROR: $op logic short-circuiting is not working!\n");
    $errors++;
    return 0;
}

sub logic_message($op) {
    if ($o.verbose)
	printf("OK: %s logic test\n", $op);
}

# logic short-circuiting test
sub logic_tests() {
    my $a = 1;
    my $b = 0;
    my $c;

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

sub switch_test($val) {
    my $rv;

    switch ($val) {
	case 0:
	case "hello":
	
	case 1:
	    $rv = "case 1";
            break;

	case 2:
	    $rv = "case 2";

        default:
	    return "default";
    }
    return $rv;
}

sub regex_switch_test($val) {
    my $rv;

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

sub switch_with_relation_test($val) {
  my $rv;
  switch ($val) {
  case < -1 : $rv = "first switch"; break;
  case > 1 : $rv = "second switch"; break;
  case <= -1: $rv = "third switch"; break;
  case >= 1: $rv = "fourth switch"; break;
  case 0: $rv = "fifth switch"; break;
  }
  return $rv;
}

sub statement_tests() {
    if ($o.verbose)
	print("%%%% statement tests\n");
    # while test
    my $a = 0;
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
    my $b = 0;
    for (my $i = 0; $i < 3; $i++)
	$b++;
    test_value($a, 3, "for");
    test_value($b, 3, "for exec");    
    # foreach tests
    $b = 0;
    my $v;
    foreach $v in (1, 2, 3)
	$b++;
    test_value($v, 3, "foreach");
    test_value($b, 3, "foreach exec");

    my $list = my $x;
    foreach my $y in (\$list) $y = "test";
    test_value($list, NOTHING, "first foreach reference");
    
    $list = (1, 2, 3);
    foreach my $y in (\$list) $y = "test";
    test_value($list, ("test", "test", "test"), "second foreach reference");
    
    $list = 1;
    foreach my $y in (\$list) $y = "test";
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
    my $err;
    my $success = False;
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
	    $b = "hello";
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

sub fibonacci($num) {
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

sub sd($d) {
    return format_date("YYYY-MM-DD HH:mm:SS", $d);
}

sub test_date($d, $y, $w, $day, $n, $i) {
    my $str = sprintf("%04d-W%02d-%d", $y, $w, $day);
    my $h = ( "year" : $y, "week" : $w, "day" : $day );
    my $d1;
    # subtract milliseconds from date to compare with timegm value
    if (my $ms = get_milliseconds($d))
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
    my $date  = 2004-02-01T12:30:00;
    # qore-specific date/time specification format ('-' instead of 'T' - more readable but non-standard)
    my $ndate = 2004-03-02-12:30:00;
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
    test_value($date - milliseconds(500), 2004-02-01-12:29:59.500, "second date millisecond subtraction");

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
    test_value($date - P0000-01-00,          2004-01-01T12:30:00, "third date month subtraction");
    test_value($date - P0000-00-01,          2004-01-31T12:30:00, "third date day subtraction");
    test_value($date + P0001-00-00,          2005-02-01T12:30:00, "third date year addition");
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
    test_value($date + 30D,  $ndate,                   "fourth date day addition");
    test_value($ndate - 30D, $date,                    "fourth date day subtraction");
    test_value($date + 23M,  2006-01-01T12:30:00,      "fourth date month addition");
    test_value($date - 4M,   2003-10-01T12:30:00,      "fourth date month subtraction");
    test_value($date,        date("20040201123000"),   "date function");

    # times without a date are assumed to be on Jan 1, 1970
    test_value(11:25:27, 1970-01-01T11:25:27.000, "direct hour");

    # test date conversion/calculation functions against known values
    my $i = 1;
    test_date(1068-01-01,              1068, 1, 3, 1,    \$i);
    test_date(1783-09-18,              1783, 38, 4, 261, \$i);
    test_date(1864-02-29,              1864, 9,  1, 60,  \$i);
    test_date(1864-03-01,              1864, 9,  2, 61,  \$i);
    test_date(1968-01-01T11:01:20,     1968, 1,  1, 1,   \$i);
    test_date(1968-02-29,              1968, 9,  4, 60,  \$i);
    test_date(1968-03-01,              1968, 9,  5, 61,  \$i);
    test_date(1969-12-31T23:59:59.999, 1970, 1,  3, 365, \$i);
    test_date(1969-12-31T00:00:00.100, 1970, 1,  3, 365, \$i);
    test_date(1969-01-01T17:25:31.380, 1969, 1,  3, 1,   \$i);
    test_date(1970-01-01,              1970, 1,  4, 1,   \$i);
    test_date(1970-12-01T00:00:00,     1970, 49, 2, 335, \$i);
    test_date(1972-01-01,              1971, 52, 6, 1,   \$i);
    test_date(1972-12-30,              1972, 52, 6, 365, \$i);
    test_date(1972-12-31,              1972, 52, 7, 366, \$i);
    test_date(2004-02-28,              2004, 9,  6, 59,  \$i);
    test_date(2004-02-29,              2004, 9,  7, 60,  \$i);
    test_date(2004-03-01,              2004, 10, 1, 61,  \$i);
    test_date(2004-03-28,              2004, 13, 7, 88,  \$i);
    test_date(2006-01-01,              2005, 52, 7, 1,   \$i);
    test_date(2006-09-01,              2006, 35, 5, 244, \$i);
    test_date(2006-12-01,              2006, 48, 5, 335, \$i);
    test_date(2007-12-30,              2007, 52, 7, 364, \$i);
    test_date(2007-12-31,              2008, 1,  1, 365, \$i);
    test_date(2008-01-01,              2008, 1,  2, 1,   \$i);
    test_date(2008-01-06,              2008, 1,  7, 6,   \$i);
    test_date(2008-01-07,              2008, 2,  1, 7,   \$i);
    test_date(2008-01-08,              2008, 2,  2, 8,   \$i);
    test_date(2008-01-09,              2008, 2,  3, 9,   \$i);
    test_date(2008-01-10,              2008, 2,  4, 10,  \$i);
    test_date(2008-12-28,              2008, 52, 7, 363, \$i);
    test_date(2008-12-29,              2009, 1,  1, 364, \$i);
    test_date(2008-12-30,              2009, 1,  2, 365, \$i);
    test_date(2010-01-03,              2009, 53, 7, 3,   \$i);
    test_date(2010-01-04,              2010, 1,  1, 4,   \$i);
    test_date(2010-01-09,              2010, 1,  6, 9,   \$i);
    test_date(2010-01-10,              2010, 1,  7, 10,  \$i);
    test_date(2010-01-11,              2010, 2,  1, 11,  \$i);
    test_date(2016-12-01,              2016, 48, 4, 336, \$i);
    test_date(2026-08-22,              2026, 34, 6, 234, \$i);
    test_date(2036-04-30,              2036, 18, 3, 121, \$i);
    test_date(2054-06-19,              2054, 25, 5, 170, \$i);
    test_date(2400-12-01,              2400, 48, 5, 336, \$i);
    test_date(2970-01-01,              2970, 1,  1, 1,   \$i);
    test_date(9999-12-31,              9999, 52, 5, 365, \$i);
    test_date(9999-12-31T23:59:59.999, 9999, 52, 5, 365, \$i);

    # absolute date difference tests
    test_value(2006-01-02T11:34:28.344 - 2006-01-01,              1D + 11h + 34m + 28s +344ms,       "date difference 1");
    test_value(2099-04-21T19:20:02.106 - 1804-03-04T20:45:19.956, 107793D + 22h + 34m + 42s + 150ms, "date difference 2");
}

sub binary_tests() {
    my $b = binary("this is a test");
    test_value(getByte($b, 3), ord("s"), "getByte()");
    test_value($b, binary("this is a test"), "binary comparison");
    test_value($b != binary("this is a test"), False, "binary negative comparison");
}

sub string_tests() {
    my $str = "Hi there, you there, pal";
    my $big = "GEE WHIZ";
    test_value(strlen($str), 24, "strlen()");
    test_value(toupper($str), "HI THERE, YOU THERE, PAL", "toupper()");
    test_value(tolower($big), "gee whiz", "tolower()");
    test_value(reverse($big), "ZIHW EEG", "reverse()");
    
    # set up a string with UTF-8 multi-byte characters
    $str = "Über die Wolken läßt sich die Höhe begrüßen";
    test_value(strlen($str), 49, "UTF-8 strlen()");
    test_value(length($str), 43, "UTF-8 length()");
    test_value(substr($str, 30), "Höhe begrüßen", "first UTF-8 substr()");
    test_value(substr($str, -8), "begrüßen", "second UTF-8 substr()");
    test_value(substr($str, 0, -39), "Über", "third UTF-8 substr()");
    test_value(index($str, "läßt"), 16, "first UTF-8 index()");
    test_value(index($str, "läßt", 1), 16, "second UTF-8 index()");
    test_value(rindex($str, "ß"), 40, "first UTF-8 rindex()");
    test_value(rindex($str, "ß", 25), 18, "second UTF-8 rindex()"); 
    test_value(reverse($str), "neßürgeb ehöH eid hcis tßäl nekloW eid rebÜ", "UTF-8 reverse()");

    test_value($str[31], "ö", "first UTF-8 string index dereference");
    test_value($str[39], "ü", "second UTF-8 string index dereference");

    # save string
    my $ostr = $str;
    # convert the string to single-byte ISO-8859-1 characters and retest
    $str = convert_encoding($str, "ISO-8859-1");
    test_value($str != $ostr, False, "string != operator with same text with different encodings");
    test_value(strlen($str), 43, "ISO-8859-1 strlen()");
    test_value(length($str), 43, "ISO-8859-1 length()");
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
    my $a = split(" ", $str);
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
    my $nstr = convert_encoding($str, "ISO-8859-1");
    test_value(length($nstr), 7, "length() with ISO-8859-1 special characters");
    test_value(strlen($nstr), 7, "strlen() with ISO-8859-1 special characters");
    test_value($str, convert_encoding($nstr, "UTF-8"), "convert_encoding()");
    # assign binary object
    my $x = <0abf83e8ca72d32c>;
    my $b64 = makeBase64String($x);
    test_value($x, parseBase64String($b64), "first base64");
    test_value("aGVsbG8=", makeBase64String("hello"), "makeBase64String()");
    my $hex = makeHexString($x);
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

    # join tests
    $str = join(":", "login","passwd",1,"gid","gcos","home","shell");
    test_value($str, "login:passwd:1:gid:gcos:home:shell", "first join");
    my $l = ("login","passwd","uid","gid","gcos","home","shell");
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
    $a = "abc def";
    $a =~ s/(\w+) +(\w+)/$2, $1/; 
    test_value($a, "def, abc", "regular expression subpattern substitution");

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
    test_value(regex_subst($l[0], "bar", "foo"), "hello foo hi bar", "first non-global regular expression substitution");
    test_value(regex_subst($l[1], "bar", "foo"), "foo hello bar hi bar", "second non-global regular expression substitution");
    test_value(regex_subst($l[2], "BAR", "foo", RE_Caseless), "hello foo hi", "case-insensitive non-global regular expression substitution");
    $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
    test_value(regex_subst($l[0], "bar", "foo", RE_Global), "hello foo hi foo", "first global regular expression substitution");
    test_value(regex_subst($l[1], "bar", "foo", RE_Global), "foo hello foo hi foo", "second global regular expression substitution");
    test_value(regex_subst($l[2], "BAR", "foo", RE_Global|RE_Caseless), "hello foo hi", "case-insensitive global regular expression substitution");
    $a = "abc def";
    # note that the escape characters have to be escaped in the following pattern string
    test_value(regex_subst($a, "(\\w+) +(\\w+)", "$2, $1"), "def, abc", "first regular expression subpattern substitution");
    # here we use single-quotes, so the escape characters do not have to be escaped
    test_value(regex_subst($a, '(\w+) +(\w+)', "$2, $1"), "def, abc", "second regular expression subpattern substitution");

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
    my $h = ( "key1" : "hello\n", "key2" : 2045, "key3": "test\r\n", "key4" : 302.223 );
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
}

sub pwd_tests() {
    # getpwuid(0).pw_name may not always be "root"
    test_value(getpwuid(0).pw_uid, 0, "getpwuid()");
}

sub simple_shift() {
    return shift $argv;
}

sub misc_tests() {
    my $dh = ( "user"    : "user",
	       "pass"    : "123pass@word",
	       "db"      : "dbname",
	       "charset" : "utf8",
	       "host"    : "hostname" );
    my $ds = "user/123pass@word@dbname(utf8)%hostname";
    test_value($dh, parseDatasource($ds), "first parseDatasource()"); 
    test_value((1, 2), simple_shift((1, 2)), "list arg function call");

    test_value(call_function("simple_shift", 1), 1, "call_function()");
    test_value(call_builtin_function("type", 1), Type::Int, "call_builtin_function()");
    test_value(existsFunction("simple_shift"), True, "existsFunction()");
    test_value(functionType("simple_shift"), "user", "functionType() user");
    test_value(functionType("printf"), "builtin", "functionType() builtin");
    test_value(typename(1), "integer", "type()");
    my $str1 = "&<>\"";
    my $str2 = "&amp;&lt;&gt;&quot;";
    test_value(html_encode($str1), $str2, "html_encode()");
    test_value(html_decode($str2), $str1, "html_decode()");

    # note that '@' signs are legal in the password field as with datasources
    my $url = "https://username:passw@rd@hostname:1044/path/is/here";
    my $uh = ( "protocol" : "https",
	       "username" : "username",
	       "password" : "passw@rd",
	       "host" : "hostname",
	       "port" : 1044,
	       "path" : "/path/is/here" );

    test_value(parseURL($url), $uh, "parseURL()");

    # test gzip
    my $str = "This is a long string xxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    my $bstr = binary($str);
    my $c = compress($str);
    test_value($str, uncompress_to_string($c), "compress() and uncompress_to_string()");
    test_value($bstr, uncompress_to_binary($c), "compress() and uncompress_to_binary()");
    my $gz = gzip($str);
    test_value($str, gunzip_to_string($gz), "gzip() and gunzip_to_string()");
    test_value($bstr, gunzip_to_binary($gz), "gzip() and gunzip_to_binary()");
    
    # test bzip2
    my $bz = bzip2($str);
    test_value($str, bunzip2_to_string($bz), "bzip2 and bunzip2_to_string");
    test_value($bstr, bunzip2_to_binary($bz), "bzip2 and bunzip2_to_binary");
}

sub math_tests() {
    test_value(ceil(2.7), 3.0, "ceil()");
    test_value(floor(3.7), 3.0, "fllor()");
    test_value(format_number(".,3", -48392093894.2349), "-48.392.093.894,235", "format_number()");
}

sub lib_tests() {
    my $pn = get_script_path();
    test_value(stat($pn)[1], hstat($pn).inode, "stat() and hstat()");
    test_value(hstat($pn).type, "REGULAR", "hstat()");
    #my $h = gethostname();
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

sub function_library_test() {
    date_time_tests();
    binary_tests();
    string_tests();  
    pwd_tests();
    misc_tests();
    math_tests();
    lib_tests();
    io_tests();
}

sub t($a) {
    return $a + 1;
}

class Test inherits Socket {
    private $.a, $.b;

    constructor($a, $b, $c) {
	$.a = 1;
	$.b = 2;
        $.data = ($a, $b, $c);
    }
    getData($elem) {
	if (exists $elem)
	    return $.data[$elem];
        return $.data;
    }
    methodGate($m) {
        return $m;
    }
    memberGate($m) {
        return "memberGate-" + $m;
    }
    memberNotification($m) {
	$.t.$m = $self.$m;
    }
    closure($x) {
	my $a = 1;
	# return a closure encapsulating the state of the object
	return sub ($y) {
	    return sprintf("%s-%n-%n-%n", $.data[1], $x, $y, ++$a);
	};
    }
}

sub class_test_Program() {
    my $func = "namespace ITest { const val = 1.0; } $gv2 = 123; sub t2($a) { return $a + 2; } sub et($a) { return t($a); } sub tot() { return getClassName($to); } sub getObject() { return new Queue(); } sub deleteException() { $ro.getData(0); delete $ro; }";

    my $pf = "newfunc();";
    my $nf = "sub newfunc() { return True; }";

    my $a = new Program();
    $a.parsePending($pf, "pending test part1");
    $a.parsePending($nf, "pending test part2");
    $a.parseCommit();    
    $a.importFunction("t");
    $a.importGlobalVariable("to");
    $a.importGlobalVariable("ro", True);
    $a.parse($func, "test");

    test_value($a.callFunction("newfunc"), True, "Program::parsePending()");
    test_value($a.callFunction("t2", 1), 3, "Program::parse()");
    test_value($a.callFunctionArgs("t2", list(int(2))), 4, "program imported function");
    test_value($a.callFunction("et", 1), 2, "program imported function");
    test_value($a.callFunction("tot", 1), "Test", "program imported object variable");
    test_value($to.member, "memberGate-member", "program imported object member gate");
    test_value($to.method(), "method", "program imported object method gate");
    try
	$a.callFunction("deleteException");
    catch ($ex)
	test_value($ex.err, "ACCESS-ERROR", "Program::importGlobalVariable() readonly");    
    my $o = $a.callFunction("getObject");
    delete $a;
    test_value(getClassName($o), "Queue", "class returned from deleted subprogram object");
}

sub class_test_File() {
    return;
/*
    # File test
    my $f = new File();
    $f.open($ENV."_");
    my $l = $f.readLine();
    my $p = $f.getPos();
    $f.setPos(0);
    test_value($l, $f.readLine(), "File::readLine() and File::setPos()");
    test_value($p, $f.getPos(), "File::getPos()");
*/
}

sub err($test) {
    test_value(True, False, $test);
}

sub check($err, $test) {
    test_value($err, "PRIVATE-MEMBER", $test);
}

class Test2 { private $.a; }

sub class_library_tests() {
    my $t = new Test(1, "gee", 2);
    test_value($t.getData(1), "gee", "first object");
    test_value(exists $t.testing, False, "memberGate() existence");
    test_value($t.testing, "memberGate-testing", "memberGate() value");
    test_value($t.test(), "test", "methodGate() value");
    test_value($t instanceof Test, True, "first instanceof");
    test_value($t instanceof Qore::Socket, True, "second instanceof");

    # verify private member access protection
    my $test = "object -= private member";
    try { $t -= "a"; err($test); } catch($ex) { check($ex.err, $test); }
    $test = "object -= list of private members";
    try { $t -= ("a", "b"); err($test); } catch($ex) { check($ex.err, $test); }
    $test = "delete object's private member";
    try { delete $t.a; err($test); } catch($ex) { check($ex.err, $test); }
    $test = "reassign object's private member";
    try { $t.a = 3; err($test); } catch($ex) { check($ex.err, $test); }

    my $t2 = new Test2();
    $test = "read object's private member";
    try { my $x = $t2.a; err($test); } catch($ex) { check($ex.err, $test); }

    # test memberGate
    test_value($t.a, "memberGate-a", "object memberGate() methods");

    # test memberNotification()
    $t.x = 1;
    # test object closure
    my $c = $t.closure(1);
    test_value($c(2), "gee-1-2-2", "first object closure");
    test_value($c(2), "gee-1-2-3", "second object closure");
    test_value($t.t.x, 1, "memberNotification() method");
    class_test_File();
    class_test_Program();
}

# find and context tests
sub context_tests() {
    my $q = ( "name" : ("david", "renata", "laura", "camilla", "isabella"),
	      "age"  : (37, 30, 7, 4, 1 ) );

    # initial matrix
    my $t = ( "name" : ("david", "renata", "laura", "camilla", "isabella"),
	      "key1" : (1, 2, 3, 4, 5),
	      "key2" : (4, 5, 6, 7, 8),
	      "key3" : (7, 8, 9, 0, 1),
	      "key4" : (2, 3, 4, 5, 6),
	      "key5" : (3, 4, 5, 6, 7) );

    # resulting transposed matrix
    my $i = ( "david"    : (1, 4, 7, 2, 3),
	      "renata"   : (2, 5, 8, 3, 4),
	      "laura"    : (3, 6, 9, 4, 5),
	      "camilla"  : (4, 7, 0, 5, 6),
	      "isabella" : (5, 8, 1, 6, 7) );

    my $h;
    context q ($q) sortBy (%name)
	context t ($t) where (%q:name == %name) sortBy (%key2)
	    $h.%q:name = (%key1, %t:key2, %key3, %key4, %key5);

    test_value($h, $i, "context");

    $t = find %age in $q where (%name == "david");
    test_value($t, 37, "find");

    $t = find %age in $q where (%name == "david" || %name == "isabella");
    test_value($t, (37, 1), "list find"); 
    context ($q) {
	test_value(%%, ("name" : "david", "age" : 37), "context row");
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

namespace NTest {
    const t1 = "hello";

    namespace Type{
        const i = 2;
    }

    const Type::hithere = 4.0;

    class T1;
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
    test_value(NTest::Type::i, 2, "third namespace constant"); 
    test_value(chash{b}, (1, 2, 3), "indirect constant");
    test_value(exp, 3, "evaluated constant");
    test_value(hexp2, (1, 2, 3), "evaluated constant hash");
}

const xsd = '<?xml version="1.0" encoding="utf-8"?>
<xsd:schema targetNamespace="http://qoretechnologies.com/test/namespace" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  <xsd:element name="TestElement">
    <xsd:complexType>
      <xsd:simpleContent>
        <xsd:extension base="xsd:string"/>
      </xsd:simpleContent>
    </xsd:complexType>
  </xsd:element>
</xsd:schema>
';

sub xml_tests() {
    my $o = ( "test" : 1, 
	      "gee" : "philly", 
	      "marguile" : 1.0392,
	      "list" : (1, 2, 3, ( "four" : 4 ), 5),
	      "hash" : ( "howdy" : 123, "partner" : 456 ),
	      "list^1" : "test",
	      "bool" : True,
	      "time" : now(),
	      "bool^1" : False,
	      "key"  : "this & that" );
    my $mo = ( "o" : $o );
    my $str = makeXMLString("o", $o);
    test_value($mo == parseXML($str), True, "first parseXML()");
    $str = makeFormattedXMLString("o", $o);
    test_value($mo == parseXML($str), True, "second parseXML()");
    my $params = (1, True, "string", NOTHING, $o);
    $str = makeXMLRPCCallStringArgs("test.method", $params);
    my $result = ( "methodName" : "test.method", "params" : $params );    
    test_value(parseXMLRPCCall($str), $result, "makeXMLRPCCallStringArgs() and parseXMLRPCCall()");
    $str = makeFormattedXMLRPCCallStringArgs("test.method", $params);

    test_value(parseXMLRPCCall($str), $result, "makeFormattedXMLRPCCallStringArgs() and parseXMLRPCCall()");
    $str = makeXMLRPCCallString("test.method", True, $o);
    $result = ( "methodName" : "test.method","params" : (True, $o) );
    test_value(parseXMLRPCCall($str), $result, "makeXMLRPCCallString() and parseXMLRPCCall()");
    $str = makeFormattedXMLRPCCallString("test.method", True, $o);
    test_value(parseXMLRPCCall($str), $result, "makeFormattedXMLRPCCallString() and parseXMLRPCCall()");
    $str = makeXMLRPCResponseString($o);
    test_value(parseXMLRPCResponse($str), ( "params" : $o ), "first makeXMLRPCResponse() and parseXMLRPCResponse()");
    $str = makeFormattedXMLRPCResponseString($o);
    test_value(parseXMLRPCResponse($str), ( "params" : $o ), "first makeFormattedXMLRPCResponse() and parseXMLRPCResponse()");
    $str = makeXMLRPCFaultResponseString(100, "error");
    my $fr = ( "fault" : ( "faultCode" : 100, "faultString" : "error" ) );
    test_value(parseXMLRPCResponse($str), $fr, "second makeXMLRPCResponse() and parseXMLRPCResponse()");
    $str = makeFormattedXMLRPCFaultResponseString(100, "error");
    test_value(parseXMLRPCResponse($str), $fr, "second makeXMLRPCResponse() and parseXMLRPCResponse()");
    $o = ( "xml" : ($o + ( "^cdata^" : "this string contains special characters &<> etc" )) );
    test_value($o == parseXML(makeXMLString($o)), True, "xml serialization with cdata");

    if (Option::HAVE_PARSEXMLWITHSCHEMA) {
        $o = ( "ns:TestElement" : ( "^attributes^" : ( "xmlns:ns" : "http://qoretechnologies.com/test/namespace" ), "^value^" : "testing" ) );

        test_value(parseXMLWithSchema(makeXMLString($o), xsd), $o, "parseXMLWithSchema()");
    }

    $str = makeXMLString($mo);
    my $xd = new XmlDoc($str);
    test_value($xd.toQore() == $mo, True, "XmlDoc::constructor(<string>), XmlDoc::toQore()");
    test_value(parseXML($xd.toString()) == $mo, True, "XmlDoc::toString()");
    my $n = $xd.evalXPath("//list[2]")[0];
    test_value($n.getContent(), "2", "XmlDoc::evalXPath()");
    test_value($n.getElementTypeName(), "XML_ELEMENT_NODE", "XmlNode::getElementTypeName()");
    $n = $xd.getRootElement().firstElementChild();
    test_value($n.getName(), "test", "XmlDoc::geRootElement(), XmlNode::firstElementChild(), XmlNode::getName()");
    $n = $xd.getRootElement().lastElementChild();
    test_value($n.getName(), "key", "XmlNode::lastElementChild()");
    test_value($n.previousElementSibling().getName(), "bool", "XmlNode::previousElementSibling()");
    test_value($xd.getRootElement().childElementCount(), 14, "XmlNode::childElementCount()");

    $xd = new XmlDoc($mo);
    test_value($xd.toQore() == $mo, True, "XmlDoc::constructor(<hash>), XmlDoc::toQore()");

    my $xr = new XmlReader($xd);
    # move to first element
    $xr.read();
    test_value($xr.nodeType(), Xml::XML_NODE_TYPE_ELEMENT, "XmlReader::read(), XmlReader::Type()");
    test_value($xr.toQore() == $mo.o, True, "XmlReader::toQoreData()");
}

sub json_tests() {
    my $h = ( "test" : 1, 
	      "gee" : "philly-\"test-quotes\"", 
	      "marguile" : 1.0392,
	      "list" : (1, 2, 3, ( "four" : 4 ), 5.0, True, ( "key1" : "one", "key2" : 2.0 )),
	      "hash" : ( "howdy" : 123, "partner" : 456 ),
	      "bool" : True,
	      "time" : format_date("YYYY-MM-DD HH:mm:SS", now()),
	      "key"  : "this & that" );
    my $jstr = makeJSONString($h);
    test_value($h == parseJSON($jstr), True, "first JSON");

    my $ver = "1.1";
    my $id = 512;
    my $method = "methodname";
    my $mess = "an error occurred, OH NO!!!!";

    my $jc = ( "version" : $ver,
	       "id" : $id,
	       "method" : $method,
	       "params" : $h );

    test_value(parseJSON(makeJSONRPCRequestString($method, $ver, $id, $h)) == $jc, True, "makeJSONRPCRequestString");
    test_value(parseJSON(makeFormattedJSONRPCRequestString($method, $ver, $id, $h)) == $jc, True, "makeJSONRPCRequestString");

    # create result hash by modifying the call hash above: delete "method" and "params" keys and add "result" key
    my $jr = $jc - "method" - "params" + ( "result" : $h );
    test_value(parseJSON(makeJSONRPCResponseString($ver, $id, $h)) == $jr, True, "makeJSONRPCResponseString");
    test_value(parseJSON(makeFormattedJSONRPCResponseString($ver, $id, $h)) == $jr, True, "makeFormattedJSONRPCResponseString");

    # create error hash by modifying the result hash: delete "result" key and add "error" key
    my $je = $jr - "result" + ( "error" : $h );
    test_value(parseJSON(makeJSONRPCErrorString($ver, $id, $h)) == $je, True, "makeJSONRPCErrorString");
    test_value(parseJSON(makeFormattedJSONRPCErrorString($ver, $id, $h)) == $je, True, "makeFormattedJSONRPCErrorString");

    # create JSON-RPC 1.1 error string
    $je = $je + ( "error" : ( "name" : "JSONRPCError", "code" : $id, "message" : $mess, "error" : $h ) );
    test_value(parseJSON(makeJSONRPC11ErrorString($id, $mess, $id, $h)) == $je, True, "makeJSONRPCErrorString");
    test_value(parseJSON(makeFormattedJSONRPC11ErrorString($id, $mess, $id, $h)) == $je, True, "makeFormattedJSONRPCErrorString");

    $jstr = "{
    \"glossary\": {
        \"title\": \"example glossary\",
		\"GlossDiv\": {
            \"title\": \"S\",
			\"GlossList\": {
                \"GlossEntry\": {
                    \"ID\": \"SGML\",
					\"SortAs\": \"SGML\",
					\"GlossTerm\": \"Standard Generalized Markup Language\",
					\"Acronym\": \"SGML\",
					\"Abbrev\": \"ISO 8879:1986\",
					\"GlossDef\": {
                        \"para\": \"A meta-markup language, used to create markup languages such as DocBook.\",
						\"GlossSeeAlso\": [\"GML\", \"XML\"]
                    },
					\"GlossSee\": \"markup\"
                }
            }
        }
    }
}";
    my $xml = "<glossary><title>example glossary</title>
  <GlossDiv><title>S</title>
   <GlossList>
    <GlossEntry>
     <ID>SGML</ID>
     <SortAs>SGML</SortAs>
     <GlossTerm>Standard Generalized Markup Language</GlossTerm>
     <Acronym>SGML</Acronym>
     <Abbrev>ISO 8879:1986</Abbrev>
     <GlossDef>
      <para>A meta-markup language, used to create markup languages such as DocBook.</para>
      <GlossSeeAlso>GML</GlossSeeAlso>
      <GlossSeeAlso>XML</GlossSeeAlso>
     </GlossDef>
     <GlossSee>markup</GlossSee>
    </GlossEntry>
   </GlossList>
  </GlossDiv>
 </glossary>";
    test_value(parseJSON($jstr), parseXML($xml), "first parseJSON() and parseXML()");

    $jstr = '
{ "web-app": {
  "servlet": [   
    {
      "servlet-name": "cofaxCDS",
      "servlet-class": "org.cofax.cds.CDSServlet",
      "init-param": {
        "configGlossary:installationAt": "Philadelphia, PA",
        "configGlossary:adminEmail": "ksm@pobox.com",
        "configGlossary:poweredBy": "Cofax",
        "configGlossary:poweredByIcon": "/images/cofax.gif",
        "configGlossary:staticPath": "/content/static",
        "templateProcessorClass": "org.cofax.WysiwygTemplate",
        "templateLoaderClass": "org.cofax.FilesTemplateLoader",
        "templatePath": "templates",
        "templateOverridePath": null,
        "defaultListTemplate": "listTemplate.htm",
        "defaultFileTemplate": "articleTemplate.htm",
        "useJSP": false,
        "jspListTemplate": "listTemplate.jsp",
        "jspFileTemplate": "articleTemplate.jsp",
        "cachePackageTagsTrack": 200,
        "cachePackageTagsStore": 200,
        "cachePackageTagsRefresh": 60,
        "cacheTemplatesTrack": 100,
        "cacheTemplatesStore": 50,
        "cacheTemplatesRefresh": 15,
        "cachePagesTrack": 200,
        "cachePagesStore": 100,
        "cachePagesRefresh": 10,
        "cachePagesDirtyRead": 10,
        "searchEngineListTemplate": "forSearchEnginesList.htm",
        "searchEngineFileTemplate": "forSearchEngines.htm",
        "searchEngineRobotsDb": "WEB-INF/robots.db",
        "useDataStore": true,
        "dataStoreClass": "org.cofax.SqlDataStore",
        "redirectionClass": "org.cofax.SqlRedirection",
        "dataStoreName": "cofax",
        "dataStoreDriver": "com.microsoft.jdbc.sqlserver.SQLServerDriver",
        "dataStoreUrl": "jdbc:microsoft:sqlserver://LOCALHOST:1433;DatabaseName=goon",
        "dataStoreUser": "sa",
        "dataStorePassword": "dataStoreTestQuery",
        "dataStoreTestQuery": "SET NOCOUNT ON;select test=\"test\";",
        "dataStoreLogFile": "/usr/local/tomcat/logs/datastore.log",
        "dataStoreInitConns": 10,
        "dataStoreMaxConns": 100,
        "dataStoreConnUsageLimit": 100,
        "dataStoreLogLevel": "debug",
        "maxUrlLength": 500}},
    {
      "servlet-name": "cofaxEmail",
      "servlet-class": "org.cofax.cds.EmailServlet",
      "init-param": {
      "mailHost": "mail1",
      "mailHostOverride": "mail2"}},
    {
      "servlet-name": "cofaxAdmin",
      "servlet-class": "org.cofax.cds.AdminServlet"},
 
    {
      "servlet-name": "fileServlet",
      "servlet-class": "org.cofax.cds.FileServlet"},
    {
      "servlet-name": "cofaxTools",
      "servlet-class": "org.cofax.cms.CofaxToolsServlet",
      "init-param": {
        "templatePath": "toolstemplates/",
        "log": 1,
        "logLocation": "/usr/local/tomcat/logs/CofaxTools.log",
        "logMaxSize": null,
        "dataLog": 1,
        "dataLogLocation": "/usr/local/tomcat/logs/dataLog.log",
        "dataLogMaxSize": null,
        "removePageCache": "/content/admin/remove?cache=pages&id=",
        "removeTemplateCache": "/content/admin/remove?cache=templates&id=",
        "fileTransferFolder": "/usr/local/tomcat/webapps/content/fileTransferFolder",
        "lookInContext": 1,
        "adminGroupID": 4,
        "betaServer": true}}],
  "servlet-mapping": {
    "cofaxCDS": "/",
    "cofaxEmail": "/cofaxutil/aemail/*",
    "cofaxAdmin": "/admin/*",
    "fileServlet": "/static/*",
    "cofaxTools": "/tools/*"},
 
  "taglib": {
    "taglib-uri": "cofax.tld",
    "taglib-location": "/WEB-INF/tlds/cofax.tld"}}}
';
    $xml = '<?xml version="1.0" encoding="UTF-8"?><web-app xmlns:configGlossary="http://nothing.com"><servlet><servlet-name>cofaxCDS</servlet-name><servlet-class>org.cofax.cds.CDSServlet</servlet-class><init-param><configGlossary:installationAt>Philadelphia, PA</configGlossary:installationAt><configGlossary:adminEmail>ksm@pobox.com</configGlossary:adminEmail><configGlossary:poweredBy>Cofax</configGlossary:poweredBy><configGlossary:poweredByIcon>/images/cofax.gif</configGlossary:poweredByIcon><configGlossary:staticPath>/content/static</configGlossary:staticPath><templateProcessorClass>org.cofax.WysiwygTemplate</templateProcessorClass><templateLoaderClass>org.cofax.FilesTemplateLoader</templateLoaderClass><templatePath>templates</templatePath><templateOverridePath></templateOverridePath><defaultListTemplate>listTemplate.htm</defaultListTemplate><defaultFileTemplate>articleTemplate.htm</defaultFileTemplate><useJSP>0</useJSP><jspListTemplate>listTemplate.jsp</jspListTemplate><jspFileTemplate>articleTemplate.jsp</jspFileTemplate><cachePackageTagsTrack>200</cachePackageTagsTrack><cachePackageTagsStore>200</cachePackageTagsStore><cachePackageTagsRefresh>60</cachePackageTagsRefresh><cacheTemplatesTrack>100</cacheTemplatesTrack><cacheTemplatesStore>50</cacheTemplatesStore><cacheTemplatesRefresh>15</cacheTemplatesRefresh><cachePagesTrack>200</cachePagesTrack><cachePagesStore>100</cachePagesStore><cachePagesRefresh>10</cachePagesRefresh><cachePagesDirtyRead>10</cachePagesDirtyRead><searchEngineListTemplate>forSearchEnginesList.htm</searchEngineListTemplate><searchEngineFileTemplate>forSearchEngines.htm</searchEngineFileTemplate><searchEngineRobotsDb>WEB-INF/robots.db</searchEngineRobotsDb><useDataStore>1</useDataStore><dataStoreClass>org.cofax.SqlDataStore</dataStoreClass><redirectionClass>org.cofax.SqlRedirection</redirectionClass><dataStoreName>cofax</dataStoreName><dataStoreDriver>com.microsoft.jdbc.sqlserver.SQLServerDriver</dataStoreDriver><dataStoreUrl>jdbc:microsoft:sqlserver://LOCALHOST:1433;DatabaseName=goon</dataStoreUrl><dataStoreUser>sa</dataStoreUser><dataStorePassword>dataStoreTestQuery</dataStorePassword><dataStoreTestQuery>SET NOCOUNT ON;select test=&quot;test&quot;;</dataStoreTestQuery><dataStoreLogFile>/usr/local/tomcat/logs/datastore.log</dataStoreLogFile><dataStoreInitConns>10</dataStoreInitConns><dataStoreMaxConns>100</dataStoreMaxConns><dataStoreConnUsageLimit>100</dataStoreConnUsageLimit><dataStoreLogLevel>debug</dataStoreLogLevel><maxUrlLength>500</maxUrlLength></init-param></servlet><servlet><servlet-name>cofaxEmail</servlet-name><servlet-class>org.cofax.cds.EmailServlet</servlet-class><init-param><mailHost>mail1</mailHost><mailHostOverride>mail2</mailHostOverride></init-param></servlet><servlet><servlet-name>cofaxAdmin</servlet-name><servlet-class>org.cofax.cds.AdminServlet</servlet-class></servlet><servlet><servlet-name>fileServlet</servlet-name><servlet-class>org.cofax.cds.FileServlet</servlet-class></servlet><servlet><servlet-name>cofaxTools</servlet-name><servlet-class>org.cofax.cms.CofaxToolsServlet</servlet-class><init-param><templatePath>toolstemplates/</templatePath><log>1</log><logLocation>/usr/local/tomcat/logs/CofaxTools.log</logLocation><logMaxSize></logMaxSize><dataLog>1</dataLog><dataLogLocation>/usr/local/tomcat/logs/dataLog.log</dataLogLocation><dataLogMaxSize></dataLogMaxSize><removePageCache>/content/admin/remove?cache=pages&amp;id=</removePageCache><removeTemplateCache>/content/admin/remove?cache=templates&amp;id=</removeTemplateCache><fileTransferFolder>/usr/local/tomcat/webapps/content/fileTransferFolder</fileTransferFolder><lookInContext>1</lookInContext><adminGroupID>4</adminGroupID><betaServer>1</betaServer></init-param></servlet><servlet-mapping><cofaxCDS>/</cofaxCDS><cofaxEmail>/cofaxutil/aemail/*</cofaxEmail><cofaxAdmin>/admin/*</cofaxAdmin><fileServlet>/static/*</fileServlet><cofaxTools>/tools/*</cofaxTools></servlet-mapping><taglib><taglib-uri>cofax.tld</taglib-uri><taglib-location>/WEB-INF/tlds/cofax.tld</taglib-location></taglib></web-app>';
    my $x = parseXML($xml);
    delete $x."web-app"."^attributes^";
    test_value(parseJSON($jstr) == $x, True, "second parseJSON() and parseXML()");
}

sub digest_tests() {
    my $str = "Hello There This is a Test - 1234567890";

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
    my $str = "Hello There This is a Test - 1234567890";

    my $key = "1234567812345abcabcdefgh";
    my $x = des_ede_encrypt_cbc($str, $key);
    my $xstr = des_ede_decrypt_cbc_to_string($x, $key);
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

    $key = des_random_key();
    $x = des_encrypt_cbc($str, $key);
    $xstr = des_decrypt_cbc_to_string($x, $key);
    test_value($str, $xstr, "DES random single key encrypt-decrypt");
}

sub closures($x) {
    my $a = 1;
    
    my $inc = sub ($y) {
	return sprintf("%s-%n-%n", $x, $y, ++$a);
    };

    my $dec = sub ($y) {
	return sprintf("%s-%n-%n", $x, $y, --$a);
    };

    return ($inc, $dec);
}

sub closure_tests() {
    my ($inc, $dec) = closures("test");
    test_value($inc(5), "test-5-2", "first closure");
    test_value($inc(7), "test-7-3", "second closure");
    test_value($dec(3), "test-3-2", "third closure");
}

sub do_tests() {
    try {
	for (my $i = 0; $i < $o.iters; $i++) {
	    if ($o.verbose)
		printf("TID %d: iteration %d\n", gettid(), $i);
	    operator_test();
	    array_tests();
	    hash_tests();
	    logic_tests();
	    statement_tests();
	    recursive_function_test();
	    parameter_tests();
	    class_library_tests();
	    function_library_test();
	    context_tests();
	    constant_tests();	
	    xml_tests();
	    json_tests();
	    crypto_tests();
	    digest_tests();
	    closure_tests();
	    if ($o.bq)
		backquote_tests();
	}
    }
    catch () {
	++$errors;
	$counter.dec();
	rethrow;	
    }
    $counter.dec();
}

sub main() {
    parse_command_line();
    printf("QORE v%s Test Script (%d thread%s, %d iteration%s per thread)\n", Qore::VersionString, 
	   $o.threads, $o.threads == 1 ? "" : "s", $o.iters, $o.iters == 1 ? "" : "s");

    $counter = new Counter();
    while ($o.threads--) {
	$counter.inc();
	background do_tests();
    }

    $counter.waitForZero();

    my $ntests = elements $thash;
    printf("%d error%s encountered in %d test%s.\n",
	   $errors, $errors == 1 ? "" : "s", 
	   $ntests, $ntests == 1 ? "" : "s");
}

main();
