#!/usr/bin/env qore

%enable-all-warnings

class T {
    private (int $.i = 1, float $.f = 0.0, Mutex $.mt = new Mutex());
    public $.b bool, $.str string;

    constructor() : i(1) {
    }
}

class Sub inherits T {
    #private $.f, $.str;
    #public $.b float, $.i;
}

sub main() {
    my T $t = new Sub();

    #$t.f = 1;
    #$t.b1 = 2;
}

main();
