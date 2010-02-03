#!/usr/bin/env qore

%require-our

sub test(int $x) {
    printf("int test(%n)\n", $x);
}

sub test(string $x) {
    printf("string test(%n)\n", $x);
}

sub main() {
    test(1);
    test("1");

    my int $i = 1;
    my string $str = "1";
    test($i);
    test($str);
}

main();
