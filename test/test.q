#!/usr/bin/env qore 

%require-our
%no-child-restrictions

# global variables needed for tests
our $to = new Test("program-test.q");
our $ro = new Test("readonly");
our ($o, $errors, $counter);

sub get_program_name()
{
    my $l = split("/", $ENV."_");
    return $l[elements $l - 1];
}

sub usage()
{
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

sub parse_command_line()
{
    my $g = new GetOpt(opts);
    $o = $g.parse(\$ARGV);
    if (exists $o."_ERRORS_")
    {
        printf("%s\n", $o."_ERRORS_"[0]);
        exit(1);
    }

    $o.iters = shift $ARGV;

    if (!$o.iters)
	$o.iters = 1;
    if (!$o.threads)
	$o.threads = 1;
}

sub test_value($v1, $v2, $msg)
{
    if ($v1 === $v2)
    {
	if ($o.verbose)
	    printf("OK: %s test\n", $msg);
    }
    else
    {
	printf("ERROR: %s test failed! (%n != %n)\n", $msg, $v1, $v2);
	#printf("%s%s", dbg_node_info($v1), dbg_node_info($v2));
	$errors++;
    }
}

sub test1() { return 1;} sub test2() { return 2; } 
sub test3() { return (1, 2, 3); }

sub array_helper($a)
{
    $a[1][1] = 2;
    test_value($a[1][1], 2, "passed local array variable assignment");    
}

sub list_return($var)
{
    return (1, test2(), $var);
}

sub hash_return($var)
{
    return ( "gee" : "whiz", 
	     "num" : test1(),
	     "var" : $var );
}

# array tests
sub array_tests()
{
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
    test_value(sort($l1), (1,2,3,4,5,6), "first sort()");
    test_value(sort($l2), ("five", "four", "one", "six", "three", "two"), "second sort()");
    test_value(sortDescending($l1), (6,5,4,3,2,1), "first sortDescending()");
    test_value(sortDescending($l2), ("two", "three", "six", "one", "four", "five"), "second sortDescending()");
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
}

sub hash_tests()
{
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
    # delete 3 keys from the $c hash
    $b = $c - "new" - "barn" - "asd";
    test_value($b, ( "key" : 3, "unique" : 100 ), "hash minus operator"); 
    $b -= "unique";
    test_value($b, ( "key" : 3 ), "hash minus-equals operator"); 
}

sub global_variable_testa()
{
    printf("user=%s\n", $ENV{"USER"});
}

# local variable operator tests
sub local_operator_test()
{
    if ($o.verbose)
	print("%%%% local operator tests\n");
    my $a = 1;
    test_value($a, 1, "local variable assignment");
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
    # array and hash tests in separate functions
}

sub no_parameter_test($p)
{
    test_value($p, NOTHING, "non-existant parameter");
}

sub parameter_and_shift_test($p)
{
    test_value($p, 1, "parameter before shift");
    test_value(shift $argv, 2, "shift on second parameter");
}

sub one_parameter_shift_test()
{
    test_value(shift $argv, 1, "one parameter shift");
}

sub shift_test()
{
    my $var = (1, 2, 3, 4, "hello");
    foreach my $v in ($var)
	test_value($v, shift $argv, ("shift " + string($v) + " parameter"));
}

sub parameter_tests()
{
    no_parameter_test();
    parameter_and_shift_test(1, 2);
    shift_test(1, test3()[1], 3, 4, "hello");
    one_parameter_shift_test(1);
}

sub short_circuit_test($op)
{
    print("ERROR: $op logic short-circuiting is not working!\n");
    $errors++;
    return 0;
}

sub logic_message($op)
{
    if ($o.verbose)
	printf("OK: %s logic test\n", $op);
}

# logic short-circuiting test
sub logic_tests()
{
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

sub printf_tests()
{
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

sub switch_test($val)
{
    my $rv;

    switch ($val)
    {
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

sub statement_tests()
{
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
    for (my $a = 0; $a < 3; $a++)
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
    foreach my $x in (\$list) $x = "test";
    test_value($list, NOTHING, "first foreach reference");
    
    $list = (1, 2, 3);
    foreach my $x in (\$list) $x = "test";
    test_value($list, ("test", "test", "test"), "second foreach reference");
    
    $list = 1;
    foreach my $x in (\$list) $x = "test";
    test_value($list, "test", "third foreach reference");

    # switch tests
    test_value(switch_test(1), "case 1", "first switch");
    test_value(switch_test(2), "default", "second switch");
    test_value(switch_test(3), "default", "third switch");
    test_value(switch_test(0), "case 1", "fourth switch");
    test_value(switch_test("hello"), "case 1", "fifth switch");
    test_value(switch_test("testing"), "default", "sixth switch");
    
}

sub fibonacci($num)
{
    if ($num == 2)
        return 2;
    return $num * fibonacci($num - 1);
}

# recursive function test
sub recursive_function_test()
{
    test_value(fibonacci(10), 3628800, "recursive function");
}

sub backquote_tests()
{
    test_value(`echo -n 1`, "1", "backquote");
}

sub sd($d)
{
    return format_date("YYYY-MM-DD HH:mm:SS", $d);
}

sub time_tests()
{
    my $date  = 2004-02-01-12:30:00;
    my $ndate = 2004-03-02-12:30:00;
    test_value(format_date("YYYY-MM-DD HH:mm:SS", $date), "2004-02-01 12:30:00", "format_date()");
    test_value($date - 1Y, 2003-02-01-12:30:00, "first date year subtraction");
    test_value($date - 1M, 2004-01-01-12:30:00, "first date month subtraction");
    test_value($date - 1D, 2004-01-31-12:30:00, "first date day subtraction");
    test_value($date - 1h, 2004-02-01-11:30:00, "first date hour subtraction");
    test_value($date - 1m, 2004-02-01-12:29:00, "first date minute subtraction");
    test_value($date - 1s, 2004-02-01-12:29:59, "first date second subtraction");
    test_value($date + 1Y, 2005-02-01-12:30:00, "first date year addition");
    test_value($date + 1M, 2004-03-01-12:30:00, "first date month addition");
    test_value($date + 1D, 2004-02-02-12:30:00, "first date day addition");
    test_value($date + 1h, 2004-02-01-13:30:00, "first date hour addition");
    test_value($date + 1m, 2004-02-01-12:31:00, "first date minute addition");
    test_value($date + 1s, 2004-02-01-12:30:01, "first date second addition");
    test_value($date - years(1),   2003-02-01-12:30:00, "second date year subtraction");
    test_value($date - months(1),  2004-01-01-12:30:00, "second date month subtraction");
    test_value($date - days(1),    2004-01-31-12:30:00, "second date day subtraction");
    test_value($date - hours(1),   2004-02-01-11:30:00, "second date hour subtraction");
    test_value($date - minutes(1), 2004-02-01-12:29:00, "second date minute subtraction");
    test_value($date - seconds(1), 2004-02-01-12:29:59, "second date second subtraction");
    test_value($date + years(1),   2005-02-01-12:30:00, "second date year addition");
    test_value($date + months(1),  2004-03-01-12:30:00, "second date month addition");
    test_value($date + days(1),    2004-02-02-12:30:00, "second date day addition");
    test_value($date + hours(1),   2004-02-01-13:30:00, "second date hour addition");
    test_value($date + minutes(1), 2004-02-01-12:31:00, "second date minute addition");
    test_value($date + seconds(1), 2004-02-01-12:30:01, "second date second addition");
    test_value($date, localtime(mktime($date)), "localtime() and mktime()");
    test_value($date + 30D, $ndate, "third date day addition");
    test_value($ndate - 30D, $date, "third date day subtraction");
    test_value($date + 23M, 2006-01-01-12:30:00, "third date month addition");
    test_value($date - 4M, 2003-10-01-12:30:00, "third date month subtraction");
    test_value($date, date(20040201123000), "date function");
}

sub binary_tests()
{
    my $b = binary("this is a test");
    test_value(getByte($b, 3), ord("s"), "getByte()");
    test_value($b, binary("this is a test"), "binary comparison");
    test_value($b != binary("this is a test"), False, "binary negative comparison");
}

sub string_tests()
{
    my $str = "Hi there, you there, pal";
    my $big = "GEE WHIZ";
    test_value(strlen($str), 24, "strlen()");
    test_value(toupper($str), "HI THERE, YOU THERE, PAL", "toupper()");
    test_value(tolower($big), "gee whiz", "tolower()");
    
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

    # convert the string to single-byte ISO-8859-1 characters and retest
    $str = convert_encoding($str, "ISO-8859-1");
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
    test_value($a[2], "is", "first split()");
    test_value($a[5], "string", "second split()");
    $str = "äüößÄÖÜ";
    # test length() with UTF-8 multi-byte characters
    test_value(length($str), 7, "length() with UTF-8 multi-byte characters");
    test_value(strlen($str), 14, "strlen() with UTF-8 multi-byte characters");
    # test charset encoding conversions
    my $nstr = convert_encoding($str, "ISO-8859-1");
    test_value(length($nstr), 7, "length() with ISO-8859-1 special characters");
    test_value(strlen($nstr), 7, "strlen() with ISO-8859-1 special characters");
    test_value($str, convert_encoding($nstr, "UTF-8"), "convert_encoding()");
    $str += "hello there";
    my $x = binary($str);
    my $b64 = makeBase64String($x);
    test_value($x, parseBase64String($b64), "first base64");

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
    my $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
    test_value($l[0] =~ s/bar/foo/, "hello foo hi bar", "first non-global regular expression substitution");
    test_value($l[1] =~ s/bar/foo/, "foo hello bar hi bar", "second non-global regular expression substitution");
    test_value($l[2] =~ s/BAR/foo/i, "hello foo hi", "case-insensitive non-global regular expression substitution");
    my $l = ( "hello bar hi bar", "bar hello bar hi bar", "hello bar hi" );
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
}

sub pwd_tests()
{
    test_value(getpwuid(0).pw_name, "root", "getpwuid()");
}

sub simple_shift()
{
    return shift $argv;
}

sub misc_tests()
{
    test_value(call_function("simple_shift", 1), 1, "call_function()");
    test_value(existsFunction("simple_shift"), True, "existsFunction()");
    test_value(functionType("simple_shift"), "user", "functionType() user");
    test_value(functionType("printf"), "builtin", "functionType() builtin");
    test_value(typename(1), "integer", "type()");
    my $str1 = "&<>\"";
    my $str2 = "&amp;&lt;&gt;&quot;";
    test_value(html_encode($str1), $str2, "html_encode()");
    test_value(html_decode($str2), $str1, "html_decode()");
    
    my $str = "This is a long string xxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    my $b = compress($str);
    test_value($str, uncompress_to_string($b, strlen($str) + 1), "compress and uncompress");
}

sub math_tests()
{
    test_value(ceil(2.7), 3.0, "ceil()");
    test_value(floor(3.7), 3.0, "fllor()");
    test_value(format_number(".,3", -48392093894.2349), "-48.392.093.894,235", "format_number()");
}

sub lib_tests()
{
    test_value(stat($ENV."_")[2], 0100755, "stat()");
    if (exists $ENV.HOSTNAME)
	test_value(gethostname(), $ENV.HOSTNAME, "gethostname()");
    else
	test_value(!strlen(gethostname()), False, "!strlen(gethostname())");
}

sub io_tests()
{
    test_value(sprintf("%04d-%.2f", 25, 101.239), "0025-101.24", "sprintf()");
    test_value(vsprintf("%04d-%.2f", (25, 101.239)), "0025-101.24", "vsprintf()");
    # check multi-byte character set support for f_*printf()
    test_value(f_sprintf("%3s", "niña"), "niñ", "UTF-8 f_sprintf()");
}

sub function_library_test()
{
    time_tests();
    binary_tests();
    string_tests();  
    pwd_tests();
    misc_tests();
    math_tests();
    lib_tests();
    io_tests();
}

sub t($a)
{
    return $a + 1;
}

class Test inherits Socket {
    constructor($a, $b, $c)
    {
        $.data = ($a, $b, $c);
    }
    getData($elem)
    {
	if (exists $elem)
	    return $.data[$elem];
        return $.data;
    }
    methodGate($m)
    {
        return $m;
    }
    memberGate($m)
    {
        return $m;
    }
}

sub class_test_Program()
{
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
    test_value($to.member, "member", "program imported object member gate");
    test_value($to.method(), "method", "program imported object method gate");
    try
	$a.callFunction("deleteException");
    catch ($ex)
	test_value($ex.err, "ACCESS-ERROR", "Program::importGlobalVariable() readonly");    
    my $o = $a.callFunction("getObject");
    delete $a;
    test_value(getClassName($o), "Queue", "class returned from deleted subprogram object");
}

sub class_test_File()
{
    return;
    # File test
    my $f = new File();
    $f.open($ENV."_");
    my $l = $f.readLine();
    my $p = $f.getPos();
    $f.setPos(0);
    test_value($l, $f.readLine(), "File::readLine() and File::setPos()");
    test_value($p, $f.getPos(), "File::getPos()");
}

sub class_library_tests()
{
    my $t = new Test(1, "gee", 2);
    test_value($t.getData(1), "gee", "first object");
    test_value(exists $t.testing, False, "memberGate() existence");
    test_value($t.testing, "testing", "memberGate() value");
    test_value($t.test(), "test", "methodGate() value");
    test_value($t instanceof Test, True, "first instanceof");
    test_value($t instanceof Qore::Socket, True, "second instanceof");
    class_test_File();
    class_test_Program();
}

# find and context tests
sub context_tests()
{
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
}

const a = "key";
const b = 1.0;
const i = 1;
const l = (1, 2, 3);
const chash = ( a : "one", b : l );

const exp   = elements l;
const hexp2 = chash{b};

namespace NTest
{
    const t1 = "hello";

    namespace Type
    {
        const i       = 2;
    }

    const Type::hithere = 4.0;

    class T1;
}

namespace NTest
{
    const t2 = 2;
}

const NTest::Type::val1 = 1;

const Qore::myconst = 1;

sub constant_tests()
{
    test_value(i, 1, "simple constant");
    test_value(type(Type::val1), "integer", "first namespace constant");
    test_value(Qore::myconst, NTest::Type::val1, "second namespace constant");
    test_value(NTest::Type::i, 2, "third namespace constant"); 
    test_value(chash{b}, (1, 2, 3), "indirect constant");
    test_value(exp, 3, "evaluated constant");
    test_value(hexp2, (1, 2, 3), "evaluated constant hash");
}

sub xml_tests()
{ 
    my $o = ( "test" : 1, "gee" : "philly", "marguile" : 1.0392,
	      "list" : (1, 2, 3, ( "four" : 4 ), 5),
	      "hash" : ( "howdy" : 123, "partner" : 456 ),
	      "bool" : True,
	      "time" : now(),
	      "key"  : "this & that" );
    my $mo = ( "o" : $o );
    my $str = makeXMLString("o", $o);
    test_value($mo == parseXML($str), True, "first parseXML()");
    $str = makeFormattedXMLString("o", $o);
    test_value($mo == parseXML($str), True, "second parseXML()");
    my $params = (1, True, "string", $o);
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
}

sub digest_tests()
{
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

sub crypt_tests()
{
    my $str = "Hello There This is a Test - 1234567890";

    my $key = "1234567812345abcabcdefgh";
    my $x = des_ede_encrypt_cbc($str, $key);
    my $xstr = des_ede_decrypt_cbc_to_string($x, $key);
    test_value($str, $xstr, "triple DES 2 key encrypt-decrypt");

    $x = des_ede3_encrypt_cbc($str, $key);
    $xstr = des_ede3_decrypt_cbc_to_string($x, $key);
    test_value($str, $xstr, "triple DES 3 key encrypt-decrypt");

    $x = des_encrypt_cbc($str, $key);
    $xstr = des_decrypt_cbc_to_string($x, $key);
    test_value($str, $xstr, "DES single key encrypt-decrypt");

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
}

sub do_tests()
{
    for (my $i = 0; $i < $o.iters; $i++)
    {
	if ($o.verbose)
	    printf("TID %d: iteration %d\n", gettid(), $i);
	local_operator_test();
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
	crypt_tests();
	digest_tests();
	if ($o.bq)
	    backquote_tests();
    }
    $counter.dec();
}

sub main()
{
    parse_command_line();
    printf("QORE v%s Test Script\n", Qore::VersionString);

    $counter = new Counter();
    while ($o.threads--)
    {
	$counter.inc();
	background do_tests();
    }

    $counter.waitForZero();

    printf("%d error%s encountered.\n", $errors, $errors == 1 ? "" : "s");
}

main();
