#!/usr/bin/env qore


sub test(any $s, int $mul, *int $off)
{
    try {
        printf("Test: %n, %n, %n\n", $s, $mul, $off);
        printf("    : %N\n", strmul($s, $mul, $off));
    } catch ($ex) {
        printf("  ex: %N\n", $ex);
    }
    printf("\n");
}

test('a', 6);
test('%v,', 6, 1);
test('a', 6, 3);
# types
test(1, 3);
# errors
test('a', 0);
test('a', 3, -1);
test( ("foo":3), 2);
