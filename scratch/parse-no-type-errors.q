#!/usr/bin/env qore 

%require-types

class T {
    constructor($a, $b, $c) {
    }
    destructor() {
    }
    copy($a) {
    }
    a($a) {
	my $x = 1;
	my ($b, $c) = (2, 3);
    }
    static b($a, $b) {
    }
}

sub t($a, $b, $c) {
    printf("a=%n\n", $a);
}

our ($x, $y) = (2, 3);

t(1, 2, 3);
