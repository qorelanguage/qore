#!/usr/bin/env qore

sub get_bool() {
    return True;
}

sub get_int() {
    return 1;
}

sub main() {
    try {
	my (int $x, string $y) = (get_bool(), get_int());
	printf("ERROR\n");
    }
    catch ($ex) {
	printf("OK\n");
    }
    try {
	my int $x = get_bool();
	printf("ERROR\n");
    }
    catch ($ex) {
	printf("OK\n");
    }

}

main();
