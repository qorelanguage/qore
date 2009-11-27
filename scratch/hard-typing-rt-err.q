#!/usr/bin/env qore

sub get_bool() {
    return True;
}

sub get_int() {
    return 1;
}

sub main() {
    my (int $x, string $y) = (get_bool(), get_int());
}

main();
