#!/usr/bin/env qore

sub get_bool() { return True; }
sub get_int() { return 1; }
sub test_int(int $x) {}
sub test_mutex(Mutex $m) {}
sub test_bool(bool $b) {}

sub do_neg_test($c, $name) {
    try {
	$c();
	printf("ERROR: %s\n", $name);
    }
    catch ($ex)
	printf("OK: %s\n", $name);
}

sub do_pos_test($c, $name) {
    try {
	$c();
	printf("OK: %s\n", $name);
    }
    catch ($ex)
	printf("ERROR: %s: %s: %s\n", $name, $ex.err, $ex.desc);
}

sub main() {
    my $t = sub () { my (int $x, string $y) = (get_bool(), get_int()); };
    do_neg_test($t, "list assignment");
    $t = sub () { my int $x = get_bool(); };
    do_neg_test($t, "simple assignment");
    $t = sub () { my $x = True; test_int($x); };
    do_neg_test($t, "argument type");

    $t = sub () { my int $i; test_int($i); };
    do_pos_test($t, "local variable default value");
    $t = sub () { our bool $ob; test_bool($ob); };
    do_pos_test($t, "global variable default value");
}

main();
