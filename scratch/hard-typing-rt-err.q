#!/usr/bin/env qore

%requires qore >= 0.8
%require-types

sub get_bool() returns bool { return True; }
sub get_int() returns int { return 1; }
sub test_int(int $x) returns nothing {}
sub test_mutex(Mutex $m) returns nothing {}
sub test_bool(bool $b) returns nothing {}

sub do_neg_test(code $c, string $name) returns nothing {
    try {
	$c();
	printf("ERROR: %s\n", $name);
    }
    catch (hash $ex)
	printf("OK: %s\n", $name);
}

sub do_pos_test(code $c, string $name) returns nothing {
    try {
	$c();
	printf("OK: %s\n", $name);
    }
    catch (hash $ex)
	printf("ERROR: %s: %s: %s\n", $name, $ex.err, $ex.desc);
}

sub main() returns nothing {
    my code $t = sub () returns nothing { my (int $x, string $y) = (get_bool(), get_int()); };
    do_neg_test($t, "list assignment");
    $t = sub () returns nothing { my int $x = get_bool(); };
    do_neg_test($t, "simple assignment");
    $t = sub () returns nothing { my any $x = True; test_int($x); };
    do_neg_test($t, "argument type");

    $t = sub () returns nothing { my int $i; test_int($i); };
    do_neg_test($t, "local variable default value");
    $t = sub () returns nothing { our bool $ob; test_bool($ob); };
    do_neg_test($t, "global variable default value");
    $t = sub () returns nothing { my code $c = sub(reference $x) returns nothing { ++$x; }; my string $str; $c(\$str); };
    do_neg_test($t, "reference type");
}

main();
